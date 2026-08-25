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
#include "SpriteRenderer.h"

#include <cmath>

#include "GpuShaderCompiler.h"
#include "core/log/LogCat.h"

namespace {
    constexpr const char *SPRITE_VERTEX_SHADER_PATH = "shaders/sprite.vert";
    constexpr const char *SPRITE_FRAGMENT_SHADER_PATH = "shaders/sprite.frag";
}

glimmer::SpriteRenderer::~SpriteRenderer() {
    Shutdown();
}

bool glimmer::SpriteRenderer::Init(GpuContext *gpuContext) {
    if (gpuContext == nullptr || gpuContext->GetDevice() == nullptr) {
        LogCat::w(std::source_location::current(), "gpuContext is nullptr or not initialized");
        return false;
    }
    gpuContext_ = gpuContext;
    device_ = gpuContext->GetDevice();

    SDL_GPUShader *vertexShader = GpuShaderCompiler::CompileFromFile(
        device_, SPRITE_VERTEX_SHADER_PATH, SDL_GPU_SHADERSTAGE_VERTEX, 0, 1);
    SDL_GPUShader *fragmentShader = GpuShaderCompiler::CompileFromFile(
        device_, SPRITE_FRAGMENT_SHADER_PATH, SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 0);
    if (vertexShader == nullptr || fragmentShader == nullptr) {
        if (vertexShader != nullptr) {
            SDL_ReleaseGPUShader(device_, vertexShader);
        }
        if (fragmentShader != nullptr) {
            SDL_ReleaseGPUShader(device_, fragmentShader);
        }
        LogCat::e(std::source_location::current(), "Failed to compile sprite shaders");
        return false;
    }

    SDL_GPUVertexBufferDescription vertexBufferDescription = {};
    vertexBufferDescription.slot = 0;
    vertexBufferDescription.pitch = sizeof(SpriteVertex);
    vertexBufferDescription.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertexBufferDescription.instance_step_rate = 0;

    SDL_GPUVertexAttribute attributes[3] = {};
    attributes[0].location = 0;
    attributes[0].buffer_slot = 0;
    attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attributes[0].offset = offsetof(SpriteVertex, x);
    attributes[1].location = 1;
    attributes[1].buffer_slot = 0;
    attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attributes[1].offset = offsetof(SpriteVertex, u);
    attributes[2].location = 2;
    attributes[2].buffer_slot = 0;
    attributes[2].format = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM;
    attributes[2].offset = offsetof(SpriteVertex, r);

    //Standard alpha blending, equivalent to SDL_BLENDMODE_BLEND.
    //标准 alpha 混合，等价于 SDL_BLENDMODE_BLEND。
    SDL_GPUColorTargetDescription colorTarget = {};
    colorTarget.format = gpuContext_->GetSwapchainFormat();
    colorTarget.blend_state.enable_blend = true;
    colorTarget.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTarget.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTarget.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTarget.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTarget.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTarget.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = &vertexBufferDescription;
    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_attributes = attributes;
    pipelineInfo.vertex_input_state.num_vertex_attributes = 3;
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipelineInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    pipelineInfo.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
    pipelineInfo.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
    pipelineInfo.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
    pipelineInfo.target_info.color_target_descriptions = &colorTarget;
    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.has_depth_stencil_target = false;
    pipelineInfo.props = 0;
    pipeline_ = SDL_CreateGPUGraphicsPipeline(device_, &pipelineInfo);
    SDL_ReleaseGPUShader(device_, vertexShader);
    SDL_ReleaseGPUShader(device_, fragmentShader);
    if (pipeline_ == nullptr) {
        LogCat::e(std::source_location::current(), "SDL_CreateGPUGraphicsPipeline failed: ", SDL_GetError());
        return false;
    }

    SDL_GPUBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    bufferCreateInfo.size = INITIAL_VERTEX_CAPACITY * sizeof(SpriteVertex);
    bufferCreateInfo.props = 0;
    vertexBuffer_ = SDL_CreateGPUBuffer(device_, &bufferCreateInfo);
    if (vertexBuffer_ == nullptr) {
        LogCat::e(std::source_location::current(), "SDL_CreateGPUBuffer failed: ", SDL_GetError());
        Shutdown();
        return false;
    }
    vertexBufferCapacity_ = INITIAL_VERTEX_CAPACITY;

    //1x1 white texture used to draw plain colored geometry (rects/lines/points).
    //1x1 白色纹理，用于绘制纯色几何图形（矩形/线/点）。
    SDL_Surface *whiteSurface = SDL_CreateSurface(1, 1, SDL_PIXELFORMAT_ABGR8888);
    if (whiteSurface == nullptr) {
        LogCat::e(std::source_location::current(), "SDL_CreateSurface failed: ", SDL_GetError());
        Shutdown();
        return false;
    }
    SDL_FillSurfaceRect(whiteSurface, nullptr, SDL_MapSurfaceRGBA(whiteSurface, 255, 255, 255, 255));
    whiteTexture_.reset(gpuContext_->CreateTextureFromSurface(whiteSurface));
    SDL_DestroySurface(whiteSurface);
    if (whiteTexture_ == nullptr) {
        LogCat::e(std::source_location::current(), "Failed to create white texture");
        Shutdown();
        return false;
    }
    LogCat::i("SpriteRenderer initialized");
    return true;
}

