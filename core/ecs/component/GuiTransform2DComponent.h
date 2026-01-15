//
// Created by coldmint on 2026/1/15.
//

#ifndef GLIMMERWORKS_GUITRANSFORM2DCOMPONENT_H
#define GLIMMERWORKS_GUITRANSFORM2DCOMPONENT_H
#include "Transform2DComponent.h"

namespace glimmer {
    /**
     * GUI transform component
     * GUI坐标组件
     */
    class GuiTransform2DComponent : public Transform2DComponent {
    public:
        [[nodiscard]] u_int32_t GetId() override;
    };
}

#endif //GLIMMERWORKS_GUITRANSFORM2DCOMPONENT_H
