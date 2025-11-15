#pragma once

#include <vector>
#include <functional>
#include <utility>
#include <cstddef>

// 简易链式散列表 (unordered_map 精简版)
template<typename K, typename V>
class umap {
    struct Node {
        K key;
        V value;
        Node* next;
        Node(const K& k, const V& v): key(k), value(v), next(nullptr) {}
    };
    struct Bucket {
        Node* head;
        Node* tail;
        Bucket(): head(nullptr), tail(nullptr) {}
    };
public:
    class iterator {
        using BucketVec = std::vector<Bucket>;
        BucketVec* buckets;          // 指向桶数组
        std::size_t bucketIndex;     // 当前所在桶索引
        Node* node;                  // 当前节点
        void advance_to_next_valid() {
            // 当前桶内已到末尾，跳到下一个非空桶
            while (!node && buckets && bucketIndex < buckets->size()) {
                Node* h = (*buckets)[bucketIndex].head;
                if (h) { node = h; break; }
                ++bucketIndex;
            }
        }
    public:
        iterator(BucketVec* b, std::size_t idx, Node* n): buckets(b), bucketIndex(idx), node(n) {
            if (!node) advance_to_next_valid();
        }
        std::pair<const K&, V&> operator*() const { return { node->key, node->value }; } // 如何保证key不被改变
        iterator& operator++() {
            if (!node) return *this;
            node = node->next;
            if (!node) { ++bucketIndex; advance_to_next_valid(); }
            return *this;
        }
        bool operator==(const iterator& other) const { return node == other.node; }
        bool operator!=(const iterator& other) const { return !(*this == other); }
        V& value() const { return node->value; }
    };

public:
    umap(): usedSize_(0), elementCount_(0) {
        buckets_.resize(bucket_sizes_[usedSize_]);
    }
    ~umap() { clear(); }

    umap(const umap& other): usedSize_(other.usedSize_), elementCount_(0) {
        buckets_.resize(bucket_sizes_[usedSize_]);
        for (auto it = other.begin(); it != other.end(); ++it) {
            insert((*it).first, (*it).second);
        }
    }
    umap& operator=(const umap& other) {
        if (this == &other) return *this;
        clear();
        usedSize_ = other.usedSize_;
        buckets_.clear();
        buckets_.resize(bucket_sizes_[usedSize_]);
        elementCount_ = 0;
        for (auto it = other.begin(); it != other.end(); ++it) {
            insert((*it).first, (*it).second);
        }
        return *this;
    }
    umap(umap&& other) noexcept: buckets_(std::move(other.buckets_)), usedSize_(other.usedSize_), elementCount_(other.elementCount_) {
        other.usedSize_ = 0; other.elementCount_ = 0; other.buckets_.clear(); other.buckets_.resize(bucket_sizes_[0]);
    }
    umap& operator=(umap&& other) noexcept {
        if (this == &other) return *this;
        clear();
        buckets_ = std::move(other.buckets_);
        usedSize_ = other.usedSize_;
        elementCount_ = other.elementCount_;
        other.usedSize_ = 0; other.elementCount_ = 0; other.buckets_.clear(); other.buckets_.resize(bucket_sizes_[0]);
        return *this;
    }

    // 插入，如果 key 已存在则覆盖
    void insert(const K& key, const V& value) {
        rehash_if_needed();
        std::size_t idx = bucket_index(key);
        Node* curr = buckets_[idx].head;
        while (curr) {
            if (curr->key == key) { curr->value = value; return; }
            curr = curr->next;
        }
        Node* node = new Node(key, value);
        Bucket& b = buckets_[idx];
        if (!b.head) { b.head = b.tail = node; }
        else { b.tail->next = node; b.tail = node; }
        ++elementCount_;
    }


    // 访问或默认插入
    V& operator[](const K& key) {
        rehash_if_needed();
        std::size_t idx = bucket_index(key);
        Node* curr = buckets_[idx].head;
        while (curr) {
            if (curr->key == key) return curr->value;
            curr = curr->next;
        }
        // 不存在，插入默认值
        Node* node = new Node(key, V());
        Bucket& b = buckets_[idx];
        if (!b.head) { b.head = b.tail = node; }
        else { b.tail->next = node; b.tail = node; }
        ++elementCount_;
        return node->value;
    }

    bool erase(const K& key) {
        std::size_t idx = bucket_index(key);
        Node* curr = buckets_[idx].head;
        Node* prev = nullptr;
        while (curr) {
            if (curr->key == key) {
                if (prev) prev->next = curr->next; else buckets_[idx].head = curr->next;
                if (buckets_[idx].tail == curr) buckets_[idx].tail = prev;
                delete curr;
                --elementCount_;
                return true;
            }
            prev = curr; curr = curr->next;
        }
        return false;
    }

    void clear() {
        for (auto& b : buckets_) {
            Node* curr = b.head;
            while (curr) { Node* del = curr; curr = curr->next; delete del; }
            b.head = b.tail = nullptr;
        }
        elementCount_ = 0;
    }

    iterator find(const K& key) {
        std::size_t idx = bucket_index(key);
        Node* curr = buckets_[idx].head;
        while (curr) { if (curr->key == key) return iterator(&buckets_, idx, curr); curr = curr->next; }
        return end();
    }

    iterator begin() { return iterator(&buckets_, 0, buckets_.empty()? nullptr: buckets_[0].head); }
    iterator end() { return iterator(&buckets_, buckets_.size(), nullptr); }

    std::size_t size() const { return elementCount_; }
    bool empty() const { return elementCount_ == 0; }
    float load_factor() const { return buckets_.empty()? 0.f : static_cast<float>(elementCount_) / static_cast<float>(buckets_.size()); }
    std::size_t bucket_count() const { return buckets_.size(); }

private:
    std::size_t bucket_index(const K& key) const { return std::hash<K>{}(key) % buckets_.size(); }

    void rehash_if_needed() {
        if (load_factor() < 0.75f) return;
        if (usedSize_ + 1 >= bucket_sizes_count_) return; // 已无更大尺寸
        ++usedSize_;
        rehash(bucket_sizes_[usedSize_]);
    }

    void rehash(std::size_t newBucketCount) {
        std::vector<Bucket> newBuckets(newBucketCount);
        for (auto& b : buckets_) {
            Node* curr = b.head;
            while (curr) {
                Node* next = curr->next;
                std::size_t idx = std::hash<K>{}(curr->key) % newBucketCount;
                Bucket& nb = newBuckets[idx];
                curr->next = nullptr;
                if (!nb.head) { nb.head = nb.tail = curr; }
                else { nb.tail->next = curr; nb.tail = curr; }
                curr = next;
            }
            b.head = b.tail = nullptr;
        }
        buckets_.swap(newBuckets);
    }

    // 成员
    std::vector<Bucket> buckets_;
    std::size_t usedSize_;
    std::size_t elementCount_;

    inline static constexpr int bucket_sizes_[] = {53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613};
    inline static constexpr std::size_t bucket_sizes_count_ = sizeof(bucket_sizes_) / sizeof(bucket_sizes_[0]);
};

