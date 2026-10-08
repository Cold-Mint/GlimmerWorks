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
#include "TileBreakParams.h"

void glimmer::TileBreakParams::SetBreakSource(const BreakSource& breakSource)
{
    breakSource_ = breakSource;
}

void glimmer::TileBreakParams::SetWorldContext(WorldContext* worldContext)
{
    worldContext_ = worldContext;
}

void glimmer::TileBreakParams::SetTileLayerComponent(const TileLayerComponent* tileLayerComponent)
{
    tileLayerComponent_ = tileLayerComponent;
}

void glimmer::TileBreakParams::SetTopLeftPosition(const TileVector2D& topLeftPosition)
{
    topLeftPosition_ = topLeftPosition;
}

void glimmer::TileBreakParams::SetPrecisionMining(bool precisionMining)
{
    precisionMining_ = precisionMining;
}

void glimmer::TileBreakParams::SetPlaceMode(bool placeMode)
{
    isPlaceMode_ = placeMode;
}

void glimmer::TileBreakParams::SetTileWidth(uint8_t tileWidth)
{
    tileWidth_ = tileWidth;
}

void glimmer::TileBreakParams::SetTileHeight(uint8_t tileHeight)
{
    tileHeight_ = tileHeight;
}

void glimmer::TileBreakParams::SetNewTileRef(const ResourceRef& newTileRef)
{
    newTileRef_ = newTileRef;
}

glimmer::BreakSource glimmer::TileBreakParams::GetBreakSource() const
{
    return breakSource_;
}

glimmer::WorldContext* glimmer::TileBreakParams::GetWorldContext() const
{
    return worldContext_;
}

const glimmer::TileLayerComponent* glimmer::TileBreakParams::GetTileLayerComponent() const
{
    return tileLayerComponent_;
}

const glimmer::TileVector2D& glimmer::TileBreakParams::GetTopLeftPosition() const
{
    return topLeftPosition_;
}

bool glimmer::TileBreakParams::IsPrecisionMining() const
{
    return precisionMining_;
}

bool glimmer::TileBreakParams::IsPlaceMode() const
{
    return isPlaceMode_;
}

uint8_t glimmer::TileBreakParams::GetTileWidth() const
{
    return tileWidth_;
}

uint8_t glimmer::TileBreakParams::GetTileHeight() const
{
    return tileHeight_;
}

glimmer::ResourceRef& glimmer::TileBreakParams::GetMutableNewTileRef()
{
    return newTileRef_;
}

const glimmer::ResourceRef& glimmer::TileBreakParams::GetNewTileRef() const
{
    return newTileRef_;
}
