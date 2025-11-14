#include <iostream>
#include <cassert>
#include <stdexcept>
#include <thread>
#include <vector>
#include <chrono>
#include "../include/ptr.h"

// 测试用的简单类
class TestObject {
public:
    int value;
    static int constructCount;
    static int destructCount;
    
    TestObject(int v = 42) : value(v) { 
        ++constructCount; 
        std::cout << "TestObject constructed with value: " << value << std::endl;
    }
    
    ~TestObject() { 
        ++destructCount; 
        std::cout << "TestObject with value " << value << " destructed" << std::endl;
    }
    
    static void resetCounters() {
        constructCount = 0;
        destructCount = 0;
    }
    
    static bool isBalanced() {
        return constructCount == destructCount;
    }
};

int TestObject::constructCount = 0;
int TestObject::destructCount = 0;

// 测试辅助函数
void assert_test(bool condition, const std::string& test_name) {
    if (condition) {
        std::cout << "✓ " << test_name << " PASSED" << std::endl;
    } else {
        std::cout << "✗ " << test_name << " FAILED" << std::endl;
        exit(1);
    }
}

// ========== Uptr 测试 ==========
void test_uptr_basic_functionality() {
    std::cout << "\n=== Testing Uptr Basic Functionality ===" << std::endl;
    TestObject::resetCounters();
    
    {
        // 测试构造和析构
        Uptr<TestObject> ptr1(new TestObject(100));
        assert_test(ptr1.get() != nullptr, "Uptr construction");
        assert_test(ptr1->value == 100, "Uptr dereference");
        assert_test(ptr1->value == 100, "Uptr arrow operator");
        assert_test(static_cast<bool>(ptr1), "Uptr bool conversion");
        
        // 测试空指针构造
        Uptr<TestObject> ptr2;
        assert_test(!ptr2, "Uptr null construction");
        assert_test(ptr2.get() == nullptr, "Uptr null get()");
        
        // 测试reset
        ptr2.reset(new TestObject(200));
        assert_test(ptr2->value == 200, "Uptr reset with new pointer");
        
        ptr1.reset();
        assert_test(!ptr1, "Uptr reset to null");
        
        // 测试release
        TestObject* raw = ptr2.release();
        assert_test(!ptr2, "Uptr after release");
        assert_test(raw->value == 200, "Released pointer value");
        delete raw; // 手动删除
        
        // 测试swap
        Uptr<TestObject> ptr3(new TestObject(300));
        Uptr<TestObject> ptr4(new TestObject(400));
        ptr3.swap(ptr4);
        assert_test(ptr3->value == 400, "Uptr swap - ptr3");
        assert_test(ptr4->value == 300, "Uptr swap - ptr4");
    }
    
    assert_test(TestObject::isBalanced(), "Uptr memory management");
}

void test_uptr_move_semantics() {
    std::cout << "\n=== Testing Uptr Move Semantics ===" << std::endl;
    TestObject::resetCounters();
    
    {
        Uptr<TestObject> ptr1(new TestObject(500));
        Uptr<TestObject> ptr2 = std::move(ptr1);
        
        assert_test(!ptr1, "Uptr after move construction - source");
        assert_test(ptr2->value == 500, "Uptr after move construction - target");
        
        // 移动赋值
        Uptr<TestObject> ptr3(new TestObject(600));
        ptr3 = std::move(ptr2);
        
        assert_test(!ptr2, "Uptr after move assignment - source");
        assert_test(ptr3->value == 500, "Uptr after move assignment - target");
    }
    
    assert_test(TestObject::isBalanced(), "Uptr move semantics memory management");
}

void test_uptr_comparisons() {
    std::cout << "\n=== Testing Uptr Comparisons ===" << std::endl;
    
    Uptr<TestObject> ptr1(new TestObject(700));
    Uptr<TestObject> ptr2(new TestObject(800));
    Uptr<TestObject> ptr3;
    
    assert_test(ptr1 != ptr2, "Uptr inequality");
    assert_test(ptr1 == ptr1, "Uptr self equality");
    assert_test(ptr3 == nullptr, "Uptr null equality");
    assert_test(ptr1 != nullptr, "Uptr non-null inequality");
}

void test_uptr_exceptions() {
    std::cout << "\n=== Testing Uptr Exception Safety ===" << std::endl;
    
    Uptr<TestObject> ptr;
    bool exception_caught = false;
    
    try {
        *ptr; // 应该抛出异常
    } catch (const std::runtime_error& e) {
        exception_caught = true;
    }
    
    assert_test(exception_caught, "Uptr null dereference exception");
}

