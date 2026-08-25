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
#include "GpuShaderCompiler.h"

#include <vector>

#include "core/log/LogCat.h"
#include "glslang/Public/ResourceLimits.h"
#include "glslang/Public/ShaderLang.h"
#include "SPIRV/GlslangToSpv.h"

void glimmer::GpuShaderCompiler::Init() {
    glslang::InitializeProcess();
    LogCat::i("GpuShaderCompiler initialized");
}

void glimmer::GpuShaderCompiler::Shutdown() {
    glslang::FinalizeProcess();
}

SDL_GPUShader *glimmer::GpuShaderCompiler::CompileFromSource(SDL_GPUDevice *device, const char *source,
                                                             const char *debugName,
                                                             const SDL_GPUShaderStage gpuStage,
                                                             const Uint32 numSamplers,
                                                             const Uint32 numUniformBuffers) {
    if (device == nullptr || source == nullptr) {
        LogCat::w(std::source_location::current(), "device or source is nullptr: ", debugName);
        return nullptr;
    }
    const EShLanguage shaderStage = gpuStage == SDL_GPU_SHADERSTAGE_VERTEX ? EShLangVertex : EShLangFragment;
    glslang::TShader glslShader(shaderStage);
    const char *sources[] = {source};
    glslShader.setStrings(sources, 1);

    const TBuiltInResource *resources = GetDefaultResources();
    //Vulkan + SPIR-V rules are required, otherwise the generated SPIR-V is invalid for Vulkan.
    //必须使用 Vulkan + SPIR-V 规则，否则生成的 SPIR-V 不符合 Vulkan 要求。
    const auto messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules);
    if (!glslShader.parse(resources, 450, false, messages)) {
        LogCat::w(std::source_location::current(), "GLSL compilation failed (", debugName, "): ",
                  glslShader.getInfoLog());
        return nullptr;
    }
    glslang::SpvOptions options;
    std::vector<unsigned int> spirv;
    glslang::GlslangToSpv(*glslShader.getIntermediate(), spirv, &options);
    if (spirv.empty()) {
        LogCat::w(std::source_location::current(), "GLSL to SPIR-V conversion produced no output (", debugName, ")");
        return nullptr;
    }

    SDL_GPUShaderCreateInfo shaderInfo = {};
    shaderInfo.code_size = spirv.size() * sizeof(unsigned int);
    shaderInfo.code = reinterpret_cast<const Uint8 *>(spirv.data());
    shaderInfo.entrypoint = "main";
    shaderInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    shaderInfo.stage = gpuStage;
    shaderInfo.num_samplers = numSamplers;
    shaderInfo.num_storage_textures = 0;
    shaderInfo.num_storage_buffers = 0;
    shaderInfo.num_uniform_buffers = numUniformBuffers;
    shaderInfo.props = 0;
    SDL_GPUShader *gpuShader = SDL_CreateGPUShader(device, &shaderInfo);
    if (gpuShader == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_CreateGPUShader failed (", debugName, "): ", SDL_GetError());
    }
    return gpuShader;
}

SDL_GPUShader *glimmer::GpuShaderCompiler::CompileFromFile(SDL_GPUDevice *device, const char *filePath,
                                                           const SDL_GPUShaderStage gpuStage,
                                                           const Uint32 numSamplers,
                                                           const Uint32 numUniformBuffers) {
    size_t sourceSize = 0;
    char *source = static_cast<char *>(SDL_LoadFile(filePath, &sourceSize));
    if (source == nullptr) {
        LogCat::w(std::source_location::current(), "Failed to load GLSL file: ", filePath, " - ", SDL_GetError());
        return nullptr;
    }
    SDL_GPUShader *shader = CompileFromSource(device, source, filePath, gpuStage, numSamplers, numUniformBuffers);
    SDL_free(source);
    return shader;
}
