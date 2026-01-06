//
// Created by Cold-Mint on 2025/10/19.
//

#ifndef GLIMMERWORKS_SAVES_H
#define GLIMMERWORKS_SAVES_H
#include <filesystem>
#include <utility>

#include "MapManifest.h"
#include "../ecs/component/TileLayerComponent.h"
#include "../vfs/VirtualFileSystem.h"
#include "saves/chunk.pb.h"

namespace glimmer {
    class Saves {
        std::string path_;
        VirtualFileSystem *virtualFileSystem_;

        std::string ToChunkPath(TileVector2D position) const;

    public:
        explicit Saves(std::string path, VirtualFileSystem *virtualFileSystem) : path_(std::move(path)),
            virtualFileSystem_(virtualFileSystem) {
        }

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

        [[nodiscard]] std::optional<ChunkMessage> ReadChunk(TileVector2D position) const;

        [[nodiscard]] bool WriteChunk(TileVector2D position, const ChunkMessage &chunkMessage) const;

        /**
         * Create a saves
         * 创建存档
         * @param manifest manifest 清单文件
         */
        void Create(MapManifest &manifest) const;
    };
}

#endif //GLIMMERWORKS_SAVES_H
