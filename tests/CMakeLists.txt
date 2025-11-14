# 测试的 CMake 配置文件
cmake_minimum_required(VERSION 3.10)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加编译选项
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# 包含头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

# 创建可执行文件
add_executable(test_smart_pointers test_smart_pointers.cpp)
add_executable(test_quick test_quick.cpp)
add_executable(test_edge_cases test_edge_cases.cpp)

# 链接线程库 (用于多线程测试)
find_package(Threads REQUIRED)
target_link_libraries(test_smart_pointers Threads::Threads)

# 设置可执行文件输出目录
set_target_properties(test_smart_pointers test_quick test_edge_cases
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# 添加测试
enable_testing()

add_test(NAME SmartPointerFullTests COMMAND test_smart_pointers)
add_test(NAME SmartPointerQuickTests COMMAND test_quick)
add_test(NAME SmartPointerEdgeCaseTests COMMAND test_edge_cases)

# 打印信息
message(STATUS "Smart pointer tests configured:")
message(STATUS "  - test_smart_pointers: 全面测试")
message(STATUS "  - test_quick: 快速功能测试")
message(STATUS "  - test_edge_cases: 边界情况测试")
message(STATUS "")
message(STATUS "运行测试命令:")
message(STATUS "  cmake --build . && ctest --output-on-failure")
message(STATUS "  或者直接运行: ./test_quick")