void glimmer::SpriteRenderer::Shutdown() {
    if (device_ != nullptr) {
        whiteTexture_.reset();
        if (vertexBuffer_ != nullptr) {
            SDL_ReleaseGPUBuffer(device_, vertexBuffer_);
            vertexBuffer_ = nullptr;
        }
        vertexBufferCapacity_ = 0;
        if (pipeline_ != nullptr) {
            SDL_ReleaseGPUGraphicsPipeline(device_, pipeline_);
            pipeline_ = nullptr;
        }
    }
    device_ = nullptr;
    gpuContext_ = nullptr;
}

bool glimmer::SpriteRenderer::BeginFrame(SDL_Window *window) {
    if (device_ == nullptr) {
        return false;
    }
    commandBuffer_ = SDL_AcquireGPUCommandBuffer(device_);
    if (commandBuffer_ == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_AcquireGPUCommandBuffer failed: ", SDL_GetError());
        return false;
    }
    swapchainTexture_ = nullptr;
    if (!SDL_AcquireGPUSwapchainTexture(commandBuffer_, window, &swapchainTexture_, &swapchainWidth_,
                                        &swapchainHeight_) ) {
        LogCat::w(std::source_location::current(), "SDL_AcquireGPUSwapchainTexture failed: ", SDL_GetError());
        SDL_CancelGPUCommandBuffer(commandBuffer_);
        commandBuffer_ = nullptr;
        return false;
    }
    if (swapchainTexture_ == nullptr) {
        //Too many frames in flight or window minimized: skip drawing this frame
        //but still submit the command buffer in SubmitFrame().
        //帧在飞行中过多或窗口最小化：跳过本帧绘制，但仍在 SubmitFrame() 中提交命令缓冲。
        frameActive_ = false;
        return false;
    }
    frameActive_ = true;
    vertices_.clear();
    runs_.clear();
    currentTexture_ = nullptr;
    EnsureRenderPass(true);
    return true;
}

void glimmer::SpriteRenderer::EnsureRenderPass(const bool clear) {
    if (renderPass_ != nullptr || !frameActive_) {
        return;
    }
    SDL_GPUColorTargetInfo colorTargetInfo = {};
    colorTargetInfo.texture = swapchainTexture_;
    colorTargetInfo.mip_level = 0;
    colorTargetInfo.layer_or_depth_plane = 0;
    colorTargetInfo.load_op = clear ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
    colorTargetInfo.clear_color = {
        static_cast<float>(drawColor_.r) / 255.0F,
        static_cast<float>(drawColor_.g) / 255.0F,
        static_cast<float>(drawColor_.b) / 255.0F,
        static_cast<float>(drawColor_.a) / 255.0F
    };
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.cycle = false;
    renderPass_ = SDL_BeginGPURenderPass(commandBuffer_, &colorTargetInfo, 1, nullptr);
    if (renderPass_ == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_BeginGPURenderPass failed: ", SDL_GetError());
    }
}

