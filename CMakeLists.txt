cmake_minimum_required(VERSION 3.10)

project(CppFoundry)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 自动发现 src 目录下的所有 .cpp 文件
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 只有在有源文件时才添加可执行文件
if(SOURCES)
    add_executable(main ${SOURCES})
    # 将 include 目录添加到头文件搜索路径
    target_include_directories(main PUBLIC include)
endif()

# 启用测试
enable_testing()

# 添加 tests 子目录
add_subdirectory(tests)
