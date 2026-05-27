//@genCode
// Created by Cold-Mint on 2025/10/10.
//
#pragma once
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
        void SetSelfPackageId(const std::string &selfPackageId);

        [[nodiscard]] const std::string &GetSelfPackageId() const;

        /**
         *The package Id for serialization from the toml file might be set to @self
         * 设置从toml文件序列化的包Id可能为@self
         * @param packId
         */
        void SetPackageId(const std::string &packId);

        void ReadResourceRefMessage(const ResourceRefMessage &resourceRefMessage);

        void WriteResourceRefMessage(ResourceRefMessage &resourceRefMessage) const;


        [[nodiscard]] static std::optional<ResourceRef> ParseFromId(const std::string &id,
                                                                    ResourceTypeMessage resourceType);

        /**
         * ReadResource
         * 从资源内读取数据
         * @param resource resource
         * @param resourceType resourceType
         */
        void ReadResource(const Resource &resource, ResourceTypeMessage resourceType);

        [[nodiscard]] std::string GetPackageId() const;

        void SetResourceType(ResourceTypeMessage resourceType);

        [[nodiscard]] ResourceTypeMessage GetResourceType() const;

        void SetResourceKey(const std::string &resourceKey);

        [[nodiscard]] uint64_t GetFingerprint() const override;

        [[nodiscard]] std::string GetResourceKey() const;
    };
}
