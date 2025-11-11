#ifndef CPPFOUNDRY_LIST_HPP
#define CPPFOUNDRY_LIST_HPP

#include <cstddef>
#include <utility>
#include <initializer_list>

namespace cppfoundry {

/**
 * @brief list - Doubly-linked list container
 * 
 * Key features:
 * - Non-contiguous memory (each element in separate node)
 * - O(1) insertion and deletion at any position (if you have iterator)
 * - No random access (must traverse from beginning or end)
 * - Bidirectional iteration
 * - No reallocation needed when adding elements
 * 
 * Memory Management:
 * - Each element stored in a node with prev/next pointers
 * - Sentinel head node simplifies implementation
 * - Insert: allocate new node, update pointers
 * - Erase: update pointers, delete node
 * - Destructor: traverse and delete all nodes
 * 
 * Node structure:
 * [prev] <- [data] -> [next]
 *    |                   |
 *    +-------------------+
 * 
 * List structure with sentinel:
 * head <-> node1 <-> node2 <-> ... <-> nodeN <-> head
 *  ^                                              |
 *  +----------------------------------------------+
 */
template<typename T>
class list {
private:
    /**
     * @brief Node structure
     * 
     * Each node contains:
     * - data: the actual element value
     * - prev: pointer to previous node
     * - next: pointer to next node
     */
    struct Node {
        T data;
        Node* prev;
        Node* next;
        
        // Constructor for value nodes
        Node(const T& value, Node* p = nullptr, Node* n = nullptr)
            : data(value), prev(p), next(n) {}
        
        Node(T&& value, Node* p = nullptr, Node* n = nullptr)
            : data(std::move(value)), prev(p), next(n) {}
        
        // Constructor for sentinel node (no data)
        Node() : prev(this), next(this) {}
    };
    
public:
    using value_type = T;
    using size_type = std::size_t;
    using reference = T&;
    using const_reference = const T&;
    
    /**
     * @brief Iterator class for list
     * 
     * Provides bidirectional iteration through the list.
     * Wraps a node pointer and provides increment/decrement operations.
     */
    class iterator {
    private:
        Node* node_;
        friend class list;
        
    public:
        iterator(Node* node = nullptr) : node_(node) {}
        
        reference operator*() const {
            return node_->data;
        }
        
        T* operator->() const {
            return &(node_->data);
        }
        
        // Pre-increment: move to next node
        iterator& operator++() {
            node_ = node_->next;
            return *this;
        }
        
        // Post-increment
        iterator operator++(int) {
            iterator temp = *this;
            node_ = node_->next;
            return temp;
        }
        
        // Pre-decrement: move to previous node
        iterator& operator--() {
            node_ = node_->prev;
            return *this;
        }
        
        // Post-decrement
        iterator operator--(int) {
            iterator temp = *this;
            node_ = node_->prev;
            return temp;
        }
        
        bool operator==(const iterator& other) const {
            return node_ == other.node_;
        }
        
        bool operator!=(const iterator& other) const {
            return node_ != other.node_;
        }
    };
    
    /**
     * @brief Const iterator class
     */
    class const_iterator {
    private:
        const Node* node_;
        friend class list;
        
    public:
        const_iterator(const Node* node = nullptr) : node_(node) {}
        const_iterator(const iterator& it) : node_(it.node_) {}
        
        const_reference operator*() const {
            return node_->data;
        }
        
        const T* operator->() const {
            return &(node_->data);
        }
        
        const_iterator& operator++() {
            node_ = node_->next;
            return *this;
        }
        
        const_iterator operator++(int) {
            const_iterator temp = *this;
            node_ = node_->next;
            return temp;
        }
        
        const_iterator& operator--() {
            node_ = node_->prev;
            return *this;
        }
        
        const_iterator operator--(int) {
            const_iterator temp = *this;
            node_ = node_->prev;
            return temp;
        }
        
        bool operator==(const const_iterator& other) const {
            return node_ == other.node_;
        }
        
