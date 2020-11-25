#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef> // size_t
#include <utility>

class Allocator {
protected:
    std::size_t m_totalSize;
    std::size_t m_used;   
    std::size_t m_peak;
public:
    
    Allocator(const std::size_t totalSize) : m_totalSize { totalSize }, m_used { 0 }, m_peak { 0 } { }

    virtual ~Allocator() { m_totalSize = 0; }

    virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) = 0;

    virtual void Free(void* ptr) = 0;

    virtual void Init() = 0;

    friend class Benchmark;
};


template <class T, class... Args> T* New(Allocator& allocator, Args&&... args)
{
    return new (allocator.Allocate(sizeof(T), __alignof(T))) T(std::forward<Args>(args)...);
}

template <class T> T* New(Allocator& allocator)
{
    return new (allocator.Allocate(sizeof(T), __alignof(T))) T;
}

template <class T> void Delete(Allocator& allocator, T& object)
{
    object.~T();
    allocator.Free(&object);
}

#endif /* ALLOCATOR_H */

