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
#include "ShapeManager.h"

#include <utility>

#include "ShapeType.h"
#include "core/log/LogCat.h"

glimmer::ShapeManager::ShapeManager() {
    LogCat::d("Initializing ShapeManager");
    std::unique_ptr<IShapeResource> droppedItemShape = std::make_unique<RectangleShapeResource>();
    auto *droppedItemShapePtr = dynamic_cast<RectangleShapeResource *>(droppedItemShape.get());
    droppedItemShapePtr->shapeType = std::to_underlying(ShapeType::RECTANGLE);
    droppedItemShapePtr->width = 0.8F;
    droppedItemShapePtr->height = 0.8F;
    droppedItemShapePtr->packId = RESOURCE_REF_CORE;
    droppedItemShapePtr->resourceId = SHAPE_ID_DROPPED_ITEM;
    Register(std::move(droppedItemShape));
}

glimmer::IShapeResource *glimmer::ShapeManager::Register(std::unique_ptr<IShapeResource> shapeResource) {
    auto &slot =
            shapeMap_[shapeResource->packId][shapeResource->resourceId];
    slot = std::move(shapeResource);
    return slot.get();
}

glimmer::IShapeResource *glimmer::ShapeManager::FindShape(const std::string &packId, const std::string &resourceId) {
    LogCat::d("Searching for shape resource: packId = ", packId, ", resourceId = ", resourceId);
    const auto packIt = shapeMap_.find(packId);
    if (packIt == shapeMap_.end()) {
        LogCat::w("Pack not found: ", packId);
        return nullptr;
    }

    auto &keyMap = packIt->second;
    const auto keyIt = keyMap.find(resourceId);
    if (keyIt == keyMap.end()) {
        LogCat::w("Key not found in pack ", packId, ": ", resourceId);
        return nullptr;
    }

    LogCat::i("Found shape resource: packId = ", packId, ", resourceId = ", resourceId);
    return keyIt->second.get();
}

std::string glimmer::ShapeManager::ListShapes() const {
    std::ostringstream oss;
    for (const auto &packPair: shapeMap_) {
        const auto &packId = packPair.first;
        const auto &keyMap = packPair.second;
        for (const auto &keyPair: keyMap) {
            const auto &key = keyPair.first;
            oss << Resource::GenerateId(packId, key) << "\n";
        }
    }
    return oss.str();
}
