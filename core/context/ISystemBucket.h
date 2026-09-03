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
#include <memory>

#include "toml11/types.hpp"

namespace toml {
    struct spec;
}

namespace glimmer {
    class AudioContext;
    class MainThreadDispatcher;
    class GraphicsContext;
    class ConsoleContext;
    class RmlContext;
    class ModContext;
    class SavesManager;
    struct LangsResources;
    class ResourceLocator;
    class ResourcePackManager;
    class VirtualFileSystem;
    class SceneManager;
    class Config;
    class CacheContext;
    class WindowContext;

    //This class is used to address circular dependencies.
    //这个类用于解决循环依赖。
    class ISystemBucket {
    public:
        virtual ~ISystemBucket() = default;

        virtual void SetWindowContext(std::unique_ptr<WindowContext> windowContext) = 0;

        virtual void SetCacheContext(std::unique_ptr<CacheContext> cacheContext) = 0;

        [[nodiscard]] virtual CacheContext *GetCacheContext() const = 0;

        [[nodiscard]] virtual WindowContext *GetWindowContext() const = 0;

        virtual void SetConfig(std::unique_ptr<Config> config) = 0;

        [[nodiscard]] virtual Config *GetConfig() const = 0;

        virtual void SetSceneManager(std::unique_ptr<SceneManager> sceneManager) = 0;

        [[nodiscard]] virtual SceneManager *GetSceneManager() const = 0;

        virtual void SetVirtualFileSystem(std::unique_ptr<VirtualFileSystem> virtualFileSystem) = 0;

        [[nodiscard]] virtual VirtualFileSystem *GetVirtualFileSystem() const = 0;

        virtual void SetResourcePackManager(std::unique_ptr<ResourcePackManager> resourcePackManager) = 0;

        [[nodiscard]] virtual ResourcePackManager *GetResourcePackManager() const = 0;

        virtual void SetResourceLocator(std::unique_ptr<ResourceLocator> resourceLocator) = 0;

        [[nodiscard]] virtual ResourceLocator *GetResourceLocator() const = 0;

        virtual void SetLangsResources(std::unique_ptr<LangsResources> langsResources) = 0;

        [[nodiscard]] virtual LangsResources *GetLangsResources() const = 0;

        virtual void SetSavesManager(std::unique_ptr<SavesManager> savesManager) = 0;

        [[nodiscard]] virtual SavesManager *GetSavesManager() const = 0;

        virtual void SetModContext(std::unique_ptr<ModContext> modContext) = 0;

        [[nodiscard]] virtual ModContext *GetModContext() const = 0;

        void virtual SetRmlContext(std::unique_ptr<RmlContext> rmlContext) = 0;

        [[nodiscard]] virtual RmlContext *GetRmlContext() const = 0;

        virtual void SetConsoleContext(std::unique_ptr<ConsoleContext> consoleContext) = 0;

        [[nodiscard]] virtual ConsoleContext *GetConsoleContext() const = 0;

        virtual void SetGraphicsContext(std::unique_ptr<GraphicsContext> graphicsContext) = 0;

        [[nodiscard]] virtual GraphicsContext *GetGraphicsContext() const = 0;

        virtual void SetMainThreadDispatcher(std::unique_ptr<MainThreadDispatcher> mainThreadDispatcher) = 0;

        [[nodiscard]] virtual MainThreadDispatcher *GetMainThreadDispatcher() const = 0;

        virtual void SetLangsValue(std::unique_ptr<toml::value> langsValue) = 0;

        virtual void SetAudioContext(std::unique_ptr<AudioContext> audioContext) = 0;

        [[nodiscard]] virtual AudioContext *GetAudioContext() const = 0;

        [[nodiscard]] virtual toml::value *GetLangsValue() const = 0;

        virtual void SetLanguage(const std::string &language) = 0;

        [[nodiscard]] virtual const std::string &GetLanguage() const = 0;
    };
}
