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
#pragma once
#include "ResourcePack.h"
#include "core/log/LogCat.h"

namespace glimmer {
    template<typename T>
    class ResourceResult {
        const ResourcePack *resourcePack_ = nullptr;
        T *resource_ = nullptr;
#if  !defined(NDEBUG)
        //Indicates whether the resources have been securely destroyed.
        //表示是否被安全销毁资源。
        bool safeDestroy_ = false;
#endif

    protected:
        virtual ~ResourceResult();

        /**
         * Perform resource destruction
         * 实现销毁资源。
         */
        virtual void DestroyResourceImpl(T *resource);

        /**
         * The subclass must call this method to release the resources!
         * 子类必须调用这个方法来销毁资源！
         */
        void DestroyResource();

    public:
        void SetResourcePack(const ResourcePack *resourcePack);

        [[nodiscard]] const ResourcePack *GetResourcePack() const;


        void SetResource(T *resource);

        [[nodiscard]] T *GetResource() const;
    };

    template<typename T>
    void ResourceResult<T>::SetResource(T *resource) {
        resource_ = resource;
    }

    template<typename T>
    void ResourceResult<T>::SetResourcePack(const ResourcePack *resourcePack) {
        if (resourcePack == nullptr) {
            LogCat::e(std::source_location::current(), "resourcePack == nullptr");
            return;
        }
        resourcePack_ = resourcePack;
    }

    template<typename T>
    const ResourcePack *ResourceResult<T>::GetResourcePack() const {
        return resourcePack_;
    }

    template<typename T>
    ResourceResult<T>::~ResourceResult() {
#if  !defined(NDEBUG)
        if (!safeDestroy_) {
            LogCat::e(std::source_location::current(),
                      "Some resources have not been released correctly. Please implement the \"DestroyResource\" method within the destructor of the subclass.");
        }
#endif
    }

    template<typename T>
    void ResourceResult<T>::DestroyResourceImpl(T *resource) {
        //This method covers the implementation of resource destruction.
        //覆盖这个方法实现资源销毁。
    }

    template<typename T>
    void ResourceResult<T>::DestroyResource() {
        T *resource = resource_;
        if (resource == nullptr) {
            return;
        }
        DestroyResourceImpl(resource);
        resource_ = nullptr;
        safeDestroy_ = true;
    }


    template<typename T>
    T *ResourceResult<T>::GetResource() const {
        return resource_;
    }
}
