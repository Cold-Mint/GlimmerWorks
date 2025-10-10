//
// Created by Cold-Mint on 2025/10/10.
//
#include "JsonUtils.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "../log/LogCat.h"

#include <optional>

std::optional<nlohmann::json> Glimmer::JsonUtils::LoadJsonFromFile(const std::string &path) {
    // Open file input stream. The ifstream will automatically close the file
    // when it goes out of scope (RAII ensures resource safety).
    // 打开文件输入流（ifstream 会在作用域结束时自动关闭文件，确保资源安全）
    std::ifstream jsonFile(path);
    if (!jsonFile.is_open()) {
        LogCat::e("Failed to open JSON file: ", path);
        return std::nullopt;
    }
    try {
        nlohmann::json jsonObject;
        jsonFile >> jsonObject;
        return jsonObject;
    } catch (const std::exception &e) {
        LogCat::e("Failed to parse JSON file: ", path, " Error: ", e.what());
        return std::nullopt;
    }
}



