//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef RESOURCE_H
#define RESOURCE_H
#include <optional>
#include <string>
#include <vector>

#include "ResourceRef.h"

namespace glimmer {
    struct TileRules;

    /**
     * Resource
     * 资源
     * All base classes that serialize objects from data packets.
     * 所有从数据包序列化对象的基类。
     */
    struct Resource {
        std::string packId;
        std::string key;

        [[nodiscard]] static std::string GenerateId(const std::string &packId, const std::string &key) {
            return packId + ":" + key;
        }

        [[nodiscard]] static std::string GenerateId(const Resource &resource) {
            return GenerateId(resource.packId, resource.key);
        }

        [[nodiscard]] static std::optional<ResourceRef> ParseFromId(const std::string &id, const int resourceType) {
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
    };

    /**
     * StringResource
     * 字符串资源
     */
    struct StringResource : Resource {
        std::string value;
    };

    /**
     * ComposableItemResource
     * 可组合的物品资源
     */
    struct ComposableItemResource : Resource {
        ResourceRef name;
        ResourceRef description;
        std::string texture;
        size_t slotSize;
        std::vector<ResourceRef> defaultAbilityList;
    };

    enum VariableDefinitionType:u_int8_t {
        INT = 0,
        FLOAT,
        BOOL,
        STRING
    };

    struct VariableDefinition {
        std::string key;
        VariableDefinitionType type = STRING;
        std::string value;


        int AsInt() const {
            if (type != INT) {
                return 0;
            }
            return std::stoi(value);
        }

        float AsFloat() const {
            if (type != FLOAT) {
                return 0.0F;
            }
            return std::stof(value);
        }

        bool AsBool() const {
            if (type != BOOL) {
                return false;
            }
            if (value == "1" || value == "true" || value == "yes" || value == "y") {
                return true;
            }
            if (value == "0" || value == "false" || value == "no" || value == "n") {
                return false;
            }
            return false;
        }

        std::string AsString() {
            if (type != STRING) {
                return "";
            }
            return value;
        }
    };

    struct VariableConfig {
        std::vector<VariableDefinition> definition;

        const VariableDefinition *FindVariable(const std::string &name) const {
            for (auto &data: definition) {
                if (data.key == name) {
                    return &data;
                }
            }
            return nullptr;
        }
    };

    /**
     * AbilityItem
     * 能力物品
     */
    struct AbilityItemResource : Resource {
        ResourceRef name;
        ResourceRef description;
        std::string texture;
        std::string ability;
        VariableConfig abilityConfig;
        bool canUseAlone = false;
    };

    /**
     * TileResource
     * 瓦片资源
     */
    struct TileResource : Resource {
        ResourceRef name;
        ResourceRef description;
        std::string texture;
        float hardness = 1.0F;
        bool breakable = true;
        uint8_t physicsType = 0;
        uint8_t layerType = 0;
        /**
         * Is it an incorrect placeholder?
         * 是否为错误占位符
         */
        bool errorPlaceholder = false;
    };


    /**
     * TilePlacerRef
     * 瓦片放置器引用
     */
    struct TilePlacerRef {
        std::string id;
        std::vector<ResourceRef> tiles;
        VariableConfig config;
    };


    /**
     * BiomeResource
     * 生物群系
     */
    struct BiomeResource : Resource {
        std::vector<TilePlacerRef> tilePlacerRefs;
        float humidity = 0.0F;
        float temperature = 0.0F;
        float weirdness = 0.0F;
        float erosion = 0.0F;
        float elevation = 0.0F;
    };
}


#endif //RESOURCE_H
