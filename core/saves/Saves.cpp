//
// Created by Cold-Mint on 2025/10/19.
//

#include "Saves.h"

#include <fstream>

#include "core/Constants.h"


std::string glimmer::Saves::ToChunkPath(TileVector2D position) const {
    return path_ + "/chunks/chunk_" + std::to_string(position.x) + "_" + std::to_string(position.y) + ".bin";
}

std::string glimmer::Saves::ToChunkEntityPath(TileVector2D position) const {
    return path_ + "/entitys/entity_" + std::to_string(position.x) + "_" + std::to_string(position.y) + ".bin";
}

std::string glimmer::Saves::ToPlayerPath() const {
    return path_ + "/" + PLAYER_FILE_NAME;
}

glimmer::Saves::Saves(std::string path, VirtualFileSystem *virtualFileSystem) : path_(std::move(path)),
    virtualFileSystem_(virtualFileSystem) {
}

void glimmer::Saves::SetOnMapManifestChanged(
    const std::function<void(const MapManifestMessage &)> &onMapManifestChanged) {
    onMapManifestChanged_ = onMapManifestChanged;
}

bool glimmer::Saves::Exist() const {
    return virtualFileSystem_->Exists(path_);
}

std::string glimmer::Saves::GetPath() const {
    return path_;
}

bool glimmer::Saves::ChunkExists(const TileVector2D position) const {
    return virtualFileSystem_->Exists(
        ToChunkPath(position));
}

bool glimmer::Saves::EntityExists(TileVector2D position) const {
    return virtualFileSystem_->Exists(
        ToChunkEntityPath(position));
}

std::optional<ChunkMessage> glimmer::Saves::ReadChunk(const TileVector2D position) const {
    const auto stream = virtualFileSystem_->ReadStream(ToChunkPath(position));
    if (!stream.has_value()) {
        return std::nullopt;
    }
    ChunkMessage chunkMessage;
    chunkMessage.ParseFromIstream(stream->get());
    return chunkMessage;
}

bool glimmer::Saves::WriteChunk(const TileVector2D position, const ChunkMessage &chunkMessage) const {
    return virtualFileSystem_->WriteFile(ToChunkPath(position), chunkMessage.SerializeAsString());
}

std::optional<ChunkEntityMessage> glimmer::Saves::ReadChunkEntity(const TileVector2D position) const {
    const auto stream = virtualFileSystem_->ReadStream(ToChunkEntityPath(position));
    if (!stream.has_value()) {
        return std::nullopt;
    }
    ChunkEntityMessage chunkMessage;
    chunkMessage.ParseFromIstream(stream->get());
    return chunkMessage;
}

bool glimmer::Saves::WriteChunkEntity(TileVector2D position, const ChunkEntityMessage &chunkEntityMessage) const {
    return virtualFileSystem_->WriteFile(ToChunkEntityPath(position), chunkEntityMessage.SerializeAsString());
}

bool glimmer::Saves::DeleteChunkEntity(TileVector2D position) const {
    return virtualFileSystem_->DeleteFileOrFolder(ToChunkEntityPath(position));
}

bool glimmer::Saves::WritePlayer(const PlayerMessage &playerMessage) const {
    return virtualFileSystem_->WriteFile(ToPlayerPath(), playerMessage.SerializeAsString());
}

std::optional<PlayerMessage> glimmer::Saves::ReadPlayer() const {
    const auto stream = virtualFileSystem_->ReadStream(ToPlayerPath());
    if (!stream.has_value()) {
        return std::nullopt;
    }
    PlayerMessage playerMessage;
    playerMessage.ParseFromIstream(stream->get());
    return playerMessage;
}

bool glimmer::Saves::PlayerExists() const {
    return virtualFileSystem_->Exists(ToPlayerPath());
}

std::optional<MapManifestMessage> glimmer::Saves::ReadMapManifest() const {
    const auto stream = virtualFileSystem_->ReadStream(path_ + "/" + MAP_MANIFEST_FILE_NAME);
    if (!stream.has_value()) {
        return std::nullopt;
    }
    MapManifestMessage mapManifestMessage;
    mapManifestMessage.ParseFromIstream(stream->get());
    return mapManifestMessage;
}

bool glimmer::Saves::WriteMapManifest(const MapManifestMessage &mapManifestMessage) const {
    if (onMapManifestChanged_ != nullptr) {
        onMapManifestChanged_(mapManifestMessage);
    }
    return virtualFileSystem_->WriteFile(path_ + "/" + MAP_MANIFEST_FILE_NAME, mapManifestMessage.SerializeAsString());
}
