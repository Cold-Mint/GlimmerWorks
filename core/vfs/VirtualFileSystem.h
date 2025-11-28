//
// Created by coldmint on 2025/11/28.
//

#ifndef GLIMMERWORKS_VIRTUALFILESYSTEM_H
#define GLIMMERWORKS_VIRTUALFILESYSTEM_H
#include <memory>
#include <vector>

#include "FileProvider.h"

namespace glimmer {
    class VirtualFileSystem {
        std::vector<std::unique_ptr<FileProvider> > fileProviders_;

    public:
        virtual ~VirtualFileSystem() = default;

        /**
         * Mount file provider
         * 挂载文件提供者
         * @param provider
         */
        void Mount(std::unique_ptr<FileProvider> provider);

        [[nodiscard]] std::string ListMounts() const;

        [[nodiscard]] std::optional<std::string> ReadFile(const std::string &path) const;

        [[nodiscard]] bool Exists(const std::string &path) const;

        [[nodiscard]] bool CreateFolder(const std::string &path) const;

        [[nodiscard]] bool IsFile(const std::string &path) const;


        [[nodiscard]] bool WriteFile(const std::string &path, const std::string &content) const;

        [[nodiscard]] std::optional<std::unique_ptr<std::istream> > ReadStream(const std::string &path) const;

        [[nodiscard]] virtual std::vector<std::string> ListFile(const std::string &path) const;

        /**
         * GetActualPath
         * 获取实际路径
         * @param path Virtual path 虚拟路径
         * @return
         */
        [[nodiscard]] std::optional<std::string> GetActualPath(const std::string &path) const;
    };
}

#endif //GLIMMERWORKS_VIRTUALFILESYSTEM_H
