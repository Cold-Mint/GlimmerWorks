//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef RESOURCEREF_H
#define RESOURCEREF_H
#include <string>

#include "../Constants.h"
#include "core/resource_ref.pb.h"

namespace glimmer {
    class ResourceRef {
        std::string packId_ = RESOURCE_REF_SELF;
        int resourceType_ = RESOURCE_TYPE_NONE;
        std::string resourceKey_;
        bool bindPackage_ = false;

    public:
        /**
         * SetSelfPackageId
         * 设置自身的包ID
         * @param selfPackageId selfPackageId 自身的包Id
         */
        void SetSelfPackageId(const std::string &selfPackageId);

        /**
         *The package Id for serialization from the json file might be set to @self
         * 设置从json文件序列化的包Id可能为@self
         * @param packId
         */
        void SetPackageId(const std::string &packId);

        void FromMessage(const ResourceRefMessage &resourceRefMessage);

        void ToMessage(ResourceRefMessage &resourceRefMessage);

        [[nodiscard]] std::string GetPackageId() const;

        void SetResourceType(int resourceType);

        [[nodiscard]] int GetResourceType() const;

        void SetResourceKey(const std::string &resourceKey);

        [[nodiscard]] std::string GetResourceKey() const;
    };
}


#endif //RESOURCEREF_H
