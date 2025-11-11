# CppFoundry

手动实现 C++ STL 核心组件与智能指针，聚焦底层原理剖析与手写实现细节，用于学习和巩固 C++ 内存管理、容器设计等核心知识点。

A comprehensive C++ educational project that manually implements core STL components and smart pointers. This project focuses on understanding underlying principles and implementation details to learn and consolidate core C++ knowledge including memory management and container design.

## 🎯 Project Overview

This repository contains from-scratch implementations of:

- **Smart Pointers**
  - `unique_ptr` - Exclusive ownership with move semantics
  - `shared_ptr` - Shared ownership with reference counting
  
- **Containers**
  - `vector` - Dynamic array with automatic memory management
  - `list` - Doubly-linked list with bidirectional iteration

All implementations include:
- ✅ Detailed comments explaining memory management logic
- ✅ Implementation principles and design decisions
- ✅ Comprehensive test cases to verify functionality
- ✅ Example programs demonstrating usage

## 📁 Project Structure

```
CppFoundry/
├── include/
│   ├── memory/
│   │   ├── unique_ptr.hpp    # Unique pointer implementation
│   │   └── shared_ptr.hpp    # Shared pointer implementation
│   └── containers/
│       ├── vector.hpp         # Dynamic array implementation
│       └── list.hpp           # Doubly-linked list implementation
├── tests/
│   ├── test_unique_ptr.cpp    # unique_ptr tests
│   ├── test_shared_ptr.cpp    # shared_ptr tests
│   ├── test_vector.cpp        # vector tests
│   └── test_list.cpp          # list tests
├── examples/
│   └── demo.cpp               # Demonstration program
├── CMakeLists.txt             # Build configuration
└── README.md                  # This file
```

## 🚀 Getting Started

### Prerequisites

- C++14 or later
- CMake 3.10 or later
- A C++ compiler (GCC, Clang, or MSVC)

### Building the Project

```bash
# Clone the repository
git clone https://github.com/BosSCaoMa/CppFoundry.git
cd CppFoundry

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build all targets
cmake --build .

# Run all tests
ctest --output-on-failure
# or
make run_all_tests

# Run the demo
./demo
```

## 📚 Components Documentation

### 1. unique_ptr - Exclusive Ownership Smart Pointer

**Key Features:**
- Unique ownership semantics (only one owner at a time)
- Move-only type (cannot be copied)
- Automatic memory cleanup via RAII
- Custom deleter support
- Array specialization with `operator[]`

**Memory Management:**
- Constructor takes ownership of raw pointer
- Destructor automatically deletes the managed object
- Move semantics transfer ownership without copying
- `release()` gives up ownership without deletion
- `reset()` replaces managed object

**Usage Example:**
```cpp
#include "memory/unique_ptr.hpp"

// Create unique_ptr
auto ptr = cppfoundry::make_unique<int>(42);

// Move ownership (not copyable)
auto ptr2 = std::move(ptr);  // ptr is now null

// Array version
cppfoundry::unique_ptr<int[]> arr(new int[10]);
arr[0] = 100;
```

### 2. shared_ptr - Shared Ownership Smart Pointer

**Key Features:**
- Shared ownership (multiple owners allowed)
- Reference counting to track owners
- Thread-safe reference counting (concept)
- Custom deleter support
- Automatic cleanup when last owner is destroyed

**Memory Management:**
- Uses control block pattern for reference counting
- Copy increments reference count
- Destructor decrements count, deletes if count reaches 0
- Move transfers ownership without changing count
- Control block deleted when both shared and weak counts reach 0

**Usage Example:**
```cpp
#include "memory/shared_ptr.hpp"

// Create shared_ptr
auto ptr1 = cppfoundry::make_shared<int>(42);
auto ptr2 = ptr1;  // Both own the object

std::cout << "Reference count: " << ptr1.use_count() << std::endl;  // 2

ptr1.reset();  // Decrements count
// Object still alive because ptr2 owns it
```

### 3. vector - Dynamic Array

**Key Features:**
- Contiguous memory storage
- O(1) random access
- Amortized O(1) push_back (geometric growth)
- Automatic reallocation when capacity exceeded
- Iterator support

