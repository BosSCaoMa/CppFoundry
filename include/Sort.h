#pragma once
#include <vector>
#include <utility>
#include <cstddef>

template<typename T>
static int partition(std::vector<T>& a, int low, int high) {
    T pivot = a[low];
    int i = low, j = high;
    while (i < j) {
        while (i < j && a[j] >= pivot) --j;
        if (i < j) a[i++] = std::move(a[j]);
        while (i < j && a[i] <= pivot) ++i;
        if (i < j) a[j--] = std::move(a[i]);
    }
    a[i] = std::move(pivot);
    return i;
}

// 递归实现
template<typename T>
static void quick_sort_impl(std::vector<T>& a, int low, int high) {
    if (low >= high) return;
    int p = partition(a, low, high);
    quick_sort_impl(a, low, p - 1);
    quick_sort_impl(a, p + 1, high);
}

// 对外接口
template<typename T>
void quick_sort(std::vector<T>& a) {
    if (a.empty()) return;
    quick_sort_impl(a, 0, static_cast<int>(a.size()) - 1);
}
