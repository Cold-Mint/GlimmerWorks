//
// Created by Cold-Mint on 2025/12/22.
//

#ifndef GLIMMERWORKS_MAGNETICCOMPONENT_H
#define GLIMMERWORKS_MAGNETICCOMPONENT_H
#include <cstdint>

#include "../GameComponent.h"
#include "../../math/Vector2D.h"

namespace glimmer {
    /**
    * Magnetic(The party being sucked)
    * 磁吸物（被吸的一方）
    */
    class MagneticComponent : public GameComponent {
        /**
         * The type of the aspirated substance
         * 被吸物的类型
         */
        uint16_t type_ = 0;

        WorldVector2D startPos_;

    public:
        void SetType(uint16_t type);

        [[nodiscard]] const WorldVector2D &GetStartPos() const;

        void SetStartPos(const WorldVector2D &startPos);

        [[nodiscard]] uint16_t GetType() const;

        [[nodiscard]] uint32_t GetId() override;

        [[nodiscard]] bool IsSerializable() override;
    };
}

#endif //GLIMMERWORKS_MAGNETICCOMPONENT_H
