//
// Created by Cold-Mint on 2025/10/19.
//

#include "Saves.h"

#include <fstream>

#include "MapManifest.h"
#include "../log/LogCat.h"
#include "../utils/JsonUtils.h"
#include "nlohmann/json.hpp"
#include "saves/map_manifest.pb.h"

std::string glimmer::Saves::ToChunkPath(TileVector2D position) const {
    return path_ + "/chunks/chunk_" + std::to_string(position.x) + "_" + std::to_string(position.y) + ".bin";
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


void glimmer::Saves::Create(MapManifest &manifest) const {
    if (!virtualFileSystem_->Exists(path_)) {
        bool createFolder = virtualFileSystem_->CreateFolder(path_);
        if (!createFolder) {
            LogCat::e("Directories cannot be created: ", path_);
            return;
        }
    }
    MapManifestMessage manifestMessage;
    manifest.ToMessage(manifestMessage);
    bool createFile = virtualFileSystem_->WriteFile(path_ + "/mapManifest.bin", manifestMessage.SerializeAsString());
    if (!createFile) {
        LogCat::e("Error writing to file: ", path_);
    }
}
