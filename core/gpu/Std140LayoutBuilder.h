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
#include <cstdint>
#include <string_view>

namespace glimmer {
    /**
     * UniformScalarType
     * Uniform 成员标量类型
     */
    enum class UniformScalarType : uint8_t {
        Float = 0,
        Vec2 = 1,
        Vec3 = 2,
        Vec4 = 3,
        Mat4 = 4,
        Invalid = 255,
    };

    /**
     * Std140LayoutBuilder
     * std140 布局计算器
     *
     * Computes the deterministic std140 offsets/sizes for a uniform block based
     * on the declared member order. The TOML declaration is authoritative;
     * glslang reflection may be used only as an optional debug cross-check.
     * 基于声明的成员顺序计算 uniform 块的确定性 std140 偏移/大小。
     * TOML 声明是权威的；glslang 反射仅作可选的调试校验。
     */
    class Std140LayoutBuilder {
    public:
        /**
         * ParseType
         * 将类型名解析为 UniformScalarType。
         * @param type type 类型名（float/vec2/vec3/vec4/mat4）
         */
        static UniformScalarType ParseType(std::string_view type);

        /**
         * GetComponentCount
         * 获取类型对应的 float 分量数量。
         */
        static uint32_t GetComponentCount(UniformScalarType type);

        /**
         * GetAlignment
         * 获取类型在 std140 布局下的对齐字节数。
         */
        static uint32_t GetAlignment(UniformScalarType type);

        /**
         * GetSize
         * 获取类型在 std140 布局下占用的字节数。
         */
        static uint32_t GetSize(UniformScalarType type);
    };
}
