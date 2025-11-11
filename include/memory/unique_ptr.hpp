#ifndef CPPFOUNDRY_UNIQUE_PTR_HPP
#define CPPFOUNDRY_UNIQUE_PTR_HPP

#include <cstddef>      // for std::size_t
#include <utility>      // for std::move, std::forward
#include <type_traits>  // for std::enable_if, std::is_array, etc.

namespace cppfoundry {

/**
 * @brief Default deleter for unique_ptr
 * 
 * This is a function object that calls delete on the pointer.
 * It's templated to work with any type.
 */
template<typename T>
struct default_delete {
    // Default constructor
    constexpr default_delete() noexcept = default;
    
    // Allow conversion from default_delete<U> to default_delete<T> when U* converts to T*
    template<typename U>
    default_delete(const default_delete<U>&) noexcept {}
    
    // Call operator that performs the deletion
    void operator()(T* ptr) const {
        delete ptr;
    }
};

/**
 * @brief Specialization for arrays
 * 
 * Uses delete[] instead of delete for array types.
 */
template<typename T>
struct default_delete<T[]> {
    constexpr default_delete() noexcept = default;
    
    template<typename U>
    default_delete(const default_delete<U[]>&) noexcept {}
    
    void operator()(T* ptr) const {
        delete[] ptr;
    }
};

/**
 * @brief unique_ptr - Smart pointer with unique ownership semantics
 * 
 * Key features:
 * - Owns and manages an object through a pointer
 * - Disposes of that object when unique_ptr goes out of scope
 * - No two unique_ptr instances can manage the same object (unique ownership)
 * - Move-only type (cannot be copied, only moved)
 * 
 * Memory Management:
 * - Constructor: Takes ownership of the raw pointer
 * - Destructor: Automatically deletes the managed object
 * - Move semantics: Transfers ownership without copying
 * - Custom deleter: Allows custom cleanup logic
 */
template<typename T, typename Deleter = default_delete<T>>
class unique_ptr {
public:
    using pointer = T*;
    using element_type = T;
    using deleter_type = Deleter;

private:
    pointer ptr_;      // The managed pointer
    Deleter deleter_;  // The deleter function object

public:
    // ========== Constructors ==========
    
    /**
     * @brief Default constructor - creates empty unique_ptr
     */
    constexpr unique_ptr() noexcept : ptr_(nullptr), deleter_() {}
    
    /**
     * @brief nullptr constructor - creates empty unique_ptr
     */
    constexpr unique_ptr(std::nullptr_t) noexcept : ptr_(nullptr), deleter_() {}
    
    /**
     * @brief Construct from raw pointer
     * @param p Raw pointer to take ownership of
     * 
     * Takes ownership of the pointer. The pointer will be deleted
     * when this unique_ptr is destroyed.
     */
    explicit unique_ptr(pointer p) noexcept : ptr_(p), deleter_() {}
    
    /**
     * @brief Construct with custom deleter
     * @param p Raw pointer to take ownership of
     * @param d Deleter to use for cleanup
     */
    unique_ptr(pointer p, const Deleter& d) noexcept : ptr_(p), deleter_(d) {}
    
    unique_ptr(pointer p, Deleter&& d) noexcept : ptr_(p), deleter_(std::move(d)) {}
    
    /**
     * @brief Move constructor
     * @param other The unique_ptr to move from
     * 
     * Transfers ownership from 'other' to this unique_ptr.
     * After the move, 'other' no longer owns the pointer.
     */
    unique_ptr(unique_ptr&& other) noexcept 
        : ptr_(other.ptr_), deleter_(std::move(other.deleter_)) {
        other.ptr_ = nullptr;  // Release ownership from 'other'
    }
    
    /**
     * @brief Converting move constructor
     * 
     * Allows moving from unique_ptr<U> to unique_ptr<T> when U* converts to T*
     * (e.g., derived class to base class)
     */
    template<typename U, typename E>
    unique_ptr(unique_ptr<U, E>&& other) noexcept 
        : ptr_(other.release()), deleter_(std::move(other.get_deleter())) {}
    
    // ========== Deleted copy operations ==========
    // unique_ptr cannot be copied (only moved)
    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;
    
    /**
     * @brief Destructor
     * 
     * Automatically deletes the managed object using the deleter.
     * This is the core of RAII (Resource Acquisition Is Initialization).
     */
    ~unique_ptr() {
        if (ptr_) {
            deleter_(ptr_);
        }
    }
    
    // ========== Assignment operators ==========
    
    /**
     * @brief Move assignment operator
     * @param other The unique_ptr to move from
     * 
     * Deletes the currently managed object (if any) and takes
     * ownership of the object from 'other'.
     */
    unique_ptr& operator=(unique_ptr&& other) noexcept {
        if (this != &other) {
            // Delete current object
            if (ptr_) {
                deleter_(ptr_);
            }
            // Transfer ownership
            ptr_ = other.ptr_;
            deleter_ = std::move(other.deleter_);
            other.ptr_ = nullptr;
        }
        return *this;
    }
    
    /**
     * @brief Converting move assignment
     */
    template<typename U, typename E>
    unique_ptr& operator=(unique_ptr<U, E>&& other) noexcept {
        // Delete current object
        if (ptr_) {
            deleter_(ptr_);
        }
        // Transfer ownership
        ptr_ = other.release();
        deleter_ = std::move(other.get_deleter());
        return *this;
    }
    
    /**
     * @brief Assign nullptr - deletes managed object
     */
    unique_ptr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }
    
    // ========== Modifiers ==========
    
    /**
     * @brief Release ownership of the managed pointer
     * @return The raw pointer (caller must delete it manually)
     * 
     * After calling release(), this unique_ptr no longer owns the pointer
     * and will not delete it.
     */
    pointer release() noexcept {
        pointer old_ptr = ptr_;
        ptr_ = nullptr;
        return old_ptr;
    }
    