void glimmer::SpriteRenderer::EndFrame() {
    if (!frameActive_) {
        return;
    }
    //Upload and draw any pending geometry.
    //上传并绘制所有待处理的几何数据。
    if (!vertices_.empty()) {
        if (renderPass_ != nullptr) {
            SDL_EndGPURenderPass(renderPass_);
            renderPass_ = nullptr;
        }
        const Uint32 dataSize = static_cast<Uint32>(vertices_.size() * sizeof(SpriteVertex));
        if (vertices_.size() > vertexBufferCapacity_) {
            Uint32 newCapacity = vertexBufferCapacity_ == 0 ? INITIAL_VERTEX_CAPACITY : vertexBufferCapacity_;
            while (newCapacity < vertices_.size()) {
                newCapacity *= 2;
            }
            SDL_GPUBufferCreateInfo bufferCreateInfo = {};
            bufferCreateInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
            bufferCreateInfo.size = newCapacity * sizeof(SpriteVertex);
            bufferCreateInfo.props = 0;
            SDL_GPUBuffer *newBuffer = SDL_CreateGPUBuffer(device_, &bufferCreateInfo);
            if (newBuffer == nullptr) {
                LogCat::w(std::source_location::current(), "Failed to grow vertex buffer: ", SDL_GetError());
            } else {
                SDL_ReleaseGPUBuffer(device_, vertexBuffer_);
                vertexBuffer_ = newBuffer;
                vertexBufferCapacity_ = newCapacity;
            }
        }
        if (vertexBuffer_ != nullptr) {
            SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {};
            transferBufferCreateInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
            transferBufferCreateInfo.size = dataSize;
            transferBufferCreateInfo.props = 0;
            SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(device_, &transferBufferCreateInfo);
            if (transferBuffer != nullptr) {
                void *mapped = SDL_MapGPUTransferBuffer(device_, transferBuffer, false);
                if (mapped != nullptr) {
                    SDL_memcpy(mapped, vertices_.data(), dataSize);
                    SDL_UnmapGPUTransferBuffer(device_, transferBuffer);
                    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer_);
                    if (copyPass != nullptr) {
                        SDL_GPUTransferBufferLocation location = {};
                        location.transfer_buffer = transferBuffer;
                        location.offset = 0;
                        SDL_GPUBufferRegion region = {};
                        region.buffer = vertexBuffer_;
                        region.offset = 0;
                        region.size = dataSize;
                        SDL_UploadToGPUBuffer(copyPass, &location, &region, false);
                        SDL_EndGPUCopyPass(copyPass);
                        EnsureRenderPass(false);
                        if (renderPass_ != nullptr) {
                            SDL_BindGPUGraphicsPipeline(renderPass_, pipeline_);
                            SDL_GPUViewport viewport = {};
                            viewport.x = 0.0F;
                            viewport.y = 0.0F;
                            viewport.w = static_cast<float>(swapchainWidth_);
                            viewport.h = static_cast<float>(swapchainHeight_);
                            viewport.min_depth = 0.0F;
                            viewport.max_depth = 1.0F;
                            SDL_SetGPUViewport(renderPass_, &viewport);
                            const float viewSize[2] = {
                                static_cast<float>(swapchainWidth_),
                                static_cast<float>(swapchainHeight_)
                            };
                            SDL_PushGPUVertexUniformData(commandBuffer_, 0, viewSize, sizeof(viewSize));
                            SDL_GPUBufferBinding vertexBufferBinding = {};
                            vertexBufferBinding.buffer = vertexBuffer_;
                            vertexBufferBinding.offset = 0;
                            SDL_BindGPUVertexBuffers(renderPass_, 0, &vertexBufferBinding, 1);
                            for (const DrawRun &run: runs_) {
                                SDL_GPUTextureSamplerBinding textureBinding = {};
                                textureBinding.texture = run.texture;
                                textureBinding.sampler = gpuContext_->GetNearestSampler();
                                SDL_BindGPUFragmentSamplers(renderPass_, 0, &textureBinding, 1);
                                SDL_DrawGPUPrimitives(renderPass_, run.vertexCount, 1, run.firstVertex, 0);
                            }
                        }
                    }
                }
                //Safe to release right after the copy pass is enqueued; SDL
                //defers destruction until the command buffer completes.
                //拷贝通道入队后即可安全释放；SDL 会将销毁延迟到命令缓冲完成。
                SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
            }
        }
        vertices_.clear();
        runs_.clear();
        currentTexture_ = nullptr;
    }
    if (renderPass_ != nullptr) {
        SDL_EndGPURenderPass(renderPass_);
        renderPass_ = nullptr;
    }
}

bool glimmer::SpriteRenderer::SubmitFrame() {
    if (commandBuffer_ == nullptr) {
        return false;
    }
    const bool result = SDL_SubmitGPUCommandBuffer(commandBuffer_);
    if (!result) {
        LogCat::w(std::source_location::current(), "SDL_SubmitGPUCommandBuffer failed: ", SDL_GetError());
    }
    commandBuffer_ = nullptr;
    swapchainTexture_ = nullptr;
    frameActive_ = false;
    return result;
}

SDL_GPUCommandBuffer *glimmer::SpriteRenderer::GetCommandBuffer() const {
    return commandBuffer_;
}

SDL_GPUTexture *glimmer::SpriteRenderer::GetSwapchainTexture() const {
    return swapchainTexture_;
}

Uint32 glimmer::SpriteRenderer::GetSwapchainWidth() const {
    return swapchainWidth_;
}

