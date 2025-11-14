#include <iostream>
#include <cassert>
#include <stdexcept>
#include "../include/ptr.h"

void test_null_pointer_handling() {
    std::cout << "\n=== Testing Null Pointer Handling ===" << std::endl;
    
    // Uptr 空指针测试
    {
        Uptr<int> null_uptr;
        assert(!null_uptr);
        assert(null_uptr.get() == nullptr);
        
        // 测试空指针解引用异常
        bool exception_caught = false;
        try {
            *null_uptr; // 应该抛出异常
        } catch (const std::runtime_error&) {
            exception_caught = true;
        }
        assert(exception_caught);
        
        // 测试空指针重置
        null_uptr.reset(); // 应该安全
        null_uptr.reset(nullptr); // 也应该安全
    }
    
    // Sptr 空指针测试
    {
        Sptr<int> null_sptr;
        assert(!null_sptr);
        assert(null_sptr.get() == nullptr);
        assert(null_sptr.use_count() == 0);
        
        // 测试空指针解引用异常
        bool exception_caught = false;
        try {
            *null_sptr; // 应该抛出异常
        } catch (const std::runtime_error&) {
            exception_caught = true;
        }
        assert(exception_caught);
        
        // 测试空指针重置
        null_sptr.reset(); // 应该安全
        null_sptr.reset(nullptr); // 也应该安全
    }
    
    std::cout << "Null pointer handling tests passed!" << std::endl;
}

void test_self_assignment() {
    std::cout << "\n=== Testing Self Assignment ===" << std::endl;
    
    // Uptr 自赋值 (移动)
    {
        auto uptr = make_uptr<int>(42);
        int* original_ptr = uptr.get();
        
        uptr = std::move(uptr); // 自移动赋值
        
        // 自移动赋值后状态是未定义的，但不应该崩溃
        // 在实际实现中，这通常会导致空指针
        std::cout << "Uptr self-move assignment completed" << std::endl;
    }
    
    // Sptr 自赋值
    {
        auto sptr = make_sptr<int>(100);
        int use_count_before = sptr.use_count();
        
        sptr = sptr; // 自拷贝赋值
        assert(sptr.use_count() == use_count_before);
        assert(*sptr == 100);
        
        sptr = std::move(sptr); // 自移动赋值
        // 自移动赋值后状态未定义，但不应该崩溃
        std::cout << "Sptr self-assignment completed" << std::endl;
    }
    
    std::cout << "Self assignment tests passed!" << std::endl;
}

void test_reset_with_same_pointer() {
    std::cout << "\n=== Testing Reset with Same Pointer ===" << std::endl;
    
    // Uptr reset 不同指针测试
    {
        Uptr<int> uptr(new int(42));
        
        // 重置为新指针
        uptr.reset(new int(100));
        assert(*uptr == 100);
        
        // 重置为空
        uptr.reset();
        assert(!uptr);
        
        // 从空状态重置为新指针
        uptr.reset(new int(200));
        assert(*uptr == 200);
    }
    
    // Sptr reset 相同指针测试
    {
        int* raw = new int(100);
        Sptr<int> sptr(raw);
        
        // Sptr::reset 检查相同指针
        sptr.reset(raw); // 应该直接返回，不创建新控制块
        assert(sptr.get() == raw);
        assert(*sptr == 100);
    }
    
    std::cout << "Reset tests passed!" << std::endl;
}

void test_comparison_operators() {
    std::cout << "\n=== Testing Comparison Operators ===" << std::endl;
    
    // Uptr 比较
    {
        auto uptr1 = make_uptr<int>(1);
        auto uptr2 = make_uptr<int>(2);
        Uptr<int> null_uptr;
        
        assert(uptr1 != uptr2);
        assert(uptr1 == uptr1);
        assert(null_uptr == nullptr);
        assert(uptr1 != nullptr);
        assert(!(null_uptr != nullptr));
    }
    
    // Sptr 比较
    {
        auto sptr1 = make_sptr<int>(1);
        auto sptr2 = make_sptr<int>(2);
        auto sptr1_copy = sptr1;
        Sptr<int> null_sptr;
        
        assert(sptr1 != sptr2);
        assert(sptr1 == sptr1_copy); // 指向同一对象
        assert(null_sptr == nullptr);
        assert(sptr1 != nullptr);
    }
    
    std::cout << "Comparison operator tests passed!" << std::endl;
}

