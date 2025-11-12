#pragma once
#include <utility> // For std::move and std::forward

// 辅助工厂函数，支持构造函数参数
template<typename T, typename... Args>
Uptr<T> make_uptr(Args&&... args);

template<typename T>
class Uptr {
public:
    // 构造与析构
    explicit Uptr(T* rawPtr = nullptr) noexcept : ptr(rawPtr) {}
    ~Uptr() {
        delete ptr;
    }

    // 删除拷贝操作
    Uptr(const Uptr&) = delete;
    Uptr& operator=(const Uptr&) = delete;

    // 实现移动操作
    Uptr(Uptr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }
    Uptr& operator=(Uptr&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    // 解引用
    T& operator*() const { return *ptr; }
    T* operator->() const noexcept { return ptr; }

    // 获取裸指针
    T* get() const noexcept { return ptr; }

    // 放弃所有权并返回裸指针
    T* release() noexcept {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    // 重置指针，可选择接管新指针
    void reset(T* newPtr = nullptr) noexcept {
        T* oldPtr = ptr;   // 为什么先删除旧的再赋值不行？ 如果newPtr 和 ptr 相同，就会删除正在使用的指针，导致悬空指针问题。
        ptr = newPtr;
        delete oldPtr;
    }

    // 交换两个 Uptr 的内容
    void swap(Uptr& other) noexcept {
        T* temp = ptr;
        ptr = other.ptr;
        other.ptr = temp;
    }

    // 在布尔上下文中检查有效性
    explicit operator bool() const noexcept {
        return ptr != nullptr;
    }

private:
    T* ptr;

    // 允许 make_uptr 访问私有构造函数
    template<typename U, typename... Args>
    friend Uptr<U> make_uptr(Args&&... args);
};

// 工厂函数实现
template<typename T, typename... Args>
Uptr<T> make_uptr(Args&&... args) {
    return Uptr<T>(new T(std::forward<Args>(args)...));
}

// 为 swap 提供非成员函数版本，符合C++习惯
template<typename T>
void swap(Uptr<T>& a, Uptr<T>& b) noexcept {
    a.swap(b);
}

// Share_ptr
template<typename T, typename... Args>
Sptr<T> make_sptr(Args&&... args);
struct ControlBlock {
    int refCount;
    int weakCount;
    void* managedPtr; // 保存对象指针，最后一个强引用释放后置为 nullptr
    ControlBlock(void* p) : refCount(1), weakCount(0), managedPtr(p) {}
};
template<typename T>
class Sptr {
public:
    // 构造：只有在 rawPtr 非空时才分配控制块
    explicit Sptr(T* rawPtr = nullptr) : ptr(rawPtr), ctrlBlock(rawPtr ? new ControlBlock(rawPtr) : nullptr) {}

    // 供 Wptr::lock 使用的私有控制块构造（提升为 public+friend 使用）
    // 从现有控制块生成新的强引用
    Sptr(ControlBlock* cb) : ptr(cb ? static_cast<T*>(cb->managedPtr) : nullptr), ctrlBlock(cb) {
        if (ctrlBlock && ctrlBlock->managedPtr) ++(ctrlBlock->refCount);
    }

    // 拷贝构造
    Sptr(const Sptr& other) : ptr(other.ptr), ctrlBlock(other.ctrlBlock) {
        if (ctrlBlock) ++(ctrlBlock->refCount);
    }

    // 拷贝赋值
    Sptr& operator=(const Sptr& other) {
        if (this != &other) {
            release();
            ptr = other.ptr;
            ctrlBlock = other.ctrlBlock;
            if (ctrlBlock) ++(ctrlBlock->refCount);
        }
        return *this;
    }

    // 移动构造
    Sptr(Sptr&& other) noexcept : ptr(other.ptr), ctrlBlock(other.ctrlBlock) {
        other.ptr = nullptr;
        other.ctrlBlock = nullptr;
    }

    // 移动赋值
    Sptr& operator=(Sptr&& other) noexcept {
        if (this != &other) {
            release();
            ptr = other.ptr;
            ctrlBlock = other.ctrlBlock;
            other.ptr = nullptr;
            other.ctrlBlock = nullptr;
        }
        return *this;
    }

    ~Sptr() { release(); }

    // 访问接口
    T& operator*() const { return *ptr; }
    T* operator->() const noexcept { return ptr; }
    T* get() const noexcept { return ptr; }
    explicit operator bool() const noexcept { return ptr != nullptr; }

    // 当前强引用计数
    int use_count() const { return ctrlBlock ? ctrlBlock->refCount : 0; }

    // 释放当前拥有的强引用
    void release() {
        if (!ctrlBlock) return;
        if (--(ctrlBlock->refCount) == 0) {
            // 删除对象
            delete static_cast<T*>(ctrlBlock->managedPtr);
            ctrlBlock->managedPtr = nullptr; // 标记对象已释放
            ptr = nullptr;
            // 若弱引用也为 0 则删除控制块
            if (ctrlBlock->weakCount == 0) {
                delete ctrlBlock;
                ctrlBlock = nullptr;
                return;
            }
        }
        // 断开本 Sptr 的连接
        ptr = nullptr;
        ctrlBlock = nullptr;
    }

    // 重置为新指针
    void reset(T* newPtr = nullptr) {
        if (get() == newPtr) return; // 同指针直接返回
        release();
        if (newPtr) {
            ptr = newPtr;
            ctrlBlock = new ControlBlock(newPtr);
        }
    }

    // 交换
    void swap(Sptr& other) noexcept {
        std::swap(ptr, other.ptr);
        std::swap(ctrlBlock, other.ctrlBlock);
    }

private:
    T* ptr{nullptr};
    ControlBlock* ctrlBlock{nullptr};

    template<typename U, typename... Args>
    friend Sptr<U> make_sptr(Args&&... args);
    template<typename U> friend class Wptr; // 允许 Wptr 使用私有成员
};

// 工厂函数
template<typename T, typename... Args>
Sptr<T> make_sptr(Args&&... args) {
    return Sptr<T>(new T(std::forward<Args>(args)...));
}

// 非成员 swap
template<typename T>
void swap(Sptr<T>& a, Sptr<T>& b) noexcept { a.swap(b); }

template<typename T>
void swap(Wptr<T>& a, Wptr<T>& b) noexcept { a.swap(b); }

// Weak_ptr
template<typename T>
class Wptr {
public:
    Wptr() noexcept : ctrlBlock(nullptr) {}
    Wptr(const Sptr<T>& sp) noexcept : ctrlBlock(sp.ctrlBlock) { if (ctrlBlock) ++(ctrlBlock->weakCount); }

    Wptr(const Wptr& other) : ctrlBlock(other.ctrlBlock) { if (ctrlBlock) ++(ctrlBlock->weakCount); }
    Wptr& operator=(const Wptr& other) {
        if (this != &other) {
            release();
            ctrlBlock = other.ctrlBlock;
            if (ctrlBlock) ++(ctrlBlock->weakCount);
        }
        return *this;
    }

    Wptr(Wptr&& other) noexcept : ctrlBlock(other.ctrlBlock) { other.ctrlBlock = nullptr; }
    Wptr& operator=(Wptr&& other) noexcept {
        if (this != &other) {
            release();
            ctrlBlock = other.ctrlBlock;
            other.ctrlBlock = nullptr;
        }
        return *this;
    }

    ~Wptr() { release(); }

    void release() {
        if (!ctrlBlock) return;
        if (--(ctrlBlock->weakCount) == 0 && ctrlBlock->refCount == 0) {
            delete ctrlBlock; // 对象已被强引用释放，弱引用也归零
        }
        ctrlBlock = nullptr;
    }

    void reset() { release(); }

    bool expired() const noexcept { return !ctrlBlock || ctrlBlock->managedPtr == nullptr; }

    int use_count() const noexcept { return ctrlBlock ? ctrlBlock->refCount : 0; }

    Sptr<T> lock() const noexcept { return expired() ? Sptr<T>() : Sptr<T>(ctrlBlock); }
    /*
    weak_ptr 只是一个“观察者”，它不增加引用计数，也不能直接解引用（不能 -> 或 *）。
    要安全地访问对象，你必须先把它提升为一个 shared_ptr，这样可以确保对象在访问期间不会被其他地方销毁。
    这个提升操作就是通过 lock() 完成的。
    */

    void swap(Wptr& other) noexcept { std::swap(ctrlBlock, other.ctrlBlock); }

private:
    ControlBlock* ctrlBlock; // 不存对象指针，依赖控制块
};