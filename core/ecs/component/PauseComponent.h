//
// Created by Cold-Mint on 2026/1/8.
//

#ifndef GLIMMERWORKS_PAUSECOMPONENT_H
#define GLIMMERWORKS_PAUSECOMPONENT_H
#include "../GameComponent.h"

namespace glimmer {
    class PauseComponent : public GameComponent {
    public:
        [[nodiscard]] u_int32_t GetId() override;
    };
}

#endif //GLIMMERWORKS_PAUSECOMPONENT_H