        bool operator!=(const const_iterator& other) const {
            return node_ != other.node_;
        }
    };
    
private:
    Node* head_;        // Sentinel node (circular, points to itself when empty)
    size_type size_;    // Number of elements
    
    /**
     * @brief Initialize the list
     * 
     * Creates the sentinel head node.
     * In an empty list, head->next and head->prev both point to head.
     */
    void init() {
        head_ = new Node();  // Sentinel node
        size_ = 0;
    }
    
    /**
     * @brief Insert node before position
     * @param pos Position to insert before
     * @param node Node to insert
     * 
     * Updates the pointers:
     * 1. Set node's prev to pos's prev
     * 2. Set node's next to pos
     * 3. Update pos's prev->next to point to node
     * 4. Update pos's prev to point to node
     */
    void insert_node(Node* pos, Node* node) {
        node->prev = pos->prev;
        node->next = pos;
        pos->prev->next = node;
        pos->prev = node;
        ++size_;
    }
    
    /**
     * @brief Remove and return node at position
     * @param pos Position to remove
     * @return Pointer to removed node
     * 
     * Updates the pointers to skip over the node:
     * 1. Set pos's prev->next to pos's next
     * 2. Set pos's next->prev to pos's prev
     * Caller is responsible for deleting the returned node.
     */
    Node* remove_node(Node* pos) {
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        --size_;
        return pos;
    }
    
public:
    // ========== Constructors ==========
    
    /**
     * @brief Default constructor - creates empty list
     */
    list() {
        init();
    }
    
    /**
     * @brief Construct with count copies of value
     */
    list(size_type count, const T& value) {
        init();
        for (size_type i = 0; i < count; ++i) {
            push_back(value);
        }
    }
    
    /**
     * @brief Construct from initializer list
     * 
     * Example: list<int> l = {1, 2, 3, 4, 5};
     */
    list(std::initializer_list<T> init_list) {
        init();
        for (const auto& item : init_list) {
            push_back(item);
        }
    }
    
    /**
     * @brief Copy constructor
     * @param other The list to copy from
     * 
     * Deep copy: creates new nodes for all elements.
     */
    list(const list& other) {
        init();
        for (const auto& item : other) {
            push_back(item);
        }
    }
    
    /**
     * @brief Move constructor
     * @param other The list to move from
     * 
     * Steals the internal structure from 'other'.
     * 'other' is left in valid but empty state.
     */
    list(list&& other) noexcept : head_(other.head_), size_(other.size_) {
        other.head_ = nullptr;
        other.size_ = 0;
        other.init();
    }
    
    /**
     * @brief Destructor
     * 
     * Traverses the list and deletes all nodes.
     * Must delete each node individually (no array delete).
     */
    ~list() {
        clear();
        delete head_;
    }
    
    // ========== Assignment operators ==========
    
    /**
     * @brief Copy assignment
     */
    list& operator=(const list& other) {
        if (this != &other) {
            clear();
            for (const auto& item : other) {
                push_back(item);
            }
        }
        return *this;
    }
    
    /**
     * @brief Move assignment
     */
    list& operator=(list&& other) noexcept {
        if (this != &other) {
            clear();
            delete head_;
            head_ = other.head_;
            size_ = other.size_;
            other.head_ = nullptr;
            other.size_ = 0;
            other.init();
        }
        return *this;
    }
    
    /**
     * @brief Assign from initializer list
     */
    list& operator=(std::initializer_list<T> init_list) {
        clear();
        for (const auto& item : init_list) {
            push_back(item);
        }
        return *this;
    }
    
    // ========== Element access ==========
    
    /**
     * @brief Access first element
     */
    reference front() {
        return head_->next->data;
    }
    
    const_reference front() const {
        return head_->next->data;
    }
    
    /**
     * @brief Access last element
     */
    reference back() {
        return head_->prev->data;
    }
    
    const_reference back() const {
        return head_->prev->data;
    }
    
    // ========== Iterators ==========
    
    iterator begin() noexcept {
        return iterator(head_->next);
    }
    
    const_iterator begin() const noexcept {
        return const_iterator(head_->next);
    }
    
