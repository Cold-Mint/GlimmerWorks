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
#include "core/math/TileVector2D.h"
#include "core/mod/ResourceRef.h"
#include "core/world/BreakSource.h"
#include "core/world/generator/TileLayerType.h"
#include "src/core/place_source_message.pb.h"
enum PlaceSourceMessage : int;

namespace glimmer {
    enum class BreakSource;
    class ResourceRef;
    class TileVector2D;
    class WorldContext;

    class TargetResourceApplierContext {
        ResourceRef actionTarget_ = {};
        TileVector2D actionOrigin_ = {};
        WorldContext *worldContext_ = nullptr;
        BreakSource breakSource_ = BreakSource::Unknown;
        TileLayerType layerType_ = TileLayerType::Ground;
        PlaceSourceMessage placeSource_ = PLACE_SOURCE_TILE_PRIMARY_ACTION;

    public:
        void SetActionOrigin(const TileVector2D &actionOrigin);

        [[nodiscard]] const TileVector2D &GetActionOrigin() const;

        void SetActionTarget(const ResourceRef &actionTarget);

        [[nodiscard]] const ResourceRef &GetActionTarget() const;

        void SetWorldContext(WorldContext *worldContext);

        [[nodiscard]] WorldContext *GetWorldContext() const;

        void SetTileLayerType(TileLayerType layerType);

        [[nodiscard]] TileLayerType GetTileLayerType() const;

        void SetBreakSource(BreakSource breakSource);

        [[nodiscard]] BreakSource GetBreakSource() const;

        void SetPlaceSource(PlaceSourceMessage placeSource);

        [[nodiscard]] PlaceSourceMessage GetPlaceSource() const;
    };
}