void test_make_uptr() {
    std::cout << "\n=== Testing make_uptr Factory Function ===" << std::endl;
    TestObject::resetCounters();
    
    {
        auto ptr = make_uptr<TestObject>(999);
        assert_test(ptr->value == 999, "make_uptr construction");
    }
    
    assert_test(TestObject::isBalanced(), "make_uptr memory management");
}

// ========== Sptr 测试 ==========
void test_sptr_basic_functionality() {
    std::cout << "\n=== Testing Sptr Basic Functionality ===" << std::endl;
    TestObject::resetCounters();
    
    {
        Sptr<TestObject> ptr1(new TestObject(1000));
        assert_test(ptr1.use_count() == 1, "Sptr initial reference count");
        assert_test(ptr1->value == 1000, "Sptr dereference");
        
        // 测试拷贝构造
        Sptr<TestObject> ptr2(ptr1);
        assert_test(ptr1.use_count() == 2, "Sptr copy construction - count 1");
        assert_test(ptr2.use_count() == 2, "Sptr copy construction - count 2");
        assert_test(ptr1.get() == ptr2.get(), "Sptr copy construction - same pointer");
        
        // 测试拷贝赋值
        Sptr<TestObject> ptr3;
        ptr3 = ptr1;
        assert_test(ptr1.use_count() == 3, "Sptr copy assignment - count");
        
        // 测试重置
        ptr3.reset();
        assert_test(ptr1.use_count() == 2, "Sptr after reset");
        assert_test(!ptr3, "Sptr after reset - bool");
        
        // 测试自赋值
        ptr1 = ptr1;
        assert_test(ptr1.use_count() == 2, "Sptr self assignment");
    }
    
    assert_test(TestObject::isBalanced(), "Sptr memory management");
}

void test_sptr_move_semantics() {
    std::cout << "\n=== Testing Sptr Move Semantics ===" << std::endl;
    TestObject::resetCounters();
    
    {
        Sptr<TestObject> ptr1(new TestObject(1100));
        assert_test(ptr1.use_count() == 1, "Sptr before move");
        
        Sptr<TestObject> ptr2 = std::move(ptr1);
        assert_test(!ptr1, "Sptr after move construction - source");
        assert_test(ptr2.use_count() == 1, "Sptr after move construction - count");
        assert_test(ptr2->value == 1100, "Sptr after move construction - value");
        
        // 移动赋值
        Sptr<TestObject> ptr3(new TestObject(1200));
        ptr3 = std::move(ptr2);
        assert_test(!ptr2, "Sptr after move assignment - source");
        assert_test(ptr3.use_count() == 1, "Sptr after move assignment - count");
        assert_test(ptr3->value == 1100, "Sptr after move assignment - value");
    }
    
    assert_test(TestObject::isBalanced(), "Sptr move semantics memory management");
}

void test_make_sptr() {
    std::cout << "\n=== Testing make_sptr Factory Function ===" << std::endl;
    TestObject::resetCounters();
    
    {
        auto ptr = make_sptr<TestObject>(1300);
        assert_test(ptr.use_count() == 1, "make_sptr reference count");
        assert_test(ptr->value == 1300, "make_sptr value");
    }
    
    assert_test(TestObject::isBalanced(), "make_sptr memory management");
}

// ========== Wptr 测试 ==========
void test_wptr_basic_functionality() {
    std::cout << "\n=== Testing Wptr Basic Functionality ===" << std::endl;
    TestObject::resetCounters();
    
    {
        Sptr<TestObject> sptr(new TestObject(1400));
        Wptr<TestObject> wptr(sptr);
        
        assert_test(!wptr.expired(), "Wptr not expired when Sptr exists");
        assert_test(wptr.use_count() == 1, "Wptr use_count");
        
        // 测试lock
        auto locked = wptr.lock();
        assert_test(locked != nullptr, "Wptr lock returns valid Sptr");
        assert_test(locked->value == 1400, "Wptr lock - value access");
        assert_test(sptr.use_count() == 2, "Wptr lock increases ref count");
        
        // 释放强引用
        sptr.reset();
        locked.reset();
        
        assert_test(wptr.expired(), "Wptr expired after Sptr destroyed");
        assert_test(wptr.use_count() == 0, "Wptr use_count after expiry");
        
        auto expired_lock = wptr.lock();
        assert_test(!expired_lock, "Wptr lock on expired returns null");
    }
    
    assert_test(TestObject::isBalanced(), "Wptr memory management");
}

