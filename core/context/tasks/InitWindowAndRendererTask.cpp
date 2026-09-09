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
#include "InitWindowAndRendererTask.h"

#include "core/config/Config.h"
#include "core/context/ISystemBucket.h"
#include "core/context/RmlContext.h"
#include "core/context/WindowContext.h"
#include "core/gpu/GpuShaderCompiler.h"
#include "core/log/LogCat.h"
#include "core/mod/ResourceLocator.h"
#include "core/mod/resourcePack/ResourcePackManager.h"
#include "core/vfs/VirtualFileSystem.h"

bool glimmer::InitWindowAndRendererTask::Run(ISystemBucket *systemBucket) {
    Config *config = systemBucket->GetConfig();
    if (config == nullptr) {
        LogCat::e(std::source_location::current(), "config_is_null", "config is nullptr");
        return false;
    }
    LogCat::i("creating_window", "Creating window: width={}, height={}, fullscreen={}", config->window.width,
              config->window.height, config->window.fullscreen);
    WindowContext *windowContext = systemBucket->GetWindowContext();
    if (windowContext == nullptr) {
        LogCat::e(std::source_location::current(), "window_context_is_null", "windowContext is nullptr");
        return false;
    }
    if (!windowContext->CreateWindowAndDevice(config->window.width, config->window.height, config->window.fullscreen)) {
        return false;
    }
    ResourcePackManager *resourcePackManager = systemBucket->GetResourcePackManager();
    if (resourcePackManager == nullptr) {
        LogCat::e(std::source_location::current(), "resource_pack_manager_is_null", "ResourcePackManager is nullptr");
        return false;
    }
    GpuShaderCompiler::Init();
    LogCat::i("gpu_renderer_created", "GpuRenderer created successfully");
    RmlContext *rmlContext = systemBucket->GetRmlContext();
    if (rmlContext == nullptr) {
        LogCat::e(std::source_location::current(), "rml_context_is_null", "RmlContext is nullptr");
        return false;
    }
    LogCat::i("initializing_rml_context", "Initializing RmlContext");
    rmlContext->Init(systemBucket->GetVirtualFileSystem(), windowContext->GetDevice(),
                     systemBucket->GetResourceLocator(), systemBucket->GetLangsValue(), windowContext->GetWindow(),
                     config->window.width, config->window.height);
    LogCat::i("rml_context_initialized", "RmlContext initialized successfully");
    return true;
}

void glimmer::InitWindowAndRendererTask::Rollback(ISystemBucket *systemBucket) {
    Shutdown(systemBucket);
}

void glimmer::InitWindowAndRendererTask::Shutdown(ISystemBucket *systemBucket) {
    GpuShaderCompiler::Shutdown();
    if (WindowContext *windowContext = systemBucket->GetWindowContext(); windowContext != nullptr) {
        windowContext->Shutdown();
    }
}

std::string glimmer::InitWindowAndRendererTask::GetTaskName() {
    return "InitWindowAndRendererTask";
}
