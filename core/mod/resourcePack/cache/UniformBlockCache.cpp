/*
 * Copyright (C) 2025-2026  Cold-Mint <cold_mint@qq.com>
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
 * 版权(C) 2025-2026  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "UniformBlockCache.h"

#include "core/gpu/UniformBlock.h"
#include "core/log/LogCat.h"
#include "core/utils/TomlUtils.h"
#include "toml11/parser.hpp"

std::shared_ptr<glimmer::UniformBlockResourceResult> glimmer::UniformBlockCache::LoadResourceFromPack(
    AppContext *appContext, const ResourceRef *resourceRef, const ResourcePack *resourcePack) {
    std::filesystem::path path = resourcePack->GetPath() / "shaders" / resourceRef->GetPackageId() /
                                 resourceRef->GetResourceKey();
    path.replace_extension("uniforms.toml");
    const VirtualFileSystem *virtualFileSystem = appContext->GetVirtualFileSystem();
    if (virtualFileSystem == nullptr) {
        LogCat::w(std::source_location::current(), "vfs_is_null", "virtualFileSystem == nullptr");
        return nullptr;
    }
    if (!virtualFileSystem->Exists(path)) {
        LogCat::w(std::source_location::current(), "uniform_block_file_not_found",
                  "Uniform block description file not found: {}", path.string());
        return nullptr;
    }
    auto data = virtualFileSystem->ReadFileAsString(path);
    if (!data.has_value()) {
        LogCat::w(std::source_location::current(), "uniform_block_file_read_failed",
                  "Failed to read uniform block description file: {}", path.string());
        return nullptr;
    }
    auto resource = std::make_unique<UniformBlockResource>(
        toml::get<UniformBlockResource>(toml::parse_str(data.value(), TOML_VERSION)));
    std::unique_ptr<CompiledUniformBlock> block = CompiledUniformBlock::Compile(*resource);
    if (block == nullptr) {
        LogCat::w(std::source_location::current(), "uniform_block_compile_failed",
                  "Failed to compile uniform block: {}", path.string());
        return nullptr;
    }
    auto result = std::make_shared<UniformBlockResourceResult>();
    result->SetResource(block.release());
    result->SetResourcePack(resourcePack);
    return result;
}

glimmer::UniformBlockCache::~UniformBlockCache() noexcept = default;
