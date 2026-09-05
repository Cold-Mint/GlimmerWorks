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
#include <optional>
#include <string>
#include <vector>

#include "core/utils/LangsResources.h"
#include "core/config/Config.h"
#include "core/saves/SavesManager.h"
#include "core/ui/UIMessage.h"
#include "core/vfs/VirtualFileSystem.h"
#include "ConsoleContext.h"
#include "GraphicsContext.h"
#include "AudioContext.h"
#include "RmlContext.h"
#include "WindowContext.h"
#include "core/scene/MainThreadDispatcher.h"
#include "core/scene/SceneManager.h"
#include "tasks/IAppContextInitTask.h"
#include "core/context/ISystemBucket.h"

namespace glimmer {
    class AppContext {
        /**
         * Pending screenshot request, captured at the end of the next rendered frame.
         * 待处理的截图请求，在下一帧渲染结束时捕获。
         */
        struct PendingScreenshot {
            std::filesystem::path path;
            const std::function<void(const std::string &text)> *onMessage = nullptr;
        };

        mutable std::optional<PendingScreenshot> pendingScreenshot_;
        std::unique_ptr<ISystemBucket> systemBucket_;
        std::vector<std::unique_ptr<IAppContextInitTask> > initTasks_;
        std::vector<UIMessage> uiMessages_;

        void RegisterInitTask(std::unique_ptr<IAppContextInitTask> initTask);

    public:
        AppContext();

        bool InitSystem();

        [[nodiscard]] WindowContext *GetWindowContext() const;

        void SetRandomSlogan() const;


        [[nodiscard]] CacheContext *GetCacheContext() const;

        [[nodiscard]] ModContext *GetModContext() const;

        [[nodiscard]] ConsoleContext *GetConsoleContext() const;

        [[nodiscard]] GraphicsContext *GetGraphicsContext() const;

        [[nodiscard]] AudioContext *GetAudioContext() const;

        [[nodiscard]] RmlContext *GetRmlContext() const;

        [[nodiscard]] MainThreadDispatcher *GetMainThreadDispatcher() const;

        [[nodiscard]] Config *GetConfig() const;

        [[nodiscard]] toml::value *GetLangsValue() const;

        [[nodiscard]] LangsResources *GetLangsResources() const;

        [[nodiscard]] ResourcePackManager *GetResourcePackManager() const;

        [[nodiscard]] ResourceLocator *GetResourceLocator() const;

        [[nodiscard]] VirtualFileSystem *GetVirtualFileSystem() const;

        [[nodiscard]] SceneManager *GetSceneManager() const;

        [[nodiscard]] SavesManager *GetSavesManager() const;

        [[nodiscard]] const std::string &GetLanguage() const;

        /**
         * Add a transient UI message that is displayed via the RmlUi data
         * binding (see UIMessageOverlay).
         * 添加一条通过 RmlUi 数据绑定显示的短暂 UI 消息（见 UIMessageOverlay）。
         * @param text text 消息文本
         */
        void AddUIMessage(const std::string &text);

        /**
         * @return The pending UI messages. 待显示的 UI 消息列表。
         */
        [[nodiscard]] std::vector<UIMessage> &GetUIMessages();

        void ExitApp() const;

        void CreateScreenshot(const std::function<void(const std::string &text)> *onMessage) const;

    };
}

#include "core/mod/ResourceLocator.h"
#include "core/mod/resourcePack/ResourcePackManager.h"
#include "ModContext.h"
#include "core/mod/BasePackManager.inl"
