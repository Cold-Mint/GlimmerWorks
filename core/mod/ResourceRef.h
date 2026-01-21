//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef RESOURCEREF_H
#define RESOURCEREF_H
#include <string>

#include "../Constants.h"
#include "src/core/resource_ref.pb.h"

namespace glimmer {
    class ResourceRefArg {
        std::string name_;
        uint32_t argType_ = RESOURCE_REF_ARG_TYPE_STRING;
        std::string data_;

    public:
        void SetName(const std::string &name);

        /**
         * Setting from int data
         * 从int数据设置
         * @param data
         */
        void SetDataFromInt(int data);

        /**
         * Setting from float data
         * 从float数据设置
         * @param data
         */
        void SetDataFromFloat(float data);

        /**
        * Setting from bool data
        * 从bool数据设置
        * @param data
        */
        void SetDataFromBool(bool data);

        /**
         * From string data settings
         * 从字符串数据设置
         * @param data
         */
        void SetDataFromString(const std::string &data);


        [[nodiscard]] int AsInt() const;

        [[nodiscard]] bool AsBool() const;

        [[nodiscard]] const std::string &AsString();

        [[nodiscard]] float AsFloat() const;

        /**
         * Obtain parameter type
         * 获取参数类型
         * @return
         */
        [[nodiscard]] uint32_t GetArgType() const;

        void FromMessage(const ResourceRefArgMessage &resourceRefArgMessage);

        void ToMessage(ResourceRefArgMessage &resourceRefArgMessage);
    };

    class ResourceRef {
        std::string packId_ = RESOURCE_REF_SELF;
        uint32_t resourceType_ = RESOURCE_TYPE_NONE;
        std::string resourceKey_;
        std::vector<ResourceRefArg> args_;
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

        void SetResourceType(uint32_t resourceType);

        [[nodiscard]] uint32_t GetResourceType() const;

        void SetResourceKey(const std::string &resourceKey);

        [[nodiscard]] std::string GetResourceKey() const;
    };
}


#endif //RESOURCEREF_H
