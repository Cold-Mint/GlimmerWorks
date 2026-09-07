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

#include <algorithm>
#include <cctype>

#include "PlayerManifest.h"
#include "core/context/AppContext.h"
#include "core/utils/StringUtils.h"
#include "src/saves/map_manifest.pb.h"


void glimmer::SavesManager::AfterRegister(Saves *resource) {
    auto mapManifestMessage = resource->ReadMapManifest();
    if (!mapManifestMessage.has_value()) {
        return;
    }
    auto playerMessage = resource->ReadLocalPlayer();
    if (!playerMessage.has_value()) {
        return;
    }
    auto mapManifest = std::make_unique<MapManifest>();
    mapManifest->FromMessage(mapManifestMessage.value());
    size_t index = manifestList_.size();
    manifestList_.push_back(std::move(mapManifest));

    auto playerData = std::make_unique<PlayerManifest>();
    playerData->FromMessage(playerMessage.value());
    localPlayers_.push_back(std::move(playerData));

    saveList_.push_back(resource);
    saveToIndex_[resource] = index;

    saveList_.back()->SetOnMapManifestChanged([this, resource](const MapManifestMessage &msg) {
        auto findIt = saveToIndex_.find(resource);
        if (findIt == saveToIndex_.end())
            return;
        size_t realIndex = findIt->second;
        if (realIndex < manifestList_.size()) {
            manifestList_[realIndex]->FromMessage(msg);
        }
    });
}

void glimmer::SavesManager::BeforeUnRegister(Saves *resource) {
    resource->SetOnMapManifestChanged(nullptr);
    auto mapIt = saveToIndex_.find(resource);
    if (mapIt == saveToIndex_.end()) {
        return;
    }
    long index = static_cast<long>(mapIt->second);
    if (index < manifestList_.size()) {
        manifestList_.erase(manifestList_.begin() + index);
    }
    if (index < localPlayers_.size()) {
        localPlayers_.erase(localPlayers_.begin() + index);
    }
    for (auto it = saveList_.begin(); it != saveList_.end(); ++it) {
        if (*it == resource) {
            saveList_.erase(it);
            break;
        }
    }
    saveToIndex_.erase(mapIt);
    for (auto &pair: saveToIndex_) {
        if (pair.second > index) {
            pair.second -= 1;
        }
    }
}


glimmer::SavesManager::SavesManager(VirtualFileSystem *virtualFileSystem)
    : virtualFileSystem_(virtualFileSystem) {
}

glimmer::Saves *glimmer::SavesManager::GetSave(const size_t index) const {
    return saveList_[index];
}

glimmer::MapManifest *glimmer::SavesManager::GetMapManifest(const size_t index) const {
    return manifestList_[index].get();
}

glimmer::PlayerManifest *glimmer::SavesManager::GetPlayerManifest(const size_t index) const {
    return localPlayers_[index].get();
}

bool glimmer::SavesManager::DeleteSave(const size_t index) {
    if (index >= saveList_.size()) {
        return false;
    }
    auto save = saveList_[index];
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

glimmer::Saves *glimmer::SavesManager::Create(const std::filesystem::path &runtimePath, MapManifest &mapManifest,
                                              PlayerManifest &playerManifest) {
    std::filesystem::path path = runtimePath / "saves" / StringUtils::ToSafeSaveName(mapManifest.name);
    if (!virtualFileSystem_->Exists(path)) {
        bool createFolder = virtualFileSystem_->CreateFolder(path);
        if (!createFolder) {
            return nullptr;
        }
    }
    auto save = std::make_unique<Saves>(path, virtualFileSystem_);
    MapManifestMessage manifestMessage;
    mapManifest.ToMessage(manifestMessage);
    if (!save->WriteMapManifest(manifestMessage)) {
        return nullptr;
    }
    PlayerMessage playerMessage;
    playerManifest.ToMessage(playerMessage);
    if (!save->WriteLocalPlayer(playerMessage)) {
        return nullptr;
    }
    Register(std::move(save));
    return GetSave(saveList_.size() - 1);
}


void glimmer::SavesManager::LoadAllSaves(const std::filesystem::path &runtimePath) {
    Clear();
    for (const std::vector<std::filesystem::path> array = virtualFileSystem_->ListFile(runtimePath / "saves", false);
         const auto &item: array) {
        Register(std::make_unique<Saves>(item, virtualFileSystem_));
    }
}

size_t glimmer::SavesManager::GetSavesListSize() const {
    return saveList_.size();
}

std::vector<size_t> glimmer::SavesManager::FilterByKeyword(const std::string &keyword) const {
    std::vector<size_t> result;
    if (keyword.empty()) {
        for (size_t i = 0; i < saveList_.size(); ++i) {
            result.push_back(i);
        }
    } else {
        std::string lowerKeyword = keyword;
        std::ranges::transform(lowerKeyword, lowerKeyword.begin(),
                               [](unsigned char c) { return std::tolower(c); });

        for (size_t i = 0; i < saveList_.size(); ++i) {
            const auto *manifest = manifestList_[i].get();
            if (manifest == nullptr) {
                continue;
            }
            std::string lowerName = manifest->name;
            std::ranges::transform(lowerName, lowerName.begin(),
                                   [](unsigned char c) { return std::tolower(c); });
            if (lowerName.find(lowerKeyword) != std::string::npos) {
                result.push_back(i);
            }
        }
    }
    std::ranges::sort(result, [this](size_t a, size_t b) {
        const auto *localPlayerA = localPlayers_[a].get();
        const auto *localPlayerB = localPlayers_[b].get();
        if (localPlayerA == nullptr || localPlayerB == nullptr) {
            return false;
        }
        return localPlayerA->lastPlayedTime > localPlayerB->lastPlayedTime;
    });
    return result;
}
