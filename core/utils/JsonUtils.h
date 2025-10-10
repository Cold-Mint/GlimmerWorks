//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef JSONUTILS_H
#define JSONUTILS_H
#include <optional>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include "../mod/Resource.h"
#include "../mod/ResourceRef.h"
#include "../mod/PackManifest.h"
#include "../Config.h"


namespace Glimmer {
    struct StringResource;

    class JsonUtils {
    public:
        static std::optional<nlohmann::json> LoadJsonFromFile(const std::string &path);
    };
}


template<>
struct nlohmann::adl_serializer<Glimmer::StringResource> {
    static void from_json(const json &j, Glimmer::StringResource &s) {
        s.key = j.at("resourceKey").get<std::string>();
        s.value = j.at("value").get<std::string>();
    }

    static void to_json(json &j, const Glimmer::StringResource &s) {
        j = json{
            {"resourceKey", s.key},
            {"value", s.value},
        };
    }
};

template<>
struct nlohmann::adl_serializer<Glimmer::Resource> {
    static void from_json(const json &j, Glimmer::Resource &r) {
        r.key = j.at("resourceKey").get<std::string>();
    }

    static void to_json(json &j, const Glimmer::Resource &r) {
        j = json{
            {"resourceKey", r.key}
        };
    }
};

template<>
struct nlohmann::adl_serializer<Glimmer::Mods> {
    static void from_json(const json &j, Glimmer::Mods &m) {
        m.dataPackPath = j.at("dataPackPath").get<std::string>();
        m.resourcePackPath = j.at("resourcePackPath").get<std::string>();
        m.enabledDataPack = j.at("enabledDataPack").get<std::vector<std::string> >();
    }

    static void to_json(json &j, const Glimmer::Mods &m) {
        j = json{
            {"dataPackPath", m.dataPackPath},
            {"resourcePackPath", m.resourcePackPath},
            {"enabledDataPack", m.enabledDataPack}
        };
    }
};

template<>
struct nlohmann::adl_serializer<Glimmer::Window> {
    static void from_json(const json &j, Glimmer::Window &w) {
        w.framerate = j.at("framerate").get<int>();
        w.height = j.at("height").get<int>();
        w.width = j.at("width").get<int>();
        w.resizable = j.at("resizable").get<bool>();
    }

    static void to_json(json &j, const Glimmer::Window &w) {
        j = json{
            {"framerate", w.framerate},
            {"height", w.height},
            {"width", w.width},
            {"resizable", w.resizable}
        };
    }
};

template<>
struct nlohmann::adl_serializer<Glimmer::ResourceRef> {
    static void from_json(const json &j, Glimmer::ResourceRef &resourceRef) {
        resourceRef.packId = j.at("packId").get<std::string>();
        resourceRef.resourceType = j.at("resourceType").get<int>();
        resourceRef.resourceKey = j.at("resourceKey").get<std::string>();
    }

    static void to_json(json &j, const Glimmer::ResourceRef &resourceRef) {
        j = json{
            {"packId", resourceRef.packId},
            {"resourceType", resourceRef.resourceType},
            {"resourceKey", resourceRef.resourceKey}
        };
    }
};

template<>
struct nlohmann::adl_serializer<Glimmer::PackManifest> {
    static void from_json(const json &j, Glimmer::PackManifest &manifest) {
        manifest.id = j.at("id").get<std::string>();
        manifest.name = j.at("name").get<Glimmer::ResourceRef>();
        manifest.description = j.at("description").get<Glimmer::ResourceRef>();
        manifest.resPack = j.at("resPack").get<bool>();
        manifest.author = j.at("author").get<std::string>();
        manifest.versionName = j.at("versionName").get<std::string>();
        manifest.versionNumber = j.at("versionNumber").get<int>();
        manifest.minGameVersion = j.at("minGameVersion").get<int>();
    }

    static void to_json(json &j, const Glimmer::PackManifest &manifest) {
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
