//
// Created by Cold-Mint on 2025/10/19.
//

#ifndef GLIMMERWORKS_SAVES_H
#define GLIMMERWORKS_SAVES_H
#include <filesystem>
#include <utility>

#include "MapManifest.h"
#include "../vfs/VirtualFileSystem.h"

namespace glimmer {
    class Saves {
        std::string path_;
        VirtualFileSystem *virtualFileSystem_;

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
         * Create a saves
         * 创建存档
         * @param manifest manifest 清单文件
         */
        void Create(MapManifest &manifest) const;
    };
}

#endif //GLIMMERWORKS_SAVES_H
