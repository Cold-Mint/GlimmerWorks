//
// Created by Cold-Mint on 2026/1/6.
//

#include "SavesManager.h"

#include "../log/LogCat.h"
#include "../scene/AppContext.h"


void glimmer::SavesManager::AddSaves(std::unique_ptr<Saves> saves) {
    auto mapManifestMessage = saves->ReadMapManifest();
    if (!mapManifestMessage.has_value()) {
        LogCat::e("Missing the mapManifest file ", saves->GetPath());
        return;
    }
    auto mapManifest = std::make_unique<MapManifest>();
    mapManifest->FromMessage(mapManifestMessage.value());
    manifestList_.push_back(std::move(mapManifest));
    saveList_.push_back(std::move(saves));
}

glimmer::Saves *glimmer::SavesManager::GetSave(const size_t index) const {
    return saveList_[index].get();
}

glimmer::MapManifest *glimmer::SavesManager::GetMapManifest(const size_t index) const {
    return manifestList_[index].get();
}

bool glimmer::SavesManager::DeleteSave(const size_t index) {
    if (index >= saveList_.size()) {
        return false;
    }
    auto save = saveList_[index].get();
    if (save == nullptr) {
        return false;
    }
    if (virtualFileSystem_->DeleteFileOrFolder(save->GetPath())) {
        saveList_.erase(saveList_.begin() + static_cast<long>(index));
        manifestList_.erase(manifestList_.begin() + static_cast<long>(index));
        return true;
    }
    return false;
}

glimmer::Saves *glimmer::SavesManager::Create(MapManifest &manifest) {
    std::string path = "saved/" + manifest.name;
    if (!virtualFileSystem_->Exists(path)) {
        bool createFolder = virtualFileSystem_->CreateFolder(path);
        if (!createFolder) {
            LogCat::e("Directories cannot be created: ", path);
            return nullptr;
        }
    }
    MapManifestMessage manifestMessage;
    manifest.ToMessage(manifestMessage);
    const bool createFile = virtualFileSystem_->WriteFile(path + "/" + MAP_MANIFEST_FILE_NAME,
                                                          manifestMessage.SerializeAsString());
    if (!createFile) {
        LogCat::e("Error writing to file: ", path);
    }
    AddSaves(std::make_unique<Saves>(path, virtualFileSystem_));
    return GetSave(saveList_.size() - 1);
}

void glimmer::SavesManager::LoadAllSaves() {
    saveList_.clear();
    const std::vector<std::string> array = virtualFileSystem_->ListFile("saved/");
    for (const auto &item: array) {
        AddSaves(std::make_unique<Saves>(item, virtualFileSystem_));
    }
}

size_t glimmer::SavesManager::GetSavesListSize() const {
    return saveList_.size();
}
