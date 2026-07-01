/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "SavesManager.h"

#include "core/log/LogCat.h"
#include "core/scene/AppContext.h"
#include "core/utils/StringUtils.h"


void glimmer::SavesManager::AddSaves(std::unique_ptr<Saves> saves) {
    auto mapManifestMessage = saves->ReadMapManifest();
    if (!mapManifestMessage.has_value()) {
        LogCat::e("Missing the mapManifest file ", saves->GetPath());
        return;
    }
    auto mapManifest = std::make_unique<MapManifest>();
    mapManifest->FromMessage(mapManifestMessage.value());
    size_t index = manifestList_.size();
    manifestList_.push_back(std::move(mapManifest));
    saveList_.push_back(std::move(saves));
    saveList_.back()->SetOnMapManifestChanged([this, index](const MapManifestMessage &msg) {
        if (index < manifestList_.size()) {
            manifestList_[index]->FromMessage(msg);
        }
    });
}

glimmer::SavesManager::SavesManager(VirtualFileSystem *virtualFileSystem) {
    virtualFileSystem_ = virtualFileSystem;
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

glimmer::Saves *glimmer::SavesManager::Create(const std::string &runtimePath, MapManifest &manifest) {
    std::string path = runtimePath + "/saved/" + StringUtils::ToSafeSaveName(manifest.name);
    if (!virtualFileSystem_->Exists(path)) {
        bool createFolder = virtualFileSystem_->CreateFolder(path);
        if (!createFolder) {
            LogCat::e("Directories cannot be created: ", path);
            return nullptr;
        }
    }
    auto save = std::make_unique<Saves>(path, virtualFileSystem_);
    MapManifestMessage manifestMessage;
    manifest.ToMessage(manifestMessage);
    if (!save->WriteMapManifest(manifestMessage)) {
        LogCat::e("Error writing map Manifest ");
        return nullptr;
    }
    AddSaves(std::move(save));
    return GetSave(saveList_.size() - 1);
}


void glimmer::SavesManager::LoadAllSaves(const std::string &runtimePath) {
    saveList_.clear();
    const std::vector<std::string> array = virtualFileSystem_->ListFile(runtimePath + "/saved/", false);
    for (const auto &item: array) {
        AddSaves(std::make_unique<Saves>(item, virtualFileSystem_));
    }
}

size_t glimmer::SavesManager::GetSavesListSize() const {
    return saveList_.size();
}