    iterator end() noexcept {
        return iterator(head_);
    }
    
    const_iterator end() const noexcept {
        return const_iterator(head_);
    }
    
    // ========== Capacity ==========
    
    /**
     * @brief Check if list is empty
     */
    bool empty() const noexcept {
        return size_ == 0;
    }
    
    /**
     * @brief Get number of elements
     */
    size_type size() const noexcept {
        return size_;
    }
    
    // ========== Modifiers ==========
    
    /**
     * @brief Clear all elements
     * 
     * Traverses the list and deletes all nodes except the sentinel.
     */
    void clear() noexcept {
        Node* current = head_->next;
        while (current != head_) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head_->next = head_;
        head_->prev = head_;
        size_ = 0;
    }
    
    /**
     * @brief Insert element before position
     * @param pos Position to insert before
     * @param value Value to insert
     * @return Iterator to inserted element
     * 
     * O(1) operation - just allocates node and updates pointers.
     */
    iterator insert(iterator pos, const T& value) {
        Node* new_node = new Node(value);
        insert_node(pos.node_, new_node);
        return iterator(new_node);
    }
    
    iterator insert(iterator pos, T&& value) {
        Node* new_node = new Node(std::move(value));
        insert_node(pos.node_, new_node);
        return iterator(new_node);
    }
    
    /**
     * @brief Erase element at position
     * @param pos Position to erase
     * @return Iterator to element after erased element
     * 
     * O(1) operation - just updates pointers and deletes node.
     */
    iterator erase(iterator pos) {
        Node* next = pos.node_->next;
        Node* removed = remove_node(pos.node_);
        delete removed;
        return iterator(next);
    }
    
    /**
     * @brief Add element to front
     * 
     * O(1) operation.
     */
    void push_front(const T& value) {
        insert(begin(), value);
    }
    
    void push_front(T&& value) {
        insert(begin(), std::move(value));
    }
    
    /**
     * @brief Add element to back
     * 
     * O(1) operation.
     */
    void push_back(const T& value) {
        insert(end(), value);
    }
    
    void push_back(T&& value) {
        insert(end(), std::move(value));
    }
    
    /**
     * @brief Remove first element
     */
    void pop_front() {
        if (!empty()) {
            erase(begin());
        }
    }
    
    /**
     * @brief Remove last element
     */
    void pop_back() {
        if (!empty()) {
            erase(--end());
        }
    }
    
    /**
     * @brief Construct element in-place before position
     */
    template<typename... Args>
    iterator emplace(iterator pos, Args&&... args) {
        Node* new_node = new Node(T(std::forward<Args>(args)...));
        insert_node(pos.node_, new_node);
        return iterator(new_node);
    }
    
    /**
     * @brief Construct element in-place at front
     */
    template<typename... Args>
    void emplace_front(Args&&... args) {
        emplace(begin(), std::forward<Args>(args)...);
    }
    
    /**
     * @brief Construct element in-place at back
     */
    template<typename... Args>
    void emplace_back(Args&&... args) {
        emplace(end(), std::forward<Args>(args)...);
    }
    
    /**
     * @brief Swap with another list
     * 
     * O(1) operation - just swaps pointers and size.
     */
    void swap(list& other) noexcept {
        std::swap(head_, other.head_);
        std::swap(size_, other.size_);
    }
};

// ========== Non-member functions ==========

template<typename T>
bool operator==(const list<T>& lhs, const list<T>& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    auto it1 = lhs.begin();
    auto it2 = rhs.begin();
    while (it1 != lhs.end()) {
        if (*it1 != *it2) {
            return false;
        }
        ++it1;
        ++it2;
    }
    return true;
}

template<typename T>
bool operator!=(const list<T>& lhs, const list<T>& rhs) {
    return !(lhs == rhs);
}

template<typename T>
void swap(list<T>& lhs, list<T>& rhs) noexcept {
    lhs.swap(rhs);
}

} // namespace cppfoundry

#endif // CPPFOUNDRY_LIST_HPP
