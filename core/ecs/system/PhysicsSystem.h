//
// Created by Cold-Mint on 2025/11/6.
//

#ifndef GLIMMERWORKS_PHYSICSSYSTEM_H
#define GLIMMERWORKS_PHYSICSSYSTEM_H
#include "../GameSystem.h"
#include "../component/RigidBody2DComponent.h"
#include "../component/Transform2DComponent.h"


namespace glimmer
{
    class PhysicsSystem : public GameSystem
    {
        b2WorldId worldId_ = b2_nullWorldId;
        void OnActivationChanged(bool activeStatus) override;

    public:
        PhysicsSystem(AppContext* appContext, WorldContext* worldContext)
            : GameSystem(appContext, worldContext)
        {
            RequireComponent<Transform2DComponent>();
            RequireComponent<RigidBody2DComponent>();
        }


        void Update(float delta) override;

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_PHYSICSSYSTEM_H
