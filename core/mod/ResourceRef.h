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
#pragma once
//@genCode
#include <string>

#include "core/Constants.h"
#include "core/IFingerprintable.h"
#include "src/core/resource_ref.pb.h"

namespace glimmer {
    struct Resource;
    class ResourceRef;

    //@content(1)
    //
    // template<>
    // struct from<glimmer::ResourceRef> {
    //     static glimmer::ResourceRef from_toml(const value &v) {
    //         glimmer::ResourceRef r;
    //         r.SetPackageId(toml::find<std::string>(v, "pack_id"));
    //         r.SetResourceType(static_cast<ResourceTypeMessage>(toml::find<int>(v, "resource_type")));
    //         r.SetResourceKey(toml::find<std::string>(v, "resource_key"));
    //         return r;
    //     }
    // };
    //
    //@endContent

    class ResourceRef : public IFingerprintable {
        std::string packId_ = RESOURCE_REF_SELF;
        ResourceTypeMessage resourceType_ = RESOURCE_NONE;
        std::string resourceKey_;
        std::string selfPackageId_;
        bool bindPackage_ = false;

    public:
        /**
         * SetSelfPackageId
         * 设置自身的包ID
         * @param selfPackageId selfPackageId 自身的包Id
         */
        void SetSelfPackageId(std::string_view selfPackageId);

        [[nodiscard]] const std::string &GetSelfPackageId() const;

        void SetPackageId(std::string_view packId);

        void ReadResourceRefMessage(const ResourceRefMessage &resourceRefMessage);

        void WriteResourceRefMessage(ResourceRefMessage &resourceRefMessage) const;


        [[nodiscard]] static std::optional<ResourceRef> ParseFromId(std::string_view id,
                                                                    ResourceTypeMessage resourceType);

        void ReadResource(const Resource &resource, ResourceTypeMessage resourceType);

        [[nodiscard]] std::string GetPackageId() const;

        void SetResourceType(ResourceTypeMessage resourceType);

        [[nodiscard]] ResourceTypeMessage GetResourceType() const;

        void SetResourceKey(std::string_view resourceKey);

        [[nodiscard]] uint64_t GetFingerprint() const override;

        [[nodiscard]] std::string GetResourceKey() const;
    };
}
