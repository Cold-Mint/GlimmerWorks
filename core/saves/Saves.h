//
// Created by Cold-Mint on 2025/10/19.
//

#ifndef GLIMMERWORKS_SAVES_H
#define GLIMMERWORKS_SAVES_H
#include <utility>

#include "MapManifest.h"
#include "../ecs/component/TileLayerComponent.h"
#include "../vfs/VirtualFileSystem.h"
#include "src/core/player.pb.h"
#include "src/saves/chunk.pb.h"
#include "src/saves/chunk_entity.pb.h"

namespace glimmer {
    class Saves {
        std::string path_;
        VirtualFileSystem *virtualFileSystem_;
        std::function<void(const MapManifestMessage &)> onMapManifestChanged_;

        [[nodiscard]] std::string ToChunkPath(TileVector2D position) const;

        [[nodiscard]] std::string ToChunkEntityPath(TileVector2D position) const;

        [[nodiscard]] std::string ToPlayerPath() const;

    public:
        explicit Saves(std::string path, VirtualFileSystem *virtualFileSystem);

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
        [[nodiscard]] std::string GetPath() const;

        /**
         * Check whether the block file at the specified location exists.
         * 获取指定位置的区块文件是否存在。
         * @param position
         * @return
         */
        [[nodiscard]] bool ChunkExists(TileVector2D position) const;

        /**
         * Check whether the specified entity file exists.
         * 获取指定的实体文件是否存在。
         * @param position
         * @return
         */
        [[nodiscard]] bool EntityExists(TileVector2D position) const;

        [[nodiscard]] std::optional<ChunkMessage> ReadChunk(TileVector2D position) const;

        [[nodiscard]] bool WriteChunk(TileVector2D position, const ChunkMessage &chunkMessage) const;

        [[nodiscard]] std::optional<ChunkEntityMessage> ReadChunkEntity(TileVector2D position) const;

        [[nodiscard]] bool WriteChunkEntity(TileVector2D position, const ChunkEntityMessage &chunkEntityMessage) const;

        [[nodiscard]] bool DeleteChunkEntity(TileVector2D position) const;

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

#endif //GLIMMERWORKS_SAVES_H
