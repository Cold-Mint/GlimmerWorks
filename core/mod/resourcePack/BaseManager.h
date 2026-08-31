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

namespace glimmer {
    /**
     * BaseManager
     * 基础管理器
     * @tparam ResourceType It must inherit from IUniqueAble to return a unique Id. 必须继承自IUniqueAble返回独一无二的Id。
     */
    template<typename ResourceType>
    class BaseManager {
        std::unordered_map<uint64_t, std::unique_ptr<ResourceType> >
        resourceMap_;

    protected:
        /**
        * When a certain resource is successfully registered.
        * 当某个资源注册成功后。
        * @param resource
        */
        virtual void AfterRegister(ResourceType *resource);

        /**
         * When the registration of a certain resource is cancelled.
         * 当取消注册某个资源后。
         * @param resource
         */
        virtual void BeforeUnRegister(ResourceType *resource);

    public:
        virtual ~BaseManager() = default;

        /**
         * Find
         * 查找某个资源
         * @param id id 资源
         * @return If it cannot be found, then return nullptr. 如果找不到那么返回nullptr
         */
        ResourceType *Find(uint64_t id);

        /**
         * Register
         * 注册
         * @param resource
         * @return If the registration fails, return nullptr. 如果注册失败返回nullptr
         */
        ResourceType *Register(std::unique_ptr<ResourceType> resource);

        /**
         * Unregister
         * 注销资源
         * @param id
         * @return
         */
        bool Unregister(uint64_t id);

        /**
         * Contains
         * 是否包含某个资源
         * @param id
         * @return
         */
        [[nodiscard]] bool Contains(uint64_t id);
    };

    template<typename ResourceType>
    ResourceType *BaseManager<ResourceType>::Find(const uint64_t id) {
        auto iterator = resourceMap_.find(id);
        if (iterator == resourceMap_.end()) {
            return nullptr;
        }
        std::unique_ptr<ResourceType> &resourcePtr = iterator->second;
        if (resourcePtr == nullptr) {
            return nullptr;
        }
        return resourcePtr.get();
    }

    template<typename ResourceType>
    void BaseManager<ResourceType>::AfterRegister(ResourceType *resource) {
    }

    template<typename ResourceType>
    void BaseManager<ResourceType>::BeforeUnRegister(ResourceType *resource) {
    }

    template<typename ResourceType>
    ResourceType *BaseManager<ResourceType>::Register(std::unique_ptr<ResourceType> resource) {
        const auto uniqueId = resource->GetUniqueId();
        auto iterator = resourceMap_.find(uniqueId);
        if (iterator == resourceMap_.end()) {
            auto [it, inserted] = resourceMap_.insert(std::make_pair(uniqueId, std::move(resource)));
            if (inserted) {
                ResourceType *result = it->second.get();
                AfterRegister(result);
                return result;
            }
            return nullptr;
        }
        return nullptr;
    }

    template<typename ResourceType>
    bool BaseManager<ResourceType>::Unregister(uint64_t id) {
        auto iterator = resourceMap_.find(id);
        if (iterator == resourceMap_.end()) {
            return false;
        }
        BeforeUnRegister(iterator->second.get());
        resourceMap_.erase(iterator);
        return true;
    }

    template<typename ResourceType>
    bool BaseManager<ResourceType>::Contains(uint64_t id) {
        return resourceMap_.contains(id);
    }
}
