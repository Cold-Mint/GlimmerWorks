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
#include "core/ui/GameUIMessage.h"
#include "core/saves/SavesManager.h"
#include "core/mod/ResourceLocator.h"
#include "core/mod/resourcePack/ResourcePackManager.h"
#include "core/vfs/VirtualFileSystem.h"
#include "ModContext.h"
#include "ConsoleContext.h"
#include "GraphicsContext.h"
#include "AudioContext.h"
#include "RmlContext.h"
#include "SystemBucket.h"
#include "WindowContext.h"
#include "core/scene/MainThreadDispatcher.h"
#include "core/scene/SceneManager.h"
#include "tasks/IAppContextInitTask.h"

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

        std::vector<GameUIMessage> gameUIMessages_;
        mutable std::optional<PendingScreenshot> pendingScreenshot_;
        SystemBucket systemBucket_;
        std::vector<std::unique_ptr<IAppContextInitTask> > initTasks_;

        void RegisterInitTask(std::unique_ptr<IAppContextInitTask> initTask);

    public:
        AppContext();

        bool InitSystem();

        [[nodiscard]] WindowContext *GetWindowContext() const;

        [[nodiscard]] const toml::spec *GetTomlVersion() const;

        void SetRandomSlogan() const;

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

        void AddUIMessage(const std::string &string);

        std::vector<GameUIMessage> &GetGameUIMessages();

        void ExitApp() const;

        void CreateScreenshot(const std::function<void(const std::string &text)> *onMessage) const;

        /**
         * Capture the frame that was just rendered when a screenshot request is
         * pending. Must be called on the main thread after all rendering
         * (including RmlUi) is done and before the frame is submitted; when it
         * returns true the frame has already been submitted and the caller must
         * not submit it again.
         * 当存在待处理截图请求时，捕获刚渲染完成的帧。必须在主线程上、
         * 所有渲染（包括 RmlUi）完成之后、帧提交之前调用；返回 true 表示
         * 帧已被提交，调用方不得再次提交。
         * @param gpuContext gpuContext GPU 上下文
         * @param renderer renderer GPU 渲染器
         * @return true if a screenshot was captured (and the frame submitted).
         * 捕获了截图（并提交了帧）时返回 true。
         */
        bool ProcessPendingScreenshot(GpuContext *gpuContext, GpuRenderer *renderer);
    };
}
