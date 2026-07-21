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

#include "core/Constants.h"
#include "core/log/LogCat.h"


std::filesystem::path glimmer::Saves::ToChunkPath(const TileVector2D& position) const
{
    std::stringstream fileNameStream;
    fileNameStream << "chunk_";
    fileNameStream << std::to_string(position.x);
    fileNameStream << "_";
    fileNameStream << std::to_string(position.y);
    fileNameStream << ".bin";
    return path_ / "chunks" / fileNameStream.str();
}

std::filesystem::path glimmer::Saves::ToChunkEntityPath(const TileVector2D& position) const
{
    std::stringstream fileNameStream;
    fileNameStream << "entity_";
    fileNameStream << std::to_string(position.x);
    fileNameStream << "_";
    fileNameStream << std::to_string(position.y);
    fileNameStream << ".bin";
    return path_ / "entities" / fileNameStream.str();
}

std::filesystem::path glimmer::Saves::ToPlayerPath() const
{
    return path_ / PLAYER_FILE_NAME;
}

glimmer::Saves::Saves(std::filesystem::path path, VirtualFileSystem* virtualFileSystem) : path_(std::move(path)),
    virtualFileSystem_(virtualFileSystem)
{
}

void glimmer::Saves::SetOnMapManifestChanged(
    const std::function<void(const MapManifestMessage&)>& onMapManifestChanged)
{
    onMapManifestChanged_ = onMapManifestChanged;
}

bool glimmer::Saves::Exist() const
{
    return virtualFileSystem_->Exists(path_);
}

const std::filesystem::path& glimmer::Saves::GetPath() const
{
    return path_;
}

bool glimmer::Saves::ChunkExists(const TileVector2D& position) const
{
    return virtualFileSystem_->Exists(
        ToChunkPath(position));
}

bool glimmer::Saves::EntityExists(const TileVector2D& position) const
{
    return virtualFileSystem_->Exists(
        ToChunkEntityPath(position));
}

std::optional<ChunkMessage> glimmer::Saves::ReadChunk(const TileVector2D& position) const
{
    const auto streamUnique = virtualFileSystem_->ReadFileAsStream(ToChunkPath(position));
    if (streamUnique == nullptr)
    {
        LogCat::w(std::source_location::current(), "Failed to open chunk file: ", ToChunkPath(position).string());
        return std::nullopt;
    }
    const auto stream = streamUnique.get();
    if (stream == nullptr)
    {
        return std::nullopt;
    }
    if (ChunkMessage chunkMessage; chunkMessage.ParseFromIstream(stream))
    {
        return chunkMessage;
    }
    LogCat::w(std::source_location::current(), "Failed to parse chunk data: ", ToChunkPath(position).string());
    return std::nullopt;
}

bool glimmer::Saves::WriteChunk(const TileVector2D& position, const ChunkMessage& chunkMessage) const
{
    bool result = virtualFileSystem_->WriteFile(ToChunkPath(position), chunkMessage.SerializeAsString());
    if (!result)
    {
        LogCat::w(std::source_location::current(), "Failed to write chunk: ", ToChunkPath(position).string());
    }
    return result;
}

std::optional<ChunkEntityMessage> glimmer::Saves::ReadChunkEntity(const TileVector2D& position) const
{
    const auto streamUnique = virtualFileSystem_->ReadFileAsStream(ToChunkPath(position));
    if (streamUnique == nullptr)
    {
        return std::nullopt;
    }
    const auto stream = streamUnique.get();
    if (stream == nullptr)
    {
        return std::nullopt;
    }
    ChunkEntityMessage chunkMessage;
    if (chunkMessage.ParseFromIstream(stream))
    {
        return chunkMessage;
    }
    return std::nullopt;
}

bool glimmer::Saves::WriteChunkEntity(const TileVector2D& position, const ChunkEntityMessage& chunkEntityMessage) const
{
    return virtualFileSystem_->WriteFile(ToChunkEntityPath(position), chunkEntityMessage.SerializeAsString());
}

bool glimmer::Saves::DeleteChunkEntity(const TileVector2D& position) const
{
    return virtualFileSystem_->DeleteFileOrFolder(ToChunkEntityPath(position));
}

bool glimmer::Saves::WritePlayer(const PlayerMessage& playerMessage) const
{
    bool result = virtualFileSystem_->WriteFile(ToPlayerPath(), playerMessage.SerializeAsString());
    if (!result)
    {
        LogCat::w(std::source_location::current(), "Failed to write player data: ", ToPlayerPath().string());
    }
    return result;
}

std::optional<PlayerMessage> glimmer::Saves::ReadPlayer() const
{
    const auto streamUnique = virtualFileSystem_->ReadFileAsStream(ToPlayerPath());
    if (streamUnique == nullptr)
    {
        return std::nullopt;
    }
    const auto stream = streamUnique.get();
    if (stream == nullptr)
    {
        return std::nullopt;
    }
    if (PlayerMessage playerMessage; playerMessage.ParseFromIstream(stream))
    {
        return playerMessage;
    }
    return std::nullopt;
}

bool glimmer::Saves::PlayerExists() const
{
    return virtualFileSystem_->Exists(ToPlayerPath());
}

std::optional<MapManifestMessage> glimmer::Saves::ReadMapManifest() const
{
    const auto streamUnique = virtualFileSystem_->ReadFileAsStream(path_ / MAP_MANIFEST_FILE_NAME);
    if (streamUnique == nullptr)
    {
        return std::nullopt;
    }
    const auto stream = streamUnique.get();
    if (stream == nullptr)
    {
        return std::nullopt;
    }
    if (MapManifestMessage mapManifestMessage; mapManifestMessage.ParseFromIstream(stream))
    {
        return mapManifestMessage;
    }
    return std::nullopt;
}

bool glimmer::Saves::WriteMapManifest(const MapManifestMessage& mapManifestMessage) const
{
    if (onMapManifestChanged_ != nullptr)
    {
        onMapManifestChanged_(mapManifestMessage);
    }
    bool result = virtualFileSystem_->WriteFile(path_ / MAP_MANIFEST_FILE_NAME, mapManifestMessage.SerializeAsString());
    if (!result)
    {
        LogCat::w(std::source_location::current(), "Failed to write map manifest: ", (path_ / MAP_MANIFEST_FILE_NAME).string());
    }
    return result;
}
