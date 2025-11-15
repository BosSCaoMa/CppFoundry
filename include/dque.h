#pragma once
#include <map>
#include <stdexcept>
#include <utility>

template<typename T>
class dque {
public:
    dque() = default;
    
    ~dque() {
        clear();
    }
    
    // 拷贝构造函数
    dque(const dque& other) : headIndex(other.headIndex), tailIndex(other.tailIndex), elementCount(other.elementCount) {
        for (const auto& [index, block] : other.blocks) {
            T* newBlock = new T[BLOCK_SIZE];
            for (int i = 0; i < BLOCK_SIZE; ++i) {
                newBlock[i] = block[i];
            }
            blocks[index] = newBlock;
        }
    }
    
    // 拷贝赋值运算符
    dque& operator=(const dque& other) {
        if (this == &other) return *this;
        
        // 清理旧数据
        clear();
        
        // 复制新数据
        for (const auto& [index, block] : other.blocks) {
            T* newBlock = new T[BLOCK_SIZE];
            for (int i = 0; i < BLOCK_SIZE; ++i) {
                newBlock[i] = block[i];
            }
            blocks[index] = newBlock;
        }
        headIndex = other.headIndex;
        tailIndex = other.tailIndex;
        elementCount = other.elementCount;
        return *this;
    }
    
    // 移动构造函数
    // std::move 本身不会真的移动数据，它只是把对象变成“右值引用”，让后续的构造函数或赋值操作可以“偷走”资源
    dque(dque&& other) noexcept 
        : blocks(std::move(other.blocks)), 
          headIndex(other.headIndex),
          tailIndex(other.tailIndex),
          elementCount(other.elementCount) {
        other.headIndex = 0;
        other.tailIndex = 0;
        other.elementCount = 0;
    }
    
    // 移动赋值运算符
    dque& operator=(dque&& other) noexcept {
        if (this == &other) return *this;
        
        clear();
        blocks = std::move(other.blocks);
        headIndex = other.headIndex;
        tailIndex = other.tailIndex;
        elementCount = other.elementCount;
        other.headIndex = 0;
        other.tailIndex = 0;
        other.elementCount = 0;
        return *this;
    }
    /*
    四大操作不用想那么复杂，headIndex 可以变成负数。
    底层实现用 headIndex/tailIndex 作为“全局索引”，理论上只要 int 不溢出，负数也能正常分块和定位。
    都是先计算 blockIndex 和 posInBlock，然后访问 blocks 里的对应块就行了。
     */
    // 在尾部添加元素
    void push_back(const T& value) {
        int blockIndex = tailIndex / BLOCK_SIZE;
        int posInBlock = tailIndex % BLOCK_SIZE;
        
        // 如果需要，分配新块
        if (blocks.find(blockIndex) == blocks.end()) {
            blocks[blockIndex] = new T[BLOCK_SIZE];
        }
        
        blocks[blockIndex][posInBlock] = value;
        ++tailIndex;
        ++elementCount;
    }
    
    // 在头部添加元素
    void push_front(const T& value) {
        --headIndex;
        ++elementCount;
        
        int blockIndex = headIndex / BLOCK_SIZE;
        int posInBlock = headIndex % BLOCK_SIZE;
        
        // 如果需要，分配新块
        if (blocks.find(blockIndex) == blocks.end()) {
            blocks[blockIndex] = new T[BLOCK_SIZE];
        }
        
        blocks[blockIndex][posInBlock] = value;
    }
    
    // 移除头部元素
    void pop_front() {
        if (empty()) {
            throw std::out_of_range("pop_front on empty deque");
        }
        
        int blockIndex = headIndex / BLOCK_SIZE;
        ++headIndex;
        --elementCount;
        
        // 如果块变空，释放它
        if (headIndex / BLOCK_SIZE > blockIndex && blocks.find(blockIndex) != blocks.end()) {
            delete[] blocks[blockIndex];
            blocks.erase(blockIndex);
        }
    }
    
    // 移除尾部元素
    void pop_back() {
        if (empty()) {
            throw std::out_of_range("pop_back on empty deque");
        }
        
        --tailIndex;
        --elementCount;
        int blockIndex = tailIndex / BLOCK_SIZE;
        
        // 如果块变空，释放它
        int nextBlockIndex = (tailIndex + 1) / BLOCK_SIZE;
        if (nextBlockIndex > blockIndex && blocks.find(nextBlockIndex) != blocks.end()) {
            delete[] blocks[nextBlockIndex];
            blocks.erase(nextBlockIndex);
        }
    }
    
    // 访问元素
    T& operator[](int index) {
        int actualIndex = headIndex + index;
        int blockIndex = actualIndex / BLOCK_SIZE;
        int posInBlock = actualIndex % BLOCK_SIZE;
        return blocks[blockIndex][posInBlock];
    }
    
    const T& operator[](int index) const {
        int actualIndex = headIndex + index;
        int blockIndex = actualIndex / BLOCK_SIZE;
        int posInBlock = actualIndex % BLOCK_SIZE;
        return blocks.at(blockIndex)[posInBlock];
    }
    
    // 带边界检查的访问
    T& at(int index) {
        if (index < 0 || index >= elementCount) {
            throw std::out_of_range("Index out of range");
        }
        return (*this)[index];
    }
    
    const T& at(int index) const {
        if (index < 0 || index >= elementCount) {
            throw std::out_of_range("Index out of range");
        }
        return (*this)[index];
    }
    
    // 访问首尾元素
    T& front() {
        if (empty()) {
            throw std::out_of_range("front on empty deque");
        }
        return (*this)[0]; // 会调用T& operator[]，一个内部重载
    }
    
    const T& front() const {
        if (empty()) {
            throw std::out_of_range("front on empty deque");
        }
        return (*this)[0];
    }
    
    T& back() {
        if (empty()) {
            throw std::out_of_range("back on empty deque");
        }
        return (*this)[elementCount - 1];
    }
    
    const T& back() const {
        if (empty()) {
            throw std::out_of_range("back on empty deque");
        }
        return (*this)[elementCount - 1];
    }
    
    // 清空
    void clear() {
        for (auto& [index, block] : blocks) {
            delete[] block;
        }
        blocks.clear();
        headIndex = 0;
        tailIndex = 0;
        elementCount = 0;
    }
    
    // 大小和容量
    int size() const { return elementCount; }
    bool empty() const { return elementCount == 0; }
    
private:
    std::map<int, T*> blocks;
    int headIndex = 0;
    int tailIndex = 0;
    int elementCount = 0;
    const static int BLOCK_SIZE = 64;
};