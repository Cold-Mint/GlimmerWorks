//
// Created by Cold-Mint on 2025/11/28.
//

#include "StdFileProvider.h"

#include <filesystem>
#include <fstream>

#include "../log/LogCat.h"


bool glimmer::StdFileProvider::DeleteFileOrFolder(const std::string &path) {
    const std::filesystem::path fullPath = root_ + "/" + path;
    if (IsFile(path)) {
        return std::filesystem::remove(fullPath);
    }
    return std::filesystem::remove_all(fullPath);
}

std::vector<std::string> glimmer::StdFileProvider::ListFile(const std::string &path) {
    std::vector<std::string> result;
    std::filesystem::path rootPath = std::filesystem::path(root_);
    std::filesystem::path dir = rootPath / path;

    if (std::filesystem::exists(dir)) {
        for (auto &p: std::filesystem::directory_iterator(dir)) {
            result.push_back(std::filesystem::relative(p.path(), rootPath).string());
        }
    }
    return result;
}


std::string glimmer::StdFileProvider::GetFileProviderName() const {
    return "StdFileProvider(" + root_ + ")";
}

std::optional<std::string> glimmer::StdFileProvider::ReadFile(const std::string &path) {
    std::filesystem::path fullPath = root_ + "/" + path;
    if (!std::filesystem::exists(fullPath) || !std::filesystem::is_regular_file(fullPath)) {
        return std::nullopt;
    }

    std::ifstream file(fullPath, std::ios::binary);
    if (!file) {
        return std::nullopt;
    }

    std::string content((std::istreambuf_iterator(file)),
                        std::istreambuf_iterator<char>());
    return content;
}

std::optional<std::unique_ptr<std::istream> > glimmer::StdFileProvider::ReadStream(const std::string &path) {
    std::filesystem::path fullPath = root_ + "/" + path;
    if (!std::filesystem::exists(fullPath) || !std::filesystem::is_regular_file(fullPath)) {
        return std::nullopt;
    }
    auto file = std::make_unique<std::ifstream>(fullPath, std::ios::binary);
    if (!file->is_open()) {
        return std::nullopt;
    }
    return file;
}

bool glimmer::StdFileProvider::Exists(const std::string &path) {
    return std::filesystem::exists(root_ + "/" + path);
}

bool glimmer::StdFileProvider::IsFile(const std::string &path) {
    return std::filesystem::is_regular_file(root_ + "/" + path);
}

bool glimmer::StdFileProvider::WriteFile(const std::string &path, const std::string &content) {
    std::filesystem::path fullPath = root_ + "/" + path;
    std::filesystem::create_directories(fullPath.parent_path());

    std::ofstream file(fullPath, std::ios::binary);
    if (!file) return false;

    file.write(content.data(), static_cast<int>(content.size()));
    return file.good();
}

bool glimmer::StdFileProvider::CreateFolder(const std::string &path) {
    const std::filesystem::path fullPath = root_ + "/" + path;
    try {
        return std::filesystem::create_directories(fullPath);
    } catch (const std::filesystem::filesystem_error &e) {
        LogCat::e("Failed to create folder: ", fullPath.string(), " Error: ", e.what());
        return false;
    }
}

std::optional<std::string> glimmer::StdFileProvider::GetActualPath(const std::string &path) const {
    std::filesystem::path actualPath = std::filesystem::path(root_) / path;
    if (!std::filesystem::exists(actualPath)) {
        return std::nullopt;
    }
    return actualPath;
}
