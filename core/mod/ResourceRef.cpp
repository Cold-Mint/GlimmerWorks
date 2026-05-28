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

std::optional<glimmer::ResourceRef> glimmer::ResourceRef::ParseFromId(const std::string &id,
                                                                      const ResourceTypeMessage resourceType) {
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

void glimmer::ResourceRef::ReadResource(const Resource &resource, const ResourceTypeMessage resourceType) {
    packId_ = resource.packId;
    resourceKey_ = resource.resourceId;
    selfPackageId_ = resource.packId;
    resourceType_ = resourceType;
    bindPackage_ = true;
}

std::string glimmer::ResourceRef::GetPackageId() const {
    if (!bindPackage_) {
        LogCat::e("Unbound packages may return placeholders.");
#if  !defined(NDEBUG)
        assert(false);
#endif
    }
    return packId_;
}

void glimmer::ResourceRef::SetResourceType(const ResourceTypeMessage resourceType) {
    resourceType_ = resourceType;
}

ResourceTypeMessage glimmer::ResourceRef::GetResourceType() const {
    return resourceType_;
}


void glimmer::ResourceRef::SetResourceKey(const std::string &resourceKey) {
    resourceKey_ = resourceKey;
}

uint64_t glimmer::ResourceRef::GetFingerprint() const {
    return static_cast<uint64_t>(resourceType_) & 0x1FULL << 59
           | (std::hash<std::string>{}(packId_) & 0x1FFFFFFFULL) << 30
           | std::hash<std::string>{}(resourceKey_) & 0x3FFFFFFFULL;
}

std::string glimmer::ResourceRef::GetResourceKey() const {
    return resourceKey_;
}
