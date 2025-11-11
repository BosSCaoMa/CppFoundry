#include "../include/memory/shared_ptr.hpp"
#include <iostream>
#include <cassert>
#include <string>

using namespace cppfoundry;

// Test class to track construction/destruction
class TestObject {
public:
    static int instance_count;
    int value;
    
    TestObject(int v = 0) : value(v) {
        ++instance_count;
        std::cout << "TestObject(" << value << ") constructed. Count: " << instance_count << std::endl;
    }
    
    ~TestObject() {
        --instance_count;
        std::cout << "TestObject(" << value << ") destroyed. Count: " << instance_count << std::endl;
    }
};

int TestObject::instance_count = 0;

void test_basic_construction() {
    std::cout << "\n=== Test: Basic Construction ===" << std::endl;
    
    {
        shared_ptr<int> p1;
        assert(p1.get() == nullptr);
        assert(!p1);
        assert(p1.use_count() == 0);
        
        shared_ptr<int> p2(new int(42));
        assert(p2.get() != nullptr);
        assert(p2);
        assert(*p2 == 42);
        assert(p2.use_count() == 1);
        
        shared_ptr<TestObject> p3(new TestObject(100));
        assert(p3->value == 100);
        assert(p3.use_count() == 1);
        assert(TestObject::instance_count == 1);
    }
    
    assert(TestObject::instance_count == 0);
    std::cout << "✓ Basic construction test passed" << std::endl;
}

void test_reference_counting() {
    std::cout << "\n=== Test: Reference Counting ===" << std::endl;
    
    {
        shared_ptr<TestObject> p1(new TestObject(200));
        assert(p1.use_count() == 1);
        assert(TestObject::instance_count == 1);
        
        {
            // Copy constructor - increments reference count
            shared_ptr<TestObject> p2(p1);
            assert(p1.use_count() == 2);
            assert(p2.use_count() == 2);
            assert(p1.get() == p2.get());  // Point to same object
            assert(TestObject::instance_count == 1);
            
            {
                // Copy assignment - increments reference count
                shared_ptr<TestObject> p3;
                p3 = p1;
                assert(p1.use_count() == 3);
                assert(p2.use_count() == 3);
                assert(p3.use_count() == 3);
                assert(TestObject::instance_count == 1);
            }
            
            // p3 destroyed - decrements reference count
            assert(p1.use_count() == 2);
            assert(p2.use_count() == 2);
            assert(TestObject::instance_count == 1);
        }
        
        // p2 destroyed - decrements reference count
        assert(p1.use_count() == 1);
        assert(TestObject::instance_count == 1);
    }
    
    // p1 destroyed - last reference, object is deleted
    assert(TestObject::instance_count == 0);
    std::cout << "✓ Reference counting test passed" << std::endl;
}

void test_move_semantics() {
    std::cout << "\n=== Test: Move Semantics ===" << std::endl;
    
    {
        shared_ptr<TestObject> p1(new TestObject(300));
        assert(p1.use_count() == 1);
        
        // Move constructor - transfers ownership without changing ref count
        shared_ptr<TestObject> p2(std::move(p1));
        assert(p1.get() == nullptr);
        assert(p1.use_count() == 0);
        assert(p2.use_count() == 1);  // Still only 1 reference
        assert(p2->value == 300);
        assert(TestObject::instance_count == 1);
        
        // Move assignment
        shared_ptr<TestObject> p3;
        p3 = std::move(p2);
        assert(p2.get() == nullptr);
        assert(p2.use_count() == 0);
        assert(p3.use_count() == 1);
        assert(p3->value == 300);
        assert(TestObject::instance_count == 1);
    }
    
    assert(TestObject::instance_count == 0);
    std::cout << "✓ Move semantics test passed" << std::endl;
}

