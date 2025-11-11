#include "../include/memory/unique_ptr.hpp"
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
    
    TestObject(const TestObject&) = delete;
    TestObject& operator=(const TestObject&) = delete;
};

int TestObject::instance_count = 0;

void test_basic_construction() {
    std::cout << "\n=== Test: Basic Construction ===" << std::endl;
    
    {
        unique_ptr<int> p1;
        assert(p1.get() == nullptr);
        assert(!p1);
        
        unique_ptr<int> p2(new int(42));
        assert(p2.get() != nullptr);
        assert(p2);
        assert(*p2 == 42);
        
        unique_ptr<TestObject> p3(new TestObject(100));
        assert(p3->value == 100);
    }
    
    assert(TestObject::instance_count == 0);
    std::cout << "✓ Basic construction test passed" << std::endl;
}

void test_move_semantics() {
    std::cout << "\n=== Test: Move Semantics ===" << std::endl;
    
    {
        unique_ptr<TestObject> p1(new TestObject(200));
        assert(p1.get() != nullptr);
        assert(TestObject::instance_count == 1);
        
        // Move constructor
        unique_ptr<TestObject> p2(std::move(p1));
        assert(p1.get() == nullptr);  // p1 is now empty
        assert(p2.get() != nullptr);
        assert(p2->value == 200);
        assert(TestObject::instance_count == 1);
        
        // Move assignment
        unique_ptr<TestObject> p3;
        p3 = std::move(p2);
        assert(p2.get() == nullptr);  // p2 is now empty
        assert(p3.get() != nullptr);
        assert(p3->value == 200);
        assert(TestObject::instance_count == 1);
    }
    
    assert(TestObject::instance_count == 0);
    std::cout << "✓ Move semantics test passed" << std::endl;
}

void test_reset_and_release() {
    std::cout << "\n=== Test: Reset and Release ===" << std::endl;
    
    {
        unique_ptr<TestObject> p1(new TestObject(300));
        assert(TestObject::instance_count == 1);
        
        // Reset with new object
        p1.reset(new TestObject(400));
        assert(TestObject::instance_count == 1);
        assert(p1->value == 400);
        
        // Release ownership
        TestObject* raw = p1.release();
        assert(p1.get() == nullptr);
        assert(raw->value == 400);
        assert(TestObject::instance_count == 1);
        
        // Must delete manually after release
        delete raw;
        assert(TestObject::instance_count == 0);
        
        // Reset to nullptr
        p1.reset(new TestObject(500));
        assert(TestObject::instance_count == 1);
        p1.reset();
        assert(TestObject::instance_count == 0);
        assert(p1.get() == nullptr);
    }
    
    std::cout << "✓ Reset and release test passed" << std::endl;
}

void test_make_unique() {
    std::cout << "\n=== Test: make_unique ===" << std::endl;
    
    {
        auto p1 = make_unique<int>(42);
        assert(*p1 == 42);
        
        auto p2 = make_unique<TestObject>(600);
        assert(p2->value == 600);
        assert(TestObject::instance_count == 1);
        
        auto p3 = make_unique<std::string>("Hello, World!");
        assert(*p3 == "Hello, World!");
    }
    
    assert(TestObject::instance_count == 0);
    std::cout << "✓ make_unique test passed" << std::endl;
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
        
        unique_ptr<TestObject, decltype(deleter)> p(new TestObject(700), deleter);
        assert(TestObject::instance_count == 1);
    }
    
    assert(deleter_called);
    assert(TestObject::instance_count == 0);
    std::cout << "✓ Custom deleter test passed" << std::endl;
}

void test_array_specialization() {
    std::cout << "\n=== Test: Array Specialization ===" << std::endl;
    
    {
        unique_ptr<int[]> arr(new int[5]);
        for (int i = 0; i < 5; ++i) {
            arr[i] = i * 10;
        }
        
        assert(arr[0] == 0);
        assert(arr[1] == 10);
        assert(arr[4] == 40);
    }
    
    std::cout << "✓ Array specialization test passed" << std::endl;
}

void test_comparison_operators() {
    std::cout << "\n=== Test: Comparison Operators ===" << std::endl;
    
    unique_ptr<int> p1(new int(42));
    unique_ptr<int> p2(new int(42));
    unique_ptr<int> p3;
    
    assert(p1 != p2);  // Different pointers
    assert(p3 == nullptr);
    assert(nullptr == p3);
    assert(p1 != nullptr);
    assert(nullptr != p1);
    
    std::cout << "✓ Comparison operators test passed" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "    unique_ptr Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        test_basic_construction();
        test_move_semantics();
        test_reset_and_release();
        test_make_unique();
        test_custom_deleter();
        test_array_specialization();
        test_comparison_operators();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "  ✓ All unique_ptr tests passed!" << std::endl;
        std::cout << "========================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "✗ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
