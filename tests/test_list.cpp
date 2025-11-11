#include "../include/containers/list.hpp"
#include <iostream>
#include <cassert>
#include <string>

using namespace cppfoundry;

void test_basic_construction() {
    std::cout << "\n=== Test: Basic Construction ===" << std::endl;
    
    // Default constructor
    list<int> l1;
    assert(l1.empty());
    assert(l1.size() == 0);
    
    // Constructor with size and value
    list<int> l2(5, 42);
    assert(l2.size() == 5);
    for (const auto& val : l2) {
        assert(val == 42);
    }
    
    // Constructor from initializer list
    list<int> l3 = {1, 2, 3, 4, 5};
    assert(l3.size() == 5);
    auto it = l3.begin();
    for (int i = 1; i <= 5; ++i, ++it) {
        assert(*it == i);
    }
    
    std::cout << "✓ Basic construction test passed" << std::endl;
}

void test_copy_and_move() {
    std::cout << "\n=== Test: Copy and Move ===" << std::endl;
    
    list<int> l1 = {1, 2, 3, 4, 5};
    
    // Copy constructor
    list<int> l2(l1);
    assert(l2.size() == l1.size());
    auto it1 = l1.begin();
    auto it2 = l2.begin();
    while (it1 != l1.end()) {
        assert(*it1 == *it2);
        ++it1;
        ++it2;
    }
    
    // Verify deep copy
    l2.front() = 100;
    assert(l1.front() == 1);
    assert(l2.front() == 100);
    
    // Move constructor
    list<int> l3(std::move(l1));
    assert(l3.size() == 5);
    assert(l1.size() == 0);  // l1 is now empty
    assert(l1.empty());
    
    // Copy assignment
    list<int> l4;
    l4 = l2;
    assert(l4.size() == l2.size());
    
    // Move assignment
    list<int> l5;
    l5 = std::move(l3);
    assert(l5.size() == 5);
    assert(l3.size() == 0);
    
    std::cout << "✓ Copy and move test passed" << std::endl;
}

void test_push_and_pop() {
    std::cout << "\n=== Test: Push and Pop ===" << std::endl;
    
    list<int> l;
    
    // push_back
    for (int i = 1; i <= 5; ++i) {
        l.push_back(i);
        assert(l.size() == static_cast<size_t>(i));
        assert(l.back() == i);
    }
    
    // push_front
    for (int i = -1; i >= -5; --i) {
        l.push_front(i);
        assert(l.front() == i);
    }
    
    assert(l.size() == 10);
    
    // Verify order: -5, -4, -3, -2, -1, 1, 2, 3, 4, 5
    int expected[] = {-5, -4, -3, -2, -1, 1, 2, 3, 4, 5};
    int idx = 0;
    for (const auto& val : l) {
        assert(val == expected[idx++]);
    }
    
    // pop_back
    for (int i = 0; i < 5; ++i) {
        size_t old_size = l.size();
        l.pop_back();
        assert(l.size() == old_size - 1);
    }
    
    // pop_front
    for (int i = 0; i < 5; ++i) {
        size_t old_size = l.size();
        l.pop_front();
        assert(l.size() == old_size - 1);
    }
    
    assert(l.empty());
    
    std::cout << "✓ Push and pop test passed" << std::endl;
}

void test_insert() {
    std::cout << "\n=== Test: Insert ===" << std::endl;
    
    list<int> l = {1, 2, 4, 5};
    
    // Insert in the middle
    auto it = l.begin();
    ++it;
    ++it;  // Points to 4
    l.insert(it, 3);
    
    assert(l.size() == 5);
    int expected[] = {1, 2, 3, 4, 5};
    int idx = 0;
    for (const auto& val : l) {
        assert(val == expected[idx++]);
    }
    
    // Insert at beginning
    l.insert(l.begin(), 0);
    assert(l.front() == 0);
    assert(l.size() == 6);
    
    // Insert at end
    l.insert(l.end(), 6);
    assert(l.back() == 6);
    assert(l.size() == 7);
    
    std::cout << "✓ Insert test passed" << std::endl;
}

void test_erase() {
    std::cout << "\n=== Test: Erase ===" << std::endl;
    
    list<int> l = {1, 2, 3, 4, 5};
    
    // Erase from middle
    auto it = l.begin();
    ++it;
    ++it;  // Points to 3
    it = l.erase(it);
    assert(*it == 4);  // Returns iterator to next element
    assert(l.size() == 4);
    
    // Verify: 1, 2, 4, 5
    int expected[] = {1, 2, 4, 5};
    int idx = 0;
    for (const auto& val : l) {
        assert(val == expected[idx++]);
    }
    
    // Erase first
    l.erase(l.begin());
    assert(l.front() == 2);
    assert(l.size() == 3);
    
    // Erase last
    it = l.end();
    --it;
    l.erase(it);
    assert(l.back() == 4);
    assert(l.size() == 2);
    
    std::cout << "✓ Erase test passed" << std::endl;
}