void test_reset() {
    std::cout << "\n=== Test: Reset ===" << std::endl;
    
    {
        shared_ptr<TestObject> p1(new TestObject(400));
        shared_ptr<TestObject> p2 = p1;
        assert(p1.use_count() == 2);
        assert(TestObject::instance_count == 1);
        
        // Reset p1 - decrements ref count but doesn't delete (p2 still holds ref)
        p1.reset();
        assert(p1.get() == nullptr);
        assert(p1.use_count() == 0);
        assert(p2.use_count() == 1);
        assert(TestObject::instance_count == 1);
        
        // Reset with new object
        p1.reset(new TestObject(500));
        assert(p1.use_count() == 1);
        assert(p2.use_count() == 1);
        assert(p1->value == 500);
        assert(p2->value == 400);
        assert(TestObject::instance_count == 2);
        
        // Reset p2 - deletes object 400
        p2.reset();
        assert(TestObject::instance_count == 1);
    }
    
    assert(TestObject::instance_count == 0);
    std::cout << "✓ Reset test passed" << std::endl;
}

void test_make_shared() {
    std::cout << "\n=== Test: make_shared ===" << std::endl;
    
    {
        auto p1 = make_shared<int>(42);
        assert(*p1 == 42);
        assert(p1.use_count() == 1);
        
        auto p2 = make_shared<TestObject>(600);
        assert(p2->value == 600);
        assert(p2.use_count() == 1);
        assert(TestObject::instance_count == 1);
        
        auto p3 = p2;
        assert(p2.use_count() == 2);
        assert(p3.use_count() == 2);
        
        auto p4 = make_shared<std::string>("Hello, World!");
        assert(*p4 == "Hello, World!");
    }
    
    assert(TestObject::instance_count == 0);
    std::cout << "✓ make_shared test passed" << std::endl;
}

void test_custom_deleter() {
    std::cout << "\n=== Test: Custom Deleter ===" << std::endl;
    
    bool deleter_called = false;
    
    {
        auto deleter = [&deleter_called](TestObject* p) {
            deleter_called = true;
            std::cout << "Custom deleter called" << std::endl;
            delete p;
        };
        
        shared_ptr<TestObject> p1(new TestObject(700), deleter);
        assert(p1.use_count() == 1);
        assert(TestObject::instance_count == 1);
        
        {
            shared_ptr<TestObject> p2 = p1;
            assert(p1.use_count() == 2);
            assert(!deleter_called);
        }
        
        assert(p1.use_count() == 1);
        assert(!deleter_called);  // Not yet
    }
    
    assert(deleter_called);  // Called when last reference destroyed
    assert(TestObject::instance_count == 0);
    std::cout << "✓ Custom deleter test passed" << std::endl;
}

void test_unique_check() {
    std::cout << "\n=== Test: Unique Check ===" << std::endl;
    
    shared_ptr<TestObject> p1(new TestObject(800));
    assert(p1.unique());
    
    shared_ptr<TestObject> p2 = p1;
    assert(!p1.unique());
    assert(!p2.unique());
    
    p2.reset();
    assert(p1.unique());
    
    std::cout << "✓ Unique check test passed" << std::endl;
}

void test_comparison_operators() {
    std::cout << "\n=== Test: Comparison Operators ===" << std::endl;
    
    shared_ptr<int> p1(new int(42));
    shared_ptr<int> p2 = p1;
    shared_ptr<int> p3(new int(42));
    shared_ptr<int> p4;
    
    assert(p1 == p2);  // Same pointer
    assert(p1 != p3);  // Different pointers
    assert(p4 == nullptr);
    assert(nullptr == p4);
    assert(p1 != nullptr);
    assert(nullptr != p1);
    
    std::cout << "✓ Comparison operators test passed" << std::endl;
}

void test_swap() {
    std::cout << "\n=== Test: Swap ===" << std::endl;
    
    shared_ptr<TestObject> p1(new TestObject(900));
    shared_ptr<TestObject> p2(new TestObject(1000));
    
    TestObject* ptr1 = p1.get();
    TestObject* ptr2 = p2.get();
    
    p1.swap(p2);
    
    assert(p1.get() == ptr2);
    assert(p2.get() == ptr1);
    assert(p1->value == 1000);
    assert(p2->value == 900);
    
    std::cout << "✓ Swap test passed" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "    shared_ptr Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        test_basic_construction();
        test_reference_counting();
        test_move_semantics();
        test_reset();
        test_make_shared();
        test_custom_deleter();
        test_unique_check();
        test_comparison_operators();
        test_swap();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "  ✓ All shared_ptr tests passed!" << std::endl;
        std::cout << "========================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "✗ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
