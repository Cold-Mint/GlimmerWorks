//
// Created by Cold-Mint on 2025/10/19.
//

#include "Saves.h"

#include <fstream>

#include "MapManifest.h"
#include "../log/LogCat.h"
#include "../utils/JsonUtils.h"
#include "nlohmann/json.hpp"

bool Glimmer::Saves::exist() const {
    return std::filesystem::exists(path);
}

std::filesystem::path Glimmer::Saves::getPath() const {
    return path;
}

void Glimmer::Saves::create(MapManifest &manifest) const {
    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
    }
    LogCat::d("Created directory: ", path.string());
    const nlohmann::json world_data = manifest;
    std::ofstream file(path / "map.json");
    file << world_data.dump(4);
    file.close();
}
