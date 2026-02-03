//
// Created by Cold-Mint on 2025/10/26.
//

#ifndef GLIMMERWORKS_COMPONENT_H
#define GLIMMERWORKS_COMPONENT_H
#include <string>


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

        [[nodiscard]] virtual u_int32_t GetId();
    };
}

#endif //GLIMMERWORKS_COMPONENT_H
