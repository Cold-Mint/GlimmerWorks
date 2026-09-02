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
#include "BlendMode.h"

glimmer::BlendMode glimmer::BlendModeFromUint8(const uint8_t value) {
    switch (value) {
        case 1:
            return BlendMode::Alpha;
        case 2:
            return BlendMode::Additive;
        case 3:
            return BlendMode::Multiply;
        case 4:
            return BlendMode::Premultiplied;
        case 0:
        default:
            return BlendMode::Opaque;
    }
}

SDL_GPUColorTargetBlendState glimmer::ToColorTargetBlendState(const BlendMode mode) {
    SDL_GPUColorTargetBlendState state = {};
    state.enable_blend = false;
    state.enable_color_write_mask = false;
    state.color_write_mask = 0;

    switch (mode) {
        case BlendMode::Opaque:
            break;
        case BlendMode::Alpha:
            state.enable_blend = true;
            state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
            state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
            state.color_blend_op = SDL_GPU_BLENDOP_ADD;
            state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
            state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
            state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
            break;
        case BlendMode::Additive:
            state.enable_blend = true;
            state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
            state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
            state.color_blend_op = SDL_GPU_BLENDOP_ADD;
            state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
            state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
            state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
            break;
        case BlendMode::Multiply:
            state.enable_blend = true;
            state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_DST_COLOR;
            state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
            state.color_blend_op = SDL_GPU_BLENDOP_ADD;
            state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
            state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
            state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
            break;
        case BlendMode::Premultiplied:
            state.enable_blend = true;
            state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
            state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
            state.color_blend_op = SDL_GPU_BLENDOP_ADD;
            state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
            state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
            state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
            break;
    }
    return state;
}
