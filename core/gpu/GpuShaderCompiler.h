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

#include <cstddef>
#include <vector>

#include <SDL3/SDL_gpu.h>

namespace glimmer {
    /**
     * GpuShaderCompiler
     * GPU 着色器编译器
     *
     * Compiles GLSL 4.50 source code to SPIR-V at runtime using glslang and
     * wraps the result in an SDL_GPUShader.
     * 在运行时使用 glslang 将 GLSL 4.50 源码编译为 SPIR-V，并封装为 SDL_GPUShader。
     *
     * Usage: call Init() once before compiling any shader and Shutdown() after
     * the last shader has been compiled (typically during GpuContext init/shutdown).
     * 用法：编译任何着色器之前调用一次 Init()，最后一次编译结束后调用 Shutdown()。
     */
    class GpuShaderCompiler {
    public:
        /**
         * Initialize the glslang process state.
         * 初始化 glslang 进程状态。
         * Must be called on the same thread that later compiles shaders.
         * 必须在之后编译着色器的同一线程上调用。
         */
        static void Init();

        /**
         * Release the glslang process state.
         * 释放 glslang 进程状态。
         */
        static void Shutdown();

        /**
         * Compile GLSL source code into an SDL_GPUShader.
         * 将 GLSL 源码编译为 SDL_GPUShader。
         * @param device device 有效的 GPU 设备
         * @param source source GLSL 源码（以 '\0' 结尾）
         * @param debugName debugName 出错时用于日志定位的名称（通常是文件路径）
         * @param gpuStage gpuStage 着色器阶段（顶点或片元）
         * @param numSamplers numSamplers 着色器声明的采样器数量
         * @param numUniformBuffers numUniformBuffers 着色器声明的 uniform 缓冲数量
         * @return The compiled shader on success (caller must release it with
         * SDL_ReleaseGPUShader), nullptr on failure.
         * 成功返回编译好的着色器（调用方需用 SDL_ReleaseGPUShader 释放），失败返回 nullptr。
         */
        static SDL_GPUShader *CompileFromSource(SDL_GPUDevice *device, const char *source, const char *debugName,
                                                SDL_GPUShaderStage gpuStage, Uint32 numSamplers,
                                                Uint32 numUniformBuffers);

        /**
         * Compile GLSL source code to SPIR-V words (without creating a GPU
         * shader). The result can be cached on disk and later turned into an
         * SDL_GPUShader with CreateFromSpirv.
         * 将 GLSL 源码编译为 SPIR-V 字（不创建 GPU 着色器）。结果可以缓存到磁盘，
         * 之后通过 CreateFromSpirv 转换为 SDL_GPUShader。
         * @param source source GLSL 源码（以 '\0' 结尾）
         * @param debugName debugName 出错时用于日志定位的名称（通常是文件路径）
         * @param gpuStage gpuStage 着色器阶段（顶点或片元）
         * @return The SPIR-V words on success, an empty vector on failure.
         * 成功返回 SPIR-V 字数组，失败返回空数组。
         */
        static std::vector<unsigned int> CompileToSpirv(const char *source, const char *debugName,
                                                        SDL_GPUShaderStage gpuStage);

        /**
         * Create an SDL_GPUShader from SPIR-V binary data.
         * 从 SPIR-V 二进制数据创建 SDL_GPUShader。
         * @param device device 有效的 GPU 设备
         * @param code code SPIR-V 二进制数据
         * @param codeSize codeSize 数据字节数（必须是 4 的倍数）
         * @param debugName debugName 出错时用于日志定位的名称
         * @param gpuStage gpuStage 着色器阶段（顶点或片元）
         * @param numSamplers numSamplers 着色器声明的采样器数量
         * @param numUniformBuffers numUniformBuffers 着色器声明的 uniform 缓冲数量
         * @return The shader on success (caller must release it with
         * SDL_ReleaseGPUShader), nullptr on failure.
         * 成功返回着色器（调用方需用 SDL_ReleaseGPUShader 释放），失败返回 nullptr。
         */
        static SDL_GPUShader *CreateFromSpirv(SDL_GPUDevice *device, const void *code, size_t codeSize,
                                              const char *debugName, SDL_GPUShaderStage gpuStage, Uint32 numSamplers,
                                              Uint32 numUniformBuffers);

        /**
         * Load a GLSL file with SDL_LoadFile and compile it into an SDL_GPUShader.
         * 使用 SDL_LoadFile 读取 GLSL 文件并编译为 SDL_GPUShader。
         * @param device device 有效的 GPU 设备
         * @param filePath filePath 着色器文件路径（相对工作目录，Android 上相对 assets）
         * @param gpuStage gpuStage 着色器阶段（顶点或片元）
         * @param numSamplers numSamplers 着色器声明的采样器数量
         * @param numUniformBuffers numUniformBuffers 着色器声明的 uniform 缓冲数量
         * @return The compiled shader on success, nullptr on failure.
         * 成功返回编译好的着色器，失败返回 nullptr。
         */
        static SDL_GPUShader *CompileFromFile(SDL_GPUDevice *device, const char *filePath,
                                              SDL_GPUShaderStage gpuStage, Uint32 numSamplers,
                                              Uint32 numUniformBuffers);
    };
}
