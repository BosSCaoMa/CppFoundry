#ifndef CPPFOUNDRY_SHARED_PTR_HPP
#define CPPFOUNDRY_SHARED_PTR_HPP

#include <cstddef>
#include <utility>
#include <atomic>

namespace cppfoundry {

/**
 * @brief Control block for reference counting
 * 
 * This is the heart of shared_ptr's memory management. It tracks:
 * - Reference count (number of shared_ptr instances)
 * - Weak reference count (number of weak_ptr instances)
 * - The managed pointer
 * - The deleter to use for cleanup
 * 
 * Memory layout:
 * [Control Block][Managed Object] - when using make_shared
 * or
 * [Control Block] -> [Managed Object] - when constructing from raw pointer
 */
template<typename T>
class control_block {
public:
    using counter_type = long;
    
private:
    T* ptr_;                      // Pointer to the managed object
    counter_type shared_count_;   // Number of shared_ptr instances
    counter_type weak_count_;     // Number of weak_ptr instances
    
public:
    /**
     * @brief Construct control block
     * @param ptr The pointer to manage
     * 
     * Initial counts:
     * - shared_count = 1 (the first shared_ptr)
     * - weak_count = 0 (no weak_ptr yet)
     */
    explicit control_block(T* ptr) 
        : ptr_(ptr), shared_count_(1), weak_count_(0) {}
    
    virtual ~control_block() = default;
    
    /**
     * @brief Get the managed pointer
     */
    T* get_ptr() const noexcept {
        return ptr_;
    }
    
    /**
     * @brief Increment shared reference count
     * 
     * Called when a new shared_ptr is created (copy constructor or assignment)
     */
    void add_shared_ref() noexcept {
        ++shared_count_;
    }
    
    /**
     * @brief Decrement shared reference count
     * 
     * Called when a shared_ptr is destroyed.
     * If count reaches 0, the managed object is deleted.
     */
    void release_shared_ref() noexcept {
        if (--shared_count_ == 0) {
            // No more shared owners - delete the object
            destroy_object();
            
            // If no weak pointers exist, delete the control block itself
            if (weak_count_ == 0) {
                delete this;
            }
        }
    }
    
    /**
     * @brief Increment weak reference count
     */
    void add_weak_ref() noexcept {
        ++weak_count_;
    }
    
    /**
     * @brief Decrement weak reference count
     */
    void release_weak_ref() noexcept {
        if (--weak_count_ == 0 && shared_count_ == 0) {
            // No shared or weak pointers - delete control block
            delete this;
        }
    }
    
    /**
     * @brief Get current shared reference count
     */
    counter_type use_count() const noexcept {
        return shared_count_;
    }
    
    /**
     * @brief Get current weak reference count
     */
    counter_type weak_count() const noexcept {
        return weak_count_;
    }
    
protected:
    /**
     * @brief Destroy the managed object
     * 
     * Virtual to allow custom deleters in derived classes.
     * Default implementation uses delete.
     */
    virtual void destroy_object() {
        delete ptr_;
        ptr_ = nullptr;
    }
};

/**
 * @brief Control block with custom deleter
 * 
 * Allows using a custom deleter function when the object is destroyed.
 */
template<typename T, typename Deleter>
class control_block_with_deleter : public control_block<T> {
private:
    Deleter deleter_;
    
public:
    control_block_with_deleter(T* ptr, Deleter d) 
        : control_block<T>(ptr), deleter_(d) {}
    
protected:
    void destroy_object() override {
        deleter_(this->get_ptr());
    }
};

/**
 * @brief shared_ptr - Smart pointer with shared ownership semantics
 * 
 * Key features:
 * - Multiple shared_ptr instances can own the same object
 * - Uses reference counting to track number of owners
 * - Object is deleted when the last shared_ptr is destroyed
 * - Thread-safe reference counting (in real implementation)
 * - Supports custom deleters
 * - Can be copied and moved
 * 
 * Memory Management:
 * - Constructor: Creates or increments reference count
 * - Copy: Increments reference count
 * - Destructor: Decrements reference count, deletes if count reaches 0
 * - Move: Transfers ownership without changing reference count
 * 
 * Implementation uses a control block pattern:
 * - Control block stores the pointer and reference counts
 * - All shared_ptr instances point to the same control block
 * - Control block is deleted when both shared and weak counts reach 0
 */
template<typename T>
class shared_ptr {
public:
    using element_type = T;
    
private:
    T* ptr_;                        // Pointer to the managed object
    control_block<T>* control_;     // Pointer to the control block
    
public:
    // ========== Constructors ==========
    
