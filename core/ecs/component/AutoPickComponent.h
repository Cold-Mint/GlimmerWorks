//
// Created by coldmint on 2025/12/22.
//

#ifndef GLIMMERWORKS_AUTOPICKCOMPONENT_H
#define GLIMMERWORKS_AUTOPICKCOMPONENT_H
#include "../GameComponent.h"

namespace glimmer {
    class AutoPickComponent : public GameComponent {
    public:
        [[nodiscard]] u_int32_t GetId() override;

        [[nodiscard]] bool IsSerializable() override;
    };
}

#endif //GLIMMERWORKS_AUTOPICKCOMPONENT_H
