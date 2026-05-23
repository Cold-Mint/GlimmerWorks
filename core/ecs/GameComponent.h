//
// Created by Cold-Mint on 2025/10/26.
//

#pragma once
#include <string>
#include "src/core/game_component_type.pb.h"

namespace glimmer {
    class AppContext;
    class WorldContext;

    class GameComponent {
    public:
        virtual ~GameComponent() = default;

        /**
         * Does the component support serialization?
         * 组件是否支持序列化。
         * @return
         */
        [[nodiscard]] virtual bool IsSerializable();

        /**
         * serialize
         * 系列化组件
         * @return data 数据
         */
        [[nodiscard]] virtual std::string Serialize();

        /**
         * deserialize
         * 反系列化
         * @param worldContext worldContext 世界上下文
         * @param data data 数据
         */
        virtual void Deserialize(WorldContext *worldContext, const std::string &data);

        [[nodiscard]] virtual GameComponentTypeMessage GetComponentType() = 0;
    };
}
