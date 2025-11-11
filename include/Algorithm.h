#pragma once
#include <vector>
#include <stdexcept> // For std::out_of_range
#include <utility>   // For std::swap

template<typename T>
class PriorityQueue {
public:
    PriorityQueue() {
        data.push_back(T()); 
    }

    bool empty() const {
        return data.size() <= 1;
    }

    size_t size() const {
        return data.size() - 1;
    }

    const T& top() const {
        if (empty()) {
            throw std::out_of_range("PriorityQueue is empty");
        }
        return data[1];
    }
    void push(const T& value) {
        data.push_back(value);
        siftUp(data.size() - 1);
    }

    void pop() {
        if (empty()) {
            throw std::out_of_range("PriorityQueue is empty");
        }
        std::swap(data[1], data.back());
        data.pop_back();
        if (!empty()) {
            siftDown(1);
        }
    }

private:
    std::vector<T> data;

    void siftUp(int idx) {
        while (idx > 1 && data[idx / 2] < data[idx]) {
            std::swap(data[idx], data[idx / 2]);
            idx /= 2;
        }
    }

    void siftDown(int idx) {
        int child = idx * 2;
        if (child >= data.size()) {
            return;
        }
        if (child + 1 < data.size() && data[child] < data[child + 1]) {
            child++;
        }
        if (data[idx] < data[child]) {
            std::swap(data[idx], data[child]);
            siftDown(child);
        }
    }

    void heapify() {
        for (int i = (data.size() - 1) / 2; i >= 1; --i) {
            siftDown(i);
        }
    }
};