    /**
     * @brief Replace the managed pointer
     * @param p New pointer to manage (default is nullptr)
     * 
     * Deletes the currently managed object and takes ownership of 'p'.
     */
    void reset(pointer p = nullptr) noexcept {
        pointer old_ptr = ptr_;
        ptr_ = p;
        if (old_ptr) {
            deleter_(old_ptr);
        }
    }
    
    /**
     * @brief Swap with another unique_ptr
     */
    void swap(unique_ptr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(deleter_, other.deleter_);
    }
    
    // ========== Observers ==========
    
    /**
     * @brief Get the raw pointer
     * @return The managed pointer (or nullptr if none)
     */
    pointer get() const noexcept {
        return ptr_;
    }
    
    /**
     * @brief Get the deleter
     */
    Deleter& get_deleter() noexcept {
        return deleter_;
    }
    
    const Deleter& get_deleter() const noexcept {
        return deleter_;
    }
    
    /**
     * @brief Check if unique_ptr owns an object
     */
    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }
    
    /**
     * @brief Dereference operator
     * @return Reference to the managed object
     * 
     * Behavior is undefined if get() == nullptr
     */
    typename std::add_lvalue_reference<T>::type operator*() const {
        return *ptr_;
    }
    
    /**
     * @brief Member access operator
     * @return The managed pointer
     */
    pointer operator->() const noexcept {
        return ptr_;
    }
};

/**
 * @brief Specialization for array types
 * 
 * Provides operator[] for array access and uses delete[] for cleanup.
 */
template<typename T, typename Deleter>
class unique_ptr<T[], Deleter> {
public:
    using pointer = T*;
    using element_type = T;
    using deleter_type = Deleter;

private:
    pointer ptr_;
    Deleter deleter_;

public:
    constexpr unique_ptr() noexcept : ptr_(nullptr), deleter_() {}
    constexpr unique_ptr(std::nullptr_t) noexcept : ptr_(nullptr), deleter_() {}
    explicit unique_ptr(pointer p) noexcept : ptr_(p), deleter_() {}
    unique_ptr(pointer p, const Deleter& d) noexcept : ptr_(p), deleter_(d) {}
    unique_ptr(pointer p, Deleter&& d) noexcept : ptr_(p), deleter_(std::move(d)) {}
    
    unique_ptr(unique_ptr&& other) noexcept 
        : ptr_(other.ptr_), deleter_(std::move(other.deleter_)) {
        other.ptr_ = nullptr;
    }
    
    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;
    
    ~unique_ptr() {
        if (ptr_) {
            deleter_(ptr_);
        }
    }
    
    unique_ptr& operator=(unique_ptr&& other) noexcept {
        if (this != &other) {
            if (ptr_) {
                deleter_(ptr_);
            }
            ptr_ = other.ptr_;
            deleter_ = std::move(other.deleter_);
            other.ptr_ = nullptr;
        }
        return *this;
    }
    
    unique_ptr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }
    
    pointer release() noexcept {
        pointer old_ptr = ptr_;
        ptr_ = nullptr;
        return old_ptr;
    }
    
    void reset(pointer p = nullptr) noexcept {
        pointer old_ptr = ptr_;
        ptr_ = p;
        if (old_ptr) {
            deleter_(old_ptr);
        }
    }
    
    void swap(unique_ptr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(deleter_, other.deleter_);
    }
    
    pointer get() const noexcept {
        return ptr_;
    }
    
    Deleter& get_deleter() noexcept {
        return deleter_;
    }
    
    const Deleter& get_deleter() const noexcept {
        return deleter_;
    }
    
    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }
    
    /**
     * @brief Array subscript operator
     */
    T& operator[](std::size_t i) const {
        return ptr_[i];
    }
};

// ========== Helper functions ==========

/**
 * @brief Create a unique_ptr (C++14 style make_unique)
 * @param args Arguments to forward to T's constructor
 * 
 * Safer than using 'new' directly because:
 * - Exception safe
 * - More concise
 * - Can't accidentally leak memory
 */
template<typename T, typename... Args>
typename std::enable_if<!std::is_array<T>::value, unique_ptr<T>>::type
make_unique(Args&&... args) {
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

/**
 * @brief Create a unique_ptr for arrays
 */
template<typename T>
typename std::enable_if<std::is_array<T>::value, unique_ptr<T>>::type
make_unique(std::size_t size) {
    return unique_ptr<T>(new typename std::remove_extent<T>::type[size]());
}

// ========== Comparison operators ==========

template<typename T1, typename D1, typename T2, typename D2>
bool operator==(const unique_ptr<T1, D1>& lhs, const unique_ptr<T2, D2>& rhs) {
    return lhs.get() == rhs.get();
}

template<typename T1, typename D1, typename T2, typename D2>
bool operator!=(const unique_ptr<T1, D1>& lhs, const unique_ptr<T2, D2>& rhs) {
    return !(lhs == rhs);
}

template<typename T, typename D>
bool operator==(const unique_ptr<T, D>& lhs, std::nullptr_t) noexcept {
    return !lhs;
}

template<typename T, typename D>
bool operator==(std::nullptr_t, const unique_ptr<T, D>& rhs) noexcept {
    return !rhs;
}

template<typename T, typename D>
bool operator!=(const unique_ptr<T, D>& lhs, std::nullptr_t) noexcept {
    return static_cast<bool>(lhs);
}

template<typename T, typename D>
bool operator!=(std::nullptr_t, const unique_ptr<T, D>& rhs) noexcept {
    return static_cast<bool>(rhs);
}

} // namespace cppfoundry

#endif // CPPFOUNDRY_UNIQUE_PTR_HPP
