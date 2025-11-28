//
// Created by Cold-Mint on 2025/10/19.
//

#include "Saves.h"

#include <fstream>

#include "MapManifest.h"
#include "../log/LogCat.h"
#include "../utils/JsonUtils.h"
#include "nlohmann/json.hpp"

bool glimmer::Saves::Exist() const {
    return virtualFileSystem_->Exists(path_);
}

std::string glimmer::Saves::GetPath() const {
    return path_;
}

void glimmer::Saves::Create(MapManifest &manifest) const {
    if (!virtualFileSystem_->Exists(path_)) {
        bool createFolder = virtualFileSystem_->CreateFolder(path_);
        if (!createFolder) {
            LogCat::e("Directories cannot be created: ", path_);
            return;
        }
    }
    const nlohmann::json world_data = manifest;
    bool createFile = virtualFileSystem_->WriteFile(path_ + "/map.json", world_data.dump(4));
    if (!createFile) {
        LogCat::e("Error writing to file: ", path_);
    }
}
