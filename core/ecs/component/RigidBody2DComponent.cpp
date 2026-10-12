/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "RigidBody2DComponent.h"

#include "core/utils/Box2DUtils.h"
#include "box2d/box2d.h"
#include "core/config/Constants.h"
#include "core/context/AppContext.h"
#include "core/ecs/EntityManager.h"
#include "core/shape/ShapeType.h"
#include "core/world/PhysicsTaskQueue.h"
#include "core/world/WorldContext.h"


void glimmer::RigidBody2DComponent::CreateBodyNow(const ResourceLocator *resourceLocator, b2WorldId worldId,
                                                  const WorldVector2D &vector2d, const RigidBodyConfig &config) {
    if (ready_.load(std::memory_order_acquire)) {
        return;
    }
    b2BodyDef bodyDef_ = b2DefaultBodyDef();
    bodyDef_.type = config.bodyType;
    bodyDef_.position = Box2DUtils::ToMeters(vector2d);
    bodyDef_.enableSleep = config.allowBodySleep;
    bodyDef_.fixedRotation = config.fixedRotation;
    bodyDef_.isEnabled = enabled_.load(std::memory_order_relaxed);
    bodyId_ = b2CreateBody(worldId, &bodyDef_);
    b2Filter filter{};
    filter.categoryBits = config.filter.categoryBits;
    filter.maskBits = config.filter.maskBits;
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.filter = filter;
    shapeDef.density = config.density;
    shapeDef.material.restitution = config.restitution;
    shapeDef.material.friction = config.friction;
    if (IShapeResource *shapeResource = resourceLocator->FindShape(&config.shapeRef); shapeResource != nullptr) {
        const auto shape = static_cast<ShapeType>(shapeResource->shapeType);
        if (shape == ShapeType::RECTANGLE) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
            const RectangleShapeResource *rectangleShapeResource = static_cast<RectangleShapeResource *>(shapeResource);
            const b2Polygon box2dShape = b2MakeBox(
                Box2DUtils::ToMeters(rectangleShapeResource->width * TILE_SIZE * 0.5F),
                Box2DUtils::ToMeters(rectangleShapeResource->height * TILE_SIZE * 0.5F)
            );
            b2CreatePolygonShape(bodyId_, &shapeDef, &box2dShape);
        }

        if (shape == ShapeType::CIRCLE) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
            const auto circularShapeResource = static_cast<CircularShapeResource *>(shapeResource);
            b2Circle circle;
            circle.radius = circularShapeResource->radius;
            circle.center = {
                Box2DUtils::ToMeters(circularShapeResource->center.x * TILE_SIZE),
                Box2DUtils::ToMeters(circularShapeResource->center.y * TILE_SIZE)
            };
            b2CreateCircleShape(bodyId_, &shapeDef, &circle);
        }

        if (shape == ShapeType::ROUNDED_RECTANGLE) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
            const auto roundedRectangleShapeResource = static_cast<RoundedRectangleShapeResource *>(shapeResource);
            const b2Polygon box2dShape = b2MakeRoundedBox(
                Box2DUtils::ToMeters(roundedRectangleShapeResource->width * TILE_SIZE * 0.5F),
                Box2DUtils::ToMeters(roundedRectangleShapeResource->height * TILE_SIZE * 0.5F),
                Box2DUtils::ToMeters(roundedRectangleShapeResource->radius * TILE_SIZE)
            );
            b2CreatePolygonShape(bodyId_, &shapeDef, &box2dShape);
        }
    }
    ready_.store(true, std::memory_order_release);
}

glimmer::RigidBody2DComponent::~RigidBody2DComponent() {
    if (ready_.load(std::memory_order_acquire)) {
        const b2BodyId bodyId = bodyId_;
        if (worldContext_ != nullptr) {
            if (PhysicsTaskQueue *taskQueue = worldContext_->GetPhysicsTaskQueue(); taskQueue != nullptr) {
                taskQueue->Enqueue([bodyId] {
                    if (b2Body_IsValid(bodyId)) {
                        b2DestroyBody(bodyId);
                    }
                });
            }
        }
    }
}

void glimmer::RigidBody2DComponent::SetFilter(const Box2dFilter filter) {
    rigidBodyConfig_.filter = filter;
}

void glimmer::RigidBody2DComponent::SetShapeRef(const ResourceRef &shapeRef) {
    rigidBodyConfig_.shapeRef = shapeRef;
}

void glimmer::RigidBody2DComponent::Disable() {
    enabled_.store(false, std::memory_order_relaxed);
    if (ready_.load(std::memory_order_acquire)) {
        const b2BodyId bodyId = bodyId_;
        if (worldContext_ != nullptr) {
            if (PhysicsTaskQueue *taskQueue = worldContext_->GetPhysicsTaskQueue(); taskQueue != nullptr) {
                taskQueue->Enqueue([bodyId] {
                    if (b2Body_IsValid(bodyId)) {
                        b2Body_Disable(bodyId);
                    }
                });
            }
        }
    }
}

