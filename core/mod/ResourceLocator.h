//
// Created by Cold-Mint on 2025/12/11.
//

#ifndef GLIMMERWORKS_RESOURCELOCATOR_H
#define GLIMMERWORKS_RESOURCELOCATOR_H
#include <optional>

#include "Resource.h"
#include "ResourceRef.h"
#include "core/inventory/Item.h"

namespace glimmer {
    class AppContext;
    /**
     * ResourceLocator，Used to interpret references and return the corresponding resources.
     * 资源定位器，用于解释引用并返回对应的资源。
     */
    class ResourceLocator {
        AppContext *appContext_;

    public:
        explicit ResourceLocator(AppContext *appContext_) : appContext_(appContext_) {
        }

        /**
         * FindString
         * 查找字符串资源
         * @param resourceRef resourceRef 字符串引用
         * @return
         */
        [[nodiscard]] std::optional<StringResource *> FindString(const ResourceRef &resourceRef) const;


        /**
         * FindTile
         * 查找瓦片资源
         * @param resourceRef  resourceRef 瓦片引用
         * @return
         */
        [[nodiscard]] std::optional<TileResource *> FindTile(const ResourceRef &resourceRef) const;

        /**
         * FindComposableItem
         * 获取可组合的物品资源
         * @param resourceRef resourceRef 物品引用
         * @return
         */
        [[nodiscard]] std::optional<ComposableItemResource *> FindComposableItem(const ResourceRef &resourceRef) const;

        /**
         * FindAbilityItem
         * 获取Ability的物品资源
         * @param resourceRef resourceRef 物品引用
         * @return
         */
        [[nodiscard]] std::optional<AbilityItemResource *> FindAbilityItem(const ResourceRef &resourceRef) const;

        /**
         * FindItem
         * 查找物品
         * @param appContext appContext 应用上下文
         * @param resourceRef resourceRef 物品引用
         * @return  Item pointer 物品指针
         */
        [[nodiscard]] std::optional<std::unique_ptr<Item> > FindItem(AppContext *appContext,
                                                                     const ResourceRef &resourceRef) const;
    };
}

#endif //GLIMMERWORKS_RESOURCELOCATOR_H
