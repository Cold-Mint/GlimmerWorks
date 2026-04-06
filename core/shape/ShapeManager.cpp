//
// Created by Cold-Mint on 2026/3/7.
//

#include "ShapeManager.h"

#include "ShapeType.h"
#include "core/log/LogCat.h"

glimmer::ShapeManager::ShapeManager() {
    LogCat::d("Initializing ShapeManager");
    std::unique_ptr<IShapeResource> droppedItemShape = std::make_unique<RectangleShapeResource>();
    auto *droppedItemShapePtr = dynamic_cast<RectangleShapeResource *>(droppedItemShape.get());
    droppedItemShapePtr->shapeType = static_cast<uint8_t>(ShapeType::RECTANGLE);
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
