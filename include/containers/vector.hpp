#ifndef CPPFOUNDRY_VECTOR_HPP
#define CPPFOUNDRY_VECTOR_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <initializer_list>
#include <algorithm>

namespace cppfoundry {

/**
 * @brief vector - Dynamic array container
 * 
 * Key features:
 * - Dynamic size with automatic resizing
 * - Contiguous memory storage (cache-friendly)
 * - Random access with O(1) complexity
 * - Amortized O(1) push_back
 * - Automatic memory management
 * 
 * Memory Management:
 * - Three key values: data pointer, size, capacity
 * - Size: number of elements currently stored
 * - Capacity: total allocated space
 * - When size == capacity, reallocate with larger capacity
 * - Growth strategy: typically double the capacity (geometric growth)
 * 
 * Implementation details:
 * - Uses raw pointer + new/delete for memory management
 * - Reallocation involves: allocate new memory, move/copy elements, deallocate old
 * - Destructor calls destructors for all elements and frees memory
 */
template<typename T>
class vector {
public:
    using value_type = T;
    using size_type = std::size_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    
    // Simple iterator (just a pointer wrapper)
    using iterator = T*;
    using const_iterator = const T*;
    
private:
    T* data_;           // Pointer to the allocated array
    size_type size_;    // Number of elements currently stored
    size_type capacity_; // Total allocated capacity
    
    /**
     * @brief Reallocate memory with new capacity
     * @param new_capacity The new capacity to allocate
     * 
     * Process:
     * 1. Allocate new memory block
     * 2. Move/copy existing elements to new location
     * 3. Destroy elements in old location
     * 4. Deallocate old memory
     * 5. Update pointers and capacity
     */
    void reallocate(size_type new_capacity) {
        // Allocate new memory
        T* new_data = new T[new_capacity];
        
        // Move existing elements to new location
        // Use move if available, otherwise copy
        for (size_type i = 0; i < size_; ++i) {
            new_data[i] = std::move(data_[i]);
        }
        
        // Deallocate old memory
        delete[] data_;
        
        // Update to new memory
        data_ = new_data;
        capacity_ = new_capacity;
    }
    
public:
    // ========== Constructors ==========
    
    /**
     * @brief Default constructor - creates empty vector
     */
    vector() : data_(nullptr), size_(0), capacity_(0) {}
    
    /**
     * @brief Construct with size
     * @param count Number of elements to create
     * 
     * Creates 'count' default-initialized elements.
     */
    explicit vector(size_type count) 
        : data_(count > 0 ? new T[count] : nullptr), 
          size_(count), 
          capacity_(count) {
        // Elements are default-initialized
    }
    
    /**
     * @brief Construct with size and value
     * @param count Number of elements
     * @param value Value to initialize each element with
     */
    vector(size_type count, const T& value) 
        : data_(count > 0 ? new T[count] : nullptr), 
          size_(count), 
          capacity_(count) {
        for (size_type i = 0; i < size_; ++i) {
            data_[i] = value;
        }
    }
    
    /**
     * @brief Construct from initializer list
     * @param init Initializer list
     * 
     * Example: vector<int> v = {1, 2, 3, 4, 5};
     */
    vector(std::initializer_list<T> init) 
        : data_(init.size() > 0 ? new T[init.size()] : nullptr),
          size_(init.size()),
          capacity_(init.size()) {
        size_type i = 0;
        for (const auto& item : init) {
            data_[i++] = item;
        }
    }
    
