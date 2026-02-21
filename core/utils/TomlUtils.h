//
// Created by coldmint on 2026/1/26.
//

#ifndef GLIMMERWORKS_TOMLUTILS_H
#define GLIMMERWORKS_TOMLUTILS_H
#include "core/math/Vector2D.h"
#include "core/mod/PackManifest.h"
#include "core/mod/Resource.h"
#include "core/mod/ResourceRef.h"
#include "toml11/find.hpp"


namespace toml {
    template<>
    struct from<glimmer::Resource> {
        static glimmer::Resource from_toml(const value &v) {
            glimmer::Resource r;
            r.key = toml::find<std::string>(v, "resourceKey");
            return r;
        }
    };


    template<>
    struct from<glimmer::ResourceRef> {
        static glimmer::ResourceRef from_toml(const value &v) {
            glimmer::ResourceRef r;
            r.SetPackageId(toml::find<std::string>(v, "packId"));
            r.SetResourceType(glimmer::ResourceRef::ResolveResourceType(toml::find<std::string>(v, "resourceType")));
            r.SetResourceKey(toml::find<std::string>(v, "resourceKey"));
            auto arg = toml::find_or_default<std::vector<glimmer::ResourceRefArg> >(
                v, "args");
            for (auto &resourceRefArg: arg) {
                r.AddArg(resourceRefArg);
            }
            return r;
        }
    };

    template<>
    struct from<glimmer::ResourceRefArg> {
        static glimmer::ResourceRefArg from_toml(const value &v) {
            glimmer::ResourceRefArg r;
            r.SetName(toml::find<std::string>(v, "name"));

            uint32_t argType =
                    glimmer::ResourceRefArg::ResolveResourceRefArgType(toml::find<std::string>(v, "type"));
            if (argType == RESOURCE_REF_ARG_TYPE_INT) {
                r.SetDataFromInt(toml::find<int>(v, "data"));
            } else if (argType == RESOURCE_REF_ARG_TYPE_FLOAT) {
                r.SetDataFromFloat(toml::find<float>(v, "data"));
            } else if (argType == RESOURCE_REF_ARG_TYPE_BOOL) {
                r.SetDataFromBool(toml::find<bool>(v, "data"));
            } else if (argType == RESOURCE_REF_ARG_TYPE_STRING) {
                r.SetDataFromString(toml::find<std::string>(v, "data"));
            } else if (argType == RESOURCE_REF_ARG_TYPE_REF_TOML) {
                glimmer::ResourceRef resource = toml::find<glimmer::ResourceRef>(v, "data");
                r.SetDataFromResourceRef(resource);
            }
            return r;
        }
    };

    template<>
    struct from<glimmer::StringResource> {
        static glimmer::StringResource from_toml(const value &v) {
            glimmer::StringResource r;
            r.key = toml::find<std::string>(v, "resourceKey");
            r.value = toml::find<std::string>(v, "value");
            return r;
        }
    };


    template<>
    struct from<glimmer::VariableDefinition> {
        static glimmer::VariableDefinition from_toml(const value &v) {
            glimmer::VariableDefinition r;
            r.key = toml::find<std::string>(v, "key");
            r.type = static_cast<glimmer::VariableDefinitionType>(toml::find<uint8_t>(v, "type"));
            if (r.type == glimmer::VariableDefinitionType::INT) {
                r.value = std::to_string(toml::find<int>(v, "value"));
            } else if (r.type == glimmer::VariableDefinitionType::FLOAT) {
                r.value = std::to_string(toml::find<float>(v, "value"));
            } else if (r.type == glimmer::VariableDefinitionType::BOOL) {
                r.value = std::to_string(toml::find<bool>(v, "value"));
            } else {
                r.value = toml::find<std::string>(v, "value");
            }
            return r;
        }
    };

    template<>
    struct from<glimmer::VariableConfig> {
        static glimmer::VariableConfig from_toml(const value &v) {
            glimmer::VariableConfig r;
            r.definition = toml::find<std::vector<glimmer::VariableDefinition> >(v, "definition");
            return r;
        }
    };

