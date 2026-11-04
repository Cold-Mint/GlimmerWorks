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
#include "TileAttachmentHelper.h"

#include "TileAttachmentSupportResult.h"
#include "core/log/LogCat.h"
#include "core/math/CoordinateTransformer.h"
#include "core/utils/TileLayerTypeUtils.h"
#include "core/world/ChunkManager.h"
#include "core/world/Tile.h"
#include "core/world/WorldContext.h"

bool glimmer::TileAttachmentHelper::SupportsDirection(const Tile &sourceTile, const ChunkManager &chunkManager,
                                                      TileLayerType targetLayerType, const TileVector2D &tileTopLeft,
                                                      TileAttachmentType direction) {
    auto &sourceTileDimensions = sourceTile.GetDimensions();
    const uint8_t width = sourceTileDimensions.GetTileWidth();
    const uint8_t height = sourceTileDimensions.GetTileHeight();
    std::vector<TileVector2D> cells;
    switch (direction) {
        case TileAttachmentType::BackLayer:
            for (int x = 0; x < width; ++x) {
                for (int y = 0; y < height; ++y) {
                    cells.emplace_back(tileTopLeft.x + x, tileTopLeft.y - y);
                }
            }
            break;
        case TileAttachmentType::Up:
            for (int x = 0; x < width; ++x) {
                cells.emplace_back(tileTopLeft.x + x, tileTopLeft.y + 1);
            }
            break;
        case TileAttachmentType::Right:
            for (int y = 0; y < height; ++y) {
                cells.emplace_back(tileTopLeft.x + width, tileTopLeft.y - y);
            }
            break;
        case TileAttachmentType::Down:
            for (int x = 0; x < width; ++x) {
                cells.emplace_back(tileTopLeft.x + x, tileTopLeft.y - height);
            }
            break;
        case TileAttachmentType::Left:
            for (int y = 0; y < height; ++y) {
                cells.emplace_back(tileTopLeft.x - 1, tileTopLeft.y - y);
            }
            break;
        case TileAttachmentType::None:
        default:
            return false;
            break;
    }
    for (auto &cell: cells) {
        const auto targetTilePtr = chunkManager.GetTileAt(targetLayerType, cell);
        if (targetTilePtr == nullptr) {
            return false;
        }
        if (const auto &targetTile = *targetTilePtr; !CanAttach(sourceTile, targetTile, direction)) {
            return false;
        }
    }
    return true;
}

glimmer::TileAttachmentType glimmer::TileAttachmentHelper::DeterminePriorityDirection(const TileVector2D &vectorA) {
    if (vectorA.x == 0 && vectorA.y == 0) {
        return TileAttachmentType::BackLayer;
    }
    if (vectorA.x == 0) {
        return vectorA.y > 0 ? TileAttachmentType::Up : TileAttachmentType::Down;
    }
    if (vectorA.y == 0) {
        return vectorA.x > 0 ? TileAttachmentType::Right : TileAttachmentType::Left;
    }
    return TileAttachmentType::Up;
}

std::vector<glimmer::TileAttachmentType> glimmer::TileAttachmentHelper::BuildSearchOrder(
    const TileAttachmentType priority) {
    switch (priority) {
        case TileAttachmentType::BackLayer:
            return {
                TileAttachmentType::BackLayer, TileAttachmentType::Up, TileAttachmentType::Right,
                TileAttachmentType::Down, TileAttachmentType::Left
            };
        case TileAttachmentType::Right:
            return {
                TileAttachmentType::Right, TileAttachmentType::Down, TileAttachmentType::Left,
                TileAttachmentType::Up, TileAttachmentType::BackLayer
            };
        case TileAttachmentType::Down:
            return {
                TileAttachmentType::Down, TileAttachmentType::Left, TileAttachmentType::Up,
                TileAttachmentType::Right, TileAttachmentType::BackLayer
            };
        case TileAttachmentType::Left:
            return {
                TileAttachmentType::Left, TileAttachmentType::Up, TileAttachmentType::Right,
                TileAttachmentType::Down, TileAttachmentType::BackLayer
            };
        case TileAttachmentType::Up:
        case TileAttachmentType::None:
        default:
            return {
                TileAttachmentType::Up, TileAttachmentType::Right, TileAttachmentType::Down,
                TileAttachmentType::Left, TileAttachmentType::BackLayer
            };
    }
}

