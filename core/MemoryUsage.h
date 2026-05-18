//
// Created by coldmint on 2026/5/18.
//
#if  !defined(NDEBUG)
#ifndef GLIMMERWORKS_MEMORYTRACKER_H
#define GLIMMERWORKS_MEMORYTRACKER_H
#include <cstdint>
#include <mutex>


namespace glimmer {
    struct MemoryUsage {
        static inline std::recursive_mutex Mutex; // 递归锁，防止重入死锁
        static inline uint64_t TotalBytes = 0;
    };
}

#endif
#endif //GLIMMERWORKS_MEMORYTRACKER_H
