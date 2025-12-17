//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef JSONUTILS_H
#define JSONUTILS_H
#include <optional>
#include "../mod/Resource.h"
#include "../mod/ResourceRef.h"
#include "../mod/PackManifest.h"
#include "../Config.h"
#include "../math/Vector2D.h"
#include "../saves/MapManifest.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

namespace glimmer {
    struct StringResource;
    class VirtualFileSystem;


    class JsonUtils {
    public:
        static std::optional<nlohmann::json> LoadJsonFromFile(const VirtualFileSystem *virtualFileSystem,
                                                              const std::string &path);


        static std::optional<nlohmann::json> LoadJsonFromString(const std::string &jsonData);

#ifdef __ANDROID__
        static std::optional<nlohmann::json> LoadJsonFromFile(FileProvider *fileProvider,
                                                              const std::string &path);
#endif
    };
}


template<>
struct nlohmann::adl_serializer<glimmer::StringResource> {
    static void from_json(const json &j, glimmer::StringResource &s) {
        s.key = j.at("resourceKey").get<std::string>();
        s.value = j.at("value").get<std::string>();
    }

    static void to_json(json &j, const glimmer::StringResource &s) {
        j = json{
            {"resourceKey", s.key},
            {"value", s.value},
        };
    }
};


template<>
struct nlohmann::adl_serializer<glimmer::Resource> {
    static void from_json(const json &j, glimmer::Resource &r) {
        r.key = j.at("resourceKey").get<std::string>();
    }

    static void to_json(json &j, const glimmer::Resource &r) {
        j = json{
            {"resourceKey", r.key}
        };
    }
};

template<>
struct nlohmann::adl_serializer<glimmer::Mods> {
    static void from_json(const json &j, glimmer::Mods &m) {
        m.dataPackPath = j.at("dataPackPath").get<std::string>();
        m.resourcePackPath = j.at("resourcePackPath").get<std::string>();
        m.enabledDataPack = j.at("enabledDataPack").get<std::vector<std::string> >();
        m.enabledResourcePack = j.at("enabledResourcePack").get<std::vector<std::string> >();
    }

    static void to_json(json &j, const glimmer::Mods &m) {
        j = json{
            {"dataPackPath", m.dataPackPath},
            {"resourcePackPath", m.resourcePackPath},
            {"enabledDataPack", m.enabledDataPack},
            {"enabledResourcePack", m.enabledResourcePack}
        };
    }
};

template<>
struct nlohmann::adl_serializer<glimmer::Debug> {
    static void from_json(const json &j, glimmer::Debug &m) {
        m.displayDebugPanel = j.at("displayDebugPanel").get<bool>();
        m.displayBox2dShape = j.at("displayBox2dShape").get<bool>();
    }

    static void to_json(json &j, const glimmer::Debug &debug) {
        j = json{
            {"displayDebugPanel", debug.displayDebugPanel},
            {"displayBox2dShape", debug.displayBox2dShape}
        };
    }
};

template<>
struct nlohmann::adl_serializer<glimmer::Window> {
    static void from_json(const json &j, glimmer::Window &w) {
        w.framerate = j.at("framerate").get<float>();
        w.height = j.at("height").get<int>();
        w.width = j.at("width").get<int>();
        w.resizable = j.at("resizable").get<bool>();
        w.uiScale = j.at("uiScale").get<float>();
    }

    static void to_json(json &j, const glimmer::Window &w) {
        j = json{
            {"framerate", w.framerate},
            {"height", w.height},
            {"width", w.width},
            {"resizable", w.resizable},
            {"uiScale", w.uiScale}
        };
    }
};

template<>
struct nlohmann::adl_serializer<glimmer::ResourceRef> {
    static void from_json(const json &j, glimmer::ResourceRef &resourceRef) {
        resourceRef.SetPackageId(j.at("packId").get<std::string>());
        resourceRef.SetResourceType(j.at("resourceType").get<int>());
        resourceRef.SetResourceKey(j.at("resourceKey").get<std::string>());
    }

    static void to_json(json &j, const glimmer::ResourceRef &resourceRef) {
        j = json{
            {"packId", resourceRef.GetPackageId()},
            {"resourceType", resourceRef.GetResourceType()},
            {"resourceKey", resourceRef.GetResourceKey()}
        };
    }
};

template<>
struct nlohmann::adl_serializer<glimmer::TileResource> {
    static void from_json(const json &j, glimmer::TileResource &s) {
        s.key = j.at("resourceKey").get<std::string>();
        s.texture = j.at("texture").get<std::string>();
        s.name = j.at("name").get<glimmer::ResourceRef>();
        s.description = j.at("description").get<glimmer::ResourceRef>();
        s.physicsType = j.at("physicsType").get<uint8_t>();
    }

    static void to_json(json &j, const glimmer::TileResource &s) {
        j = json{
            {"resourceKey", s.key},
            {"texture", s.texture},
            {"name", s.name},
            {"description", s.description},
            {"physicsType", s.physicsType},
        };
    }
};

