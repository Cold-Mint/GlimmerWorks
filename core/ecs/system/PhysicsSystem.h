//
// Created by Cold-Mint on 2025/11/6.
//

#ifndef GLIMMERWORKS_PHYSICSSYSTEM_H
#define GLIMMERWORKS_PHYSICSSYSTEM_H
#include "../GameSystem.h"
#include "../component/Transform2DComponent.h"


namespace glimmer {
    class PhysicsSystem : public GameSystem {
        /**
         * Fixed time step of physical simulation (unit: seconds)
         * The physical world (such as Box2D) usually requires a fixed time step for updates.
         * Otherwise, if delta (time consumed per frame) is directly used for simulation, when the frame rate is unstable,
         * It will lead to inconsistent and unstable physical behaviors (such as speed jitter, collision anomalies, etc.).
         * The common value of FIXED_TIME_STEP is generally 1/60 second (approximately 0.016f).
         * Corresponding to a physical simulation frequency of 60Hz.
         * 物理模拟的固定时间步长（单位：秒）
         * 物理世界（例如 Box2D）通常需要使用固定时间步长来更新，
         * 否则如果用 delta（每帧耗时）直接进行模拟，当帧率不稳定时，
         * 会导致物理行为不一致、不稳定（例如速度抖动、碰撞异常等）。
         * FIXED_TIME_STEP 的常用值一般为 1/60 秒（约 0.016f），
         * 对应 60Hz 的物理模拟频率。
         */
        static constexpr float FIXED_TIME_STEP = 0.016F;
        float accumulator_ = 0.0F;

    public:
        PhysicsSystem(AppContext *appContext, WorldContext *worldContext);


        void Update(float delta) override;

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_PHYSICSSYSTEM_H
