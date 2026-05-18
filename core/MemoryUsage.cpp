#if  !defined(NDEBUG)
#include "MemoryUsage.h"
#include <cstdlib>
#include <new>

static void *getOriginalPtr(void *userPtr, std::size_t &outSize) {
    void *originalPtr = static_cast<char *>(userPtr) - sizeof(std::size_t);
    outSize = *static_cast<std::size_t *>(originalPtr);
    return originalPtr;
}

void *operator new(std::size_t size) {
    void *originalPtr = std::malloc(size + sizeof(std::size_t));
    if (!originalPtr) {
        throw std::bad_alloc();
    }
    *static_cast<std::size_t *>(originalPtr) = size;
    void *userPtr = static_cast<char *>(originalPtr) + sizeof(std::size_t);

    std::lock_guard lock(glimmer::MemoryUsage::Mutex);
    glimmer::MemoryUsage::TotalBytes += size;
    return userPtr;
}

void operator delete(void *userPtr) noexcept {
    if (!userPtr) return;
    std::size_t size = 0;
    if (void *originalPtr = getOriginalPtr(userPtr, size)) {
        {
            std::lock_guard lock(glimmer::MemoryUsage::Mutex);
            glimmer::MemoryUsage::TotalBytes -= size;
        }
        std::free(originalPtr);
    }
}

void operator delete(void *userPtr, std::size_t) noexcept {
    ::operator delete(userPtr);
}

void *operator new[](std::size_t size) {
    void *originalPtr = std::malloc(size + sizeof(std::size_t));
    if (!originalPtr) {
        throw std::bad_alloc();
    }
    *static_cast<std::size_t *>(originalPtr) = size;
    void *userPtr = static_cast<char *>(originalPtr) + sizeof(std::size_t);

    std::lock_guard lock(glimmer::MemoryUsage::Mutex);
    glimmer::MemoryUsage::TotalBytes += size;
    return userPtr;
}

void operator delete[](void *userPtr) noexcept {
    if (!userPtr) return;
    std::size_t size = 0;
    if (void *originalPtr = getOriginalPtr(userPtr, size)) {
        {
            std::lock_guard lock(glimmer::MemoryUsage::Mutex);
            glimmer::MemoryUsage::TotalBytes -= size;
        }
        std::free(originalPtr);
    }
}

void operator delete[](void *userPtr, std::size_t) noexcept {
    operator delete[](userPtr);
}
#endif
