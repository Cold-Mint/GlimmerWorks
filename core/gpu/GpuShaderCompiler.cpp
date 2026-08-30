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

#include "GpuShaderCompileResult.h"
#include "core/config/Constants.h"
#include "core/log/LogCat.h"
#include "glslang/MachineIndependent/localintermediate.h"
#include "glslang/Public/ResourceLimits.h"
#include "glslang/Public/ShaderLang.h"
#include "SPIRV/GlslangToSpv.h"

void glimmer::GpuShaderCompiler::Init() {
    glslang::InitializeProcess();
}

void glimmer::GpuShaderCompiler::Shutdown() {
    glslang::FinalizeProcess();
}

std::unique_ptr<glimmer::GpuShaderCompileResult> glimmer::GpuShaderCompiler::CompileToSpirv(const std::string &source,
    const bool vertex) {
    if (source.empty()) {
        return nullptr;
    }
    const EShLanguage shaderStage = vertex ? EShLangVertex : EShLangFragment;
    glslang::TShader glslShader(shaderStage);
    const char *sources[] = {source.c_str()};
    glslShader.setStrings(sources, 1);
    const TBuiltInResource *resources = GetDefaultResources();
    constexpr auto messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules);
    if (!glslShader.parse(resources, 450, false, messages)) {
        LogCat::w(std::source_location::current(), "GLSL compilation failed : ",
                  glslShader.getInfoLog());
        return nullptr;
    }
    glslang::SpvOptions options;
    //Use TProgram for linking, reflection, and for calculating numSamplers and numUniformBuffers
    //用TProgram做链接，反射，用于计算numSamplers和numUniformBuffers
    glslang::TProgram program;
    program.addShader(&glslShader);
    if (!program.link(messages)) {
        return nullptr;
    }
    if (!program.buildReflection()) {
        return nullptr;
    }
    auto result = std::make_unique<GpuShaderCompileResult>();
    result->SetNumUniformBuffers(static_cast<uint32_t>(program.getNumUniformBlocks()));
    uint32_t numSamplers = 0;
    for (int i = 0; i < program.getNumUniformVariables(); ++i) {
        const glslang::TType *type = program.getUniformTType(i);
        if (type == nullptr) {
            continue;
        }
        if (type->getBasicType() == glslang::EbtSampler && type->isTexture()) {
            ++numSamplers;
        }
    }
    result->SetNumSamplers(numSamplers);
    glslang::GlslangToSpv(*glslShader.getIntermediate(), result->GetMutableCode(), &options);
    return result;
}