Uint32 glimmer::SpriteRenderer::GetSwapchainHeight() const {
    return swapchainHeight_;
}

void glimmer::SpriteRenderer::SetDrawColor(const SDL_Color color) {
    drawColor_ = color;
}

SDL_Color glimmer::SpriteRenderer::GetDrawColor() const {
    return drawColor_;
}

void glimmer::SpriteRenderer::AppendQuad(SDL_GPUTexture *texture, const SDL_FPoint positions[4],
                                         const SDL_FPoint uvs[4], const SDL_Color &color) {
    if (!frameActive_ || renderPass_ == nullptr) {
        return;
    }
    if (vertices_.size() + 6 > vertexBufferCapacity_) {
        //Flush pending geometry before growing the CPU batch.
        //在扩充 CPU 批次之前先刷新待处理的几何数据。
        EndFrame();
        EnsureRenderPass(false);
        if (renderPass_ == nullptr) {
            return;
        }
    }
    if (currentTexture_ != texture) {
        currentTexture_ = texture;
        runs_.push_back({texture, static_cast<Uint32>(vertices_.size()), 0});
    }
    const SpriteVertex topLeft = {positions[0].x, positions[0].y, uvs[0].x, uvs[0].y, color.r, color.g, color.b, color.a};
    const SpriteVertex topRight = {positions[1].x, positions[1].y, uvs[1].x, uvs[1].y, color.r, color.g, color.b, color.a};
    const SpriteVertex bottomLeft = {positions[2].x, positions[2].y, uvs[2].x, uvs[2].y, color.r, color.g, color.b, color.a};
    const SpriteVertex bottomRight = {positions[3].x, positions[3].y, uvs[3].x, uvs[3].y, color.r, color.g, color.b, color.a};
    vertices_.push_back(topLeft);
    vertices_.push_back(topRight);
    vertices_.push_back(bottomLeft);
    vertices_.push_back(topRight);
    vertices_.push_back(bottomRight);
    vertices_.push_back(bottomLeft);
    runs_.back().vertexCount += 6;
}

void glimmer::SpriteRenderer::DrawTexture(const GpuTexture *texture, const SDL_FRect *src, const SDL_FRect *dst,
                                          const SDL_Color &mod) {
    if (texture == nullptr || !texture->IsValid()) {
        return;
    }
    const float textureWidth = static_cast<float>(texture->w);
    const float textureHeight = static_cast<float>(texture->h);
    SDL_FRect dstRect;
    if (dst == nullptr) {
        dstRect = {0.0F, 0.0F, textureWidth, textureHeight};
    } else {
        dstRect = *dst;
    }
    if (dstRect.w <= 0.0F || dstRect.h <= 0.0F) {
        return;
    }
    float u0 = 0.0F;
    float v0 = 0.0F;
    float u1 = 1.0F;
    float v1 = 1.0F;
    if (src != nullptr) {
        u0 = src->x / textureWidth;
        v0 = src->y / textureHeight;
        u1 = (src->x + src->w) / textureWidth;
        v1 = (src->y + src->h) / textureHeight;
    }
    const SDL_FPoint positions[4] = {
        {dstRect.x, dstRect.y},
        {dstRect.x + dstRect.w, dstRect.y},
        {dstRect.x, dstRect.y + dstRect.h},
        {dstRect.x + dstRect.w, dstRect.y + dstRect.h}
    };
    const SDL_FPoint uvs[4] = {
        {u0, v0},
        {u1, v0},
        {u0, v1},
        {u1, v1}
    };
    AppendQuad(texture->GetGpuTexture(), positions, uvs, mod);
}

