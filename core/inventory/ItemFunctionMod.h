//
// Created by coldmint on 2025/12/23.
//

#ifndef GLIMMERWORKS_ITEMFUNCTIONMOD_H
#define GLIMMERWORKS_ITEMFUNCTIONMOD_H
#include <string>

namespace glimmer {
    class AppContext;
    class WorldContext;
    class GameEntity;

    /**
     * Item Function module
     * 物品的功能模块
     */
    class ItemFunctionMod {
    protected:
        std::string name_;
        std::string description_;

    public:
        virtual ~ItemFunctionMod() = default;

        [[nodiscard]] virtual std::string GetId() const = 0;

        [[nodiscard]] std::string GetName();

        [[nodiscard]] std::string GetDescription();

        virtual void OnUse(AppContext *appContext, WorldContext *worldContext, GameEntity *user) = 0;
    };
}

#endif //GLIMMERWORKS_ITEMFUNCTIONMOD_H
