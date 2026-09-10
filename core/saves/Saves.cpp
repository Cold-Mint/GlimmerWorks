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
#include "Saves.h"

#include <fstream>

#include "core/config/Constants.h"
#include "core/log/LogCat.h"


std::filesystem::path glimmer::Saves::ToDimensionPath(const std::string &dimensionFolderName) const {
    return path_ / DIMENSIONS_FOLDER_NAME / dimensionFolderName;
}

std::filesystem::path glimmer::Saves::ToChunkPath(const std::string &dimensionFolderName,
                                                  const TileVector2D &position) const {
    std::stringstream fileNameStream;
    fileNameStream << "chunk_";
    fileNameStream << std::to_string(position.x);
    fileNameStream << "_";
    fileNameStream << std::to_string(position.y);
    fileNameStream << ".bin";
    return ToDimensionPath(dimensionFolderName) / "chunks" / fileNameStream.str();
}

std::filesystem::path glimmer::Saves::ToChunkEntityPath(const std::string &dimensionFolderName,
                                                        const TileVector2D &position) const {
    std::stringstream fileNameStream;
    fileNameStream << "entity_";
    fileNameStream << std::to_string(position.x);
    fileNameStream << "_";
    fileNameStream << std::to_string(position.y);
    fileNameStream << ".bin";
    return ToDimensionPath(dimensionFolderName) / "entities" / fileNameStream.str();
}

std::filesystem::path glimmer::Saves::ToLocalPlayerPath() const {
    return path_ / "players" / LOCAL_PLAYER_FILE_NAME;
}

glimmer::Saves::Saves(std::filesystem::path path, VirtualFileSystem *virtualFileSystem) : path_(std::move(path)),
    virtualFileSystem_(virtualFileSystem) {
    uniqueId_ = StringUtils::StringToUint64(path_.string());
    LogCat::i("saves_constructed", "Saves created: path={}, uniqueId={}", path_.string(), uniqueId_);
}

void glimmer::Saves::SetOnMapManifestChanged(const std::function<void(const MapManifestMessage &)> &onMapManifestChanged
) {
    onMapManifestChanged_ = onMapManifestChanged;
}

bool glimmer::Saves::Exist() const {
    return virtualFileSystem_->Exists(path_);
}

const std::filesystem::path &glimmer::Saves::GetPath() const {
    return path_;
}

bool glimmer::Saves::ChunkExists(const std::string &dimensionFolderName, const TileVector2D &position) const {
    return virtualFileSystem_->Exists(
        ToChunkPath(dimensionFolderName, position));
}

bool glimmer::Saves::EntityExists(const std::string &dimensionFolderName, const TileVector2D &position) const {
    return virtualFileSystem_->Exists(
        ToChunkEntityPath(dimensionFolderName, position));
}

std::optional<ChunkMessage> glimmer::Saves::ReadChunk(const std::string &dimensionFolderName,
                                                      const TileVector2D &position) const {
    const auto streamUnique = virtualFileSystem_->ReadFileAsStream(ToChunkPath(dimensionFolderName, position));
    if (streamUnique == nullptr) {
        LogCat::w(std::source_location::current(), "chunk_file_open_failed", "Failed to open chunk file: {}",
                  ToChunkPath(dimensionFolderName, position).string());
        return std::nullopt;
    }
    const auto stream = streamUnique.get();
    if (stream == nullptr) {
        return std::nullopt;
    }
    if (ChunkMessage chunkMessage; chunkMessage.ParseFromIstream(stream)) {
        LogCat::d("saves_chunk_read_success", "Read chunk successfully: {}",
                  ToChunkPath(dimensionFolderName, position).string());
        return chunkMessage;
    }
    LogCat::w(std::source_location::current(), "chunk_data_parse_failed", "Failed to parse chunk data: {}",
              ToChunkPath(dimensionFolderName, position).string());
    return std::nullopt;
}

bool glimmer::Saves::WriteChunk(const std::string &dimensionFolderName, const TileVector2D &position,
                                const ChunkMessage &chunkMessage) const {
    bool result = virtualFileSystem_->WriteFile(ToChunkPath(dimensionFolderName, position),
                                                chunkMessage.SerializeAsString());
    if (!result) {
        LogCat::w(std::source_location::current(), "chunk_write_failed", "Failed to write chunk: {}",
                  ToChunkPath(dimensionFolderName, position).string());
    } else {
        LogCat::d("saves_chunk_write_success", "Wrote chunk successfully: {}",
                  ToChunkPath(dimensionFolderName, position).string());
    }
    return result;
}