    /**
     * @brief Default constructor - creates empty shared_ptr
     */
    constexpr shared_ptr() noexcept : ptr_(nullptr), control_(nullptr) {}
    
    /**
     * @brief nullptr constructor
     */
    constexpr shared_ptr(std::nullptr_t) noexcept : ptr_(nullptr), control_(nullptr) {}
    
    /**
     * @brief Construct from raw pointer
     * @param ptr Raw pointer to manage
     * 
     * Creates a new control block with reference count = 1.
     * Takes ownership of the pointer.
     */
    explicit shared_ptr(T* ptr) : ptr_(ptr), control_(nullptr) {
        if (ptr) {
            try {
                control_ = new control_block<T>(ptr);
            } catch (...) {
                // If control block allocation fails, delete the object
                delete ptr;
                throw;
            }
        }
    }
    
    /**
     * @brief Construct with custom deleter
     * @param ptr Raw pointer to manage
     * @param deleter Custom deleter to use
     */
    template<typename Deleter>
    shared_ptr(T* ptr, Deleter deleter) : ptr_(ptr), control_(nullptr) {
        if (ptr) {
            try {
                control_ = new control_block_with_deleter<T, Deleter>(ptr, deleter);
            } catch (...) {
                deleter(ptr);
                throw;
            }
        }
    }
    
    /**
     * @brief Copy constructor
     * @param other The shared_ptr to copy from
     * 
     * Shares ownership with 'other'.
     * Increments the reference count.
     */
    shared_ptr(const shared_ptr& other) noexcept 
        : ptr_(other.ptr_), control_(other.control_) {
        if (control_) {
            control_->add_shared_ref();
        }
    }
    
    /**
     * @brief Converting copy constructor
     * 
     * Allows copying from shared_ptr<U> to shared_ptr<T> when U* converts to T*
     */
    template<typename U>
    shared_ptr(const shared_ptr<U>& other) noexcept 
        : ptr_(other.ptr_), control_(reinterpret_cast<control_block<T>*>(other.control_)) {
        if (control_) {
            control_->add_shared_ref();
        }
    }
    
    /**
     * @brief Move constructor
     * @param other The shared_ptr to move from
     * 
     * Transfers ownership from 'other'.
     * Does not change reference count (just transfers the reference).
     */
    shared_ptr(shared_ptr&& other) noexcept 
        : ptr_(other.ptr_), control_(other.control_) {
        other.ptr_ = nullptr;
        other.control_ = nullptr;
    }
    
    /**
     * @brief Converting move constructor
     */
    template<typename U>
    shared_ptr(shared_ptr<U>&& other) noexcept 
        : ptr_(other.ptr_), control_(reinterpret_cast<control_block<T>*>(other.control_)) {
        other.ptr_ = nullptr;
        other.control_ = nullptr;
    }
    
    /**
     * @brief Destructor
     * 
     * Decrements reference count.
     * If count reaches 0, deletes the managed object and control block.
     */
    ~shared_ptr() {
        if (control_) {
            control_->release_shared_ref();
        }
    }
    
    // ========== Assignment operators ==========
    
    /**
     * @brief Copy assignment
     * @param other The shared_ptr to copy from
     * 
     * Releases current ownership and shares ownership with 'other'.
     */
    shared_ptr& operator=(const shared_ptr& other) noexcept {
        if (this != &other) {
            // Create a temporary and swap (copy-and-swap idiom)
            shared_ptr(other).swap(*this);
        }
        return *this;
    }
    
