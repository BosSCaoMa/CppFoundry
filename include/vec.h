#pragma once
#include <stdexcept> // For std::out_of_range
#include <utility>   // For std::move

template<typename T>
class vec {
public:
    // 构造函数: 初始容量为 cap，大小为 0
    explicit vec(int cap = 8) : currentSize(0), currentCapacity(cap) {
        if (cap == 0) currentCapacity = 1; // 避免容量为0
        data = new T[currentCapacity];
    }

    // 析构函数
    ~vec() {
        delete[] data;
    }

    // 拷贝构造函数
    vec(const vec& other) : currentSize(other.currentSize), currentCapacity(other.currentCapacity) {
        data = new T[currentCapacity];
        for (int i = 0; i < currentSize; ++i) {
            data[i] = other.data[i];
        }
    }

    // 拷贝赋值运算符
    vec& operator=(const vec& other) {
        if (this == &other) {
            return *this; // 处理自赋值
        }
        delete[] data; // 释放旧资源
        currentSize = other.currentSize;
        currentCapacity = other.currentCapacity;
        data = new T[currentCapacity];
        for (int i = 0; i < currentSize; ++i) {
            data[i] = other.data[i];
        }
        return *this;
    }

    // 移动构造函数
    vec(vec&& other) noexcept : data(other.data), currentSize(other.currentSize), currentCapacity(other.currentCapacity) {
        other.data = nullptr;
        other.currentSize = 0;
        other.currentCapacity = 0;
    }

    // 移动赋值运算符
    vec& operator=(vec&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        delete[] data; // 释放旧资源
        data = other.data;
        currentSize = other.currentSize;
        currentCapacity = other.currentCapacity;
        other.data = nullptr;
        other.currentSize = 0;
        other.currentCapacity = 0;
        return *this;
    }

    void push_back(const T& value) {
        if (currentSize >= currentCapacity) {
            reserve(currentCapacity * 2);
        }
        data[currentSize++] = value;
    }

    void pop_back() {
        if (currentSize > 0) {
            --currentSize;
        }
    }

    void clear() {
        currentSize = 0;
    }

    // 预留容量
    void reserve(int newCapacity) {
        if (newCapacity <= currentCapacity) return;
        T* newData = new T[newCapacity];
        for (int i = 0; i < currentSize; ++i) {
            newData[i] = std::move(data[i]); // 使用 move 提高效率
        }
        delete[] data;
        data = newData;
        currentCapacity = newCapacity;
    }

    // 重设大小
    void resize(int newSize) {
        if (newSize > currentCapacity) {
            reserve(newSize);
        }
        // 如果新大小大于旧大小，用默认值填充
        for (int i = currentSize; i < newSize; ++i) {
            data[i] = T();
        }
        currentSize = newSize;
    }

    T& operator[](int index) {
        return data[index];
    }

    const T& operator[](int index) const {
        return data[index];
    }

    T& at(int index) {
        if (index < 0 || index >= currentSize) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    const T& at(int index) const {
        if (index < 0 || index >= currentSize) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    int size() const { return currentSize; }
    int capacity() const { return currentCapacity; }
    bool empty() const { return currentSize == 0; }

    // 迭代器支持
    T* begin() { return data; }
    T* end() { return data + currentSize; }
    const T* begin() const { return data; }
    const T* end() const { return data + currentSize; }


private:
    T* data;
    int currentCapacity; // 容量
    int currentSize;     // 当前大小
};