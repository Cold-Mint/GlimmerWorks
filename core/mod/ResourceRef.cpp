//
// Created by Cold-Mint on 2025/10/10.
//
#include "ResourceRef.h"

#include <cassert>

#include "../log/LogCat.h"

#include "../utils/TomlUtils.h"
#include "toml11/parser.hpp"

void glimmer::ResourceRef::SetSelfPackageId(const std::string &selfPackageId) {
    if (packId_ == RESOURCE_REF_SELF) {
        packId_ = selfPackageId;
    }
    selfPackageId_ = selfPackageId;
    bindPackage_ = true;
}

const std::string &glimmer::ResourceRef::GetSelfPackageId() const {
    return selfPackageId_;
}

uint32_t glimmer::ResourceRef::ResolveResourceType(const std::string &typeName) {
    const auto it = resourceTypeMap_.find(typeName);
    return it == resourceTypeMap_.end() ? RESOURCE_TYPE_NONE : it->second;
}


void glimmer::ResourceRef::SetPackageId(const std::string &packId) {
    packId_ = packId;
}

void glimmer::ResourceRef::ReadResourceRefMessage(const ResourceRefMessage &resourceRefMessage) {
    packId_ = resourceRefMessage.packid();
    resourceType_ = resourceRefMessage.resourcetype();
    resourceKey_ = resourceRefMessage.resourcekey();
    selfPackageId_ = resourceRefMessage.selfpackageid();
    bindPackage_ = true;
}

void glimmer::ResourceRef::WriteResourceRefMessage(ResourceRefMessage &resourceRefMessage) const {
    resourceRefMessage.set_packid(packId_);
    resourceRefMessage.set_resourcetype(resourceType_);
    resourceRefMessage.set_resourcekey(resourceKey_);
    resourceRefMessage.set_selfpackageid(selfPackageId_);
}

void glimmer::ResourceRef::ReadResource(const Resource &resource, const uint32_t resourceType) {
    packId_ = resource.packId;
    resourceKey_ = resource.resourceId;
    selfPackageId_ = resource.packId;
    resourceType_ = resourceType;
}

std::optional<glimmer::ResourceRef> glimmer::ResourceRef::ParseFromId(const std::string &id, const int resourceType) {
    auto pos = id.find(':');
    if (pos == std::string::npos) {
        return std::nullopt;
    }
    ResourceRef ref;
    ref.SetSelfPackageId(id.substr(0, pos));
    if (ref.GetPackageId().empty()) {
        return std::nullopt;
    }
    ref.SetResourceKey(id.substr(pos + 1));
    if (ref.GetResourceKey().empty()) {
        return std::nullopt;
    }
    ref.SetResourceType(resourceType);
    return ref;
}

std::string glimmer::ResourceRef::GetPackageId() const {
    if (!bindPackage_) {
        LogCat::e("Unbound packages may return placeholders.");
        assert(false);
    }
    return packId_;
}

void glimmer::ResourceRef::SetResourceType(const uint32_t resourceType) {
    resourceType_ = resourceType;
}

uint32_t glimmer::ResourceRef::GetResourceType() const {
    return resourceType_;
}

void glimmer::ResourceRef::SetResourceKey(const std::string &resourceKey) {
    resourceKey_ = resourceKey;
}

std::string glimmer::ResourceRef::GetResourceKey() const {
    return resourceKey_;
}
