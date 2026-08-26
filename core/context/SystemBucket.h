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

#include "AudioContext.h"
#include "ConsoleContext.h"
#include "GraphicsContext.h"
#include "ModContext.h"
#include "WindowContext.h"
#include "core/config/Config.h"
#include "core/saves/SavesManager.h"
#include "core/scene/MainThreadDispatcher.h"
#include "core/scene/SceneManager.h"
#include "core/ui/GameUIMessage.h"
#include "core/utils/LanguageUtils.h"

namespace glimmer {
    class SystemBucket {
        std::string language_ = LanguageUtils::getLanguage();
        toml::spec tomlVersion_ = toml::spec::v(1, 1, 0);
        std::unique_ptr<WindowContext> windowContext_ = nullptr;
        std::unique_ptr<Config> config_ = nullptr;
        std::unique_ptr<SceneManager> sceneManager_ = nullptr;
        std::unique_ptr<VirtualFileSystem> virtualFileSystem_ = nullptr;
        std::unique_ptr<ResourcePackManager> resourcePackManager_ = nullptr;
        std::unique_ptr<ResourceLocator> resourceLocator_ = nullptr;
        std::unique_ptr<LangsResources> langsResources_ = nullptr;
        std::unique_ptr<SavesManager> savesManager_ = nullptr;
        std::unique_ptr<ModContext> modContext_ = nullptr;
        std::unique_ptr<RmlContext> rmlContext_ = nullptr;
        std::unique_ptr<ConsoleContext> consoleContext_ = nullptr;
        std::unique_ptr<GraphicsContext> graphicsContext_ = nullptr;
        std::unique_ptr<AudioContext> audioContext_ = nullptr;
        std::unique_ptr<MainThreadDispatcher> mainThreadDispatcher_ = nullptr;
        std::unique_ptr<toml::value> langsValue_ = nullptr;

    public:
        void SetWindowContext(std::unique_ptr<WindowContext> windowContext);

        [[nodiscard]] WindowContext *GetWindowContext() const;

        void SetConfig(std::unique_ptr<Config> config);

        [[nodiscard]] Config *GetConfig() const;

        void SetSceneManager(std::unique_ptr<SceneManager> sceneManager);

        [[nodiscard]] SceneManager *GetSceneManager() const;

        void SetVirtualFileSystem(std::unique_ptr<VirtualFileSystem> virtualFileSystem);

        [[nodiscard]] VirtualFileSystem *GetVirtualFileSystem() const;

        void SetResourcePackManager(std::unique_ptr<ResourcePackManager> resourcePackManager);

        [[nodiscard]] ResourcePackManager *GetResourcePackManager() const;

        void SetResourceLocator(std::unique_ptr<ResourceLocator> resourceLocator);

        [[nodiscard]] ResourceLocator *GetResourceLocator() const;

        void SetLangsResources(std::unique_ptr<LangsResources> langsResources);

        [[nodiscard]] LangsResources *GetLangsResources() const;

        void SetSavesManager(std::unique_ptr<SavesManager> savesManager);

        [[nodiscard]] SavesManager *GetSavesManager() const;

        void SetModContext(std::unique_ptr<ModContext> modContext);

        [[nodiscard]] ModContext *GetModContext() const;

        void SetRmlContext(std::unique_ptr<RmlContext> rmlContext);

        [[nodiscard]] RmlContext *GetRmlContext() const;

        void SetConsoleContext(std::unique_ptr<ConsoleContext> consoleContext);

        [[nodiscard]] ConsoleContext *GetConsoleContext() const;

        void SetGraphicsContext(std::unique_ptr<GraphicsContext> graphicsContext);

        [[nodiscard]] GraphicsContext *GetGraphicsContext() const;

        void SetMainThreadDispatcher(std::unique_ptr<MainThreadDispatcher> mainThreadDispatcher);

        [[nodiscard]] MainThreadDispatcher *GetMainThreadDispatcher() const;

        void SetLangsValue(std::unique_ptr<toml::value> langsValue);

        void SetAudioContext(std::unique_ptr<AudioContext> audioContext);

        [[nodiscard]] AudioContext *GetAudioContext() const;

        [[nodiscard]] toml::value *GetLangsValue() const;

        [[nodiscard]] const toml::spec *GetTomlVersion() const;

        void SetLanguage(const std::string &language);

        [[nodiscard]] const std::string &GetLanguage() const;
    };
}
