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
#pragma once
#include <string_view>

#include "UniformInjectContext.h"

namespace glimmer {
    //Builtin semantic source names. 内置语义来源名。
    inline constexpr std::string_view BUILTIN_RESOLUTION = "@builtin.resolution";
    inline constexpr std::string_view BUILTIN_CAMERA_POSITION = "@builtin.camera_position";
    inline constexpr std::string_view BUILTIN_CAMERA_SIZE = "@builtin.camera_size";
    inline constexpr std::string_view BUILTIN_ZOOM = "@builtin.zoom";
    inline constexpr std::string_view BUILTIN_TILE_SIZE = "@builtin.tile_size";
    inline constexpr std::string_view BUILTIN_LIGHTMAP_ORIGIN = "@builtin.lightmap_origin";
    inline constexpr std::string_view BUILTIN_LIGHTMAP_SIZE = "@builtin.lightmap_size";
    inline constexpr std::string_view BUILTIN_TIME_OF_DAY = "@builtin.time_of_day";
    inline constexpr std::string_view BUILTIN_AMBIENT_COLOR = "@builtin.ambient_color";

    /**
     * UniformInjector
     * Uniform 注入器
     *
     * Stateless function that writes the current value of a builtin semantic
     * into the destination float buffer. The number of floats written is fixed
     * per builtin and must match the member's declared component count.
     * 无状态函数，将某个内置语义的当前值写入目标 float 缓冲区。
     * 每个内置语义写入的 float 数量固定，须与成员声明的分量数一致。
     */
    using UniformInjector = void(*)(const UniformInjectContext &ctx, float *dst);

    /**
     * UniformInjectorRegistry
     * Uniform 注入器注册表
     *
     * Maps builtin semantic names ("@builtin.*") to injector functions. Builtin
     * injectors are registered lazily on first access.
     * 将内置语义名（"@builtin.*"）映射到注入器函数。内置注入器首次访问时懒注册。
     */
    class UniformInjectorRegistry {
    public:
        /**
         * Register
         * 注册一个语义注入器。
         * @param builtin builtin 语义名（含 "@builtin." 前缀）
         * @param injector injector 注入函数
         */
        static void Register(std::string_view builtin, UniformInjector injector);

        /**
         * Find
         * 查找语义对应的注入器。
         * @param builtin builtin 语义名
         * @return 注入器；未注册时返回 nullptr。
         */
        static UniformInjector Find(std::string_view builtin);
    };
}