void test_swap_operations() {
    std::cout << "\n=== Testing Swap Operations ===" << std::endl;
    
    // Uptr swap
    {
        auto uptr1 = make_uptr<int>(111);
        auto uptr2 = make_uptr<int>(222);
        
        int* ptr1_before = uptr1.get();
        int* ptr2_before = uptr2.get();
        
        uptr1.swap(uptr2);
        
        assert(uptr1.get() == ptr2_before);
        assert(uptr2.get() == ptr1_before);
        assert(*uptr1 == 222);
        assert(*uptr2 == 111);
        
        // 测试非成员 swap
        swap(uptr1, uptr2);
        assert(*uptr1 == 111);
        assert(*uptr2 == 222);
    }
    
    // Sptr swap
    {
        auto sptr1 = make_sptr<int>(333);
        auto sptr2 = make_sptr<int>(444);
        
        int* ptr1_before = sptr1.get();
        int* ptr2_before = sptr2.get();
        
        sptr1.swap(sptr2);
        
        assert(sptr1.get() == ptr2_before);
        assert(sptr2.get() == ptr1_before);
        assert(*sptr1 == 444);
        assert(*sptr2 == 333);
    }
    
    std::cout << "Swap operation tests passed!" << std::endl;
}

void test_wptr_edge_cases() {
    std::cout << "\n=== Testing Wptr Edge Cases ===" << std::endl;
    
    // 从空 Sptr 构造 Wptr
    {
        Sptr<int> empty_sptr;
        Wptr<int> wptr(empty_sptr);
        
        assert(wptr.expired());
        assert(wptr.use_count() == 0);
        
        auto locked = wptr.lock();
        assert(!locked);
    }
    
    // Wptr 在 Sptr 销毁后的行为
    {
        Wptr<int> wptr;
        
        {
            auto sptr = make_sptr<int>(555);
            wptr = Wptr<int>(sptr);
            
            assert(!wptr.expired());
            assert(wptr.use_count() == 1);
            
            // 多个 Wptr 指向同一对象
            Wptr<int> wptr2 = wptr;
            assert(!wptr2.expired());
        } // sptr 销毁
        
        assert(wptr.expired());
        assert(wptr.use_count() == 0);
        
        auto locked = wptr.lock();
        assert(!locked);
    }
    
    std::cout << "Wptr edge case tests passed!" << std::endl;
}

class ThrowingClass {
public:
    static bool should_throw;
    static int instance_count;
    
    ThrowingClass() {
        ++instance_count;
        if (should_throw) {
            --instance_count;
            throw std::runtime_error("Constructor exception");
        }
    }
    
    ~ThrowingClass() {
        --instance_count;
    }
};

bool ThrowingClass::should_throw = false;
int ThrowingClass::instance_count = 0;

void test_exception_safety() {
    std::cout << "\n=== Testing Exception Safety ===" << std::endl;
    
    // 测试 make_uptr 异常安全
    ThrowingClass::should_throw = true;
    ThrowingClass::instance_count = 0;
    
    try {
        auto ptr = make_uptr<ThrowingClass>();
        assert(false); // 不应该到达这里
    } catch (const std::runtime_error&) {
        // 异常应该被正确传播
        assert(ThrowingClass::instance_count == 0); // 没有泄露
    }
    
    // 测试 make_sptr 异常安全
    try {
        auto ptr = make_sptr<ThrowingClass>();
        assert(false); // 不应该到达这里
    } catch (const std::runtime_error&) {
        // 异常应该被正确传播
        assert(ThrowingClass::instance_count == 0); // 没有泄露
    }
    
    ThrowingClass::should_throw = false;
    
    std::cout << "Exception safety tests passed!" << std::endl;
}

int main() {
    std::cout << "Smart Pointer Edge Case Tests\n";
    std::cout << "=============================\n";
    
    try {
        test_null_pointer_handling();
        test_self_assignment();
        test_reset_with_same_pointer();
        test_comparison_operators();
        test_swap_operations();
        test_wptr_edge_cases();
        test_exception_safety();
        
        std::cout << "\n🎉 All edge case tests passed!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "\n❌ Edge case test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
