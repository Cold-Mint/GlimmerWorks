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
#include <string>
#include <vector>

#include "core/LangsResources.h"
#include "core/Config.h"
#include "core/GameUIMessage.h"
#include "core/saves/SavesManager.h"
#include "core/mod/ResourceLocator.h"
#include "core/mod/resourcePack/ResourcePackManager.h"
#include "core/vfs/VirtualFileSystem.h"
#include "SceneManager.h"
#include "ModContext.h"
#include "ConsoleContext.h"
#include "GraphicsContext.h"
#include "AudioContext.h"
#include "MainThreadDispatcher.h"
#include "WindowContext.h"

namespace glimmer
{
    class AppContext
    {
        std::unique_ptr<WindowContext> windowContext_;
        std::string language_;
        std::unique_ptr<Config> config_;
        std::unique_ptr<toml::value> configValue;
        std::unique_ptr<SceneManager> sceneManager_;
        std::unique_ptr<VirtualFileSystem> virtualFileSystem_;
        std::unique_ptr<ResourcePackManager> resourcePackManager_;
        std::unique_ptr<ResourceLocator> resourceLocator_;
        std::unique_ptr<LangsResources> langsResources_;
        std::unique_ptr<SavesManager> savesManager_;
        std::unique_ptr<ModContext> modContext_;
        std::unique_ptr<ConsoleContext> consoleContext_;
        std::unique_ptr<GraphicsContext> graphicsContext_;
        std::unique_ptr<AudioContext> audioContext_;
        std::unique_ptr<MainThreadDispatcher> mainThreadDispatcher_;
        std::vector<GameUIMessage> gameUIMessages_;
        toml::spec tomlVersion_ = toml::spec::v(1, 1, 0);
        bool initSuccess_ = false;

        void LoadLanguage(const std::string& data) const;

        static std::string GetTimeFileName(const std::string& prefix = "screenshot", const std::string& ext = ".png");

    public:
        AppContext();

        ~AppContext();

        [[nodiscard]] bool InitSuccess() const;

        [[nodiscard]] WindowContext* GetWindowContext() const;

        [[nodiscard]] const toml::spec& GetTomlVersion() const;

        void LoadMainMenuBGM() const;

        void PlayMainMenuBGM() const;

        void SetRandomSlogan() const;

        static void RestoreColorRenderer(SDL_Renderer* sdlRenderer);

        [[nodiscard]] ModContext* GetModContext() const;

        [[nodiscard]] ConsoleContext* GetConsoleContext() const;

        [[nodiscard]] GraphicsContext* GetGraphicsContext() const;

        [[nodiscard]] AudioContext* GetAudioContext() const;

        [[nodiscard]] MainThreadDispatcher* GetMainThreadDispatcher() const;

        [[nodiscard]] Config* GetConfig() const;

        [[nodiscard]] LangsResources* GetLangsResources() const;

        [[nodiscard]] ResourcePackManager* GetResourcePackManager() const;

        [[nodiscard]] ResourceLocator* GetResourceLocator() const;

        [[nodiscard]] VirtualFileSystem* GetVirtualFileSystem() const;

        [[nodiscard]] SceneManager* GetSceneManager() const;

        [[nodiscard]] SavesManager* GetSavesManager() const;

        [[nodiscard]] const std::string& GetLanguage() const;

        void AddUIMessage(const std::string& string);

        std::vector<GameUIMessage>& GetGameUIMessages();

        void ExitApp() const;

        void CreateScreenshot(const std::function<void(const std::string& text)>* onMessage) const;
    };
}