    /**
     * @brief Copy constructor
     * @param other The vector to copy from
     * 
     * Deep copy: allocates new memory and copies all elements.
     */
    vector(const vector& other) 
        : data_(other.size_ > 0 ? new T[other.size_] : nullptr),
          size_(other.size_),
          capacity_(other.size_) {
        for (size_type i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }
    
    /**
     * @brief Move constructor
     * @param other The vector to move from
     * 
     * Transfers ownership of memory from 'other'.
     * 'other' is left in valid but empty state.
     */
    vector(vector&& other) noexcept 
        : data_(other.data_), 
          size_(other.size_), 
          capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    
    /**
     * @brief Destructor
     * 
     * Destroys all elements and frees allocated memory.
     * This is automatic cleanup - core of RAII.
     */
    ~vector() {
        delete[] data_;
    }
    
    // ========== Assignment operators ==========
    
    /**
     * @brief Copy assignment
     * @param other The vector to copy from
     * 
     * Uses copy-and-swap idiom for exception safety.
     */
    vector& operator=(const vector& other) {
        if (this != &other) {
            vector temp(other);  // Copy
            swap(temp);          // Swap
        }
        return *this;
    }
    
    /**
     * @brief Move assignment
     * @param other The vector to move from
     */
    vector& operator=(vector&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }
    
    /**
     * @brief Assign from initializer list
     */
    vector& operator=(std::initializer_list<T> init) {
        vector temp(init);
        swap(temp);
        return *this;
    }
    
    // ========== Element access ==========
    
    /**
     * @brief Access element with bounds checking
     * @param index Index of element
     * @throws std::out_of_range if index >= size
     */
    reference at(size_type index) {
        if (index >= size_) {
            throw std::out_of_range("vector::at: index out of range");
        }
        return data_[index];
    }
    
    const_reference at(size_type index) const {
        if (index >= size_) {
            throw std::out_of_range("vector::at: index out of range");
        }
        return data_[index];
    }
    
    /**
     * @brief Access element without bounds checking
     * @param index Index of element
     * 
     * Undefined behavior if index >= size.
     * Prefer at() if you need bounds checking.
     */
    reference operator[](size_type index) {
        return data_[index];
    }
    
    const_reference operator[](size_type index) const {
        return data_[index];
    }
    
    /**
     * @brief Access first element
     */
    reference front() {
        return data_[0];
    }
    
    const_reference front() const {
        return data_[0];
    }
    
    /**
     * @brief Access last element
     */
    reference back() {
        return data_[size_ - 1];
    }
    
    const_reference back() const {
        return data_[size_ - 1];
    }
    
    /**
     * @brief Get pointer to underlying array
     */
    T* data() noexcept {
        return data_;
    }
    
    const T* data() const noexcept {
        return data_;
    }
    
    // ========== Iterators ==========
    
    iterator begin() noexcept {
        return data_;
    }
    
    const_iterator begin() const noexcept {
        return data_;
    }
    
    iterator end() noexcept {
        return data_ + size_;
    }
    
    const_iterator end() const noexcept {
        return data_ + size_;
    }
    
    // ========== Capacity ==========
    
    /**
     * @brief Check if vector is empty
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
    
    /**
     * @brief Get allocated capacity
     */
    size_type capacity() const noexcept {
        return capacity_;
    }
    
    /**
     * @brief Reserve capacity
     * @param new_capacity Minimum capacity to reserve
     * 
     * If new_capacity > capacity, reallocates with new capacity.
     * Does not change size or initialize new elements.
     * Used to avoid multiple reallocations when you know size in advance.
     */
    void reserve(size_type new_capacity) {
        if (new_capacity > capacity_) {
            reallocate(new_capacity);
        }
    }
    
    /**
     * @brief Reduce capacity to fit size
     * 
     * Reallocates to capacity = size, freeing unused memory.
     */
    void shrink_to_fit() {
        if (capacity_ > size_) {
            if (size_ == 0) {
                delete[] data_;
                data_ = nullptr;
                capacity_ = 0;
            } else {
                reallocate(size_);
            }
        }
    }
    
    // ========== Modifiers ==========
    
    /**
     * @brief Clear all elements
     * 
     * Destroys all elements and sets size to 0.
     * Does not change capacity.
     */
    void clear() noexcept {
        size_ = 0;
        // Note: In a real implementation, we'd call destructors here
        // For simple types, just resetting size_ is sufficient
    }
    
    /**
     * @brief Add element to end
     * @param value Element to add
     * 
     * Amortized O(1) complexity due to geometric growth strategy.
     * 
     * Process:
     * 1. Check if size == capacity
     * 2. If yes, reallocate with larger capacity (typically 2x)
     * 3. Add element at end
     * 4. Increment size
     */
    void push_back(const T& value) {
        if (size_ == capacity_) {
            // Geometric growth: double capacity (or start with 1)
            size_type new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            reallocate(new_capacity);
        }
        data_[size_++] = value;
    }
    
    /**
     * @brief Add element to end (move version)
     */
    void push_back(T&& value) {
        if (size_ == capacity_) {
            size_type new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            reallocate(new_capacity);
        }
        data_[size_++] = std::move(value);
    }
    
    /**
     * @brief Construct element in-place at end
     * @param args Arguments to forward to T's constructor
     * 
     * Avoids unnecessary copies/moves by constructing directly in vector.
     */
    template<typename... Args>
    void emplace_back(Args&&... args) {
        if (size_ == capacity_) {
            size_type new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            reallocate(new_capacity);
        }
        // In a full implementation, we'd use placement new here
        // For simplicity, we'll just use assignment
        data_[size_++] = T(std::forward<Args>(args)...);
    }
    
    /**
     * @brief Remove last element
     * 
     * Undefined behavior if vector is empty.
     */
    void pop_back() {
        if (size_ > 0) {
            --size_;
            // In real implementation, call destructor here
        }
    }
    
    /**
     * @brief Resize vector
     * @param new_size New size
     * 
     * If new_size > size, adds default-initialized elements.
     * If new_size < size, removes elements from end.
     */
    void resize(size_type new_size) {
        if (new_size > capacity_) {
            reserve(new_size);
        }
        
        if (new_size > size_) {
            // Default-initialize new elements
            for (size_type i = size_; i < new_size; ++i) {
                data_[i] = T();
            }
        }
        size_ = new_size;
    }
    
    /**
     * @brief Resize with value
     * @param new_size New size
     * @param value Value to initialize new elements with
     */
    void resize(size_type new_size, const T& value) {
        if (new_size > capacity_) {
            reserve(new_size);
        }
        
        if (new_size > size_) {
            for (size_type i = size_; i < new_size; ++i) {
                data_[i] = value;
            }
        }
        size_ = new_size;
    }
    
    /**
     * @brief Swap with another vector
     */
    void swap(vector& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }
};

// ========== Non-member functions ==========

template<typename T>
bool operator==(const vector<T>& lhs, const vector<T>& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    for (typename vector<T>::size_type i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

template<typename T>
bool operator!=(const vector<T>& lhs, const vector<T>& rhs) {
    return !(lhs == rhs);
}

template<typename T>
void swap(vector<T>& lhs, vector<T>& rhs) noexcept {
    lhs.swap(rhs);
}

} // namespace cppfoundry

#endif // CPPFOUNDRY_VECTOR_HPP
