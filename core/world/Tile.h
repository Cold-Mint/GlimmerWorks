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

#include "BreakSource.h"
#include "TileBlueprintData.h"
#include "TileDimensions.h"
#include "TileLightResourceData.h"
#include "TileLootData.h"
#include "TileMiningData.h"
#include "TileResourceData.h"
#include "core/ecs/EcsTypes.h"
#include "core/scene/AppContext.h"
#include "generator/TilePhysicsType.h"
#include "src/core/place_source_message.pb.h"

namespace glimmer
{
    enum class TileAnchorType : uint8_t;

    class Tile
    {
        friend class TileInstancePool;

        std::string id_;
        std::string name_;
        std::optional<std::string> description_;
        std::unordered_map<Vector2DIFingerprint, GameEntityID> gameEntities_;
        bool isOverwritable_ = false;
        TilePhysicsType physicsType_ = TilePhysicsType::None;
        TileLayerType layerType_ = TileLayerType::Ground;
        uint8_t technologyLevel_ = 0;
        uint8_t recipeGroup_ = 0;

        TileResourceData resourceData_;
        TileBlueprintData blueprintData_;
        TileDimensions dimensions_;
        TileMiningData miningData_;
        TileLootData lootData_;
        TileLightResourceData lightData_;

        static TileVector2D CalculateTileAnchor(TileAnchorType tileAnchorType, uint8_t tileWidth, uint8_t tileHeight,
                                                const Vector2DIResource& customTileAnchor);

        static std::unique_ptr<Tile> FromTileResource(const AppContext* appContext,
                                                      const TileResource* tileResource);

    public:
        [[nodiscard]] TilePhysicsType GetTilePhysicsType() const;

        [[nodiscard]] const std::string& GetId() const;

        [[nodiscard]] bool IsOverwritable() const;

        [[nodiscard]] bool IsWorkBlock() const;

        [[nodiscard]] const std::string& GetName() const;

        [[nodiscard]] TileLayerType GetLayerType() const;

        [[nodiscard]] const std::optional<std::string>& GetDescription() const;

        void OnPlace(const WorldContext* worldContext, PlaceSourceMessage placeSource, const TileVector2D& position);

        void OnBreak(const WorldContext* worldContext, BreakSource breakSource, const TileVector2D& position);

        [[nodiscard]] TileResourceData* GetMutableResourceData();

        [[nodiscard]] const TileResourceData* GetResourceData() const;

        [[nodiscard]] TileBlueprintData* GetMutableBlueprintData();

        [[nodiscard]] const TileBlueprintData* GetBlueprintData() const;

        [[nodiscard]] TileDimensions* GetMutableDimensions();

        [[nodiscard]] const TileDimensions* GetDimensions() const;

        [[nodiscard]] TileMiningData* GetMutableMiningData();

        [[nodiscard]] const TileMiningData* GetMiningData() const;

        [[nodiscard]] TileLootData* GetMutableLootData();

        [[nodiscard]] const TileLootData* GetLootData() const;

        [[nodiscard]] TileLightResourceData* GetMutableLightResourceData();

        [[nodiscard]] const TileLightResourceData* GetLightResourceData() const;
    };
}
