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
#include "Resource.h"
#include "ResourceRef.h"
#include "core/math/Color.h"
#include "core/shape/ShapeManager.h"
#include "dataPack/BiomeDecoratorResourcesManager.h"
#include "dataPack/DataPackManager.h"
#include "dataPack/FixedColorManager.h"
#include "dataPack/LightMaskManager.h"
#include "dataPack/LightSourceManager.h"
#include "dataPack/StringManager.h"
#include "resourcePack/AudioResourceResult.h"
#include "resourcePack/ResourcePackManager.h"
#include "resourcePack/TextureResourceResult.h"

namespace glimmer
{
    class TileResourceManager;
    class MobManager;
    class ItemManager;
    class LootTableManager;
    class WorldContext;
    enum class TileLayerType : uint8_t;
    class AppContext;
    class Item;
    class TileInstancePool;
    class ResourcePackManager;
    /**
     * ResourceLocator，Used to interpret references and return the corresponding resources.
     * 资源定位器，用于解释引用并返回对应的资源。
     */
    class ResourceLocator
    {
        AppContext* appContext_ = nullptr;
        ResourcePackManager* resourcePackManager_ = nullptr;
        FixedColorManager* fixedColorManager_ = nullptr;
        LootTableManager* lootTableManager_ = nullptr;
        ItemManager* itemManager_ = nullptr;
        MobManager* mobManager_ = nullptr;
        TileResourceManager* tileResourceManager_ = nullptr;
        LightMaskManager* lightMaskManager_ = nullptr;
        LightSourceManager* lightSourceManager_ = nullptr;
        ShapeManager* shapeManager_ = nullptr;
        StringManager* stringManager_ = nullptr;
        BiomeDecoratorResourcesManager* biomeDecoratorResourcesManager_ = nullptr;
        DataPackManager* dataPackManager_ = nullptr;

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
        [[nodiscard]] bool ValidateAccessPermission(const ResourceRef* resourceRef) const;

    public:
        explicit ResourceLocator(AppContext* appContext_);

        /**
         * Load the texture and return an error placeholder if the loading fails.
         * 加载纹理，并在加载失败后返回错误占位符。
         * @param resourceRef
         * @return
         */
        [[nodiscard]] std::shared_ptr<TextureResourceResult> FindTexture(const ResourceRef* resourceRef) const;

        /**
         * 加载纹理
         * @param resourceRef 资源引用
         * @return 找不到返回null，权限受限返回accessDeniedTexture。
         */
        [[nodiscard]] std::shared_ptr<TextureResourceResult> FindTextureRaw(const ResourceRef* resourceRef) const;

        [[nodiscard]] std::shared_ptr<AudioResourceResult> FindAudio(const ResourceRef* resourceRef) const;

        [[nodiscard]] std::unique_ptr<Color> FindColor(const ResourceRef* resourceRef) const;

        [[nodiscard]] IShapeResource* FindShape(const ResourceRef* resourceRef) const;

        [[nodiscard]] IBiomeDecoratorResource* FindBiomeDecorator(const ResourceRef* resourceRef) const;

        /**
         * FindString
         * 查找字符串资源
         * @param resourceRef resourceRef 字符串引用
         * @return
         */
        [[nodiscard]] StringResource* FindString(const ResourceRef* resourceRef) const;

        [[nodiscard]] LightSourceResource* FindLightSource(const ResourceRef* resourceRef) const;

        [[nodiscard]] LightMaskResource* FindLightMask(const ResourceRef* resourceRef) const;

        /**
         * FindTile
         * 查找瓦片资源
         * @param resourceRef  resourceRef 瓦片引用
         * @param tileLayer tileLayer
         * @return
         */
        [[nodiscard]] TileResource* FindTileFallback(const ResourceRef* resourceRef, TileLayerType tileLayer) const;

        [[nodiscard]] TileResource* FindTileRaw(const ResourceRef* resourceRef) const;

        [[nodiscard]] MobResource* FindMob(const ResourceRef* resourceRef) const;

        /**
         * FindComposableItem
         * 获取可组合的物品资源
         * @param resourceRef resourceRef 物品引用
         * @return
         */
        [[nodiscard]] ComposableItemResource* FindComposableItem(const ResourceRef* resourceRef) const;

        /**
         * FindAbilityItem
         * 获取Ability的物品资源
         * @param resourceRef resourceRef 物品引用
         * @return
         */
        [[nodiscard]] AbilityItemResource* FindAbilityItem(const ResourceRef* resourceRef) const;

        [[nodiscard]] MaterialItemResource* FindMaterialItem(const ResourceRef* resourceRef) const;

        /**
         * FindLoot
         * 获取lootTable的物品资源
         * @param resourceRef resourceRef 物品引用
         * @return
         */
        [[nodiscard]] LootResource* FindLoot(const ResourceRef* resourceRef) const;

        /**
         * FindItem
         * 查找物品
         * @param worldContext worldContext
         * @param itemMessage itemMessage 物品数据
         * @return  Item pointer 物品指针
         */
        [[nodiscard]] std::unique_ptr<Item> FindItem(WorldContext* worldContext,
                                                     const ItemMessage& itemMessage) const;

        [[nodiscard]] std::unique_ptr<Item> FindItem(WorldContext* worldContext,
                                                     const ItemMessageResource& itemMessageResource) const;
    };
}
