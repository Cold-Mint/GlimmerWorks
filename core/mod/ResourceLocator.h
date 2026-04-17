//
// Created by Cold-Mint on 2025/12/11.
//

#ifndef GLIMMERWORKS_RESOURCELOCATOR_H
#define GLIMMERWORKS_RESOURCELOCATOR_H

#include "Resource.h"
#include "ResourceRef.h"
#include "core/scene/AppContext.h"
#include "SDL3_mixer/SDL_mixer.h"

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
        [[nodiscard]] bool ValidateAccessPermission(const ResourceRef &resourceRef) const;

    public:
        explicit ResourceLocator(AppContext *appContext_);

        [[nodiscard]] std::shared_ptr<SDL_Texture> FindTexture(const ResourceRef &resourceRef) const;

        [[nodiscard]] std::shared_ptr<MIX_Audio> FindAudio(const ResourceRef &resourceRef) const;

        [[nodiscard]] std::unique_ptr<SDL_Color> FindColor(const ResourceRef &resourceRef) const;

        [[nodiscard]] IShapeResource *FindShape(const ResourceRef &resourceRef) const;

        [[nodiscard]] IBiomeDecoratorResource *FindBiomeDecorator(const ResourceRef &resourceRef) const;

        /**
         * FindString
         * 查找字符串资源
         * @param resourceRef resourceRef 字符串引用
         * @return
         */
        [[nodiscard]] StringResource *FindString(const ResourceRef &resourceRef) const;

        [[nodiscard]] LightSourceResource *FindLightSource(const ResourceRef &resourceRef) const;

        [[nodiscard]] LightMaskResource *FindLightMask(const ResourceRef &resourceRef) const;

        /**
         * FindTile
         * 查找瓦片资源
         * @param resourceRef  resourceRef 瓦片引用
         * @return
         */
        [[nodiscard]] TileResource *FindTile(const ResourceRef &resourceRef) const;

        [[nodiscard]] MobResource *FindMob(const ResourceRef &resourceRef) const;

        /**
         * FindComposableItem
         * 获取可组合的物品资源
         * @param resourceRef resourceRef 物品引用
         * @return
         */
        [[nodiscard]] ComposableItemResource *FindComposableItem(const ResourceRef &resourceRef) const;

        /**
         * FindAbilityItem
         * 获取Ability的物品资源
         * @param resourceRef resourceRef 物品引用
         * @return
         */
        [[nodiscard]] AbilityItemResource *FindAbilityItem(const ResourceRef &resourceRef) const;

        /**
         * FindLoot
         * 获取lootTable的物品资源
         * @param resourceRef resourceRef 物品引用
         * @return
         */
        [[nodiscard]] LootResource *FindLoot(const ResourceRef &resourceRef) const;

        /**
         * FindItem
         * 查找物品
         * @param itemMessage itemMessage 物品数据
         * @return  Item pointer 物品指针
         */
        [[nodiscard]] std::unique_ptr<Item> FindItem(
            const ItemMessage &itemMessage) const;

        [[nodiscard]] std::unique_ptr<Item> FindItem(
            const ItemMessageResource &itemMessageResource) const;
    };
}

#endif //GLIMMERWORKS_RESOURCELOCATOR_H
