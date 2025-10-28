//
// Created by Cold-Mint on 2025/10/26.
//

#ifndef GLIMMERWORKS_GAMESYSTEM_H
#define GLIMMERWORKS_GAMESYSTEM_H

#include <typeindex>
#include <unordered_set>

#include "../world/WorldContext.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_render.h"

namespace glimmer {
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

        template<typename TComponent>
        void RequireComponent() {
            requiredComponents.insert(std::type_index(typeid(TComponent)));
        }


        virtual void OnActivationChanged(bool activeStatus);

        WorldContext *worldContext;

    public:
        virtual ~GameSystem() = default;

        explicit GameSystem(WorldContext *worldContext) : worldContext(worldContext) {
        }

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
            return requiredComponents.contains(type);
        }

        virtual bool HandleEvent(const SDL_Event &event);

        virtual void Update(float delta);

        virtual void Render(SDL_Renderer *renderer);
    };
}

#endif //GLIMMERWORKS_GAMESYSTEM_H