bool glimmer::TileAttachmentHelper::CanAttach(const Tile &sourceTile, const Tile &targetTile,
                                              const TileAttachmentType direction) {
    if (direction == TileAttachmentType::None) {
        return false;
    }
    const auto &attachmentTagFilter = sourceTile.GetAttachmentTagFilter();
    if (!attachmentTagFilter.empty()) {
        //EnableTagFilter
        //启用tag过滤。
        const TileResourceData &resourceData = targetTile.GetResourceData();
        const auto &tags = resourceData.GetTags();
        if (tags.empty()) {
            return false;
        }
        bool found = false;
        for (const ItemTagResource &tag: tags) {
            for (const uint64_t attachmentTag: attachmentTagFilter) {
                if (tag.cachedTagId == attachmentTag) {
                    //Satisfy if any tag is found
                    //发现任意一个tag，就满足条件。
                    found = true;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    const auto value = static_cast<uint8_t>(direction);
    if ((sourceTile.GetAttachmentProbeMask() & value) == 0) {
        return false;
    }
    if ((targetTile.GetAttachmentReceptionMask() & value) == 0) {
        return false;
    }
    return true;
}

glimmer::TileAttachmentSupportResult glimmer::TileAttachmentHelper::ValidateAttachment(const Tile &sourceTile,
    const ChunkManager &chunkManager, const TileVector2D &tileTopLeft, const TileAttachmentType direction) {
    TileAttachmentSupportResult result;
    const TileLayerType sourceTileLayerType = sourceTile.GetLayerType();
    result.SetTileLayerType(sourceTileLayerType);
    if (sourceTile.GetAttachmentProbeMask() == 0 || (
            sourceTile.GetAttachmentProbeMask() & static_cast<uint8_t>(direction)) == 0 || direction ==
        TileAttachmentType::None) {
        return result;
    }
    if (direction == TileAttachmentType::BackLayer) {
        auto backLayerTypeOptional = TileLayerTypeUtils::GetBackLayerType(sourceTileLayerType);
        bool hasNextLayer = backLayerTypeOptional.has_value();
        while (hasNextLayer) {
            const TileLayerType backLayer = backLayerTypeOptional.value();
            result.SetTileLayerType(backLayer);
            if (SupportsDirection(sourceTile, chunkManager, backLayer, tileTopLeft, direction)) {
                result.SetSupport(true);
                return result;
            }
            backLayerTypeOptional = TileLayerTypeUtils::GetBackLayerType(backLayer);
            hasNextLayer = backLayerTypeOptional.has_value();
        }
        return result;
    }
    result.SetSupport(SupportsDirection(sourceTile, chunkManager, sourceTileLayerType, tileTopLeft, direction));
    return result;
}

glimmer::TileAttachmentResult glimmer::TileAttachmentHelper::FindAttachment(const Tile &sourceTile,
                                                                            const ChunkManager &chunkManager,
                                                                            const TileVector2D &tileTopLeft,
                                                                            const TileVector2D &vectorA) {
    TileAttachmentResult result;
    for (const TileAttachmentType priority = DeterminePriorityDirection(vectorA); const TileAttachmentType direction:
         BuildSearchOrder(priority)) {
        const TileAttachmentSupportResult supportResult = ValidateAttachment(
            sourceTile, chunkManager, tileTopLeft, direction);
        if (supportResult.IsSupport()) {
            result.SetFindAttachment(true);
            result.SetAttachmentDirection(direction);
            result.SetAttachmentOffset(GetAttachmentOffset(direction));
            result.SetLayerType(supportResult.GetTileLayerType());
            return result;
        }
    }
    return result;
}

glimmer::Vector2DI glimmer::TileAttachmentHelper::GetAttachmentOffset(const TileAttachmentType direction) {
    switch (direction) {
        case TileAttachmentType::Up:
            return Vector2DI{0, 1};
        case TileAttachmentType::Right:
            return Vector2DI{1, 0};
        case TileAttachmentType::Down:
            return Vector2DI{0, -1};
        case TileAttachmentType::Left:
            return Vector2DI{-1, 0};
        case TileAttachmentType::BackLayer:
        case TileAttachmentType::None:
        default:
            return Vector2DI{0, 0};
    }
}
