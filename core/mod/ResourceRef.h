//@genCode
// Created by Cold-Mint on 2025/10/10.
//
#ifndef RESOURCEREF_H
#define RESOURCEREF_H
#include <string>

#include "../Constants.h"
#include "src/core/resource_ref.pb.h"
#include "toml11/spec.hpp"

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
    //         r.SetResourceType(glimmer::ResourceRef::ResolveResourceType(toml::find<std::string>(v, "resource_type")));
    //         r.SetResourceKey(toml::find<std::string>(v, "resource_key"));
    //         return r;
    //     }
    // };
    //
    //@endContent

    class ResourceRef {
        std::string packId_ = RESOURCE_REF_SELF;
        uint32_t resourceType_ = RESOURCE_TYPE_NONE;
        std::string resourceKey_;
        std::string selfPackageId_;
        bool bindPackage_ = false;

        inline static const std::unordered_map<std::string, uint32_t> resourceTypeMap_{
            {"none", RESOURCE_TYPE_NONE},
            {"string", RESOURCE_TYPE_STRING},
            {"tile", RESOURCE_TYPE_TILE},
            {"composable", RESOURCE_TYPE_COMPOSABLE_ITEM},
            {"ability", RESOURCE_TYPE_ABILITY_ITEM},
            {"loot", RESOURCE_TYPE_LOOT_TABLE},
            {"structure", RESOURCE_TYPE_STRUCTURE},
            {"texture", RESOURCE_TYPE_TEXTURES},
            {"audio", RESOURCE_TYPE_AUDIO},
            {"biome", RESOURCE_TYPE_BIOME},
            {"color", RESOURCE_TYPE_COLOR},
            {"shape", RESOURCE_TYPE_SHAPE},
        };

    public:
        /**
         * SetSelfPackageId
         * 设置自身的包ID
         * @param selfPackageId selfPackageId 自身的包Id
         */
        void SetSelfPackageId(const std::string &selfPackageId);

        [[nodiscard]] const std::string &GetSelfPackageId() const;

        static uint32_t ResolveResourceType(const std::string &typeName);

        /**
         *The package Id for serialization from the toml file might be set to @self
         * 设置从toml文件序列化的包Id可能为@self
         * @param packId
         */
        void SetPackageId(const std::string &packId);

        void ReadResourceRefMessage(const ResourceRefMessage &resourceRefMessage);

        void WriteResourceRefMessage(ResourceRefMessage &resourceRefMessage) const;


        [[nodiscard]] static std::optional<ResourceRef> ParseFromId(const std::string &id, int resourceType);

        /**
         * ReadResource
         * 从资源内读取数据
         * @param resource resource
         * @param resourceType resourceType
         */
        void ReadResource(const Resource &resource, uint32_t resourceType);

        [[nodiscard]] std::string GetPackageId() const;

        void SetResourceType(uint32_t resourceType);

        [[nodiscard]] uint32_t GetResourceType() const;

        void SetResourceKey(const std::string &resourceKey);

        [[nodiscard]] std::string GetResourceKey() const;
    };
}

#endif //RESOURCEREF_H
