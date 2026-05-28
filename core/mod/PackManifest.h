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
//@genCode
#include <string>

#include "ResourceRef.h"
#include "dataPack/PackDependence.h"


namespace glimmer {
    //@genNextLine(PackManifest|包清单)
    struct PackManifest {
        //@genNextLine(id|包唯一标识)
        std::string id;
        //@genNextLine(name|包名称资源引用)
        ResourceRef name;
        //@genNextLine(description|包描述资源引用)
        ResourceRef description;
        //@genNextLine(author|包作者)
        std::string author;
        //@genNextLine(versionName|版本名称)
        std::string versionName;
        //@genNextLine(versionNumber|版本号)
        uint32_t versionNumber;
        //@genNextLine(minGameVersion|最低兼容游戏版本号)
        uint32_t minGameVersion;
        //@genNextLine(resPack|是否为资源包)
        bool resPack;
        //@genNextLine(templateSearchPath|模板搜索路径)
        std::vector<std::string> templateSearchPath = {TEMPLATE_CURRENT, TEMPLATE_ROOT};
    };

    //@genNextLine(DataPackManifest|数据包包清单)
    struct DataPackManifest : PackManifest {
        //@genNextLine(DatapackDependencies|包依赖列表)
        std::vector<PackDependence> packDependencies;
    };

    //@genNextLine(ResourcePackManifest|资源包包清单)
    struct ResourcePackManifest : PackManifest {
    };
}