template<>
struct nlohmann::adl_serializer<glimmer::DataPackManifest> {
    static void from_json(const json &j, glimmer::DataPackManifest &manifest) {
        manifest.id = j.at("id").get<std::string>();
        manifest.name = j.at("name").get<glimmer::ResourceRef>();
        manifest.description = j.at("description").get<glimmer::ResourceRef>();
        manifest.resPack = j.at("resPack").get<bool>();
        manifest.author = j.at("author").get<std::string>();
        manifest.versionName = j.at("versionName").get<std::string>();
        manifest.versionNumber = j.at("versionNumber").get<int>();
        manifest.minGameVersion = j.at("minGameVersion").get<int>();
    }

    static void to_json(json &j, const glimmer::DataPackManifest &manifest) {
        j = json{
            {"id", manifest.id},
            {"name", manifest.name},
            {"description", manifest.description},
            {"resPack", manifest.resPack},
            {"author", manifest.author},
            {"versionName", manifest.versionName},
            {"versionNumber", manifest.versionNumber},
            {"minGameVersion", manifest.minGameVersion}
        };
    }
};

template<>
struct nlohmann::adl_serializer<glimmer::MapManifest> {
    static void from_json(const json &j, glimmer::MapManifest &manifest) {
        manifest.name = j.at("name").get<std::string>();
        manifest.gameVersionName = j.at("gameVersionName").get<std::string>();
        manifest.gameVersionNumber = j.at("gameVersionNumber").get<int>();
        manifest.seed = j.at("seed").get<int>();
    }

    static void to_json(json &j, const glimmer::MapManifest &manifest) {
        j = json{
            {"name", manifest.name},
            {"gameVersionName", manifest.gameVersionName},
            {"gameVersionNumber", manifest.gameVersionNumber},
            {"seed", manifest.seed}
        };
    }
};

template<>
struct nlohmann::adl_serializer<glimmer::Vector2D> {
    static void from_json(const json &j, glimmer::Vector2D &vector_2d) {
        vector_2d.x = j.at("x").get<float>();
        vector_2d.y = j.at("y").get<float>();
    }

    static void to_json(json &j, const glimmer::Vector2D &vector_2d) {
        j = json{
            {"x", vector_2d.x},
            {"y", vector_2d.y},
        };
    }
};


template<>
struct nlohmann::adl_serializer<glimmer::TilePlacerRef> {
    static void from_json(const json &j, glimmer::TilePlacerRef &tilePlacerRef) {
        tilePlacerRef.id = j.at("id").get<std::string>();
        tilePlacerRef.tiles = j.at("tiles").get<std::vector<glimmer::ResourceRef> >();
        tilePlacerRef.config = j.at("config").get<std::string>();
    }

    static void to_json(json &j, const glimmer::TilePlacerRef &tilePlacerRef) {
        j = json{
            {"id", tilePlacerRef.id},
            {"tiles", tilePlacerRef.tiles},
            {"config", tilePlacerRef.config},
        };
    }
};

template<>
struct nlohmann::adl_serializer<glimmer::BiomeResource> {
    static void from_json(const json &j, glimmer::BiomeResource &biomeResource) {
        biomeResource.key = j.at("resourceKey").get<std::string>();
        biomeResource.tilePlacerRefs = j.at("tilePlacerRefs").get<std::vector<glimmer::TilePlacerRef> >();
        biomeResource.humidity = j.at("humidity").get<float>();
        biomeResource.temperature = j.at("temperature").get<float>();
        biomeResource.weirdness = j.at("weirdness").get<float>();
        biomeResource.erosion = j.at("erosion").get<float>();
        biomeResource.elevation = j.at("elevation").get<float>();
    }

    static void to_json(json &j, const glimmer::BiomeResource &biomeResource) {
        j = json{
            {"tilePlacerRefs", biomeResource.tilePlacerRefs},
            {"humidity", biomeResource.humidity},
            {"temperature", biomeResource.temperature},
            {"weirdness", biomeResource.weirdness},
            {"erosion", biomeResource.erosion},
            {"elevation", biomeResource.elevation},
            {"resourceKey", biomeResource.key},
        };
    }
};

template<>
struct nlohmann::adl_serializer<glimmer::ResourcePackManifest> {
    static void from_json(const json &j, glimmer::ResourcePackManifest &manifest) {
        manifest.id = j.at("id").get<std::string>();
        manifest.name = j.at("name").get<glimmer::ResourceRef>();
        manifest.description = j.at("description").get<glimmer::ResourceRef>();
        manifest.resPack = j.at("resPack").get<bool>();
        manifest.author = j.at("author").get<std::string>();
        manifest.versionName = j.at("versionName").get<std::string>();
        manifest.versionNumber = j.at("versionNumber").get<int>();
        manifest.minGameVersion = j.at("minGameVersion").get<int>();
    }

    static void to_json(json &j, const glimmer::ResourcePackManifest &manifest) {
        j = json{
            {"id", manifest.id},
            {"name", manifest.name},
            {"description", manifest.description},
            {"resPack", manifest.resPack},
            {"author", manifest.author},
            {"versionName", manifest.versionName},
            {"versionNumber", manifest.versionNumber},
            {"minGameVersion", manifest.minGameVersion}
        };
    }
};


#endif //JSONUTILS_H
