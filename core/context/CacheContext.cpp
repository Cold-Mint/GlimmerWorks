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
#include "CacheContext.h"

#include "core/context/AppContext.h"


glimmer::CacheContext::CacheContext(AppContext *appContext) : appContext_(appContext) {
    audioCache_ = std::make_unique<AudioCache>();
    gpuPipelineCache_ = std::make_unique<GpuPipelineCache>();
    shaderCache_ = std::make_unique<ShaderCache>();
    textureCache_ = std::make_unique<TextureCache>();
    rmlCache_ = std::make_unique<RmlCache>();
    colorCache_ = std::make_unique<ColorCache>();
    gpuSamplerCache_ = std::make_unique<GpuSamplerCache>();
}

glimmer::GpuSamplerCache *glimmer::CacheContext::GetGpuSamplerCache() const {
    return gpuSamplerCache_.get();
}

glimmer::AudioCache *glimmer::CacheContext::GetAudioCache() const {
    return audioCache_.get();
}

glimmer::GpuPipelineCache *glimmer::CacheContext::GetPipelineCache() const {
    return gpuPipelineCache_.get();
}


glimmer::ShaderCache *glimmer::CacheContext::GetShaderCache() const {
    return shaderCache_.get();
}

glimmer::ColorCache *glimmer::CacheContext::GetColorCache() const {
    return colorCache_.get();
}

glimmer::RmlCache *glimmer::CacheContext::GetRmlCache() const {
    return rmlCache_.get();
}

glimmer::TextureCache *glimmer::CacheContext::GetTextureCache() const {
    return textureCache_.get();
}
