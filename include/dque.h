#pragma once
#include <vector>

template<typename T>
class dque {
public:
    deque()
private:
    std::map<int, T*> blocks;
    int headIndex = 0;
    const static int BLOCK_SIZE = 64;
};