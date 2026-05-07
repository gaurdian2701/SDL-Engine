#pragma once
#ifdef _DEBUG
#include "tracy/Tracy.hpp"
#endif

inline void* operator new(std::size_t size)
{
    void* ptr = malloc(size);

    TracyAllocS(ptr, size, 20);

    return ptr;
}

inline void operator delete(void* ptr) noexcept
{
    TracyFreeS(ptr, 20);

    free(ptr);
}