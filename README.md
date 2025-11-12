# CppFoundry
手动实现 C++ STL 核心组件与智能指针，聚焦底层原理剖析与手写实现细节，用于学习和巩固 C++ 内存管理、容器设计等核心知识点。



# 总结

## const成员函数
```cpp
    T& operator*() const { return *ptr; }
    T* operator->() const noexcept { return ptr; }
```
在类中，如果一个成员函数被声明为 const（在函数参数列表后加上 const 关键字），它就向编译器和程序员做出了一个承诺：“调用这个函数不会改变对象的任何成员变量。”

C++ 的 const 成员函数调用规则
const 成员函数：可以被 const 对象 和 非 const 对象 调用。

1. 它向编译器承诺“我不会修改对象”，所以无论对象本身是否可修改，调用它都是安全的。
非 const 成员函数：只能被 非 const 对象 调用。

2. 因为它可能会修改对象，所以编译器禁止 const 对象调用它，以信守 const 对象的“只读”承诺。