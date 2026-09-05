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
#include "AppContext.h"

#include <random>

#include "SystemBucket.h"
#include "core/log/LogCat.h"
#include "core/utils/RandomUtils.h"
#include "core/utils/StringUtils.h"
#include "fmt/xchar.h"
#include "SDL3/SDL.h"
#include "tasks/InitConfigTask.h"
#include "tasks/InitConsoleContextTask.h"
#include "tasks/InitCoreContextsTask.h"
#include "tasks/InitDataPackTask.h"
#include "tasks/InitLangsTask.h"
#include "tasks/InitModContextTask.h"
#include "tasks/InitResourceLocatorTask.h"
#include "tasks/InitResourcePackTask.h"
#include "tasks/InitRmlContextTask.h"
#include "tasks/InitSavesManagerTask.h"
#include "tasks/InitVFSTask.h"

#ifdef __ANDROID__
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "core/vfs/AndroidAssetsFileProvider.h"
#endif


void glimmer::AppContext::RegisterInitTask(std::unique_ptr<IAppContextInitTask> initTask) {
    initTasks_.emplace_back(std::move(initTask));
}

glimmer::AppContext::AppContext() {
    systemBucket_ = std::make_unique<SystemBucket>();
    RegisterInitTask(std::make_unique<InitVFSTask>());
    RegisterInitTask(std::make_unique<InitLangsTask>());
    RegisterInitTask(std::make_unique<InitCoreContextsTask>(this));
    RegisterInitTask(std::make_unique<InitConfigTask>());
    RegisterInitTask(std::make_unique<InitSavesManagerTask>());
    RegisterInitTask(std::make_unique<InitRmlContextTask>());
    RegisterInitTask(std::make_unique<InitModContextTask>());
    RegisterInitTask(std::make_unique<InitConsoleContextTask>(this));
    RegisterInitTask(std::make_unique<InitDataPackTask>(this));
    RegisterInitTask(std::make_unique<InitResourcePackTask>(this));
    RegisterInitTask(std::make_unique<InitResourceLocatorTask>(this));
}

bool glimmer::AppContext::InitSystem() {
    bool success = true;
    std::stack<IAppContextInitTask *> initTaskStack;
    ISystemBucket *systemBucket = systemBucket_.get();
    for (auto &initTask: initTasks_) {
        IAppContextInitTask *initTaskPtr = initTask.get();
        if (initTaskPtr == nullptr) {
            continue;
        }
        if (initTaskPtr->Run(systemBucket)) {
            initTaskStack.push(initTaskPtr);
        } else {
            success = false;
            break;
        }
    }
    if (success) {
        LogCat::i("AppContext initialization completed successfully");
        return true;
    }
    while (!initTaskStack.empty()) {
        initTaskStack.top()->Rollback(systemBucket);
        initTaskStack.pop();
    }
    return false;
}

glimmer::WindowContext *glimmer::AppContext::GetWindowContext() const {
    return systemBucket_->GetWindowContext();
}

void glimmer::AppContext::ExitApp() const {
    if (const ConsoleContext *consoleContext = systemBucket_->GetConsoleContext(); consoleContext != nullptr) {
        consoleContext->StopConsoleWorker();
        if (const Config *config = systemBucket_->GetConfig();
            config != nullptr && config->console.maxHistoryEntries > 0) {
            consoleContext->SaveCommandHistory();
        }
    }
    SceneManager *sceneManager = systemBucket_->GetSceneManager();
    if (sceneManager != nullptr) {
        sceneManager->ClearScenes();
    }
    if (WindowContext *windowContext = systemBucket_->GetWindowContext(); windowContext != nullptr) {
        windowContext->Exit();
    }
}

