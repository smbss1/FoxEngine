
#ifndef MY_ALLOCATOR_H
#define MY_ALLOCATOR_H

#include <cstddef>

static const int PARSE_CONTEXT_MEMORY_BLOCK_SIZE_DEFAULT = 4096;

struct MemoryBlock
{
    char *m_pMemory;
    std::size_t m_lMemorySize;
    size_t m_lMemoryAllocationPosition;
    MemoryBlock* m_pNext;
};

class MyAllocator
{
    MemoryBlock* m_pFirstBlock;
    MemoryBlock* m_pActiveBlock;

public:
    template <typename T> T* Allocate(unsigned int uiSize);
    void CleanUp();

private:
    MemoryBlock* AllocateMemoryBlock(unsigned int uiNeededBytes);
};

#endif /* MY_ALLOCATOR_H */