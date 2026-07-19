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
#pragma once

#include "MapManifest.h"
#include "core/ecs/component/TileLayerComponent.h"
#include "core/vfs/VirtualFileSystem.h"
#include "src/core/player.pb.h"
#include "src/saves/chunk.pb.h"
#include "src/saves/chunk_entity.pb.h"

namespace glimmer {
    class Saves {
        std::filesystem::path path_;
        VirtualFileSystem *virtualFileSystem_;
        std::function<void(const MapManifestMessage &)> onMapManifestChanged_;

        [[nodiscard]] std::filesystem::path ToChunkPath(const TileVector2D& position) const;

        [[nodiscard]] std::filesystem::path ToChunkEntityPath(const TileVector2D& position) const;

        [[nodiscard]] std::filesystem::path ToPlayerPath() const;

    public:
        explicit Saves(std::filesystem::path path, VirtualFileSystem *virtualFileSystem);

        void SetOnMapManifestChanged(const std::function<void(const MapManifestMessage &)> &onMapManifestChanged);

        /**
         * Does the archive exist?
         * 存档是否存在
         * @return exist 存档是否存在
         */
        [[nodiscard]] bool Exist() const;

        /**
         * Get the path of the archive
         * 获取存档路径
         * @return path 存档路径
         */
        [[nodiscard]] const std::filesystem::path& GetPath() const;

        /**
         * Check whether the block file at the specified location exists.
         * 获取指定位置的区块文件是否存在。
         * @param position
         * @return
         */
        [[nodiscard]] bool ChunkExists(const TileVector2D& position) const;

        /**
         * Check whether the specified entity file exists.
         * 获取指定的实体文件是否存在。
         * @param position
         * @return
         */
        [[nodiscard]] bool EntityExists(const TileVector2D& position) const;

        [[nodiscard]] std::optional<ChunkMessage> ReadChunk(const TileVector2D& position) const;

        [[nodiscard]] bool WriteChunk(const TileVector2D& position, const ChunkMessage &chunkMessage) const;

        [[nodiscard]] std::optional<ChunkEntityMessage> ReadChunkEntity(const TileVector2D& position) const;

        [[nodiscard]] bool WriteChunkEntity(const TileVector2D& position, const ChunkEntityMessage &chunkEntityMessage) const;

        [[nodiscard]] bool DeleteChunkEntity(const TileVector2D& position) const;

        [[nodiscard]] bool WritePlayer(const PlayerMessage &playerMessage) const;

        [[nodiscard]] std::optional<PlayerMessage> ReadPlayer() const;

        [[nodiscard]] bool PlayerExists() const;

        /**
         * ReadMapManifest
         * 读取存档清单
         * @return
         */
        [[nodiscard]] std::optional<MapManifestMessage> ReadMapManifest() const;

        [[nodiscard]] bool WriteMapManifest(const MapManifestMessage &mapManifestMessage) const;
    };
}