std::optional<ChunkEntityMessage> glimmer::Saves::ReadChunkEntity(const std::string &dimensionFolderName,
                                                                  const TileVector2D &position) const {
    const auto streamUnique = virtualFileSystem_->ReadFileAsStream(
        ToChunkEntityPath(dimensionFolderName, position));
    if (streamUnique == nullptr) {
        LogCat::w(std::source_location::current(), "saves_chunk_entity_open_failed",
                  "Failed to open chunk entity file: {}",
                  ToChunkEntityPath(dimensionFolderName, position).string());
        return std::nullopt;
    }
    const auto stream = streamUnique.get();
    if (stream == nullptr) {
        return std::nullopt;
    }
    ChunkEntityMessage chunkMessage;
    if (chunkMessage.ParseFromIstream(stream)) {
        LogCat::d("saves_chunk_entity_read_success", "Read chunk entity successfully: {}",
                  ToChunkEntityPath(dimensionFolderName, position).string());
        return chunkMessage;
    }
    LogCat::w(std::source_location::current(), "saves_chunk_entity_parse_failed",
              "Failed to parse chunk entity data: {}",
              ToChunkEntityPath(dimensionFolderName, position).string());
    return std::nullopt;
}

bool glimmer::Saves::WriteChunkEntity(const std::string &dimensionFolderName, const TileVector2D &position,
                                      const ChunkEntityMessage &chunkEntityMessage) const {
    bool result = virtualFileSystem_->WriteFile(ToChunkEntityPath(dimensionFolderName, position),
                                                chunkEntityMessage.SerializeAsString());
    if (!result) {
        LogCat::w(std::source_location::current(), "saves_chunk_entity_write_failed",
                  "Failed to write chunk entity: {}",
                  ToChunkEntityPath(dimensionFolderName, position).string());
    } else {
        LogCat::d("saves_chunk_entity_write_success", "Wrote chunk entity successfully: {}",
                  ToChunkEntityPath(dimensionFolderName, position).string());
    }
    return result;
}

bool glimmer::Saves::DeleteChunkEntity(const std::string &dimensionFolderName, const TileVector2D &position) const {
    bool result = virtualFileSystem_->DeleteFileOrFolder(ToChunkEntityPath(dimensionFolderName, position));
    if (!result) {
        LogCat::w(std::source_location::current(), "saves_chunk_entity_delete_failed",
                  "Failed to delete chunk entity: {}",
                  ToChunkEntityPath(dimensionFolderName, position).string());
    } else {
        LogCat::d("saves_chunk_entity_delete_success", "Deleted chunk entity successfully: {}",
                  ToChunkEntityPath(dimensionFolderName, position).string());
    }
    return result;
}

std::optional<DimensionManifestMessage> glimmer::Saves::ReadDimensionManifest(
    const std::string &dimensionFolderName) const {
    const auto streamUnique = virtualFileSystem_->ReadFileAsStream(
        ToDimensionPath(dimensionFolderName) / DIMENSION_MANIFEST_FILE_NAME);
    if (streamUnique == nullptr) {
        LogCat::w(std::source_location::current(), "saves_dimension_manifest_open_failed",
                  "Failed to open dimension manifest: {}",
                  (ToDimensionPath(dimensionFolderName) / DIMENSION_MANIFEST_FILE_NAME).string());
        return std::nullopt;
    }
    const auto stream = streamUnique.get();
    if (stream == nullptr) {
        return std::nullopt;
    }
    if (DimensionManifestMessage dimensionManifestMessage; dimensionManifestMessage.ParseFromIstream(stream)) {
        LogCat::d("saves_dimension_manifest_read_success", "Read dimension manifest successfully: {}",
                  (ToDimensionPath(dimensionFolderName) / DIMENSION_MANIFEST_FILE_NAME).string());
        return dimensionManifestMessage;
    }
    LogCat::w(std::source_location::current(), "saves_dimension_manifest_parse_failed",
              "Failed to parse dimension manifest: {}",
              (ToDimensionPath(dimensionFolderName) / DIMENSION_MANIFEST_FILE_NAME).string());
    return std::nullopt;
}

