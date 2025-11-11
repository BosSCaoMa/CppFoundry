cmake_minimum_required(VERSION 3.10)

project(CppFoundry)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 自动发现 src 目录下的所有 .cpp 文件
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 添加可执行文件
add_executable(main ${SOURCES})

# 将 include 目录添加到头文件搜索路径
target_include_directories(main PUBLIC include)