void glimmer::SpriteRenderer::DrawTextureRotated(const GpuTexture *texture, const SDL_FRect *src,
                                                 const SDL_FRect *dst, const double angleDegrees,
                                                 const SDL_FPoint *center, const Uint8 flip, const SDL_Color &mod) {
    if (texture == nullptr || !texture->IsValid() || dst == nullptr) {
        return;
    }
    if (dst->w <= 0.0F || dst->h <= 0.0F) {
        return;
    }
    const float textureWidth = static_cast<float>(texture->w);
    const float textureHeight = static_cast<float>(texture->h);
    float u0 = 0.0F;
    float v0 = 0.0F;
    float u1 = 1.0F;
    float v1 = 1.0F;
    if (src != nullptr) {
        u0 = src->x / textureWidth;
        v0 = src->y / textureHeight;
        u1 = (src->x + src->w) / textureWidth;
        v1 = (src->y + src->h) / textureHeight;
    }
    SDL_FPoint positions[4] = {
        {dst->x, dst->y},
        {dst->x + dst->w, dst->y},
        {dst->x, dst->y + dst->h},
        {dst->x + dst->w, dst->y + dst->h}
    };
    if (angleDegrees != 0.0) {
        SDL_FPoint rotationCenter;
        if (center == nullptr) {
            rotationCenter = {dst->x + dst->w * 0.5F, dst->y + dst->h * 0.5F};
        } else {
            rotationCenter = {dst->x + center->x, dst->y + center->y};
        }
        //Positive angles rotate clockwise (SDL_RenderTextureRotated semantics,
        //+Y points down in screen space).
        //正角度顺时针旋转（SDL_RenderTextureRotated 语义，屏幕空间 +Y 向下）。
        const double radians = angleDegrees * (3.14159265358979323846 / 180.0);
        const auto cosValue = static_cast<float>(std::cos(radians));
        const auto sinValue = static_cast<float>(std::sin(radians));
        for (auto &position: positions) {
            const float dx = position.x - rotationCenter.x;
            const float dy = position.y - rotationCenter.y;
            position.x = rotationCenter.x + dx * cosValue - dy * sinValue;
            position.y = rotationCenter.y + dx * sinValue + dy * cosValue;
        }
    }
    SDL_FPoint uvs[4] = {
        {u0, v0},
        {u1, v0},
        {u0, v1},
        {u1, v1}
    };
    if ((flip & FLIP_HORIZONTAL) != 0) {
        std::swap(uvs[0].x, uvs[1].x);
        std::swap(uvs[2].x, uvs[3].x);
    }
    if ((flip & FLIP_VERTICAL) != 0) {
        std::swap(uvs[0].y, uvs[2].y);
        std::swap(uvs[1].y, uvs[3].y);
    }
    AppendQuad(texture->GetGpuTexture(), positions, uvs, mod);
}

void glimmer::SpriteRenderer::FillRect(const SDL_FRect *rect) {
    if (rect == nullptr || whiteTexture_ == nullptr) {
        return;
    }
    if (rect->w <= 0.0F || rect->h <= 0.0F) {
        return;
    }
    const SDL_FPoint positions[4] = {
        {rect->x, rect->y},
        {rect->x + rect->w, rect->y},
        {rect->x, rect->y + rect->h},
        {rect->x + rect->w, rect->y + rect->h}
    };
    const SDL_FPoint uvs[4] = {
        {0.0F, 0.0F},
        {1.0F, 0.0F},
        {0.0F, 1.0F},
        {1.0F, 1.0F}
    };
    AppendQuad(whiteTexture_->GetGpuTexture(), positions, uvs, drawColor_);
}

void glimmer::SpriteRenderer::DrawRect(const SDL_FRect *rect) {
    if (rect == nullptr) {
        return;
    }
    const SDL_FRect top = {rect->x, rect->y, rect->w, 1.0F};
    const SDL_FRect bottom = {rect->x, rect->y + rect->h - 1.0F, rect->w, 1.0F};
    const SDL_FRect left = {rect->x, rect->y + 1.0F, 1.0F, rect->h - 2.0F};
    const SDL_FRect right = {rect->x + rect->w - 1.0F, rect->y + 1.0F, 1.0F, rect->h - 2.0F};
    FillRect(&top);
    FillRect(&bottom);
    FillRect(&left);
    FillRect(&right);
}

void glimmer::SpriteRenderer::DrawLine(const float x1, const float y1, const float x2, const float y2) {
    const float dx = x2 - x1;
    const float dy = y2 - y1;
    const float length = std::sqrt(dx * dx + dy * dy);
    if (length <= 0.0F) {
        DrawPoint(x1, y1);
        return;
    }
    //A 1-pixel thick quad perpendicular to the line direction.
    //垂直于线段方向的 1 像素宽四边形。
    const float nx = -dy / length * 0.5F;
    const float ny = dx / length * 0.5F;
    const SDL_FPoint positions[4] = {
        {x1 + nx, y1 + ny},
        {x1 - nx, y1 - ny},
        {x2 + nx, y2 + ny},
        {x2 - nx, y2 - ny}
    };
    const SDL_FPoint uvs[4] = {
        {0.0F, 0.0F},
        {1.0F, 0.0F},
        {0.0F, 1.0F},
        {1.0F, 1.0F}
    };
    if (whiteTexture_ == nullptr) {
        return;
    }
    AppendQuad(whiteTexture_->GetGpuTexture(), positions, uvs, drawColor_);
}

void glimmer::SpriteRenderer::DrawPoint(const float x, const float y) {
    const SDL_FRect rect = {x, y, 1.0F, 1.0F};
    FillRect(&rect);
}
