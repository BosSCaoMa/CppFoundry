#include <iostream>
#include <cassert>

int add(int a, int b) {
    return a + b;
}

void test_add() {
    std::cout << "Testing add function..." << std::endl;
    
    // 测试基本加法
    assert(add(1, 2) == 3);
    std::cout << "✓ add(1, 2) == 3" << std::endl;
    
    assert(add(-1, 1) == 0);
    std::cout << "✓ add(-1, 1) == 0" << std::endl;
    
    assert(add(0, 0) == 0);
    std::cout << "✓ add(0, 0) == 0" << std::endl;
    
    std::cout << "All tests passed!" << std::endl;
}

int main() {
    std::cout << "Simple C++ Test Demo" << std::endl;
    std::cout << "===================" << std::endl;
    
    try {
        test_add();
        std::cout << "\n🎉 All tests passed successfully!" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cout << "\n❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
