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
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "ResourceFileLoader.h"
#include "core/mod/PackManifest.h"
#include "core/mod/PackVerifyState.h"
#include "core/mod/TomlTemplateExpander.h"
#include "core/utils/IUniqueAble.h"
#include "core/vfs/VirtualFileSystem.h"
#include "toml11/spec.hpp"

namespace glimmer {
    class AppContext;
    class PackSignatureVerifier;

    /**
     * DataPack
     * 数据包
     * Orchestrates manifest loading, file traversal and signature verification, delegating
     * resource loading to ResourceFileLoader.
     * 编排清单加载、文件遍历与签名校验，并将资源加载委托给 ResourceFileLoader。
     */
    class DataPack : public IUniqueAble {
        std::filesystem::path rootPath_;
        DataPackManifest manifest_;
        toml::spec tomlVersion_;
        const VirtualFileSystem *virtualFileSystem_;
        const TomlTemplateExpander *tomlTemplateExpander_;
        PackVerifyState packVerifyState_ = PackVerifyState::Unsigned;
        ResourceFileLoader resourceFileLoader_;

        /**
         * ReadFileContent
         * 读取文件内容
         * @param stream stream 输入流
         * @return The file bytes, or nullopt on failure 文件字节内容，失败时返回nullopt
         */
        static std::optional<std::vector<char> > ReadFileContent(std::istream *stream);

        /**
         * ProcessFile
         * 处理单个文件
         * @param file file 文件路径
         * @param appContext appContext 应用上下文
         * @param signatureVerifier signatureVerifier 签名校验器
         * @param defaultLanguageFiles defaultLanguageFiles 默认语言文件列表
         * @param targetLanguageFiles targetLanguageFiles 目标语言文件列表
         * @param allHashData allHashData 全部哈希数据
         * @return The number of loaded resources 加载的资源数量
         */
        int ProcessFile(const std::filesystem::path &file, const AppContext *appContext,
                        PackSignatureVerifier &signatureVerifier,
                        std::vector<std::filesystem::path> &defaultLanguageFiles,
                        std::vector<std::filesystem::path> &targetLanguageFiles,
                        std::vector<uint8_t> &allHashData) const;

    public:
        explicit DataPack(std::filesystem::path path, const VirtualFileSystem *virtualFileSystem,
                          const TomlTemplateExpander *tomlTemplateExpander, const toml::spec &tomlVersion);

        [[nodiscard]] uint64_t GetUniqueId() const override;

        bool LoadManifest();

        [[nodiscard]] PackVerifyState GetPackVerifyState() const;

        [[nodiscard]] bool LoadPack(AppContext *appContext);

        [[nodiscard]] const DataPackManifest *GetManifest() const;
    };
}
