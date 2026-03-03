//
// Created by coldmint on 2026/3/1.
//

#include "ContributorManager.h"

glimmer::ContributorManager::ContributorManager() {
    auto coldMintContributor = std::make_unique<Contributor>();
    coldMintContributor->uuid = DEV_UUID_COLD_MINT;
    coldMintContributor->name = DEV_NAME_COLO_MINT;
    ResourceRef displayName;
    displayName.SetSelfPackageId(RESOURCE_REF_CORE);
    displayName.SetResourceKey(DEV_DISPLAY_NAME_KEY_COLD_MINT);
    displayName.SetResourceType(RESOURCE_TYPE_STRING);
    coldMintContributor->displayName = displayName;
    coldMintContributor->country = "CN";
    Register(std::move(coldMintContributor));
}

bool glimmer::ContributorManager::Register(std::unique_ptr<Contributor> contributor) {
    if (contributor == nullptr) {
        return false;
    }
    std::string &uuid = contributor->uuid;
    if (contributors.contains(uuid)) {
        return false;
    }
    contributors.insert(std::make_pair(std::move(uuid), std::move(contributor)));
    return true;
}

glimmer::Contributor *glimmer::ContributorManager::Find(const std::string &uuid) const {
    return contributors.find(uuid)->second.get();
}