bool glimmer::RigidBody2DComponent::IsEnabled() const {
    return enabled_.load(std::memory_order_relaxed);
}

void glimmer::RigidBody2DComponent::Enable() {
    enabled_.store(true, std::memory_order_relaxed);
    if (ready_.load(std::memory_order_acquire)) {
        const b2BodyId bodyId = bodyId_;
        if (worldContext_ != nullptr) {
            if (PhysicsTaskQueue *taskQueue = worldContext_->GetPhysicsTaskQueue(); taskQueue != nullptr) {
                taskQueue->Enqueue([bodyId] {
                    if (b2Body_IsValid(bodyId)) {
                        b2Body_Enable(bodyId);
                    }
                });
            }
        }
    }
}

void glimmer::RigidBody2DComponent::SetDensity(const float density) {
    rigidBodyConfig_.density = density;
}

float glimmer::RigidBody2DComponent::GetDensity() const {
    return rigidBodyConfig_.density;
}

void glimmer::RigidBody2DComponent::SetFriction(const float friction) {
    rigidBodyConfig_.friction = friction;
}

void glimmer::RigidBody2DComponent::SetRestitution(float restitution) {
    rigidBodyConfig_.restitution = restitution;
}

float glimmer::RigidBody2DComponent::GetFriction() const {
    return rigidBodyConfig_.friction;
}

void glimmer::RigidBody2DComponent::CreateBody(WorldContext *worldContext, const GameEntityID entityId,
                                               const WorldVector2D &vector2d) {
    if (worldContext == nullptr) {
        return;
    }
    worldContext_ = worldContext;
    PhysicsTaskQueue *taskQueue = worldContext->GetPhysicsTaskQueue();
    if (taskQueue == nullptr) {
        return;
    }
    RigidBodyConfig rigidBodyConfigCopy = rigidBodyConfig_;
    taskQueue->Enqueue([worldContext, entityId, vector2d,rigidBodyConfigCopy] {
        EntityManager *entityManager = worldContext->GetEntityManager();
        if (entityManager == nullptr) {
            return;
        }
        auto *component = entityManager->GetComponent<RigidBody2DComponent>(entityId);
        if (component == nullptr || component->IsReady()) {
            return;
        }
        const ResourceLocator *resourceLocator = worldContext->GetAppContext()->GetResourceLocator();
        if (resourceLocator == nullptr) {
            return;
        }
        component->CreateBodyNow(resourceLocator, worldContext->GetWorldId(), vector2d, rigidBodyConfigCopy);
    });
}

b2BodyId glimmer::RigidBody2DComponent::GetBodyId() const {
    return bodyId_;
}

bool glimmer::RigidBody2DComponent::IsReady() const {
    return ready_.load(std::memory_order_acquire);
}

void glimmer::RigidBody2DComponent::SetBodyType(const b2BodyType bodyType) {
    if (ready_.load(std::memory_order_acquire)) {
        return;
    }
    rigidBodyConfig_.bodyType = bodyType;
}

void glimmer::RigidBody2DComponent::SetAllowBodySleep(bool enable) {
    if (ready_.load(std::memory_order_acquire)) {
        return;
    }
    rigidBodyConfig_.allowBodySleep = enable;
}

bool glimmer::RigidBody2DComponent::AllowBodySleep() const {
    return rigidBodyConfig_.allowBodySleep;
}

void glimmer::RigidBody2DComponent::SetFixedRotation(bool fixedRotation) {
    if (ready_.load(std::memory_order_acquire)) {
        return;
    }
    rigidBodyConfig_.fixedRotation = fixedRotation;
}

bool glimmer::RigidBody2DComponent::GetFixedRotation() const {
    return rigidBodyConfig_.fixedRotation;
}

bool glimmer::RigidBody2DComponent::IsDynamicBody() const {
    return rigidBodyConfig_.bodyType == b2_dynamicBody;
}

bool glimmer::RigidBody2DComponent::IsKinematicBody() const {
    return rigidBodyConfig_.bodyType == b2_kinematicBody;
}

bool glimmer::RigidBody2DComponent::IsStaticBody() const {
    return rigidBodyConfig_.bodyType == b2_staticBody;
}

GameComponentTypeMessage glimmer::RigidBody2DComponent::GetComponentTypeStatic() {
    return COMPONENT_RIGID_BODY_2D;
}

GameComponentTypeMessage glimmer::RigidBody2DComponent::GetComponentType() {
    return GetComponentTypeStatic();
}
