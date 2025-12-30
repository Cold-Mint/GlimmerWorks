//
// Created by Cold-Mint on 2025/10/10.
//
#include "JsonUtils.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "../log/LogCat.h"

#include <optional>

std::optional<nlohmann::json> glimmer::JsonUtils::LoadJsonFromFile(const VirtualFileSystem *virtualFileSystem,
                                                                   const std::string &path) {
    std::optional<std::unique_ptr<std::istream> > ifstream = virtualFileSystem->ReadStream(path);
    if (!ifstream.has_value()) {
        return std::nullopt;
    }
    std::istream *ifs = ifstream->get();
    if (ifs->fail()) {
        LogCat::e("Failed to open JSON file: ", path);
        return std::nullopt;
    }
    try {
        nlohmann::json jsonObject;
        *ifs >> jsonObject;
        return jsonObject;
    } catch (const std::exception &e) {
        LogCat::e("Failed to parse JSON file: ", path, " Error: ", e.what());
        return std::nullopt;
    }
}

std::optional<nlohmann::json> glimmer::JsonUtils::LoadJsonFromString(const std::string &jsonData) {
    try {
        return nlohmann::json::parse(jsonData);
    } catch (const std::exception &e) {
        LogCat::e("Failed to parse JSON string: ", e.what());
        return std::nullopt;
    }
}


#ifdef __ANDROID__
std::optional<nlohmann::json> glimmer::JsonUtils::LoadJsonFromFile(FileProvider *fileProvider,
                                                                   const std::string &path) {
    std::optional<std::unique_ptr<std::istream> > ifstream = fileProvider->ReadStream(path);
    if (!ifstream.has_value()) {
        return std::nullopt;
    }
    std::istream *ifs = ifstream->get();
    if (ifs->fail()) {
        LogCat::e("Failed to open JSON file: ", path);
        return std::nullopt;
    }
    try {
        nlohmann::json jsonObject;
        *ifs >> jsonObject;
        return jsonObject;
    } catch (const std::exception &e) {
        LogCat::e("Failed to parse JSON file: ", path, " Error: ", e.what());
        return std::nullopt;
    }
}
#endif
