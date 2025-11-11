#include <iostream>
#include <cassert>
#include <string>
#include "../include/MyVector.hpp"

// 测试计数器
int test_passed = 0;
int test_failed = 0;

#define TEST(name) \
    void name(); \
    void run_##name() { \
        try { \
            name(); \
            test_passed++; \
            std::cout << "[PASS] " << #name << std::endl; \
        } catch (const std::exception& e) { \
            test_failed++; \
            std::cout << "[FAIL] " << #name << " - " << e.what() << std::endl; \
        } \
    } \
    void name()

TEST(test_default_constructor) {
    MyVector<int> vec;
    assert(vec.size() == 0);
    assert(vec.capacity() == 0);
    assert(vec.empty());
}

TEST(test_push_back) {
    MyVector<int> vec;
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);
    
    assert(vec.size() == 3);
    assert(vec[0] == 10);
    assert(vec[1] == 20);
    assert(vec[2] == 30);
}

TEST(test_dynamic_expansion) {
    MyVector<int> vec;
    
    // 添加足够多的元素以触发扩容
    for (int i = 0; i < 100; ++i) {
        vec.push_back(i);
    }
    
    assert(vec.size() == 100);
    assert(vec.capacity() >= 100);
    
    // 验证所有元素
    for (int i = 0; i < 100; ++i) {
        assert(vec[i] == i);
    }
}

TEST(test_access_operators) {
    MyVector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    
    assert(vec[0] == 1);
    assert(vec[1] == 2);
    assert(vec[2] == 3);
    
    assert(vec.front() == 1);
    assert(vec.back() == 3);
}

TEST(test_at_with_bounds_check) {
    MyVector<int> vec;
    vec.push_back(10);
    
    assert(vec.at(0) == 10);
    
    bool exception_thrown = false;
    try {
        vec.at(10);  // 应该抛出异常
    } catch (const std::out_of_range&) {
        exception_thrown = true;
    }
    assert(exception_thrown);
}

TEST(test_iterator_traversal) {
    MyVector<int> vec;
    for (int i = 0; i < 5; ++i) {
        vec.push_back(i);
    }
    
    int expected = 0;
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        assert(*it == expected);
        expected++;
    }
    
    // 测试范围for循环
    expected = 0;
    for (const auto& val : vec) {
        assert(val == expected);
        expected++;
    }
}

TEST(test_copy_constructor) {
    MyVector<int> vec1;
    vec1.push_back(1);
    vec1.push_back(2);
    vec1.push_back(3);
    
    MyVector<int> vec2(vec1);
    
    assert(vec2.size() == vec1.size());
    for (size_t i = 0; i < vec1.size(); ++i) {
        assert(vec2[i] == vec1[i]);
    }
    
    // 修改vec2不应影响vec1
    vec2[0] = 100;
    assert(vec1[0] == 1);
}

TEST(test_copy_assignment) {
    MyVector<int> vec1;
    vec1.push_back(1);
    vec1.push_back(2);
    
    MyVector<int> vec2;
    vec2 = vec1;
    
    assert(vec2.size() == vec1.size());
    assert(vec2[0] == 1);
    assert(vec2[1] == 2);
}

TEST(test_move_constructor) {
    MyVector<int> vec1;
    vec1.push_back(1);
    vec1.push_back(2);
    vec1.push_back(3);
    
    size_t original_size = vec1.size();
    MyVector<int> vec2(std::move(vec1));
    
    assert(vec2.size() == original_size);
    assert(vec2[0] == 1);
    assert(vec2[1] == 2);
    assert(vec2[2] == 3);
    
    // vec1应该被清空
    assert(vec1.size() == 0);
}

TEST(test_move_assignment) {
    MyVector<int> vec1;
    vec1.push_back(1);
    vec1.push_back(2);
    
    MyVector<int> vec2;
    vec2 = std::move(vec1);
    
    assert(vec2.size() == 2);
    assert(vec2[0] == 1);
    assert(vec1.size() == 0);
}

TEST(test_reserve) {
    MyVector<int> vec;
    vec.reserve(100);
    
    assert(vec.capacity() >= 100);
    assert(vec.size() == 0);
    
    // 添加元素不应导致重新分配
    size_t capacity_after_reserve = vec.capacity();
    for (int i = 0; i < 50; ++i) {
        vec.push_back(i);
    }
    assert(vec.capacity() == capacity_after_reserve);
}

TEST(test_clear) {
    MyVector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    
    size_t capacity_before = vec.capacity();
    vec.clear();
    
    assert(vec.size() == 0);
    assert(vec.empty());
    // 容量不应改变
    assert(vec.capacity() == capacity_before);
}

TEST(test_pop_back) {
    MyVector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    
    vec.pop_back();
    assert(vec.size() == 2);
    assert(vec.back() == 2);
    
    vec.pop_back();
    assert(vec.size() == 1);
    assert(vec.back() == 1);
}

TEST(test_with_string) {
    MyVector<std::string> vec;
    vec.push_back("Hello");
    vec.push_back("World");
    
    assert(vec.size() == 2);
    assert(vec[0] == "Hello");
    assert(vec[1] == "World");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "     MyVector 测试套件                  " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    // 运行所有测试
    run_test_default_constructor();
    run_test_push_back();
    run_test_dynamic_expansion();
    run_test_access_operators();
    run_test_at_with_bounds_check();
    run_test_iterator_traversal();
    run_test_copy_constructor();
    run_test_copy_assignment();
    run_test_move_constructor();
    run_test_move_assignment();
    run_test_reserve();
    run_test_clear();
    run_test_pop_back();
    run_test_with_string();
    
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "测试结果: " << test_passed << " 通过, " 
              << test_failed << " 失败" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return test_failed > 0 ? 1 : 0;
}
