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
            r.SetResourceType(toml::find<int>(v, "resourceType"));
            r.SetResourceKey(toml::find<std::string>(v, "resourceKey"));
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
            r.value = toml::find<std::string>(v, "value");
            r.type = static_cast<glimmer::VariableDefinitionType>(toml::find<uint8_t>(v, "type"));
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
            r.texture = toml::find<std::string>(v, "texture");
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
            r.texture = toml::find<std::string>(v, "texture");
            r.name = toml::find<glimmer::ResourceRef>(v, "name");
            r.description = toml::find<glimmer::ResourceRef>(v, "description");
            r.slotSize = toml::find<size_t>(v, "slotSize");
            r.defaultAbilityList = toml::find_or_default<std::vector<glimmer::ResourceRef> >(v, "defaultAbilityList");
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
            r.versionNumber = toml::find<int>(v, "versionNumber");
            r.minGameVersion = toml::find<int>(v, "minGameVersion");
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
    struct from<glimmer::TilePlacerRef> {
        static glimmer::TilePlacerRef from_toml(const value &v) {
            glimmer::TilePlacerRef r;
            r.id = toml::find<std::string>(v, "id");
            r.tiles = toml::find<std::vector<glimmer::ResourceRef> >(v, "tiles");
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
            r.tilePlacerRefs = toml::find<std::vector<glimmer::TilePlacerRef> >(v, "tilePlacerRefs");
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
            r.key = toml::find<std::string>(v, "resourceKey");
            r.texture = toml::find<std::string>(v, "texture");
            r.physicsType = toml::find<uint8_t>(v, "physicsType");
            r.hardness = toml::find<float>(v, "hardness");
            r.breakable = toml::find<bool>(v, "breakable");
            r.layerType = toml::find<uint8_t>(v, "layerType");
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
            r.versionNumber = toml::find<int>(v, "versionNumber");
            r.minGameVersion = toml::find<int>(v, "minGameVersion");
            return r;
        }
    };
}


#endif //GLIMMERWORKS_TOMLUTILS_H