    template<>
    struct from<glimmer::AbilityItemResource> {
        static glimmer::AbilityItemResource from_toml(const value &v) {
            glimmer::AbilityItemResource r;
            r.key = toml::find<std::string>(v, "resourceKey");
            r.texture = toml::find<glimmer::ResourceRef>(v, "texture");
            r.name = toml::find<glimmer::ResourceRef>(v, "name");
            r.description = toml::find<glimmer::ResourceRef>(v, "description");
            r.ability = toml::find<std::string>(v, "ability");
            r.abilityConfig = toml::find<glimmer::VariableConfig>(v, "abilityConfig");
            r.canUseAlone = toml::find<bool>(v, "canUseAlone");
            return r;
        }
    };


    template<>
    struct from<glimmer::ComposableItemResource> {
        static glimmer::ComposableItemResource from_toml(const value &v) {
            glimmer::ComposableItemResource r;
            r.key = toml::find<std::string>(v, "resourceKey");
            r.texture = toml::find<glimmer::ResourceRef>(v, "texture");
            r.name = toml::find<glimmer::ResourceRef>(v, "name");
            r.description = toml::find<glimmer::ResourceRef>(v, "description");
            r.slotSize = toml::find<size_t>(v, "slotSize");
            r.defaultAbilityList = toml::find_or_default<std::vector<glimmer::ResourceRef> >(v, "defaultAbilityList");
            return r;
        }
    };

    template<>
    struct from<glimmer::PackDependence> {
        static glimmer::PackDependence from_toml(const value &v) {
            glimmer::PackDependence r;
            r.packId = toml::find<std::string>(v, "packId");
            r.minVersion = toml::find<uint32_t>(v, "minVersion");
            return r;
        }
    };

    template<>
    struct from<glimmer::DataPackManifest> {
        static glimmer::DataPackManifest from_toml(const value &v) {
            glimmer::DataPackManifest r;
            r.id = toml::find<std::string>(v, "id");
            r.name = toml::find<glimmer::ResourceRef>(v, "name");
            r.description = toml::find<glimmer::ResourceRef>(v, "description");
            r.resPack = toml::find<bool>(v, "resPack");
            r.author = toml::find<std::string>(v, "author");
            r.versionName = toml::find<std::string>(v, "versionName");
            r.versionNumber = toml::find<uint32_t>(v, "versionNumber");
            r.minGameVersion = toml::find<uint32_t>(v, "minGameVersion");
            r.packDependencies = toml::find_or_default<std::vector<glimmer::PackDependence> >(v, "packDependencies");
            return r;
        }
    };


    template<>
    struct from<glimmer::Vector2D> {
        static glimmer::Vector2D from_toml(const value &v) {
            glimmer::Vector2D r;
            r.x = toml::find<float>(v, "x");
            r.y = toml::find<float>(v, "y");
            return r;
        }
    };


    template<>
    struct from<glimmer::BiomeDecoratorResource> {
        static glimmer::BiomeDecoratorResource from_toml(const value &v) {
            glimmer::BiomeDecoratorResource r;
            r.id = toml::find<std::string>(v, "id");
            r.data = toml::find<std::vector<glimmer::ResourceRef> >(v, "data");
            r.config = toml::find_or<glimmer::VariableConfig>(
                v,
                "config",
                glimmer::VariableConfig{}
            );

            return r;
        }
    };


    template<>
    struct from<glimmer::BiomeResource> {
        static glimmer::BiomeResource from_toml(const value &v) {
            glimmer::BiomeResource r;
            r.key = toml::find<std::string>(v, "resourceKey");
            r.decorator = toml::find_or_default<std::vector<glimmer::BiomeDecoratorResource> >(v, "decorator");
            r.humidity = toml::find<float>(v, "humidity");
            r.temperature = toml::find<float>(v, "temperature");
            r.weirdness = toml::find<float>(v, "weirdness");
            r.erosion = toml::find<float>(v, "erosion");
            r.elevation = toml::find<float>(v, "elevation");
            return r;
        }
    };


