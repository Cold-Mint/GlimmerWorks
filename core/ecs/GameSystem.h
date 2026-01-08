//
// Created by Cold-Mint on 2025/10/26.
//

#ifndef GLIMMERWORKS_GAMESYSTEM_H
#define GLIMMERWORKS_GAMESYSTEM_H

#include <typeindex>
#include <unordered_set>

#include "../scene/AppContext.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_render.h"

namespace glimmer {
    class WorldContext;

    class GameSystem {
        /**
         * Is the system in an activated state
         * 系统是否处于激活状态
         */
        bool active = false;

        /**
         * Is it necessary to activate the system
         * 是否需要激活系统
         * @return
         */
        [[nodiscard]] virtual bool ShouldActivate();

        std::unordered_set<std::type_index> requiredComponents;


        virtual void OnActivationChanged(bool activeStatus);

    protected:
        template<typename TComponent>
        void RequireComponent() {
            requiredComponents.insert(std::type_index(typeid(TComponent)));
        }

        WorldContext *worldContext_;
        AppContext *appContext_;

    public:
        virtual ~GameSystem() = default;

        explicit GameSystem(AppContext *appContext, WorldContext *worldContext) : appContext_(appContext),
            worldContext_(worldContext) {
        }

        virtual std::string GetName() = 0;

        /**
         * Check if activation is needed. (Detection per frame
         * 检查是否需要激活。（每帧检测）
         * @return IsActive 激活状态
         */
        bool CheckActivation();

        /**
         * Is the system in an activated state
         * 系统是否为激活状态
         * @return
         */
        [[nodiscard]] bool IsActive() const;

        bool SupportsComponentType(const std::type_index &type) const {
            if (requiredComponents.empty()) {
                return false;
            }
            return requiredComponents.contains(type);
        }


        /**
         * Determine whether the system can continue to run when the game is paused
         * 判断该系统是否能在游戏暂停时继续运行
         * By default, it is false (stops running when paused), and subclasses can override this method to customize its behavior
         * 默认为false（暂停时停止运行），子类可重写此方法自定义行为
         * @return true= Continues running when paused, false= stops running when paused true=暂停时继续运行，false=暂停时停止运行
         */
        [[nodiscard]] virtual bool CanRunWhilePaused() const {
            return false;
        }

        virtual bool HandleEvent(const SDL_Event &event);

        virtual bool OnBackPressed();

        virtual void Update(float delta);

        virtual uint8_t GetRenderOrder();

        virtual void Render(SDL_Renderer *renderer);
    };
}

#endif //GLIMMERWORKS_GAMESYSTEM_H
