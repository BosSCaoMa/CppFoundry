#ifndef MY_VECTOR_HPP
#define MY_VECTOR_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <algorithm>

template<typename T>
class MyVector {
private:
    T* data_;           // 指向动态分配的数组
    size_t size_;       // 当前元素个数
    size_t capacity_;   // 当前容量

    // 扩容函数
    void resize(size_t new_capacity) {
        T* new_data = new T[new_capacity];
        // 移动或复制现有元素到新内存
        for (size_t i = 0; i < size_; ++i) {
            new_data[i] = std::move(data_[i]);
        }
        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }

public:
    // 默认构造函数
    MyVector() : data_(nullptr), size_(0), capacity_(0) {}

    // 带初始容量的构造函数
    explicit MyVector(size_t initial_capacity) 
        : data_(new T[initial_capacity]), size_(0), capacity_(initial_capacity) {}

    // 析构函数
    ~MyVector() {
        delete[] data_;
    }

    // 拷贝构造函数
    MyVector(const MyVector& other) 
        : data_(new T[other.capacity_]), size_(other.size_), capacity_(other.capacity_) {
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }

    // 拷贝赋值操作符
    MyVector& operator=(const MyVector& other) {
        if (this != &other) {
            delete[] data_;
            capacity_ = other.capacity_;
            size_ = other.size_;
            data_ = new T[capacity_];
            for (size_t i = 0; i < size_; ++i) {
                data_[i] = other.data_[i];
            }
        }
        return *this;
    }

    // 移动构造函数
    MyVector(MyVector&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    // 移动赋值操作符
    MyVector& operator=(MyVector&& other) noexcept {
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

    // 在末尾添加元素（动态扩容）
    void push_back(const T& value) {
        if (size_ == capacity_) {
            // 如果容量为0，初始分配为1；否则翻倍
            size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
            resize(new_capacity);
        }
        data_[size_++] = value;
    }

    // 在末尾添加元素（移动语义）
    void push_back(T&& value) {
        if (size_ == capacity_) {
            size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
            resize(new_capacity);
        }
        data_[size_++] = std::move(value);
    }

    // 删除末尾元素
    void pop_back() {
        if (size_ > 0) {
            --size_;
        }
    }

    // 访问元素（带边界检查）
    T& at(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    const T& at(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    // 访问元素（不检查边界）
    T& operator[](size_t index) {
        return data_[index];
    }

    const T& operator[](size_t index) const {
        return data_[index];
    }

    // 获取第一个元素
    T& front() {
        return data_[0];
    }

    const T& front() const {
        return data_[0];
    }

    // 获取最后一个元素
    T& back() {
        return data_[size_ - 1];
    }

    const T& back() const {
        return data_[size_ - 1];
    }

    // 获取当前元素个数
    size_t size() const {
        return size_;
    }

    // 获取当前容量
    size_t capacity() const {
        return capacity_;
    }

    // 判断是否为空
    bool empty() const {
        return size_ == 0;
    }

    // 清空所有元素
    void clear() {
        size_ = 0;
    }

    // 预留容量
    void reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            resize(new_capacity);
        }
    }

    // 简单的迭代器实现
    using iterator = T*;
    using const_iterator = const T*;

    iterator begin() {
        return data_;
    }

    iterator end() {
        return data_ + size_;
    }

    const_iterator begin() const {
        return data_;
    }

    const_iterator end() const {
        return data_ + size_;
    }

    const_iterator cbegin() const {
        return data_;
    }

    const_iterator cend() const {
        return data_ + size_;
    }
};

#endif // MY_VECTOR_HPP
