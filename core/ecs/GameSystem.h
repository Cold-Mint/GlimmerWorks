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
#include <unordered_set>

#include "EntityManager.h"
#include "EntityShortCut.h"
#include "GameSystemType.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_render.h"
#include "src/core/game_component_type.pb.h"

namespace glimmer
{
    class Scene;
    class WorldContext;
    class Config;

    class GameSystem
    {
        std::unordered_set<GameComponentTypeMessage> watchComponents_;
        std::unordered_set<GameComponentTypeMessage> activeWatchComponents_;

        bool lockWatchComponents_ = false;
        bool initSubclassFinish_ = false;
        WorldContext* worldContext_ = nullptr;
        EntityManager* entityManager_ = nullptr;
        EntityShortCut* entityShortCut_ = nullptr;
#if  !defined(NDEBUG)
        float initTimeOut_ = 0.0F;
#endif

    protected:
        void WatchComponent(GameComponentTypeMessage gameComponentType);

        [[nodiscard]] WorldContext* GetWorldContext() const;

        [[nodiscard]] EntityManager* GetEntityManager() const;

        [[nodiscard]] EntityShortCut* GetEntityShortCut() const;

        /**
        * Initialize the system after construction.
        * 在构造完成后初始化系统。
        */
        void Init();

    public:
        virtual ~GameSystem() = default;

        explicit GameSystem(WorldContext* worldContext);


        /**
        * The component for locking observation
        * 锁定观察的组件
        */
        void LockWatchComponent();

        [[nodiscard]] virtual GameSystemType GetGameSystemType() const = 0;

        virtual void OnActivationChanged(bool activeStatus);

        void AddActiveWatchComponent(GameComponentTypeMessage gameComponentType);

        void RemoveActiveWatchComponent(GameComponentTypeMessage gameComponentType);

        /**
         * Have all the observing components been activated?
         * 所有的观察的组件是否已激活。
         * @return
         */
        bool IsAllWatchComponentsReady() const;

        /**
         * Does the current system meet the activation conditions?
         * 当前系统是否满足激活条件？
         * @return
         */
        virtual bool CanActive() const;

        /**
        * When the number of observed components changes
        * 当观察的组件数量改变时
        * @param gameComponentType
        * @param count
        */
        virtual void OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count);

        [[nodiscard]] bool IsWatchingComponent(GameComponentTypeMessage gameComponentType) const;

        virtual bool HandleEvent(const SDL_Event& event);

        virtual void OnConfigChanged(const Config* config);

        virtual void OnWindowSizeChanged(const int& width, const int& height);

        virtual bool OnBackPressed();

        virtual void OnFrameStart();

        virtual void Update(float delta);

        /**
       * 获取系统执行调度序号
       * System execution scheduling order
       * 决定各个GameSystem的调度先后次序，数值越小，越早执行。
       * Affects system scheduling sequence. Smaller value = earlier execution.
       * @return 调度序号 uint8_t
       */
        virtual uint8_t GetExecutionOrder();

        virtual void Render(SDL_Renderer* renderer);
    };
}
