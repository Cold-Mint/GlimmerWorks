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

#include <optional>
#include <string>

#include "ResourcePack.h"
#include "core/mod/BasePackManager.h"

namespace glimmer {
    class ResourcePackManager : public BasePackManager<ResourcePack> {
    protected:
        std::vector<uint64_t> *GetEnabledPack(Config *config) const override;

        std::filesystem::path GetPackPath(Config *config) const override;

        std::unique_ptr<ResourcePack>
        LoadPack(const PackScanRequest *packScanRequest, std::filesystem::path path) override;

    public:
        /**
         * GetFontPath
         * 获取字体文件路径
         * @param enabledResourcePack 启用的资源包ID列表
         * @param language 语言
         * @param virtualFileSystem 虚拟文件系统
         * @return 优先返回 fonts/<language>.ttf，其次返回 fonts/default.ttf，都没有则返回 nullopt
         */
        std::optional<std::filesystem::path> GetFontPath(const std::vector<uint64_t> &enabledResourcePack,
                                                         const std::string &language,
                                                         const VirtualFileSystem *virtualFileSystem);
    };
}
