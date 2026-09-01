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
#include "SystemBucket.h"

glimmer::SystemBucket::~SystemBucket() = default;

void glimmer::SystemBucket::SetWindowContext(std::unique_ptr<WindowContext> windowContext) {
    windowContext_ = std::move(windowContext);
}

void glimmer::SystemBucket::SetCacheContext(std::unique_ptr<CacheContext> cacheContext) {
    cacheContext_ = std::move(cacheContext);
}

glimmer::CacheContext *glimmer::SystemBucket::GetCacheContext() const {
    CacheContext *cacheContext = cacheContext_.get();
    if (cacheContext == nullptr) {
        return nullptr;
    }
    return cacheContext;
}

glimmer::WindowContext *glimmer::SystemBucket::GetWindowContext() const {
    WindowContext *windowContext = windowContext_.get();
    if (windowContext == nullptr) {
        return nullptr;
    }
    return windowContext;
}

void glimmer::SystemBucket::SetConfig(std::unique_ptr<Config> config) {
    config_ = std::move(config);
}

glimmer::Config *glimmer::SystemBucket::GetConfig() const {
    Config *config = config_.get();
    if (config == nullptr) {
        return nullptr;
    }
    return config;
}

void glimmer::SystemBucket::SetSceneManager(std::unique_ptr<SceneManager> sceneManager) {
    sceneManager_ = std::move(sceneManager);
}

glimmer::SceneManager *glimmer::SystemBucket::GetSceneManager() const {
    SceneManager *sceneManager = sceneManager_.get();
    if (sceneManager == nullptr) {
        return nullptr;
    }
    return sceneManager;
}

void glimmer::SystemBucket::SetVirtualFileSystem(std::unique_ptr<VirtualFileSystem> virtualFileSystem) {
    virtualFileSystem_ = std::move(virtualFileSystem);
}

glimmer::VirtualFileSystem *glimmer::SystemBucket::GetVirtualFileSystem() const {
    VirtualFileSystem *virtualFileSystem = virtualFileSystem_.get();
    if (virtualFileSystem == nullptr) {
        return nullptr;
    }
    return virtualFileSystem;
}

void glimmer::SystemBucket::SetResourcePackManager(std::unique_ptr<ResourcePackManager> resourcePackManager) {
    resourcePackManager_ = std::move(resourcePackManager);
}

glimmer::ResourcePackManager *glimmer::SystemBucket::GetResourcePackManager() const {
    ResourcePackManager *resourcePackManager = resourcePackManager_.get();
    if (resourcePackManager == nullptr) {
        return nullptr;
    }
    return resourcePackManager;
}

void glimmer::SystemBucket::SetResourceLocator(std::unique_ptr<ResourceLocator> resourceLocator) {
    resourceLocator_ = std::move(resourceLocator);
}

glimmer::ResourceLocator *glimmer::SystemBucket::GetResourceLocator() const {
    ResourceLocator *resourceLocator = resourceLocator_.get();
    if (resourceLocator == nullptr) {
        return nullptr;
    }
    return resourceLocator;
}

void glimmer::SystemBucket::SetLangsResources(std::unique_ptr<LangsResources> langsResources) {
    langsResources_ = std::move(langsResources);
}

glimmer::LangsResources *glimmer::SystemBucket::GetLangsResources() const {
    LangsResources *langsResources = langsResources_.get();
    if (langsResources == nullptr) {
        return nullptr;
    }
    return langsResources;
}

void glimmer::SystemBucket::SetSavesManager(std::unique_ptr<SavesManager> savesManager) {
    savesManager_ = std::move(savesManager);
}

glimmer::SavesManager *glimmer::SystemBucket::GetSavesManager() const {
    SavesManager *savesManager = savesManager_.get();
    if (savesManager == nullptr) {
        return nullptr;
    }
    return savesManager;
}

void glimmer::SystemBucket::SetModContext(std::unique_ptr<ModContext> modContext) {
    modContext_ = std::move(modContext);
}

glimmer::ModContext *glimmer::SystemBucket::GetModContext() const {
    ModContext *modContext = modContext_.get();
    if (modContext == nullptr) {
        return nullptr;
    }
    return modContext;
}

void glimmer::SystemBucket::SetRmlContext(std::unique_ptr<RmlContext> rmlContext) {
    rmlContext_ = std::move(rmlContext);
}

glimmer::RmlContext *glimmer::SystemBucket::GetRmlContext() const {
    RmlContext *rmlContext = rmlContext_.get();
    if (rmlContext == nullptr) {
        return nullptr;
    }
    return rmlContext;
}

void glimmer::SystemBucket::SetConsoleContext(std::unique_ptr<ConsoleContext> consoleContext) {
    consoleContext_ = std::move(consoleContext);
}

glimmer::ConsoleContext *glimmer::SystemBucket::GetConsoleContext() const {
    ConsoleContext *consoleContext = consoleContext_.get();
    if (consoleContext == nullptr) {
        return nullptr;
    }
    return consoleContext;
}

void glimmer::SystemBucket::SetGraphicsContext(std::unique_ptr<GraphicsContext> graphicsContext) {
    graphicsContext_ = std::move(graphicsContext);
}

glimmer::GraphicsContext *glimmer::SystemBucket::GetGraphicsContext() const {
    GraphicsContext *graphicsContext = graphicsContext_.get();
    if (graphicsContext == nullptr) {
        return nullptr;
    }
    return graphicsContext;
}

void glimmer::SystemBucket::SetMainThreadDispatcher(std::unique_ptr<MainThreadDispatcher> mainThreadDispatcher) {
    mainThreadDispatcher_ = std::move(mainThreadDispatcher);
}

glimmer::MainThreadDispatcher *glimmer::SystemBucket::GetMainThreadDispatcher() const {
    MainThreadDispatcher *mainThreadDispatcher = mainThreadDispatcher_.get();
    if (mainThreadDispatcher == nullptr) {
        return nullptr;
    }
    return mainThreadDispatcher;
}

void glimmer::SystemBucket::SetLangsValue(std::unique_ptr<toml::value> langsValue) {
    langsValue_ = std::move(langsValue);
}

void glimmer::SystemBucket::SetAudioContext(std::unique_ptr<AudioContext> audioContext) {
    audioContext_ = std::move(audioContext);
}

glimmer::AudioContext *glimmer::SystemBucket::GetAudioContext() const {
    AudioContext *audioContext = audioContext_.get();
    if (audioContext == nullptr) {
        return nullptr;
    }
    return audioContext;
}

toml::value *glimmer::SystemBucket::GetLangsValue() const {
    toml::value *langsValue = langsValue_.get();
    if (langsValue == nullptr) {
        return nullptr;
    }
    return langsValue;
}

const toml::spec *glimmer::SystemBucket::GetTomlVersion() const {
    return &tomlVersion_;
}

void glimmer::SystemBucket::SetLanguage(const std::string &language) {
    language_ = language;
}

const std::string &glimmer::SystemBucket::GetLanguage() const {
    return language_;
}