    template<>
    struct from<glimmer::TileResource> {
        static glimmer::TileResource from_toml(const value &v) {
            glimmer::TileResource r;
            r.name = toml::find<glimmer::ResourceRef>(v, "name");
            r.description = toml::find<glimmer::ResourceRef>(v, "description");
            r.customLootTable = toml::find_or<bool>(v, "customLootTable", false);
            r.lootTable = toml::find_or_default<glimmer::ResourceRef>(v, "lootTable");
            r.key = toml::find<std::string>(v, "resourceKey");
            r.texture = toml::find<glimmer::ResourceRef>(v, "texture");
            r.physicsType = toml::find<uint8_t>(v, "physicsType");
            r.hardness = toml::find<float>(v, "hardness");
            r.breakable = toml::find<bool>(v, "breakable");
            r.layerType = toml::find<uint8_t>(v, "layerType");
            return r;
        }
    };

    template<>
    struct from<glimmer::StructureResource> {
        static glimmer::StructureResource from_toml(const value &v) {
            glimmer::StructureResource r;
            r.key = toml::find<std::string>(v, "resourceKey");
            r.generatorId = toml::find<std::string>(v, "generatorId");
            r.generatorConfig = toml::find_or_default<glimmer::VariableConfig>(v, "generatorConfig");
            r.width = toml::find_or<uint32_t>(v, "width", 1);
            r.data = toml::find_or_default<std::vector<glimmer::ResourceRef> >(v, "data");
            return r;
        }
    };

    template<>
    struct from<glimmer::ResourcePackManifest> {
        static glimmer::ResourcePackManifest from_toml(const value &v) {
            glimmer::ResourcePackManifest r;
            r.id = toml::find<std::string>(v, "id");
            r.name = toml::find<glimmer::ResourceRef>(v, "name");
            r.description = toml::find<glimmer::ResourceRef>(v, "description");
            r.resPack = toml::find<bool>(v, "resPack");
            r.author = toml::find<std::string>(v, "author");
            r.versionName = toml::find<std::string>(v, "versionName");
            r.versionNumber = toml::find<uint32_t>(v, "versionNumber");
            r.minGameVersion = toml::find<uint32_t>(v, "minGameVersion");
            return r;
        }
    };

    template<>
    struct from<glimmer::LootEntry> {
        static glimmer::LootEntry from_toml(const value &v) {
            glimmer::LootEntry r;
            r.item = toml::find<glimmer::ResourceRef>(v, "item");
            r.weight = toml::find_or<uint32_t>(v, "weight", 1);
            r.max = toml::find_or<uint32_t>(v, "max", 1);
            r.min = toml::find_or<uint32_t>(v, "min", 1);
            return r;
        }
    };

    template<>
    struct from<glimmer::LootResource> {
        static glimmer::LootResource from_toml(const value &v) {
            glimmer::LootResource r;
            r.key = toml::find<std::string>(v, "resourceKey");
            r.mandatory = toml::find_or_default<std::vector<glimmer::LootEntry> >(v, "mandatory");
            r.empty_weight = toml::find_or<uint32_t>(v, "empty_weight", 0);
            r.rolls = toml::find_or<uint32_t>(v, "rolls", 1);
            r.pool = toml::find_or_default<std::vector<glimmer::LootEntry> >(v, "pool");
            return r;
        }
    };

    template<>
    struct from<glimmer::InitialInventoryResource> {
        static glimmer::InitialInventoryResource from_toml(const value &v) {
            glimmer::InitialInventoryResource r;
            r.key = toml::find<std::string>(v, "resourceKey");
            r.addItems = toml::find_or_default<std::vector<glimmer::ResourceRef> >(v, "addItems");
            return r;
        }
    };
}


#endif //GLIMMERWORKS_TOMLUTILS_H
