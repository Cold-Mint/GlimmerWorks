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

#include "core/log/LogCat.h"
#include "core/math/CoordinateTransformer.h"
#include "core/world/ChunkManager.h"
#include "core/world/Tile.h"
#include "core/world/WorldContext.h"

const glimmer::Tile *glimmer::TileAttachmentHelper::GetTileAt(const WorldContext *worldContext,
                                                              const TileLayerType layerType, const TileVector2D &cell) {
    if (worldContext == nullptr) {
        return nullptr;
    }
    const ChunkManager *chunkManager = worldContext->GetChunkManager();
    if (chunkManager == nullptr) {
        return nullptr;
    }
    return chunkManager->GetTileAt(layerType, cell);
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

bool glimmer::TileAttachmentHelper::HasAnyTag(const Tile *tile, const std::vector<uint64_t> &tagIds) {
    if (tile == nullptr || tagIds.empty()) {
        return false;
    }
    const TileResourceData *resourceData = tile->GetResourceData();
    if (resourceData == nullptr) {
        return false;
    }
    for (const auto &tag: resourceData->GetTags()) {
        for (const uint64_t tagId: tagIds) {
            if (tag.cachedTagId == tagId) {
                return true;
            }
        }
    }
    return false;
}

bool glimmer::TileAttachmentHelper::IsSupportValid(const WorldContext *worldContext, TileLayerType layerType,
                                                   const TileVector2D &tileTopLeft, uint8_t width, uint8_t height,
                                                   TileAttachmentType direction, const std::vector<uint64_t> &tagIds) {
    if (tagIds.empty()) {
        LogCat::d(LogLabel::ATTACHMENT, "attachment_support_tag_empty",
                  "IsSupportValid: tagIds is empty, direction={}", static_cast<int>(direction));
        return false;
    }
    LogCat::d(LogLabel::ATTACHMENT, "attachment_support_check_start",
              "IsSupportValid start: layer={}, topLeft=({}, {}), size={}x{}, direction={}, tagCount={}",
              static_cast<int>(layerType), tileTopLeft.x, tileTopLeft.y, width, height,
              static_cast<int>(direction), tagIds.size());
    if (direction == TileAttachmentType::BackLayer) {
        if (layerType != TileLayerType::Ground) {
            LogCat::d(LogLabel::ATTACHMENT, "attachment_support_back_layer_invalid",
                      "IsSupportValid: BackLayer requires Ground layer, current layer={}",
                      static_cast<int>(layerType));
            return false;
        }
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                const TileVector2D cell{tileTopLeft.x + x, tileTopLeft.y - y};
                const Tile *support = GetTileAt(worldContext, TileLayerType::BackGround, cell);
                if (!HasAnyTag(support, tagIds)) {
                    LogCat::d(LogLabel::ATTACHMENT, "attachment_support_cell_invalid",
                              "IsSupportValid cell invalid: direction={}, supportLayer={}, cell=({}, {})",
                              static_cast<int>(direction), static_cast<int>(TileLayerType::BackGround),
                              cell.x, cell.y);
                    return false;
                }
            }
        }
        LogCat::d(LogLabel::ATTACHMENT, "attachment_support_valid",
                  "IsSupportValid valid: direction={}", static_cast<int>(direction));
        return true;
    }
    switch (direction) {
        case TileAttachmentType::Up:
            for (int x = 0; x < width; ++x) {
                const TileVector2D cell{tileTopLeft.x + x, tileTopLeft.y + 1};
                if (!HasAnyTag(GetTileAt(worldContext, layerType, cell), tagIds)) {
                    LogCat::d(LogLabel::ATTACHMENT, "attachment_support_cell_invalid",
                              "IsSupportValid cell invalid: direction={}, supportLayer={}, cell=({}, {})",
                              static_cast<int>(direction), static_cast<int>(layerType), cell.x, cell.y);
                    return false;
                }
            }
            break;
        case TileAttachmentType::Down:
            for (int x = 0; x < width; ++x) {
                const TileVector2D cell{tileTopLeft.x + x, tileTopLeft.y - height};
                if (!HasAnyTag(GetTileAt(worldContext, layerType, cell), tagIds)) {
                    LogCat::d(LogLabel::ATTACHMENT, "attachment_support_cell_invalid",
                              "IsSupportValid cell invalid: direction={}, supportLayer={}, cell=({}, {})",
                              static_cast<int>(direction), static_cast<int>(layerType), cell.x, cell.y);
                    return false;
                }
            }
            break;
        case TileAttachmentType::Right:
            for (int y = 0; y < height; ++y) {
                const TileVector2D cell{tileTopLeft.x + width, tileTopLeft.y - y};
                if (!HasAnyTag(GetTileAt(worldContext, layerType, cell), tagIds)) {
                    LogCat::d(LogLabel::ATTACHMENT, "attachment_support_cell_invalid",
                              "IsSupportValid cell invalid: direction={}, supportLayer={}, cell=({}, {})",
                              static_cast<int>(direction), static_cast<int>(layerType), cell.x, cell.y);
                    return false;
                }
            }
            break;
        case TileAttachmentType::Left:
            for (int y = 0; y < height; ++y) {
                const TileVector2D cell{tileTopLeft.x - 1, tileTopLeft.y - y};
                if (!HasAnyTag(GetTileAt(worldContext, layerType, cell), tagIds)) {
                    LogCat::d(LogLabel::ATTACHMENT, "attachment_support_cell_invalid",
                              "IsSupportValid cell invalid: direction={}, supportLayer={}, cell=({}, {})",
                              static_cast<int>(direction), static_cast<int>(layerType), cell.x, cell.y);
                    return false;
                }
            }
            break;
        default:
            return false;
    }
    LogCat::d(LogLabel::ATTACHMENT, "attachment_support_valid",
              "IsSupportValid valid: direction={}", static_cast<int>(direction));
    return true;
}