**Memory Management:**
- Three key values: data pointer, size, capacity
- Geometric growth strategy (typically 2x) for efficiency
- Reallocation: allocate new memory → move elements → deallocate old
- Destructor calls element destructors and frees memory

**Usage Example:**
```cpp
#include "containers/vector.hpp"

cppfoundry::vector<int> v;
v.push_back(1);
v.push_back(2);
v.push_back(3);

std::cout << "Size: " << v.size() << std::endl;
std::cout << "Element 0: " << v[0] << std::endl;

for (const auto& val : v) {
    std::cout << val << " ";
}
```

### 4. list - Doubly-Linked List

**Key Features:**
- Non-contiguous memory (each element in separate node)
- O(1) insertion/deletion at any position (with iterator)
- Bidirectional iteration
- No reallocation needed

**Memory Management:**
- Each element in a node with prev/next pointers
- Sentinel head node simplifies implementation
- Insert: allocate node, update pointers
- Erase: update pointers, delete node
- Destructor traverses and deletes all nodes

**Usage Example:**
```cpp
#include "containers/list.hpp"

cppfoundry::list<int> l;
l.push_back(1);
l.push_front(0);
l.push_back(2);

// Bidirectional iteration
for (auto it = l.begin(); it != l.end(); ++it) {
    std::cout << *it << " ";
}
```

## 🧪 Testing

Each component has comprehensive tests covering:

- Basic construction and destruction
- Copy and move semantics
- Element access and modification
- Memory management verification
- Edge cases and error handling
- Custom types and complex scenarios

Run tests:
```bash
cd build
ctest --output-on-failure
```

Or run individual tests:
```bash
./test_unique_ptr
./test_shared_ptr
./test_vector
./test_list
```

## 📖 Learning Resources

### Key Concepts Demonstrated

1. **RAII (Resource Acquisition Is Initialization)**
   - Resources acquired in constructor
   - Resources released in destructor
   - Automatic cleanup, exception-safe

2. **Move Semantics**
   - Transfer ownership without copying
   - Efficient resource management
   - Used in unique_ptr and containers

3. **Reference Counting**
   - Track number of owners
   - Control block pattern
   - Used in shared_ptr

4. **Memory Allocation Strategies**
   - Geometric growth in vector
   - Node-based allocation in list
   - Trade-offs between memory and performance

5. **Iterator Pattern**
   - Uniform interface for traversal
   - Pointer-based (vector) vs node-based (list)
   - Bidirectional iteration

## 🎓 Educational Value

This project is ideal for:

- Understanding C++ memory management
- Learning STL implementation details
- Practicing template programming
- Understanding design patterns (RAII, control block, iterator)
- Preparing for C++ interviews
- Building foundational knowledge for systems programming

## 🔍 Implementation Notes

### Design Decisions

1. **Simplified but Educational**
   - Focus on core concepts rather than full standard compliance
   - Includes essential features and clear documentation
   - Trade-offs noted in comments

2. **Not Production Ready**
   - Missing some standard features (e.g., allocator support)
   - Thread safety is conceptual, not fully implemented
   - Optimizations simplified for clarity

3. **Emphasis on Clarity**
   - Extensive comments explaining "why" not just "what"
   - Step-by-step breakdowns of complex operations
   - Clear variable naming

### Differences from Standard Library

- Simplified exception handling
- No allocator support
- Some edge cases not handled
- Performance optimizations omitted for clarity
- Limited iterator categories

These trade-offs are intentional to focus on educational value.

## 🤝 Contributing

Contributions are welcome! This is an educational project, so please focus on:

- Improving documentation and comments
- Adding more test cases
- Fixing bugs while maintaining simplicity
- Adding educational examples

## 📝 License

This project is created for educational purposes. Feel free to use and modify for learning.

## 🙏 Acknowledgments

This implementation is inspired by the C++ Standard Library but simplified for educational purposes. It demonstrates core concepts and implementation principles rather than providing a production-ready library.

## 📧 Contact

For questions or suggestions, please open an issue on GitHub.

---

**Happy Learning! 📚🚀**