bool glimmer::Saves::WriteDimensionManifest(const std::string &dimensionFolderName,
                                            const DimensionManifestMessage &dimensionManifestMessage) const {
    bool result = virtualFileSystem_->WriteFile(ToDimensionPath(dimensionFolderName) / DIMENSION_MANIFEST_FILE_NAME,
                                                dimensionManifestMessage.SerializeAsString());
    if (!result) {
        LogCat::w(std::source_location::current(), "saves_dimension_manifest_write_failed",
                  "Failed to write dimension manifest: {}",
                  (ToDimensionPath(dimensionFolderName) / DIMENSION_MANIFEST_FILE_NAME).string());
    } else {
        LogCat::d("saves_dimension_manifest_write_success", "Wrote dimension manifest successfully: {}",
                  (ToDimensionPath(dimensionFolderName) / DIMENSION_MANIFEST_FILE_NAME).string());
    }
    return result;
}

bool glimmer::Saves::WriteLocalPlayer(const PlayerMessage &playerMessage) const {
    bool result = virtualFileSystem_->WriteFile(ToLocalPlayerPath(), playerMessage.SerializeAsString());
    if (!result) {
        LogCat::w(std::source_location::current(), "player_data_write_failed", "Failed to write player data: {}",
                  ToLocalPlayerPath().string());
    } else {
        LogCat::d("saves_local_player_write_success", "Wrote local player data successfully: {}",
                  ToLocalPlayerPath().string());
    }
    return result;
}

std::optional<PlayerMessage> glimmer::Saves::ReadLocalPlayer() const {
    const auto streamUnique = virtualFileSystem_->ReadFileAsStream(ToLocalPlayerPath());
    if (streamUnique == nullptr) {
        LogCat::w(std::source_location::current(), "saves_local_player_open_failed",
                  "Failed to open local player data: {}", ToLocalPlayerPath().string());
        return std::nullopt;
    }
    const auto stream = streamUnique.get();
    if (stream == nullptr) {
        return std::nullopt;
    }
    if (PlayerMessage playerMessage; playerMessage.ParseFromIstream(stream)) {
        LogCat::d("saves_local_player_read_success", "Read local player data successfully: {}",
                  ToLocalPlayerPath().string());
        return playerMessage;
    }
    LogCat::w(std::source_location::current(), "saves_local_player_parse_failed",
              "Failed to parse local player data: {}", ToLocalPlayerPath().string());
    return std::nullopt;
}

bool glimmer::Saves::PlayerExists() const {
    return virtualFileSystem_->Exists(ToLocalPlayerPath());
}

uint64_t glimmer::Saves::GetUniqueId() const {
    return uniqueId_;
}

std::optional<MapManifestMessage> glimmer::Saves::ReadMapManifest() const {
    const auto streamUnique = virtualFileSystem_->ReadFileAsStream(path_ / MAP_MANIFEST_FILE_NAME);
    if (streamUnique == nullptr) {
        LogCat::w(std::source_location::current(), "saves_map_manifest_open_failed",
                  "Failed to open map manifest: {}", (path_ / MAP_MANIFEST_FILE_NAME).string());
        return std::nullopt;
    }
    const auto stream = streamUnique.get();
    if (stream == nullptr) {
        return std::nullopt;
    }
    if (MapManifestMessage mapManifestMessage; mapManifestMessage.ParseFromIstream(stream)) {
        LogCat::d("saves_map_manifest_read_success", "Read map manifest successfully: {}",
                  (path_ / MAP_MANIFEST_FILE_NAME).string());
        return mapManifestMessage;
    }
    LogCat::w(std::source_location::current(), "saves_map_manifest_parse_failed",
              "Failed to parse map manifest: {}", (path_ / MAP_MANIFEST_FILE_NAME).string());
    return std::nullopt;
}

bool glimmer::Saves::WriteMapManifest(const MapManifestMessage &mapManifestMessage) const {
    if (onMapManifestChanged_ != nullptr) {
        onMapManifestChanged_(mapManifestMessage);
    }
    bool result = virtualFileSystem_->WriteFile(path_ / MAP_MANIFEST_FILE_NAME, mapManifestMessage.SerializeAsString());
    if (!result) {
        LogCat::w(std::source_location::current(), "map_manifest_write_failed", "Failed to write map manifest: {}",
                  (path_ / MAP_MANIFEST_FILE_NAME).string());
    } else {
        LogCat::d("saves_map_manifest_write_success", "Wrote map manifest successfully: {}",
                  (path_ / MAP_MANIFEST_FILE_NAME).string());
    }
    return result;
}
