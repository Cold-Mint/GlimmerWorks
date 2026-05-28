/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
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
