#pragma once
#include <utility>
#include <stdexcept>

// 简易二叉搜索树实现 map，接口类似 std::map

template<typename K, typename V>
class rbmap {
    struct Node {
        K key;
        V value;
        Node* left = nullptr;
        Node* right = nullptr;
        bool color = true; // true: RED, false: BLACK
        Node(const K& k, const V& v, bool c = true) : key(k), value(v), color(c) {}
    };
    Node* root = nullptr;
    size_t nodeCount = 0;
    static bool isRed(Node* node) { return node && node->color; }
    static Node* rotateLeft(Node* h) {
        Node* x = h->right;
        h->right = x->left;
        x->left = h;
        x->color = h->color;
        h->color = true;
        return x;
    }
    static Node* rotateRight(Node* h) {
        Node* x = h->left;
        h->left = x->right;
        x->right = h;
        x->color = h->color;
        h->color = true;
        return x;
    }
    static void flipColors(Node* h) {
        h->color = true;
        if (h->left) h->left->color = false;
        if (h->right) h->right->color = false;
    }
public:
    rbmap() = default;
    ~rbmap() { clear(root); }
    // 禁止拷贝
    rbmap(const rbmap&) = delete;
    rbmap& operator=(const rbmap&) = delete;
    // 插入或更新
    void insert(const K& key, const V& value) {
        root = insert(root, key, value);
        if (root) root->color = false;
    }
    // 下标访问，自动插入默认值
    V& operator[](const K& key) {
        Node* n = findNode(root, key);
        if (n) return n->value;
        insert(key, V());
        return findNode(root, key)->value;
    }
    // 查找
    V* find(const K& key) {
        Node* n = findNode(root, key);
        return n ? &n->value : nullptr;
    }
    // 删除
    void erase(const K& key) {
        if (!findNode(root, key)) return;
        if (!isRed(root->left) && !isRed(root->right)) root->color = true;
        root = erase(root, key);
        if (root) root->color = false;
    }
    // 大小
    size_t size() const { return nodeCount; }
    bool empty() const { return nodeCount == 0; }
private:
    Node* insert(Node* h, const K& key, const V& value) {
        if (!h) {
            ++nodeCount;
            return new Node(key, value, true);
        }
        if (key < h->key) h->left = insert(h->left, key, value);
        else if (key > h->key) h->right = insert(h->right, key, value);
        else h->value = value;
        if (isRed(h->right) && !isRed(h->left)) h = rotateLeft(h);
        if (isRed(h->left) && isRed(h->left->left)) h = rotateRight(h);
        if (isRed(h->left) && isRed(h->right)) flipColors(h);
        return h;
    }
    Node* findNode(Node* node, const K& key) const {
        while (node) {
            if (key < node->key) node = node->left;
            else if (key > node->key) node = node->right;
            else return node;
        }
        return nullptr;
    }
    Node* erase(Node* h, const K& key) {
        if (!h) return nullptr;
        if (key < h->key) {
            if (!isRed(h->left) && h->left && !isRed(h->left->left))
                h = moveRedLeft(h);
            h->left = erase(h->left, key);
        } else {
            if (isRed(h->left)) h = rotateRight(h);
            if (key == h->key && !h->right) {
                delete h;
                --nodeCount;
                return nullptr;
            }
            if (!isRed(h->right) && h->right && !isRed(h->right->left))
                h = moveRedRight(h);
            if (key == h->key) {
                Node* min = minNode(h->right);
                h->key = min->key;
                h->value = min->value;
                h->right = eraseMin(h->right);
                --nodeCount;
            } else h->right = erase(h->right, key);
        }
        return fixUp(h);
    }
    Node* minNode(Node* h) const {
        while (h && h->left) h = h->left;
        return h;
    }
    Node* eraseMin(Node* h) {
        if (!h->left) {
            delete h;
            return nullptr;
        }
        if (!isRed(h->left) && h->left && !isRed(h->left->left))
            h = moveRedLeft(h);
        h->left = eraseMin(h->left);
        return fixUp(h);
    }
    Node* moveRedLeft(Node* h) {
        flipColors(h);
        if (h->right && isRed(h->right->left)) {
            h->right = rotateRight(h->right);
            h = rotateLeft(h);
            flipColors(h);
        }
        return h;
    }
    Node* moveRedRight(Node* h) {
        flipColors(h);
        if (h->left && isRed(h->left->left)) {
            h = rotateRight(h);
            flipColors(h);
        }
        return h;
    }
    Node* fixUp(Node* h) {
        if (isRed(h->right)) h = rotateLeft(h);
        if (isRed(h->left) && isRed(h->left->left)) h = rotateRight(h);
        if (isRed(h->left) && isRed(h->right)) flipColors(h);
        return h;
    }
    void clear(Node* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }
};
