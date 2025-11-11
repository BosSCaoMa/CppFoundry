#include "../include/containers/vector.hpp"
#include <iostream>
#include <cassert>
#include <string>

using namespace cppfoundry;

void test_basic_construction() {
    std::cout << "\n=== Test: Basic Construction ===" << std::endl;
    
    // Default constructor
    vector<int> v1;
    assert(v1.empty());
    assert(v1.size() == 0);
    assert(v1.capacity() == 0);
    
    // Constructor with size
    vector<int> v2(5);
    assert(v2.size() == 5);
    assert(v2.capacity() == 5);
    
    // Constructor with size and value
    vector<int> v3(5, 42);
    assert(v3.size() == 5);
    for (size_t i = 0; i < 5; ++i) {
        assert(v3[i] == 42);
    }
    
    // Constructor from initializer list
    vector<int> v4 = {1, 2, 3, 4, 5};
    assert(v4.size() == 5);
    assert(v4[0] == 1);
    assert(v4[4] == 5);
    
    std::cout << "✓ Basic construction test passed" << std::endl;
}

void test_copy_and_move() {
    std::cout << "\n=== Test: Copy and Move ===" << std::endl;
    
    vector<int> v1 = {1, 2, 3, 4, 5};
    
    // Copy constructor
    vector<int> v2(v1);
    assert(v2.size() == v1.size());
    for (size_t i = 0; i < v1.size(); ++i) {
        assert(v2[i] == v1[i]);
    }
    // Verify deep copy
    v2[0] = 100;
    assert(v1[0] == 1);
    assert(v2[0] == 100);
    
    // Move constructor
    vector<int> v3(std::move(v1));
    assert(v3.size() == 5);
    assert(v1.size() == 0);  // v1 is now empty
    
    // Copy assignment
    vector<int> v4;
    v4 = v2;
    assert(v4.size() == v2.size());
    for (size_t i = 0; i < v2.size(); ++i) {
        assert(v4[i] == v2[i]);
    }
    
    // Move assignment
    vector<int> v5;
    v5 = std::move(v3);
    assert(v5.size() == 5);
    assert(v3.size() == 0);
    
    std::cout << "✓ Copy and move test passed" << std::endl;
}

void test_element_access() {
    std::cout << "\n=== Test: Element Access ===" << std::endl;
    
    vector<int> v = {10, 20, 30, 40, 50};
    
    // operator[]
    assert(v[0] == 10);
    assert(v[4] == 50);
    v[2] = 100;
    assert(v[2] == 100);
    
    // at() with bounds checking
    assert(v.at(0) == 10);
    assert(v.at(4) == 50);
    v.at(2) = 200;
    assert(v.at(2) == 200);
    
    // Test exception for out of bounds
    bool exception_thrown = false;
    try {
        v.at(10);
    } catch (const std::out_of_range&) {
        exception_thrown = true;
    }
    assert(exception_thrown);
    
    // front() and back()
    assert(v.front() == 10);
    assert(v.back() == 50);
    
    // data()
    int* ptr = v.data();
    assert(ptr[0] == 10);
    assert(ptr[4] == 50);
    
    std::cout << "✓ Element access test passed" << std::endl;
}

void test_capacity_operations() {
    std::cout << "\n=== Test: Capacity Operations ===" << std::endl;
    
    vector<int> v;
    assert(v.empty());
    
    // reserve
    v.reserve(10);
    assert(v.capacity() >= 10);
    assert(v.size() == 0);  // reserve doesn't change size
    
    // Add elements
    for (int i = 0; i < 5; ++i) {
        v.push_back(i);
    }
    assert(v.size() == 5);
    assert(v.capacity() >= 10);
    
    // shrink_to_fit
    v.shrink_to_fit();
    assert(v.capacity() == v.size());
    
    std::cout << "✓ Capacity operations test passed" << std::endl;
}

void test_push_back_and_pop_back() {
    std::cout << "\n=== Test: Push Back and Pop Back ===" << std::endl;
    
    vector<int> v;
    
    // Test geometric growth
    for (int i = 0; i < 100; ++i) {
        v.push_back(i);
        assert(v.size() == static_cast<size_t>(i + 1));
        assert(v.back() == i);
    }
    
    // Verify all elements
    for (size_t i = 0; i < v.size(); ++i) {
        assert(v[i] == static_cast<int>(i));
    }
    
    // pop_back
    while (!v.empty()) {
        size_t old_size = v.size();
        int last = v.back();
        v.pop_back();
        assert(v.size() == old_size - 1);
        if (!v.empty()) {
            assert(v.back() == last - 1);
        }
    }
    
    assert(v.empty());
    
    std::cout << "✓ Push back and pop back test passed" << std::endl;
}

