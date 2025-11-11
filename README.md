# CppFoundry
手动实现 C++ STL 核心组件与智能指针，聚焦底层原理剖析与手写实现细节，用于学习和巩固 C++ 内存管理、容器设计等核心知识点。

## 项目结构

```
CppFoundry/
├── include/          # 头文件
│   └── MyVector.hpp  # Vector 容器实现
├── examples/         # 示例程序
│   └── vector_demo.cpp
├── tests/            # 测试程序
│   └── test_vector.cpp
└── CMakeLists.txt    # CMake 构建配置
```

## 已实现的组件

### MyVector - STL vector 容器

手动实现的动态数组容器，支持以下功能：

#### 核心特性
- **动态扩容**: 当元素数量超过容量时自动扩容（容量翻倍策略）
- **元素插入**: 支持 `push_back()` 在末尾添加元素
- **迭代器遍历**: 支持标准迭代器和范围 for 循环
- **内存管理**: RAII 原则，自动管理内存生命周期

#### 支持的操作
- 构造与析构：默认构造、拷贝构造、移动构造
- 元素访问：`operator[]`, `at()`, `front()`, `back()`
- 修改操作：`push_back()`, `pop_back()`, `clear()`
- 容量管理：`size()`, `capacity()`, `empty()`, `reserve()`
- 迭代器：`begin()`, `end()`, `cbegin()`, `cend()`

## 构建与运行

### 使用 CMake 构建

```bash
# 创建构建目录
mkdir build && cd build

# 配置项目
cmake ..

# 编译
cmake --build .

# 运行示例程序
./vector_demo

# 运行测试
./test_vector
# 或使用 CTest
ctest
```

### 直接编译（无需 CMake）

```bash
# 编译示例程序
g++ -std=c++14 -Wall -Wextra -I./include examples/vector_demo.cpp -o vector_demo

# 运行示例
./vector_demo

# 编译并运行测试
g++ -std=c++14 -Wall -Wextra -I./include tests/test_vector.cpp -o test_vector
./test_vector
```

## 示例代码

```cpp
#include "MyVector.hpp"
#include <iostream>

int main() {
    // 创建 vector
    MyVector<int> vec;
    
    // 添加元素（自动扩容）
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);
    
    // 访问元素
    std::cout << "第一个元素: " << vec.front() << std::endl;
    std::cout << "最后一个元素: " << vec.back() << std::endl;
    
    // 迭代器遍历
    for (const auto& val : vec) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    // 查看大小和容量
    std::cout << "大小: " << vec.size() << std::endl;
    std::cout << "容量: " << vec.capacity() << std::endl;
    
    return 0;
}
```

## 实现细节

### 动态扩容策略
- 初始容量为 0
- 第一次插入时分配容量 1
- 之后每次容量不足时，容量翻倍（容量 × 2）
- 这保证了 `push_back()` 操作的均摊时间复杂度为 O(1)

### 内存管理
- 使用 `new[]` 和 `delete[]` 进行动态内存分配
- 遵循 RAII 原则，在析构函数中自动释放内存
- 实现了拷贝和移动语义，支持深拷贝和高效移动

### 迭代器实现
- 使用原始指针作为迭代器类型
- 支持标准迭代器接口
- 兼容范围 for 循环和 STL 算法

## 学习要点

通过这个实现可以学习到：
1. 动态内存管理和 RAII 原则
2. 容器的动态扩容策略
3. 拷贝构造和移动语义
4. 迭代器的设计与实现
5. 模板编程基础
6. 异常安全性考虑

## License

MIT
