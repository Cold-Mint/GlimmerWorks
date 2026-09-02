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

#include "core/mod/BasePackManager.h"

namespace glimmer {
    template<typename ResourceType>
    int BasePackManager<ResourceType>::Scan(const PackScanRequest *packScanRequest) {
        AppContext *appContext = packScanRequest->GetAppContext();
        if (appContext == nullptr) {
            LogCat::w(std::source_location::current(), "appContext is nullptr");
            return 0;
        }
        const VirtualFileSystem *virtualFileSystem = appContext->GetVirtualFileSystem();
        if (virtualFileSystem == nullptr) {
            LogCat::e(std::source_location::current(), "virtualFileSystem_ is nullptr");
            return 0;
        }
        Config *config = appContext->GetConfig();
        if (config == nullptr) {
            LogCat::e(std::source_location::current(), "config is nullptr");
            return 0;
        }
        const std::filesystem::path &packPath = GetPackPath(config);
        if (!virtualFileSystem->Exists(packPath)) {
            LogCat::w(std::source_location::current(), "Data pack path does not exist: ", packPath.string());
            return 0;
        }
        int success = 0;
        for (const std::vector<std::filesystem::path> files = virtualFileSystem->ListFile(packPath, false); const
             auto &entry: files) {
            if (!virtualFileSystem->IsFile(entry)) {
                std::unique_ptr<ResourceType> pack = LoadPack(packScanRequest, entry);
                if (pack == nullptr) {
                    continue;
                }
                if (!IsPackAvailable(pack->GetManifest())) {
                    continue;
                }
                if (!IsPackEnabled(pack->GetUniqueId(), GetEnabledPack(config))) {
                    continue;
                }
                if (this->Register(std::move(pack)) == nullptr) {
                    continue;
                }
                success++;
            }
        }
        return success;
    }
}
