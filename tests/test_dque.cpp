#include <iostream>
#include <cassert>
#include "../include/dque.h"

void test_basic_operations() {
    std::cout << "\n=== Testing Basic Operations ===" << std::endl;
    
    dque<int> dq;
    assert(dq.empty());
    assert(dq.size() == 0);
    
    // push_back
    dq.push_back(1);
    dq.push_back(2);
    dq.push_back(3);
    assert(dq.size() == 3);
    assert(dq[0] == 1);
    assert(dq[1] == 2);
    assert(dq[2] == 3);
    
    std::cout << "push_back tests passed!" << std::endl;
}

void test_push_front() {
    std::cout << "\n=== Testing Push Front ===" << std::endl;
    
    dque<int> dq;
    dq.push_front(1);
    dq.push_front(2);
    dq.push_front(3);
    
    assert(dq.size() == 3);
    assert(dq[0] == 3);
    assert(dq[1] == 2);
    assert(dq[2] == 1);
    
    std::cout << "push_front tests passed!" << std::endl;
}

void test_mixed_operations() {
    std::cout << "\n=== Testing Mixed Operations ===" << std::endl;
    
    dque<int> dq;
    dq.push_back(10);
    dq.push_front(5);
    dq.push_back(15);
    dq.push_front(0);
    
    // 应该是: 0, 5, 10, 15
    assert(dq.size() == 4);
    assert(dq[0] == 0);
    assert(dq[1] == 5);
    assert(dq[2] == 10);
    assert(dq[3] == 15);
    
    std::cout << "Mixed operations tests passed!" << std::endl;
}

void test_pop_operations() {
    std::cout << "\n=== Testing Pop Operations ===" << std::endl;
    
    dque<int> dq;
    for (int i = 0; i < 10; ++i) {
        dq.push_back(i);
    }
    
    dq.pop_front();
    assert(dq.size() == 9);
    assert(dq.front() == 1);
    
    dq.pop_back();
    assert(dq.size() == 8);
    assert(dq.back() == 8);
    
    std::cout << "Pop operations tests passed!" << std::endl;
}

void test_front_back() {
    std::cout << "\n=== Testing Front and Back ===" << std::endl;
    
    dque<int> dq;
    dq.push_back(100);
    assert(dq.front() == 100);
    assert(dq.back() == 100);
    
    dq.push_back(200);
    assert(dq.front() == 100);
    assert(dq.back() == 200);
    
    dq.push_front(50);
    assert(dq.front() == 50);
    assert(dq.back() == 200);
    
    std::cout << "Front and back tests passed!" << std::endl;
}

void test_copy_semantics() {
    std::cout << "\n=== Testing Copy Semantics ===" << std::endl;
    
    dque<int> dq1;
    for (int i = 0; i < 5; ++i) {
        dq1.push_back(i);
    }
    
    // 拷贝构造
    dque<int> dq2(dq1);
    assert(dq2.size() == 5);
    assert(dq2[0] == 0);
    assert(dq2[4] == 4);
    
    // 拷贝赋值
    dque<int> dq3;
    dq3 = dq1;
    assert(dq3.size() == 5);
    assert(dq3[0] == 0);
    
    std::cout << "Copy semantics tests passed!" << std::endl;
}

void test_move_semantics() {
    std::cout << "\n=== Testing Move Semantics ===" << std::endl;
    
    dque<int> dq1;
    for (int i = 0; i < 5; ++i) {
        dq1.push_back(i * 10);
    }
    
    // 移动构造
    dque<int> dq2(std::move(dq1));
    assert(dq2.size() == 5);
    assert(dq2[0] == 0);
    assert(dq2[4] == 40);
    assert(dq1.size() == 0);
    
    // 移动赋值
    dque<int> dq3;
    dq3 = std::move(dq2);
    assert(dq3.size() == 5);
    assert(dq2.size() == 0);
    
    std::cout << "Move semantics tests passed!" << std::endl;
}

void test_clear() {
    std::cout << "\n=== Testing Clear ===" << std::endl;
    
    dque<int> dq;
    for (int i = 0; i < 100; ++i) {
        dq.push_back(i);
    }
    
    assert(dq.size() == 100);
    dq.clear();
    assert(dq.empty());
    assert(dq.size() == 0);
    
    // 清空后可以继续使用
    dq.push_back(42);
    assert(dq.size() == 1);
    assert(dq[0] == 42);
    
    std::cout << "Clear tests passed!" << std::endl;
}

void test_at_bounds_check() {
    std::cout << "\n=== Testing Bounds Check ===" << std::endl;
    
    dque<int> dq;
    dq.push_back(1);
    dq.push_back(2);
    
    try {
        dq.at(5);
        assert(false && "Should throw exception");
    } catch (const std::out_of_range&) {
        std::cout << "Bounds check working correctly" << std::endl;
    }
    
    std::cout << "Bounds check tests passed!" << std::endl;
}

void test_large_deque() {
    std::cout << "\n=== Testing Large Deque ===" << std::endl;
    
    dque<int> dq;
    
    // 测试跨越多个块
    for (int i = 0; i < 200; ++i) {
        dq.push_back(i);
    }
    
    assert(dq.size() == 200);
    assert(dq[0] == 0);
    assert(dq[199] == 199);
    
    // 从前面也添加
    for (int i = 0; i < 100; ++i) {
        dq.push_front(-i - 1);
    }
    
    assert(dq.size() == 300);
    assert(dq[0] == -100);
    assert(dq[299] == 199);
    
    std::cout << "Large deque tests passed!" << std::endl;
}

int main() {
    std::cout << "Deque Tests" << std::endl;
    std::cout << "===========" << std::endl;
    
    try {
        test_basic_operations();
        test_push_front();
        test_mixed_operations();
        test_pop_operations();
        test_front_back();
        test_copy_semantics();
        test_move_semantics();
        test_clear();
        test_at_bounds_check();
        test_large_deque();
        
        std::cout << "\n🎉 All deque tests passed!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "\n❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
