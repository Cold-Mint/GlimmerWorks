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
#include <string>
#include <vector>

#include "core/vfs/VirtualFileSystem.h"
#include "core/mod/PackManifest.h"
#include "core/mod/TomlTemplateExpander.h"
#include "toml11/spec.hpp"

namespace glimmer {
    enum class PackVerifyState : uint8_t;
    class AppContext;
    class BiomesManager;
    class TileResourceManager;
    class StringManager;
    class DataPack;
    class Config;


    class DataPackManager {
        VirtualFileSystem *virtualFileSystem_;
        TomlTemplateExpander *tomlTemplateExpander_;
        std::vector<DataPackManifest> packManifestVector_;
        std::unordered_map<std::string, PackVerifyState> packVerifyStateMap_;

        //Check whether the data packet is available (for example, determine whether the minimum game version declared by the data packet exceeds the game version)
        //检测数据包是否可用（例如判断数据包声明的最低游戏版本是否超过了游戏版本）
        [[nodiscard]] bool IsDataPackAvailable(const DataPack &pack) const;

        static bool IsDataPackEnabled(const DataPack &pack,
                                      const std::vector<std::string> &enabledDataPack);

    public:
        explicit DataPackManager(VirtualFileSystem *virtualFilesystem, TomlTemplateExpander *tomlTemplateExpander);

        /**
         * Determine whether data packet 1 and data packet 2 meet the dependency conditions (whether the list of data packet 1 includes the id of data packet 2, and whether the minimum version required by the version dependency of data packet 2 is equal to or greater than the specified dependency version in data packet 1.)
         * 判断数据包1和数据包2是否满足依赖条件(数据包1的清单是否包含数据包2的id，且数据包2版本依赖的最小版本等于或大于数据包1内指定的依赖版本。)
         * @return
         */
        bool IsDependencySatisfied(const std::string &pack1Id, const std::string &pack2Id);

        bool Contains(const std::string &packId) const;

        PackVerifyState GetPackVerifyState(const std::string &packId);

        std::vector<std::string> GetPackIdList() const;

        //Scan the data packets in the specified directory(Return the number of data packets successfully loaded)
        //扫描指定目录下的数据包（返回成功加载多少个数据包）
        int Scan(AppContext *appContext,
                 const toml::spec &tomlVersion);
    };
}