void test_wptr_copy_and_move() {
    std::cout << "\n=== Testing Wptr Copy and Move ===" << std::endl;
    TestObject::resetCounters();
    
    {
        Sptr<TestObject> sptr(new TestObject(1500));
        Wptr<TestObject> wptr1(sptr);
        
        // 拷贝构造
        Wptr<TestObject> wptr2(wptr1);
        assert_test(!wptr2.expired(), "Wptr copy construction");
        
        // 拷贝赋值
        Wptr<TestObject> wptr3;
        wptr3 = wptr1;
        assert_test(!wptr3.expired(), "Wptr copy assignment");
        
        // 移动构造
        Wptr<TestObject> wptr4 = std::move(wptr2);
        assert_test(!wptr4.expired(), "Wptr move construction");
        
        // 移动赋值
        Wptr<TestObject> wptr5;
        wptr5 = std::move(wptr3);
        assert_test(!wptr5.expired(), "Wptr move assignment");
    }
    
    assert_test(TestObject::isBalanced(), "Wptr copy/move memory management");
}

// ========== 循环引用测试 ==========
struct Node {
    int data;
    Sptr<Node> next;
    Wptr<Node> parent;
    
    Node(int d) : data(d) {}
    ~Node() {
        std::cout << "Node " << data << " destroyed" << std::endl;
    }
};

void test_circular_reference() {
    std::cout << "\n=== Testing Circular Reference Prevention ===" << std::endl;
    
    {
        auto node1 = make_sptr<Node>(1);
        auto node2 = make_sptr<Node>(2);
        auto node3 = make_sptr<Node>(3);
        
        // 建立链表: node1 -> node2 -> node3
        node1->next = node2;
        node2->next = node3;
        
        // 使用弱引用避免循环
        node2->parent = node1;
        node3->parent = node2;
        
        assert_test(node1.use_count() == 1, "Node1 ref count (no cycle)");
        assert_test(node2.use_count() == 2, "Node2 ref count (referenced by node1)");
        assert_test(node3.use_count() == 2, "Node3 ref count (referenced by node2)");
        
        // 验证弱引用工作正常
        auto parent = node3->parent.lock();
        assert_test(parent && parent->data == 2, "Weak reference lock works");
    }
    
    std::cout << "All nodes should be destroyed above" << std::endl;
}

// ========== 多线程测试 ==========
void test_sptr_thread_safety() {
    std::cout << "\n=== Testing Sptr Thread Safety ===" << std::endl;
    
    const int num_threads = 4;
    const int operations_per_thread = 1000;
    
    auto shared_obj = make_sptr<TestObject>(2000);
    std::vector<std::thread> threads;
    
    // 每个线程都会复制和释放共享指针
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&shared_obj, operations_per_thread]() {
            for (int j = 0; j < operations_per_thread; ++j) {
                Sptr<TestObject> local_copy = shared_obj;
                // 简单的访问操作
                volatile int value = local_copy->value;
                (void)value; // 避免未使用变量警告
            }
        });
    }
    
    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }
    
    assert_test(shared_obj.use_count() == 1, "Sptr thread safety - final ref count");
    assert_test(shared_obj->value == 2000, "Sptr thread safety - value unchanged");
}

// ========== 性能测试 ==========
void test_performance() {
    std::cout << "\n=== Performance Tests ===" << std::endl;
    
    const int iterations = 100000;
    
    // Uptr 性能测试
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        auto ptr = make_uptr<TestObject>(i);
        volatile int value = ptr->value;
        (void)value;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto uptr_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Sptr 性能测试
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        auto ptr = make_sptr<TestObject>(i);
        volatile int value = ptr->value;
        (void)value;
    }
    end = std::chrono::high_resolution_clock::now();
    auto sptr_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Uptr " << iterations << " operations: " << uptr_time.count() << " μs" << std::endl;
    std::cout << "Sptr " << iterations << " operations: " << sptr_time.count() << " μs" << std::endl;
    std::cout << "Sptr overhead: " << (double)sptr_time.count() / uptr_time.count() << "x" << std::endl;
}

// ========== 主测试函数 ==========
int main() {
    std::cout << "Starting Smart Pointer Tests...\n" << std::endl;
    
    try {
        // Uptr 测试
        test_uptr_basic_functionality();
        test_uptr_move_semantics();
        test_uptr_comparisons();
        test_uptr_exceptions();
        test_make_uptr();
        
        // Sptr 测试
        test_sptr_basic_functionality();
        test_sptr_move_semantics();
        test_make_sptr();
        
        // Wptr 测试
        test_wptr_basic_functionality();
        test_wptr_copy_and_move();
        
        // 高级测试
        test_circular_reference();
        test_sptr_thread_safety();
        test_performance();
        
        std::cout << "\n🎉 All tests passed! Your smart pointer implementation is working correctly!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "\n❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
