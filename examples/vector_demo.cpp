#include <iostream>
#include <string>
#include "../include/MyVector.hpp"

// 演示 MyVector 的各种功能
void demonstrate_basic_operations() {
    std::cout << "=== 基本操作演示 ===" << std::endl;
    
    // 创建 vector
    MyVector<int> vec;
    
    // 添加元素（动态扩容）
    std::cout << "添加元素: 10, 20, 30, 40, 50" << std::endl;
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);
    vec.push_back(40);
    vec.push_back(50);
    
    std::cout << "大小: " << vec.size() << ", 容量: " << vec.capacity() << std::endl;
    
    // 访问元素
    std::cout << "元素访问: ";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << " ";
    }
    std::cout << std::endl;
    
    std::cout << "第一个元素: " << vec.front() << std::endl;
    std::cout << "最后一个元素: " << vec.back() << std::endl;
    std::cout << std::endl;
}

void demonstrate_iterator_traversal() {
    std::cout << "=== 迭代器遍历演示 ===" << std::endl;
    
    MyVector<std::string> words;
    words.push_back("Hello");
    words.push_back("World");
    words.push_back("C++");
    words.push_back("Vector");
    
    std::cout << "使用迭代器遍历: ";
    for (auto it = words.begin(); it != words.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    std::cout << "使用范围for循环遍历: ";
    for (const auto& word : words) {
        std::cout << word << " ";
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

void demonstrate_dynamic_expansion() {
    std::cout << "=== 动态扩容演示 ===" << std::endl;
    
    MyVector<int> vec;
    std::cout << "初始 - 大小: " << vec.size() << ", 容量: " << vec.capacity() << std::endl;
    
    for (int i = 1; i <= 10; ++i) {
        vec.push_back(i);
        std::cout << "添加元素 " << i << " - 大小: " << vec.size() 
                  << ", 容量: " << vec.capacity() << std::endl;
    }
    std::cout << std::endl;
}

void demonstrate_copy_and_move() {
    std::cout << "=== 拷贝和移动语义演示 ===" << std::endl;
    
    MyVector<int> vec1;
    vec1.push_back(1);
    vec1.push_back(2);
    vec1.push_back(3);
    
    // 拷贝构造
    MyVector<int> vec2(vec1);
    std::cout << "拷贝构造后，vec2: ";
    for (const auto& val : vec2) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    // 移动构造
    MyVector<int> vec3(std::move(vec2));
    std::cout << "移动构造后，vec3: ";
    for (const auto& val : vec3) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    std::cout << "移动后，vec2大小: " << vec2.size() << std::endl;
    std::cout << std::endl;
}

void demonstrate_reserve() {
    std::cout << "=== 预留容量演示 ===" << std::endl;
    
    MyVector<int> vec;
    std::cout << "初始容量: " << vec.capacity() << std::endl;
    
    vec.reserve(100);
    std::cout << "预留100后的容量: " << vec.capacity() << std::endl;
    
    // 添加元素不会导致重新分配
    for (int i = 0; i < 50; ++i) {
        vec.push_back(i);
    }
    std::cout << "添加50个元素后的容量: " << vec.capacity() << std::endl;
    std::cout << "当前大小: " << vec.size() << std::endl;
    std::cout << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   MyVector - STL Vector 手动实现演示   " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    demonstrate_basic_operations();
    demonstrate_iterator_traversal();
    demonstrate_dynamic_expansion();
    demonstrate_copy_and_move();
    demonstrate_reserve();
    
    std::cout << "========================================" << std::endl;
    std::cout << "           演示完成！                    " << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