bool glimmer::TileAttachmentHelper::FindAttachment(const WorldContext *worldContext, const TileLayerType layerType,
                                                   const TileVector2D &tileTopLeft, const uint8_t width,
                                                   const uint8_t height,
                                                   const uint8_t attachmentMask, const std::vector<uint64_t> &tagIds,
                                                   const TileVector2D &vectorA, TileAttachmentDirection &outDirection,
                                                   Vector2DI &outOffset) {
    outDirection = TileAttachmentDirection::None;
    outOffset = Vector2DI{0, 0};
    if (attachmentMask == 0) {
        return false;
    }
    const TileAttachmentType priority = DeterminePriorityDirection(vectorA);
    LogCat::d(LogLabel::ATTACHMENT, "attachment_find_start",
              "FindAttachment start: layer={}, topLeft=({}, {}), size={}x{}, mask={}, tagCount={}, vectorA=({}, {}), priority={}",
              static_cast<int>(layerType), tileTopLeft.x, tileTopLeft.y, width, height, attachmentMask, tagIds.size(),
              vectorA.x, vectorA.y, static_cast<int>(priority));
    for (const TileAttachmentType direction: BuildSearchOrder(priority)) {
        if ((attachmentMask & static_cast<uint8_t>(direction)) == 0) {
            LogCat::d(LogLabel::ATTACHMENT, "attachment_direction_skipped_by_mask",
                      "Direction {} skipped: not set in mask {}", static_cast<int>(direction), attachmentMask);
            continue;
        }
        if (direction == TileAttachmentType::BackLayer && layerType != TileLayerType::Ground) {
            LogCat::d(LogLabel::ATTACHMENT, "attachment_direction_skipped_by_layer",
                      "Direction BackLayer skipped: layer {} is not Ground", static_cast<int>(layerType));
            continue;
        }
        const bool valid = IsSupportValid(worldContext, layerType, tileTopLeft, width, height, direction, tagIds);
        LogCat::d(LogLabel::ATTACHMENT, "attachment_direction_check",
                  "Direction {} support valid: {}", static_cast<int>(direction), valid);
        if (valid) {
            outDirection = ToDirection(direction);
            outOffset = GetAttachmentOffset(direction);
            LogCat::d(LogLabel::ATTACHMENT, "attachment_find_success",
                      "FindAttachment success: direction={}, offset=({}, {})", static_cast<int>(outDirection),
                      outOffset.x, outOffset.y);
            return true;
        }
    }
    LogCat::d(LogLabel::ATTACHMENT, "attachment_find_failed",
              "FindAttachment failed: mask={}, tagCount={}, vectorA=({}, {})", attachmentMask, tagIds.size(),
              vectorA.x, vectorA.y);
    return false;
}

bool glimmer::TileAttachmentHelper::ValidateAttachment(const WorldContext *worldContext, TileLayerType layerType,
                                                       const TileVector2D &tileTopLeft, uint8_t width, uint8_t height,
                                                       const std::vector<uint64_t> &tagIds,
                                                       TileAttachmentDirection direction) {
    if (direction == TileAttachmentDirection::None) {
        return false;
    }
    const TileAttachmentType maskType = ToMaskType(direction);
    if (maskType == TileAttachmentType::None) {
        return false;
    }
    return IsSupportValid(worldContext, layerType, tileTopLeft, width, height, maskType, tagIds);
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

glimmer::TileAttachmentDirection glimmer::TileAttachmentHelper::ToDirection(const TileAttachmentType type) {
    switch (type) {
        case TileAttachmentType::Up:
            return TileAttachmentDirection::Up;
        case TileAttachmentType::Right:
            return TileAttachmentDirection::Right;
        case TileAttachmentType::Down:
            return TileAttachmentDirection::Down;
        case TileAttachmentType::Left:
            return TileAttachmentDirection::Left;
        case TileAttachmentType::BackLayer:
            return TileAttachmentDirection::BackLayer;
        case TileAttachmentType::None:
        default:
            return TileAttachmentDirection::None;
    }
}

glimmer::TileAttachmentType glimmer::TileAttachmentHelper::ToMaskType(const TileAttachmentDirection direction) {
    switch (direction) {
        case TileAttachmentDirection::Up:
            return TileAttachmentType::Up;
        case TileAttachmentDirection::Right:
            return TileAttachmentType::Right;
        case TileAttachmentDirection::Down:
            return TileAttachmentType::Down;
        case TileAttachmentDirection::Left:
            return TileAttachmentType::Left;
        case TileAttachmentDirection::BackLayer:
            return TileAttachmentType::BackLayer;
        case TileAttachmentDirection::None:
        default:
            return TileAttachmentType::None;
    }
}
