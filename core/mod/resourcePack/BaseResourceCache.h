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

#include "core/context/AppContext.h"

namespace glimmer {
    /**
     * BaseResourceCache
     * 基础资源缓存
     *
     * Resource caching refers to the dynamic-loaded data such as textures and audio within the asset package, whose loading process is postponed until they are actually used.
     * 资源缓存是指资产包内的纹理，音频等动态加载的数据，它们的加载工作延迟到使用时。
     */
    template<typename ResourceResultType>
    class BaseResourceCache {
        std::unordered_map<uint64_t, std::weak_ptr<ResourceResultType> >
        resourceCache_;

        /**
         * If resource placeholders are enabled, then return the resource placeholder.
         * 如果启用了资源占位符，那么返回资源占位。
         * @param appContext
         * @param resourceRef
         * @param enablePlaceholder
         * @return
         */
        std::shared_ptr<ResourceResultType> TryGetPlaceholder(
            const AppContext *appContext, const ResourceRef *resourceRef, bool enablePlaceholder);

    protected:
        /**
        * Load placeholder resources when no resources are found.
        * 当找不到资源时加载占位资源。
        * @param appContext
        * @param resourceRef
        * @return
        */
        virtual std::shared_ptr<ResourceResultType> CreatePlaceholderResource(
            const AppContext *appContext, const ResourceRef *resourceRef);

        /**
        * Load resources from the resource package
        * 从资源包内加载资源
        * @param appContext
        * @param resourceRef
        * @param resourcePack
        * @return
        */
        virtual std::shared_ptr<ResourceResultType> LoadResourceFromPack(
            AppContext *appContext, const ResourceRef *resourceRef,
            const ResourcePack *resourcePack) = 0;

    public:
        virtual ~BaseResourceCache() = default;

        /**
         * LoadResource
         * 加载资源
         * @param appContext appContext 应用上下文
         * @param resourceRef resourceRef 资源引用
         * @param enablePlaceholder 有效资源；缓存未命中/加载失败时，根据enablePlaceholder返回占位资源或nullptr
         * @return
         */
        std::shared_ptr<ResourceResultType> LoadResource(const AppContext *appContext,
                                                         const ResourceRef *resourceRef, bool enablePlaceholder = true);

        /**
         * Clear
         * 清理全部缓存。
         *
         * It will not affect the existing resources. The resources currently in use externally still exist; they will just be recreated the next time you perform LoadResource.
         * 不会影响现有的资源。外部正在用的资源还存在，只是下次再次LoadResource时会创建新的。
         */
        void Clear();
    };

    template<typename ResourceResultType>
    std::shared_ptr<ResourceResultType> BaseResourceCache<ResourceResultType>::LoadResource(
        const AppContext *appContext, const ResourceRef *resourceRef, const bool enablePlaceholder) {
        if (appContext == nullptr) {
            return TryGetPlaceholder(appContext, resourceRef);
        }
        MainThreadDispatcher *mainThreadDispatcher = appContext->GetMainThreadDispatcher();
        if (mainThreadDispatcher == nullptr) {
            return TryGetPlaceholder(appContext, resourceRef, enablePlaceholder);
        }
        Config *config = appContext->GetConfig();
        if (config == nullptr) {
            return TryGetPlaceholder(appContext, resourceRef, enablePlaceholder);
        }
        const Mods &mods = config->mods;
        const std::vector<uint64_t> &enabledResourcePack = mods.enabledResourcePack;
        ResourcePackManager *resourcePackManager = appContext->GetResourcePackManager();
        if (resourcePackManager == nullptr) {
            return TryGetPlaceholder(appContext, resourceRef, enablePlaceholder);
        }

        return mainThreadDispatcher->AddMainThreadTaskAwait(
            [this, enabledResourcePack, resourcePackManager, appContext, resourceRef] {
                uint64_t fingerprint = resourceRef->GetFingerprint();
                const auto cache = resourceCache_.find(fingerprint);
                if (cache != resourceCache_.end()) {
                    if (auto cacheTexture = cache->second.lock()) {
                        return cacheTexture;
                    }
                    //Cache has expired.
                    //缓存过期。
                    resourceCache_.erase(cache);
                }
                for (const auto &packId: enabledResourcePack) {
                    const ResourcePack *resourcePack = resourcePackManager->Find(packId);
                    if (resourcePack == nullptr) {
                        continue;
                    }
                    auto result = LoadResourceFromPack(appContext, resourceRef->GetResourceType(), resourcePack);
                    if (result == nullptr) {
                        continue;
                    }
                    //Establish a cache.
                    //建立缓存。
                    resourceCache_[fingerprint] = result;
                    return result;
                }
                return nullptr;
            }
        ).get();
        return TryGetPlaceholder(appContext, resourceRef, enablePlaceholder);
    }

    template<typename ResourceResultType>
    void BaseResourceCache<ResourceResultType>::Clear() {
        resourceCache_.clear();
    }

    template<typename ResourceResultType>
    std::shared_ptr<ResourceResultType> BaseResourceCache<ResourceResultType>::CreatePlaceholderResource(
        const AppContext *appContext, const ResourceRef *resourceRef) {
        return nullptr;
    }

    template<typename ResourceResultType>
    std::shared_ptr<ResourceResultType> BaseResourceCache<ResourceResultType>::TryGetPlaceholder(
        const AppContext *appContext, const ResourceRef *resourceRef, const bool enablePlaceholder) {
        if (!enablePlaceholder) {
            return nullptr;
        }
        return CreatePlaceholderResource(appContext, resourceRef);
    }
}