void test_emplace() {
    std::cout << "\n=== Test: Emplace ===" << std::endl;
    
    list<std::string> l;
    
    l.emplace_back("World");
    l.emplace_front("Hello");
    
    assert(l.size() == 2);
    assert(l.front() == "Hello");
    assert(l.back() == "World");
    
    // Emplace in middle
    auto it = l.begin();
    ++it;
    l.emplace(it, "Beautiful");
    
    assert(l.size() == 3);
    it = l.begin();
    assert(*it == "Hello");
    ++it;
    assert(*it == "Beautiful");
    ++it;
    assert(*it == "World");
    
    std::cout << "✓ Emplace test passed" << std::endl;
}

void test_clear() {
    std::cout << "\n=== Test: Clear ===" << std::endl;
    
    list<int> l = {1, 2, 3, 4, 5};
    assert(!l.empty());
    
    l.clear();
    assert(l.empty());
    assert(l.size() == 0);
    
    // Can still add elements after clear
    l.push_back(10);
    assert(l.size() == 1);
    assert(l.front() == 10);
    
    std::cout << "✓ Clear test passed" << std::endl;
}

void test_iterators() {
    std::cout << "\n=== Test: Iterators ===" << std::endl;
    
    list<int> l = {1, 2, 3, 4, 5};
    
    // Forward iteration
    int sum = 0;
    for (auto it = l.begin(); it != l.end(); ++it) {
        sum += *it;
    }
    assert(sum == 15);
    
    // Range-based for loop
    sum = 0;
    for (int val : l) {
        sum += val;
    }
    assert(sum == 15);
    
    // Backward iteration
    auto it = l.end();
    --it;
    assert(*it == 5);
    --it;
    assert(*it == 4);
    
    // Modify through iterator
    for (auto& val : l) {
        val *= 2;
    }
    it = l.begin();
    assert(*it == 2);
    ++it;
    assert(*it == 4);
    
    std::cout << "✓ Iterators test passed" << std::endl;
}

void test_front_and_back() {
    std::cout << "\n=== Test: Front and Back ===" << std::endl;
    
    list<int> l = {1, 2, 3, 4, 5};
    
    assert(l.front() == 1);
    assert(l.back() == 5);
    
    l.front() = 100;
    l.back() = 500;
    
    assert(l.front() == 100);
    assert(l.back() == 500);
    
    std::cout << "✓ Front and back test passed" << std::endl;
}

void test_comparison_operators() {
    std::cout << "\n=== Test: Comparison Operators ===" << std::endl;
    
    list<int> l1 = {1, 2, 3};
    list<int> l2 = {1, 2, 3};
    list<int> l3 = {1, 2, 4};
    list<int> l4 = {1, 2};
    
    assert(l1 == l2);
    assert(l1 != l3);
    assert(l1 != l4);
    
    std::cout << "✓ Comparison operators test passed" << std::endl;
}

void test_swap() {
    std::cout << "\n=== Test: Swap ===" << std::endl;
    
    list<int> l1 = {1, 2, 3};
    list<int> l2 = {4, 5, 6, 7};
    
    size_t size1 = l1.size();
    size_t size2 = l2.size();
    
    l1.swap(l2);
    
    assert(l1.size() == size2);
    assert(l2.size() == size1);
    assert(l1.front() == 4);
    assert(l2.front() == 1);
    
    std::cout << "✓ Swap test passed" << std::endl;
}

void test_bidirectional_iteration() {
    std::cout << "\n=== Test: Bidirectional Iteration ===" << std::endl;
    
    list<int> l = {1, 2, 3, 4, 5};
    
    // Forward then backward
    auto it = l.begin();
    for (int i = 0; i < 3; ++i) ++it;
    assert(*it == 4);
    
    for (int i = 0; i < 2; ++i) --it;
    assert(*it == 2);
    
    ++it;
    ++it;
    ++it;
    assert(*it == 5);
    
    --it;
    assert(*it == 4);
    
    std::cout << "✓ Bidirectional iteration test passed" << std::endl;
}

void test_with_custom_type() {
    std::cout << "\n=== Test: Custom Type ===" << std::endl;
    
    struct Point {
        int x, y;
        Point(int x_ = 0, int y_ = 0) : x(x_), y(y_) {}
        bool operator==(const Point& other) const {
            return x == other.x && y == other.y;
        }
        bool operator!=(const Point& other) const {
            return !(*this == other);
        }
    };
    
    list<Point> l;
    l.push_back(Point(1, 2));
    l.push_back(Point(3, 4));
    l.emplace_back(5, 6);
    
    assert(l.size() == 3);
    auto it = l.begin();
    assert(it->x == 1 && it->y == 2);
    ++it;
    assert(it->x == 3 && it->y == 4);
    ++it;
    assert(it->x == 5 && it->y == 6);
    
    std::cout << "✓ Custom type test passed" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "       list Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        test_basic_construction();
        test_copy_and_move();
        test_push_and_pop();
        test_insert();
        test_erase();
        test_emplace();
        test_clear();
        test_iterators();
        test_front_and_back();
        test_comparison_operators();
        test_swap();
        test_bidirectional_iteration();
        test_with_custom_type();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "     ✓ All list tests passed!" << std::endl;
        std::cout << "========================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "✗ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
