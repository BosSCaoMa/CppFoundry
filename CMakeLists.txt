cmake_minimum_required(VERSION 3.10)
project(CppFoundry)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加编译选项
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# 包含头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 构建示例程序
add_executable(vector_demo examples/vector_demo.cpp)

# 构建测试程序
add_executable(test_vector tests/test_vector.cpp)

# 启用测试
enable_testing()
add_test(NAME VectorTests COMMAND test_vector)
