//
// Created by coldmint on 2025/11/28.
//

#ifndef GLIMMERWORKS_SDLFILEPROVIDER_H
#define GLIMMERWORKS_SDLFILEPROVIDER_H
#include <memory>

#include "FileProvider.h"

namespace glimmer {
    /**
     * File provider implemented based on the standard library
     * 基于标准库实现的文件提供者
     */
    class StdFileProvider : public FileProvider {
        std::string root_;

    public:
        ~StdFileProvider() override = default;

        explicit StdFileProvider(std::string rootPath)
            : root_(std::move(rootPath)) {
        }

        std::vector<std::string> ListFile(const std::string &path) override;

        [[nodiscard]] std::string GetFileProviderName() const override;

        [[nodiscard]] std::optional<std::string> ReadFile(const std::string &path) override;

        std::optional<std::unique_ptr<std::ifstream> > ReadStream(const std::string &path) override;

        [[nodiscard]] bool Exists(const std::string &path) override;

        [[nodiscard]] bool IsFile(const std::string &path) override;

        [[nodiscard]] bool WriteFile(const std::string &path, const std::string &content) override;

        [[nodiscard]] bool CreateFolder(const std::string &path) override;

        [[nodiscard]] std::optional<std::string> GetActualPath(const std::string &path) const override;
    };
}

#endif //GLIMMERWORKS_SDLFILEPROVIDER_H