void test_emplace_back() {
    std::cout << "\n=== Test: Emplace Back ===" << std::endl;
    
    vector<std::string> v;
    
    v.emplace_back("Hello");
    v.emplace_back("World");
    
    assert(v.size() == 2);
    assert(v[0] == "Hello");
    assert(v[1] == "World");
    
    // Emplace with constructor args
    vector<std::pair<int, std::string>> v2;
    v2.emplace_back(1, "one");
    v2.emplace_back(2, "two");
    
    assert(v2.size() == 2);
    assert(v2[0].first == 1);
    assert(v2[0].second == "one");
    
    std::cout << "✓ Emplace back test passed" << std::endl;
}

void test_resize() {
    std::cout << "\n=== Test: Resize ===" << std::endl;
    
    vector<int> v = {1, 2, 3};
    
    // Resize larger
    v.resize(5);
    assert(v.size() == 5);
    assert(v[0] == 1);
    assert(v[1] == 2);
    assert(v[2] == 3);
    // New elements are default-initialized
    
    // Resize larger with value
    v.resize(7, 42);
    assert(v.size() == 7);
    assert(v[5] == 42);
    assert(v[6] == 42);
    
    // Resize smaller
    v.resize(3);
    assert(v.size() == 3);
    assert(v[0] == 1);
    assert(v[2] == 3);
    
    std::cout << "✓ Resize test passed" << std::endl;
}

void test_clear() {
    std::cout << "\n=== Test: Clear ===" << std::endl;
    
    vector<int> v = {1, 2, 3, 4, 5};
    size_t old_capacity = v.capacity();
    
    v.clear();
    assert(v.empty());
    assert(v.size() == 0);
    assert(v.capacity() == old_capacity);  // Capacity unchanged
    
    // Can still add elements after clear
    v.push_back(10);
    assert(v.size() == 1);
    assert(v[0] == 10);
    
    std::cout << "✓ Clear test passed" << std::endl;
}

void test_iterators() {
    std::cout << "\n=== Test: Iterators ===" << std::endl;
    
    vector<int> v = {1, 2, 3, 4, 5};
    
    // Range-based for loop
    int sum = 0;
    for (int val : v) {
        sum += val;
    }
    assert(sum == 15);
    
    // Iterator arithmetic
    auto it = v.begin();
    assert(*it == 1);
    ++it;
    assert(*it == 2);
    it += 2;
    assert(*it == 4);
    
    // Modify through iterator
    for (auto& val : v) {
        val *= 2;
    }
    assert(v[0] == 2);
    assert(v[4] == 10);
    
    std::cout << "✓ Iterators test passed" << std::endl;
}

void test_comparison_operators() {
    std::cout << "\n=== Test: Comparison Operators ===" << std::endl;
    
    vector<int> v1 = {1, 2, 3};
    vector<int> v2 = {1, 2, 3};
    vector<int> v3 = {1, 2, 4};
    vector<int> v4 = {1, 2};
    
    assert(v1 == v2);
    assert(v1 != v3);
    assert(v1 != v4);
    
    std::cout << "✓ Comparison operators test passed" << std::endl;
}

void test_swap() {
    std::cout << "\n=== Test: Swap ===" << std::endl;
    
    vector<int> v1 = {1, 2, 3};
    vector<int> v2 = {4, 5, 6, 7};
    
    size_t size1 = v1.size();
    size_t size2 = v2.size();
    
    v1.swap(v2);
    
    assert(v1.size() == size2);
    assert(v2.size() == size1);
    assert(v1[0] == 4);
    assert(v2[0] == 1);
    
    std::cout << "✓ Swap test passed" << std::endl;
}

void test_with_custom_type() {
    std::cout << "\n=== Test: Custom Type ===" << std::endl;
    
    struct Point {
        int x, y;
        Point(int x_ = 0, int y_ = 0) : x(x_), y(y_) {}
    };
    
    vector<Point> v;
    v.push_back(Point(1, 2));
    v.push_back(Point(3, 4));
    v.emplace_back(5, 6);
    
    assert(v.size() == 3);
    assert(v[0].x == 1 && v[0].y == 2);
    assert(v[1].x == 3 && v[1].y == 4);
    assert(v[2].x == 5 && v[2].y == 6);
    
    std::cout << "✓ Custom type test passed" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "      vector Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        test_basic_construction();
        test_copy_and_move();
        test_element_access();
        test_capacity_operations();
        test_push_back_and_pop_back();
        test_emplace_back();
        test_resize();
        test_clear();
        test_iterators();
        test_comparison_operators();
        test_swap();
        test_with_custom_type();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "    ✓ All vector tests passed!" << std::endl;
        std::cout << "========================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "✗ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
