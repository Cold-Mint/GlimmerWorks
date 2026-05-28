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
#include <vector>

#include "Saves.h"

namespace glimmer {
    class SavesManager {
        VirtualFileSystem *virtualFileSystem_;
        std::vector<std::unique_ptr<Saves> > saveList_;
        std::vector<std::unique_ptr<MapManifest> > manifestList_;

        void AddSaves(std::unique_ptr<Saves> saves);

    public:
        explicit SavesManager(VirtualFileSystem *virtualFileSystem);

        /**
         * Retrieve the archive at the specified location.
         * 获取指定位置的存档。
         * @param index
         * @return
         */
        [[nodiscard]] Saves *GetSave(size_t index) const;

        [[nodiscard]] MapManifest *GetMapManifest(size_t index) const;

        /**
         * Delete the specified archive
         * 删除指定存档
         * @param index
         * @return
         */
        bool DeleteSave(size_t index);


        /**
        * Create a saves
        * 创建存档
        * @param manifest manifest 清单文件
        */
        Saves *Create(const std::string &runtimePath, MapManifest &manifest);


        /**
         * Load all the saved files.
         * 加载所有的存档。
         */
        void LoadAllSaves(const std::string &runtimePath);

        /**
         * Find out how many archives there are.
         * 获取有多少个存档。
         * @return
         */
        [[nodiscard]] size_t GetSavesListSize() const;
    };
}
