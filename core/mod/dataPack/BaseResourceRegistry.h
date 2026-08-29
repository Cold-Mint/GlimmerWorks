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
#include <memory>
#include <unordered_map>

#include "core/mod/Resource.h"
#include "core/utils/TransparentStringHash.h"
#include "google/protobuf/compiler/csharp/csharp_field_base.h"

namespace glimmer {
    /**
     * BaseResourceRegistry
     * 基础资源注册表
     *
     * Used to manage the data loaded from the data packets.
     * 用于管理从数据包内加载的数据。
     * @tparam ResourceType
     */
    template<typename ResourceType>
    class BaseResourceRegistry {
        std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<ResourceType>,
                TransparentStringHash, std::equal_to<> >,
            TransparentStringHash, std::equal_to<> > resourceMap_{};

        std::vector<std::string> list_;

    public:
        virtual ~BaseResourceRegistry() = default;


        ResourceType *Register(std::unique_ptr<ResourceType> resource);

        /**
         * When registering the resources
         * 当注册资源时
         * @param resource
         */
        virtual void OnRegister(ResourceType *resource);

        /**
         * OnNotFound(This method can be used to achieve resource reservation.)
         * 当找不到资源时（可以覆盖此方法来实现资源占位）
         * @param packId
         * @param key
         * @return
         */
        virtual ResourceType *OnNotFound(std::string_view packId, std::string_view key);

        /**
         * Find
         * 查找资源
         * @param packId
         * @param key
         * @return
         */
        [[nodiscard]] ResourceType *Find(std::string_view packId, std::string_view key);

        [[nodiscard]] const std::vector<std::string> &List() const;


#if  !defined(NDEBUG)
        /**
         * List
         * 列出所有资源
         * @return
         */
        [[nodiscard]] std::string ListString() const;
#endif
    };

    template<typename ResourceType>
    ResourceType *BaseResourceRegistry<ResourceType>::Register(std::unique_ptr<ResourceType> resource) {
        auto &slot =
                resourceMap_[resource->packId][resource->resourceId];
        slot = std::move(resource);
        ResourceType *ptr = slot.get();
        list_.emplace_back(Resource::GenerateId(ptr->packId, ptr->resourceId));
        OnRegister(ptr);
        return ptr;
    }

    template<typename ResourceType>
    void BaseResourceRegistry<ResourceType>::OnRegister(ResourceType *resource) {
    }

    template<typename ResourceType>
    ResourceType *BaseResourceRegistry<ResourceType>::OnNotFound(std::string_view packId, std::string_view key) {
        return nullptr;
    }

    template<typename ResourceType>
    ResourceType *BaseResourceRegistry<ResourceType>::Find(std::string_view packId, std::string_view key) {
        if (const auto packIt = resourceMap_.find(packId); packIt != resourceMap_.end()) {
            if (const auto keyIt = packIt->second.find(key); keyIt != packIt->second.end()) {
                return keyIt->second.get();
            }
        }
        return OnNotFound(packId, key);
    }

    template<typename ResourceType>
    const std::vector<std::string> &BaseResourceRegistry<ResourceType>::List() const {
        return list_;
    }

#if  !defined(NDEBUG)
    template<typename ResourceType>
    std::string BaseResourceRegistry<ResourceType>::ListString() const {
        std::ostringstream oss;
        for (const auto &[packId, keyMap]: resourceMap_) {
            for (const auto &[key, resource]: keyMap) {
                oss << Resource::GenerateId(packId, key) << "\n";
            }
        }
        return oss.str();
    }
#endif
}
