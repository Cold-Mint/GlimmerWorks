//
// Created by Cold-Mint on 2025/10/10.
//
#include "ResourceRef.h"

#include <cassert>

#include "../log/LogCat.h"


void glimmer::ResourceRef::SetSelfPackageId(const std::string &selfPackageId) {
    if (packId_ == RESOURCE_REF_SELF) {
        packId_ = selfPackageId;
    }
    bindPackage_ = true;
}

void glimmer::ResourceRef::SetPackageId(const std::string &packId) {
    packId_ = packId;
}

std::string glimmer::ResourceRef::GetPackageId() const {
    if (!bindPackage_) {
        LogCat::e("Unbound packages may return placeholders.");
        assert(true);
    }
    return packId_;
}

void glimmer::ResourceRef::SetResourceType(const int resourceType) {
    resourceType_ = resourceType;
}

int glimmer::ResourceRef::GetResourceType() const {
    return resourceType_;
}

void glimmer::ResourceRef::SetResourceKey(const std::string &resourceKey) {
    resourceKey_ = resourceKey;
}

std::string glimmer::ResourceRef::GetResourceKey() const {
    return resourceKey_;
}
