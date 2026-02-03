//
// Created by coldmint on 2026/1/15.
//

#ifndef GLIMMERWORKS_GUITRANSFORM2DCOMPONENT_H
#define GLIMMERWORKS_GUITRANSFORM2DCOMPONENT_H
#include "core/ecs/GameComponent.h"
#include "core/math/Vector2D.h"

namespace glimmer {
    /**
     * GUI transform component
     * GUI坐标组件
     */
    class GuiTransform2DComponent : public GameComponent {
        CameraVector2D position_;
        CameraVector2D size_;

    public:
        void SetPosition(CameraVector2D position);

        void SetSize(CameraVector2D size);

        [[nodiscard]] CameraVector2D GetPosition() const;

        [[nodiscard]] CameraVector2D GetSize() const;

        [[nodiscard]] bool IsSerializable() override;

        [[nodiscard]] std::string Serialize() override;

        void Deserialize(WorldContext *worldContext, const std::string &data) override;

        [[nodiscard]] u_int32_t GetId() override;
    };
}

#endif //GLIMMERWORKS_GUITRANSFORM2DCOMPONENT_H
