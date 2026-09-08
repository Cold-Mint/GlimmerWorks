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
#include <cstdint>

namespace glimmer {
    /**
     * ErrorCode
     * 错误代码
     *
     * Do not reuse incorrect codes. For newly added errors, a new error code should be added.
     * 不要复用错误代码，新增的错误要增加错误代码。
     */
    enum class ErrorCode : uint32_t {
        SUCCESS = 0,
        //缺少必要的资源包
        MISSING_RESPACK = 1,
        //缺少必要的玩家定义（玩家toml文件）
        MISSING_PLAYER = 2,
        //缺少必要的数据包
        MISSING_DATAPACK = 3,
        //无法读取配置文件config.toml（配置文件不存在或者读写权限受限。）
        CAN_NOT_READ_CONFIG_DATA = 4,
        //无法读取语言文件/langs文件下的toml。
        CAN_NOT_READ_LANG = 5,

    };
}
