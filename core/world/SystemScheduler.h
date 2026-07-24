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

#include <cstdint>
#include <memory>
#include <queue>
#include <stack>
#include <unordered_map>
#include <vector>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>

#include "core/ecs/GameSystem.h"
#include "core/ecs/GuiGameSystem.h"
#include "core/scene/WorldScene.h"
#include "src/core/game_component_type.pb.h"

namespace glimmer {
    class WorldContext;
    class Config;

    /**
     * SystemScheduler
     * 系统调度器，负责游戏系统的注册、激活/停用、事件分发、渲染调度和GUI栈管理。
     * 从 WorldContext 拆分而来。
     */
    class SystemScheduler {
        std::vector<std::unique_ptr<GameSystem> > activeSystems_;
        std::vector<std::unique_ptr<GameSystem> > inactiveSystems_;
        std::vector<GuiGameSystem *> guiGameSystems_;
        std::stack<GameSystemType> activeSystemStack_;
        uint64_t persistentGuiSystemCount_ = 0;
        bool allowRegisterSystem_ = false;
        uint32_t onComponentCountChangedId_ = 0;
        std::unordered_map<GameComponentTypeMessage, uint32_t> onComponentCountChangeBuffer_;
        WorldContext *worldContext_ = nullptr;

        void RegisterSystem(std::unique_ptr<GameSystem> system);

        void RegisterGuiSystem(std::unique_ptr<GuiGameSystem> system);

        void MoveSystemsToActive(std::queue<GameSystem *> &toActivate);

        void MoveSystemsToInactive(std::queue<GameSystem *> &toDeactivate);

        void OnWatchedComponentChanged(GameComponentTypeMessage type, uint32_t count);

        void NotifySystemsOfComponentChange(GameComponentTypeMessage gameComponentType, uint32_t count) const;

        void NotifyActiveSystems(GameComponentTypeMessage gameComponentType, uint32_t count) const;

        void NotifyInactiveSystems(GameComponentTypeMessage gameComponentType, uint32_t count) const;

    public:
        explicit SystemScheduler(WorldContext *worldContext);

        ~SystemScheduler();

        /**
       * PushGuiSystemType
       * 推送系统
       * @param systemType
       */
        void PushGuiSystemType(GameSystemType systemType);

        /**
         * PushPersistentGuiSystem
         * 推送不可关闭的系统
         * @param systemType
         */
        void PushPersistentGuiSystem(GameSystemType systemType);

        void PopGuiSystemType();

        [[nodiscard]] GameSystemType GetTopGuiSystemType() const;

        [[nodiscard]] std::vector<GameSystemType> GetAllActiveSystemType() const;

        /**
         * Is there a GUI system that can be closed currently being displayed?
         * 是否有可关闭的GUI系统正在显示中。
         * @return
         */
        [[nodiscard]] bool HasAnyModalGuiOpen() const;

        bool HandleEvent(const SDL_Event &event) const;

        void Update(float delta) const;

        bool OnBackPressed();

        void Render(SDL_Renderer *renderer) const;

        void LoadDocuments(IDocumentRegistry *documentRegistry) const;

        void OnCreateDataModels(IDocumentRegistry *documentRegistry) const;

        void OnFrameStart();

        void InitSystem();

        void OnConfigChanged(const Config *config) const;

        void OnWindowSizeChanged(const int &width, const int &height) const;
    };
}
