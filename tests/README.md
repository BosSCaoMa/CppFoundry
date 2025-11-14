# Smart Pointer Tests

这个目录包含了对智能指针实现的全面测试。

## 测试文件说明

### 1. `test_quick.cpp` - 快速功能测试
- 基本功能验证
- 适合快速检查核心功能是否正常
- 运行时间短，适合日常开发

### 2. `test_smart_pointers.cpp` - 全面功能测试
- 完整的功能测试套件
- 包括性能测试和多线程测试
- 内存管理验证
- 循环引用测试

### 3. `test_edge_cases.cpp` - 边界情况测试
- 空指针处理
- 异常安全测试
- 自赋值测试
- 边界条件验证

## 如何运行测试

### 方法1: 使用 CMake (推荐)

```powershell
# 在 tests 目录下
mkdir build
cd build
cmake ..
cmake --build .
ctest --output-on-failure
```

### 方法2: 直接编译 (需要支持 C++17)

```powershell
# 快速测试
g++ -std=c++17 -I../include test_quick.cpp -o test_quick
./test_quick

# 全面测试 (需要链接线程库)
g++ -std=c++17 -I../include test_smart_pointers.cpp -o test_smart_pointers -pthread
./test_smart_pointers

# 边界情况测试
g++ -std=c++17 -I../include test_edge_cases.cpp -o test_edge_cases
./test_edge_cases
```

### 方法3: 在 Visual Studio 中

1. 打开 Visual Studio
2. 创建新的控制台项目
3. 添加测试文件和头文件
4. 设置 C++17 标准
5. 编译并运行

## 测试内容覆盖

### Uptr (Unique Pointer) 测试
- ✅ 构造和析构
- ✅ 移动语义
- ✅ 解引用操作
- ✅ 所有权转移 (release)
- ✅ 重置操作 (reset)
- ✅ 比较操作符
- ✅ 异常处理

### Sptr (Shared Pointer) 测试
- ✅ 引用计数管理
- ✅ 拷贝和移动语义
- ✅ 多线程安全性
- ✅ 内存管理
- ✅ 工厂函数 (make_sptr)

### Wptr (Weak Pointer) 测试
- ✅ 弱引用语义
- ✅ 过期检测 (expired)
- ✅ 锁定操作 (lock)
- ✅ 循环引用预防

### 高级特性测试
- ✅ 循环引用预防
- ✅ 多线程安全
- ✅ 异常安全
- ✅ 性能基准测试

## 预期输出

运行成功时，您应该看到类似这样的输出：

```
Smart Pointer Quick Tests
========================

=== Testing Uptr ===
TestObj(42) created
TestObj(42) destroyed
Uptr tests passed!

=== Testing Sptr ===
TestObj(100) created
TestObj(100) destroyed
Sptr tests passed!

=== Testing Wptr ===
TestObj(200) created
TestObj(200) destroyed
Wptr tests passed!

=== Testing Circular Reference ===
Node 1 destroyed
Node 2 destroyed
Both nodes should be destroyed above
Circular reference test passed!

🎉 All quick tests passed!
```

## 故障排除

如果测试失败，请检查：

1. **编译错误**: 确保使用 C++17 标准
2. **链接错误**: 多线程测试需要链接 pthread
3. **运行时错误**: 检查智能指针实现中的逻辑错误
4. **内存泄漏**: 使用 valgrind (Linux) 或 Application Verifier (Windows) 检查

## 注意事项

- 测试使用了一些 C++17 特性，确保编译器支持
- 多线程测试可能在某些系统上运行时间较长
- 性能测试结果受系统负载影响，仅供参考
