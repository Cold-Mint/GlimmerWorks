//
// Created by Cold-Mint on 2026/5/18.
//
#pragma once
#if  !defined(NDEBUG)
#include <cstdint>
#include <mutex>


namespace glimmer {
    struct MemoryUsage {
        static inline std::recursive_mutex Mutex;
        static inline uint64_t TotalBytes = 0;
    };
}

#endif
