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
#include "ResourcePackManager.h"

#include "core/config/Config.h"
#include "core/context/AppContext.h"

std::vector<uint64_t> *glimmer::ResourcePackManager::GetEnabledPack(Config *config) const {
    return &config->mods.enabledResourcePack;
}

std::filesystem::path glimmer::ResourcePackManager::GetPackPath(Config *config) const {
    return config->mods.resourcePackPath;
}

std::unique_ptr<glimmer::ResourcePack> glimmer::ResourcePackManager::LoadPack(const PackScanRequest *packScanRequest,
                                                                              std::filesystem::path path) {
    AppContext *appContext = packScanRequest->GetAppContext();
    if (appContext == nullptr) {
        return nullptr;
    }
    VirtualFileSystem *virtualFileSystem = appContext->GetVirtualFileSystem();
    if (virtualFileSystem == nullptr) {
        return nullptr;
    }
    auto resourcePack = std::make_unique<ResourcePack>(path, virtualFileSystem, *packScanRequest->GetTomlVersion());
    if (!resourcePack->LoadManifest()) {
        return nullptr;
    }
    return resourcePack;
}

std::optional<std::filesystem::path> glimmer::ResourcePackManager::GetFontPath(
    const std::vector<uint64_t> &enabledResourcePack,
    const std::string &language,
    const VirtualFileSystem *virtualFileSystem) {
    if (virtualFileSystem == nullptr) {
        return std::nullopt;
    }
    std::optional<std::filesystem::path> defaultFontPath;
    for (const uint64_t packId: enabledResourcePack) {
        const ResourcePack *pack = Find(packId);
        if (pack == nullptr) {
            continue;
        }
        const std::filesystem::path fontsDir = pack->GetPath() / "fonts";

        // Prefer fonts/<language>.ttf
        // 优先使用 fonts/<language>.ttf
        std::filesystem::path languageFont = fontsDir / language;
        languageFont.replace_extension("ttf");
        if (virtualFileSystem->Exists(languageFont)) {
            return languageFont;
        }

        // Record the first fonts/default.ttf as fallback
        // 记录第一个 fonts/default.ttf 作为回退
        if (!defaultFontPath.has_value()) {
            std::filesystem::path defaultFont = fontsDir / "default";
            defaultFont.replace_extension("ttf");
            if (virtualFileSystem->Exists(defaultFont)) {
                defaultFontPath = defaultFont;
            }
        }
    }
    return defaultFontPath;
}