    /**
     * @brief Converting copy assignment
     */
    template<typename U>
    shared_ptr& operator=(const shared_ptr<U>& other) noexcept {
        shared_ptr(other).swap(*this);
        return *this;
    }
    
    /**
     * @brief Move assignment
     * @param other The shared_ptr to move from
     */
    shared_ptr& operator=(shared_ptr&& other) noexcept {
        shared_ptr(std::move(other)).swap(*this);
        return *this;
    }
    
    /**
     * @brief Converting move assignment
     */
    template<typename U>
    shared_ptr& operator=(shared_ptr<U>&& other) noexcept {
        shared_ptr(std::move(other)).swap(*this);
        return *this;
    }
    
    /**
     * @brief Assign nullptr - releases ownership
     */
    shared_ptr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }
    
    // ========== Modifiers ==========
    
    /**
     * @brief Replace managed object
     * @param ptr New pointer to manage (default is nullptr)
     * 
     * Releases current ownership and takes ownership of 'ptr'.
     */
    void reset(T* ptr = nullptr) {
        shared_ptr(ptr).swap(*this);
    }
    
    /**
     * @brief Replace managed object with custom deleter
     */
    template<typename Deleter>
    void reset(T* ptr, Deleter deleter) {
        shared_ptr(ptr, deleter).swap(*this);
    }
    
    /**
     * @brief Swap with another shared_ptr
     */
    void swap(shared_ptr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(control_, other.control_);
    }
    
    // ========== Observers ==========
    
    /**
     * @brief Get the raw pointer
     */
    T* get() const noexcept {
        return ptr_;
    }
    
    /**
     * @brief Dereference operator
     */
    T& operator*() const noexcept {
        return *ptr_;
    }
    
    /**
     * @brief Member access operator
     */
    T* operator->() const noexcept {
        return ptr_;
    }
    
    /**
     * @brief Get reference count
     * @return Number of shared_ptr instances managing the object
     * 
     * Returns 0 if this is an empty shared_ptr.
     */
    long use_count() const noexcept {
        return control_ ? control_->use_count() : 0;
    }
    
    /**
     * @brief Check if this is the only owner
     */
    bool unique() const noexcept {
        return use_count() == 1;
    }
    
    /**
     * @brief Check if shared_ptr owns an object
     */
    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }
    
    // Allow access from converting constructors
    template<typename U>
    friend class shared_ptr;
};

// ========== Helper functions ==========

/**
 * @brief Create a shared_ptr (C++11 style make_shared)
 * @param args Arguments to forward to T's constructor
 * 
 * Advantages over shared_ptr<T>(new T(...)):
 * - Single memory allocation for control block and object
 * - More cache-friendly (better locality)
 * - Exception safe
 * - More efficient
 * 
 * Note: This simplified implementation doesn't actually do the
 * single-allocation optimization, but shows the interface.
 */
template<typename T, typename... Args>
shared_ptr<T> make_shared(Args&&... args) {
    return shared_ptr<T>(new T(std::forward<Args>(args)...));
}

// ========== Comparison operators ==========

template<typename T, typename U>
bool operator==(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) noexcept {
    return lhs.get() == rhs.get();
}

template<typename T, typename U>
bool operator!=(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) noexcept {
    return !(lhs == rhs);
}

template<typename T>
bool operator==(const shared_ptr<T>& lhs, std::nullptr_t) noexcept {
    return !lhs;
}

template<typename T>
bool operator==(std::nullptr_t, const shared_ptr<T>& rhs) noexcept {
    return !rhs;
}

template<typename T>
bool operator!=(const shared_ptr<T>& lhs, std::nullptr_t) noexcept {
    return static_cast<bool>(lhs);
}

template<typename T>
bool operator!=(std::nullptr_t, const shared_ptr<T>& rhs) noexcept {
    return static_cast<bool>(rhs);
}

} // namespace cppfoundry

#endif // CPPFOUNDRY_SHARED_PTR_HPP
