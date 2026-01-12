//
// Created by Cold-Mint on 2025/10/26.
//

#ifndef GLIMMERWORKS_COMPONENT_H
#define GLIMMERWORKS_COMPONENT_H
#include <string>

namespace glimmer {
    class GameComponent {
    public:
        virtual ~GameComponent() = default;

        /**
         * Does the component support serialization?
         * 组件是否支持序列化。
         * @return
         */
        [[nodiscard]] virtual bool isSerializable();

        /**
         * serialize
         * 系列化组件
         * @return data 数据
         */
        [[nodiscard]] virtual std::string serialize();

        /**
         * deserialize
         * 反系列化
         * @param data data 数据
         */
        virtual void deserialize(std::string &data);
    };
}

#endif //GLIMMERWORKS_COMPONENT_H
