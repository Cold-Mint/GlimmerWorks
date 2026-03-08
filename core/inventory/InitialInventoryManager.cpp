//
// Created by coldmint on 2026/2/21.
//

#include "InitialInventoryManager.h"


void glimmer::InitialInventoryManager::AddResource(
    std::unique_ptr<InitialInventoryResource> resource) {
    initialInventoryPtr_.emplace_back(resource.get());
    initialInventory_.emplace_back(std::move(resource));
}

const std::vector<glimmer::InitialInventoryResource *> &glimmer::InitialInventoryManager::
GetAllInitialInventory() const {
    return initialInventoryPtr_;
}


std::string glimmer::InitialInventoryManager::ListInitialInventory() const {
    std::ostringstream oss;
    for (const auto &value: initialInventory_) {
        if (!value) {
            continue;
        }
        oss << Resource::GenerateId(value->packId, value->resourceId) << "\n";
    }
    return oss.str();
}
