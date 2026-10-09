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
#include "Std140LayoutBuilder.h"

glimmer::UniformScalarType glimmer::Std140LayoutBuilder::ParseType(const std::string_view type) {
    if (type == "float") {
        return UniformScalarType::Float;
    }
    if (type == "vec2") {
        return UniformScalarType::Vec2;
    }
    if (type == "vec3") {
        return UniformScalarType::Vec3;
    }
    if (type == "vec4") {
        return UniformScalarType::Vec4;
    }
    if (type == "mat4") {
        return UniformScalarType::Mat4;
    }
    return UniformScalarType::Invalid;
}

uint32_t glimmer::Std140LayoutBuilder::GetComponentCount(const UniformScalarType type) {
    switch (type) {
        case UniformScalarType::Float:
            return 1;
        case UniformScalarType::Vec2:
            return 2;
        case UniformScalarType::Vec3:
            return 3;
        case UniformScalarType::Vec4:
            return 4;
        case UniformScalarType::Mat4:
            return 16;
        case UniformScalarType::Invalid:
        default:
            return 0;
    }
}

uint32_t glimmer::Std140LayoutBuilder::GetAlignment(const UniformScalarType type) {
    switch (type) {
        case UniformScalarType::Float:
            return 4;
        case UniformScalarType::Vec2:
            return 8;
        case UniformScalarType::Vec3:
        case UniformScalarType::Vec4:
        case UniformScalarType::Mat4:
            return 16;
        case UniformScalarType::Invalid:
        default:
            return 0;
    }
}

uint32_t glimmer::Std140LayoutBuilder::GetSize(const UniformScalarType type) {
    switch (type) {
        case UniformScalarType::Float:
            return 4;
        case UniformScalarType::Vec2:
            return 8;
        case UniformScalarType::Vec3:
            return 12;
        case UniformScalarType::Vec4:
            return 16;
        case UniformScalarType::Mat4:
            return 64;
        case UniformScalarType::Invalid:
        default:
            return 0;
    }
}
