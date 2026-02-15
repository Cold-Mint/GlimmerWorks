//
// Created by Cold-Mint on 2025/12/11.
//

#ifndef GLIMMERWORKS_RESOURCELOCATOR_H
#define GLIMMERWORKS_RESOURCELOCATOR_H
#include <optional>

#include "Resource.h"
#include "ResourceRef.h"
#include "core/scene/AppContext.h"

namespace glimmer {
    class Item;
    /**
     * ResourceLocator，Used to interpret references and return the corresponding resources.
     * 资源定位器，用于解释引用并返回对应的资源。
     */
    class ResourceLocator {
        AppContext *appContext_;

        /**
     * ValidateAccessPermission
     * Verify whether the current resource reference has the permission to access the package where the target resource is located.
     * 校验当前资源引用是否有权限访问目标资源所在的包。
     *
     * Check the package to which the resourceRef belongs (GetSelfPackageId())
     * 检查 resourceRef 所属包（GetSelfPackageId()）
     * Is access permitted to the target package (GetPackageId())?
     * 是否被允许访问目标包（GetPackageId()）。
     *
     * @param resourceRef resourceRef 资源引用
     * @return If access is permitted, return true; otherwise, return false. 若允许访问则返回 true，否则返回 false
     */
        [[nodiscard]] static bool ValidateAccessPermission(const ResourceRef &resourceRef);

    public:
        explicit ResourceLocator(AppContext *appContext_);

        [[nodiscard]] std::shared_ptr<SDL_Texture> FindTexture(const ResourceRef &resourceRef) const;

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
        [[nodiscard]] TileResource *FindTile(const ResourceRef &resourceRef) const;

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
         * FindLoot
         * 获取lootTable的物品资源
         * @param resourceRef resourceRef 物品引用
         * @return
         */
        [[nodiscard]] std::optional<LootResource *> FindLoot(const ResourceRef &resourceRef) const;

        /**
         * FindItem
         * 查找物品
         * @param resourceRef resourceRef 物品引用
         * @return  Item pointer 物品指针
         */
        [[nodiscard]] std::optional<std::unique_ptr<Item> > FindItem(
            const ResourceRef &resourceRef) const;
    };
}

#endif //GLIMMERWORKS_RESOURCELOCATOR_H
