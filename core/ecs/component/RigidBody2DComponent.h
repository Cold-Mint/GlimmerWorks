//
// Created by Cold-Mint on 2025/11/6.
//

#ifndef GLIMMERWORKS_RIGIDBODY2DCOMPONENT_H
#define GLIMMERWORKS_RIGIDBODY2DCOMPONENT_H
#include "../GameComponent.h"
#include "box2d/box2d.h"
#include "box2d/types.h"

namespace glimmer {
    class RigidBody2DComponent : public GameComponent {
        b2BodyId bodyId_ = b2_nullBodyId;
        uint64_t categoryBits_ = 0;
        uint64_t maskBits_ = 0;
        bool ready_ = false;
        b2BodyType bodyType_ = b2_staticBody;
        bool enableSleep_ = true;
        float width_ = 0;
        float height_ = 0;
        bool fixedRotation_ = false;
        bool enabled_ = true;

    public:
        ~RigidBody2DComponent() override;

        /**
         * SetCategoryBits
         * 设置自身碰撞
         * @param categoryBits
         */
        void SetCategoryBits(uint64_t categoryBits);

        void Enable();

        void Disable();

        [[nodiscard]] bool IsEnabled() const;

        /**
         * SetMaskBits
         * 设置能够碰撞谁
         * @param maskBits
         */
        void SetMaskBits(uint64_t maskBits);

        /**
         * Create Body
         * 创建刚体
         * @param worldId worldId 世界ID
         * @param b2Vec2 b2Vec2 刚体位置(单位：米)
         */
        void CreateBody(b2WorldId worldId, b2Vec2 b2Vec2);

        /**
         * GetBodyId
         * 获取刚体ID
         * @return 刚体ID
         */
        [[nodiscard]] b2BodyId GetBodyId() const;


        /**
         * SetWidth
         * 设置刚体宽度
         * @param width Rigid body width (unit: pixels) 刚体宽度(单位：像素)
         */
        void SetWidth(float width);

        /**
         * SetHeight
         * 设置刚体高度
         * @param height Rigid body height (unit: pixels) 刚体高度(单位：像素)
         */
        void SetHeight(float height);

        /**
         * GetWidth
         * 获取刚体宽度
         * @return Rigid body width (unit: pixels) 刚体宽度(单位：像素)
         */
        [[nodiscard]] float GetWidth() const;

        /**
         * GetHeight
         * 获取刚体高度
         * @return Rigid body height (unit: pixels) 刚体高度(单位：像素)
         */
        [[nodiscard]] float GetHeight() const;

        /**
         * Is the body ready
         * 刚体是否就绪
         * @return
         */
        [[nodiscard]] bool IsReady() const;

        /**
         * SetBodyType
         * 设置刚体类型
         * @param bodyType 刚体类型
         */
        void SetBodyType(b2BodyType bodyType);

        /**
         * SetEnableSleep
         * 设置是否启用休眠
         * @param enable 是否启用休眠
         */
        void SetEnableSleep(bool enable);

        /**
         * GetEnableSleep
         * 获取是否启用休眠
         * @return 是否启用休眠
         */
        [[nodiscard]] bool GetEnableSleep() const;

        /**
         * SetFixedRotation
         * 设置是否固定旋转
         * @param fixedRotation 是否固定旋转
         */
        void SetFixedRotation(bool fixedRotation);

        /**
         * GetFixedRotation
         * 获取是否固定旋转
         * @return 是否固定旋转
         */
        [[nodiscard]] bool GetFixedRotation() const;

        /**
         * Check if the body is a dynamic body.
         * 动态刚体可以被物理力影响，位置会根据力自动更新。
         * Dynamic bodies are affected by forces and their positions are automatically updated by the physics engine.
         * 动态物体受到力的影响，它们的位置由物理引擎自动更新。
         * @return true if the body is dynamic
         */
        [[nodiscard]] bool IsDynamicBody() const;

        /**
         * Check if the body is a kinematic body.
         * 运动学刚体不受力影响，但可以通过设置速度或位置移动。
         * Kinematic bodies are not affected by forces but can be moved by setting velocity or position.
         * 运动体不受力的影响，但可以通过设定速度或位置来移动。
         * @return true if the body is kinematic
         */
        [[nodiscard]] bool IsKinematicBody() const;

        /**
         * Check if the body is a static body.
         * 静态刚体不受力影响，位置只能手动设置，通常用于地面或墙壁。
         * Static bodies are unaffected by forces and can only be moved manually, typically used for ground or walls.
         * 静态物体不受外力影响，只能手动移动，通常用于地面或墙壁。
         * @return true if the body is static
         */
        [[nodiscard]] bool IsStaticBody() const;

        [[nodiscard]] u_int32_t GetId() override;

        [[nodiscard]] bool IsSerializable() override;

        [[nodiscard]] std::string Serialize() override;

        void Deserialize(AppContext *appContext, WorldContext *worldContext, const std::string &data) override;
    };
}

#endif //GLIMMERWORKS_RIGIDBODY2DCOMPONENT_H
