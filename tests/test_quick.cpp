#include <iostream>
#include <cassert>
#include "../include/ptr.h"

// 简单测试类
class TestObj {
public:
    int value;
    TestObj(int v) : value(v) {
        std::cout << "TestObj(" << v << ") created\n";
    }
    ~TestObj() {
        std::cout << "TestObj(" << value << ") destroyed\n";
    }
};

void test_uptr() {
    std::cout << "\n=== Testing Uptr ===" << std::endl;
    
    // 基本功能
    auto ptr1 = make_uptr<TestObj>(42);
    assert(ptr1->value == 42);
    assert(ptr1.get() != nullptr);
    assert(static_cast<bool>(ptr1));
    
    // 移动语义
    auto ptr2 = std::move(ptr1);
    assert(!ptr1);
    assert(ptr2->value == 42);
    
    // Reset 和 release
    auto* raw = ptr2.release();
    assert(!ptr2);
    assert(raw->value == 42);
    delete raw;
    
    std::cout << "Uptr tests passed!" << std::endl;
}

void test_sptr() {
    std::cout << "\n=== Testing Sptr ===" << std::endl;
    
    // 基本功能
    auto ptr1 = make_sptr<TestObj>(100);
    assert(ptr1.use_count() == 1);
    assert(ptr1->value == 100);
    
    // 共享语义
    auto ptr2 = ptr1;
    assert(ptr1.use_count() == 2);
    assert(ptr2.use_count() == 2);
    assert(ptr1.get() == ptr2.get());
    
    // 移动语义
    auto ptr3 = std::move(ptr2);
    assert(!ptr2);
    assert(ptr1.use_count() == 2);
    assert(ptr3.use_count() == 2);
    
    std::cout << "Sptr tests passed!" << std::endl;
}

void test_wptr() {
    std::cout << "\n=== Testing Wptr ===" << std::endl;
    
    Wptr<TestObj> weak;
    
    {
        auto shared = make_sptr<TestObj>(200);
        weak = Wptr<TestObj>(shared);
        
        assert(!weak.expired());
        assert(weak.use_count() == 1);
        
        auto locked = weak.lock();
        assert(locked != nullptr);
        assert(locked->value == 200);
        assert(shared.use_count() == 2);
    }
    
    // shared 超出作用域后
    assert(weak.expired());
    assert(weak.use_count() == 0);
    
    auto locked = weak.lock();
    assert(!locked);
    
    std::cout << "Wptr tests passed!" << std::endl;
}

void test_circular_reference() {
    std::cout << "\n=== Testing Circular Reference ===" << std::endl;
    
    struct Node {
        int data;
        Sptr<Node> child;
        Wptr<Node> parent; // 使用弱引用避免循环
        
        Node(int d) : data(d) {}
        ~Node() { std::cout << "Node " << data << " destroyed\n"; }
    };
    
    {
        auto parent = make_sptr<Node>(1);
        auto child = make_sptr<Node>(2);
        
        parent->child = child;
        child->parent = parent; // 弱引用，不增加引用计数
        
        assert(parent.use_count() == 1); // 只有我们持有
        assert(child.use_count() == 2);  // parent->child 也持有
        
        auto locked_parent = child->parent.lock();
        assert(locked_parent && locked_parent->data == 1);
    }
    
    std::cout << "Both nodes should be destroyed above\n";
    std::cout << "Circular reference test passed!" << std::endl;
}

int main() {
    std::cout << "Smart Pointer Quick Tests\n";
    std::cout << "========================\n";
    
    try {
        test_uptr();
        test_sptr();
        test_wptr();
        test_circular_reference();
        
        std::cout << "\n🎉 All quick tests passed!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "\n❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
