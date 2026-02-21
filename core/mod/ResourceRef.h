//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef RESOURCEREF_H
#define RESOURCEREF_H
#include <string>

#include "../Constants.h"
#include "src/core/resource_ref.pb.h"
#include "toml11/spec.hpp"

namespace glimmer {
    class ResourceRef;

    class ResourceRefArg {
        std::string name_;
        uint32_t argType_ = RESOURCE_REF_ARG_TYPE_NONE;
        std::string data_;

        inline static const std::unordered_map<std::string, uint32_t> resourceArgTypeMap_{
            {"none", RESOURCE_REF_ARG_TYPE_NONE},
            {"string", RESOURCE_REF_ARG_TYPE_STRING},
            {"int", RESOURCE_REF_ARG_TYPE_INT},
            {"float", RESOURCE_REF_ARG_TYPE_FLOAT},
            {"bool", RESOURCE_REF_ARG_TYPE_BOOL},
            {"pb", RESOURCE_REF_ARG_TYPE_REF_PB},
            {"ref", RESOURCE_REF_ARG_TYPE_REF_TOML},
        };

    public:
        const std::string &GetName();

        static uint32_t ResolveResourceRefArgType(const std::string &typeName);

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

        /**
         * Set data from the resource reference.
         * 从资源引用内设置数据
         * @param data
         */
        void SetDataFromResourceRef(ResourceRef &data);


        [[nodiscard]] int AsInt() const;

        [[nodiscard]] bool AsBool() const;

        [[nodiscard]] std::string AsString() const;

        [[nodiscard]] float AsFloat() const;

        [[nodiscard]] std::optional<ResourceRef> AsResourceRef(const toml::spec &tomlVersion) const;

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
        std::string selfPackageId_;
        std::vector<ResourceRefArg> args_;
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
        };

    public:
        void UpdateArgs(const toml::spec &tomlVersion);

        /**
         * SetSelfPackageId
         * 设置自身的包ID
         * @param selfPackageId selfPackageId 自身的包Id
         */
        void SetSelfPackageId(const std::string &selfPackageId);

        [[nodiscard]] const std::string &GetSelfPackageId() const;

        static uint32_t ResolveResourceType(const std::string &typeName);

        void AddArg(const ResourceRefArg &arg);

        [[nodiscard]] size_t GetArgCount() const;

        /**
         * Obtain the parameters at the specified location
         * 获取指定位置的参数
         * @param index
         * @return
         */
        [[nodiscard]] std::optional<ResourceRefArg> GetArg(int index) const;

        bool RemoveArg(int index);

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
