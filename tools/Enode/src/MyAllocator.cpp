
#include "MyAllocator.h"
#include <stdlib.h>     /* malloc, free */
#include <cstring>
#include <cassert>

using namespace std;

MemoryBlock* MyAllocator::AllocateMemoryBlock(unsigned int uiNeededBytes)
{
    MemoryBlock* pNewBlock = 0;
    pNewBlock = (MemoryBlock *) malloc(sizeof(MemoryBlock) + uiNeededBytes);
    memset(pNewBlock, 0, sizeof(MemoryBlock) + uiNeededBytes);
    assert(pNewBlock != 0);
    pNewBlock->m_pMemory = (char *) pNewBlock + sizeof(MemoryBlock);
    pNewBlock->m_lMemorySize = uiNeededBytes;
    pNewBlock->m_pNext = 0;
    return (pNewBlock);
}

template <typename T> T* MyAllocator::Allocate(unsigned int uiSize)
{
    if (!m_pActiveBlock || m_pActiveBlock->m_lMemoryAllocationPosition + uiSize > m_pActiveBlock->m_lMemorySize) {
        unsigned int iNeededBytes = PARSE_CONTEXT_MEMORY_BLOCK_SIZE_DEFAULT;
        if (uiSize > iNeededBytes) {
            iNeededBytes = uiSize;
        }
        MemoryBlock* pNewBlock = 0;
        pNewBlock = AllocateMemoryBlock(iNeededBytes);
        if (m_pActiveBlock) {
            m_pActiveBlock->m_pNext = pNewBlock;
            m_pActiveBlock = pNewBlock;
        } else {
            m_pFirstBlock = pNewBlock;
            m_pActiveBlock = pNewBlock;
        }
    }
    assert(m_pActiveBlock &&
           m_pActiveBlock->m_lMemoryAllocationPosition + uiSize <= m_pActiveBlock->m_lMemorySize);

    void* pMemory = m_pActiveBlock->m_pMemory + m_pActiveBlock->m_lMemoryAllocationPosition;
    m_pActiveBlock->m_lMemoryAllocationPosition += uiSize;
    return reinterpret_cast<T *>(pMemory);
}

void MyAllocator::CleanUp()
{
    for (MemoryBlock* pBlock = m_pFirstBlock; pBlock;) {
        MemoryBlock* pNext = pBlock->m_pNext;
        free(pBlock);
        pBlock = pNext;
    }
}