void glimmer::AppContext::CreateScreenshot(const std::function<void(const std::string &text)> *onMessage) const {
    if (onMessage == nullptr) {
        return;
    }
    const std::function<void(const std::string &text)> &onMessageRef = *onMessage;
    const WindowContext *windowContext = systemBucket_->GetWindowContext();
    if (windowContext == nullptr) {
        return;
    }
    const auto config = systemBucket_->GetConfig();
    if (config == nullptr) {
        return;
    }
    const VirtualFileSystem *virtualFileSystem = systemBucket_->GetVirtualFileSystem();
    if (virtualFileSystem == nullptr) {
        return;
    }
    const std::filesystem::path screenshotsFolder = std::filesystem::path(config->runtimePath) / "screenshots";
    if (!virtualFileSystem->Exists(screenshotsFolder) && !virtualFileSystem->CreateFolder(screenshotsFolder)) {
        onMessageRef(fmt::format(
            fmt::runtime(GetLangsResources()->screenshotSavedFailed),
            "CreateFolder failed"));
        return;
    }
    const auto actualPath = virtualFileSystem->GetActualPath(
        screenshotsFolder / StringUtils::GetScreenshotFileName());
    if (!actualPath.has_value()) {
        onMessageRef(fmt::format(
            fmt::runtime(GetLangsResources()->screenshotSavedFailed),
            "GetActualPath failed"));
        return;
    }
    pendingScreenshot_ = PendingScreenshot{actualPath.value(), onMessage};
}

glimmer::ModContext *glimmer::AppContext::GetModContext() const {
    return systemBucket_->GetModContext();
}

glimmer::ConsoleContext *glimmer::AppContext::GetConsoleContext() const {
    return systemBucket_->GetConsoleContext();
}

glimmer::GraphicsContext *glimmer::AppContext::GetGraphicsContext() const {
    return systemBucket_->GetGraphicsContext();
}

glimmer::AudioContext *glimmer::AppContext::GetAudioContext() const {
    return systemBucket_->GetAudioContext();
}

glimmer::RmlContext *glimmer::AppContext::GetRmlContext() const {
    return systemBucket_->GetRmlContext();
}

glimmer::MainThreadDispatcher *glimmer::AppContext::GetMainThreadDispatcher() const {
    return systemBucket_->GetMainThreadDispatcher();
}

glimmer::Config *glimmer::AppContext::GetConfig() const {
    return systemBucket_->GetConfig();
}

toml::value *glimmer::AppContext::GetLangsValue() const {
    return systemBucket_->GetLangsValue();
}

glimmer::LangsResources *glimmer::AppContext::GetLangsResources() const {
    return systemBucket_->GetLangsResources();
}

glimmer::ResourcePackManager *glimmer::AppContext::GetResourcePackManager() const {
    return systemBucket_->GetResourcePackManager();
}

glimmer::ResourceLocator *glimmer::AppContext::GetResourceLocator() const {
    return systemBucket_->GetResourceLocator();
}

glimmer::VirtualFileSystem *glimmer::AppContext::GetVirtualFileSystem() const {
    return systemBucket_->GetVirtualFileSystem();
}

glimmer::SceneManager *glimmer::AppContext::GetSceneManager() const {
    return systemBucket_->GetSceneManager();
}

glimmer::SavesManager *glimmer::AppContext::GetSavesManager() const {
    return systemBucket_->GetSavesManager();
}

void glimmer::AppContext::SetRandomSlogan() const {
    const WindowContext *windowContext = systemBucket_->GetWindowContext();
    if (windowContext == nullptr) {
        return;
    }
    const LangsResources *langsResources = systemBucket_->GetLangsResources();
    if (langsResources == nullptr) {
        windowContext->SetWindowTitle(PROJECT_NAME.c_str());
        return;
    }
    if (const std::vector<std::string> &slogans = langsResources->slogans; slogans.empty()) {
        windowContext->SetWindowTitle(PROJECT_NAME.c_str());
    } else {
        const int idx = RandomUtils::Random(0, static_cast<int>(slogans.size()) - 1);
        const std::string &random_str = slogans[idx];
        windowContext->SetWindowTitle(random_str.c_str());
    }
}

glimmer::CacheContext *glimmer::AppContext::GetCacheContext() const {
    return systemBucket_->GetCacheContext();
}

const std::string &glimmer::AppContext::GetLanguage() const {
    return systemBucket_->GetLanguage();
}

void glimmer::AppContext::AddUIMessage(const std::string &text) {
    UIMessage message;
    message.message = text;
    message.expireTime = SDL_GetTicks() + UI_MESSAGE_DURATION_MS;
    uiMessages_.emplace_back(std::move(message));
}

std::vector<glimmer::UIMessage> &glimmer::AppContext::GetUIMessages() {
    return uiMessages_;
}
