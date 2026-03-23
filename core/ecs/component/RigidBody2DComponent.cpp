//
// Created by Cold-Mint on 2025/11/6.
//

#include "RigidBody2DComponent.h"

#include "../../log/LogCat.h"
#include "../../utils/Box2DUtils.h"
#include "box2d/box2d.h"
#include "core/Constants.h"
#include "core/shape/ShapeType.h"


glimmer::RigidBody2DComponent::~RigidBody2DComponent() {
    if (ready_) {
        b2DestroyBody(bodyId_);
    }
}

void glimmer::RigidBody2DComponent::SetFilter(const Box2dFilter filter) {
    filter_ = filter;
}

void glimmer::RigidBody2DComponent::SetShapeRef(const ResourceRef &shapeRef) {
    shapeRef_ = shapeRef;
}

void glimmer::RigidBody2DComponent::Disable() {
    enabled_ = false;
}

bool glimmer::RigidBody2DComponent::IsEnabled() const {
    return enabled_;
}

void glimmer::RigidBody2DComponent::Enable() {
    enabled_ = true;
}

void glimmer::RigidBody2DComponent::SetDensity(const float density) {
    density_ = density;
}

float glimmer::RigidBody2DComponent::GetDensity() const {
    return density_;
}

void glimmer::RigidBody2DComponent::SetFriction(const float friction) {
    friction_ = friction;
}

void glimmer::RigidBody2DComponent::SetRestitution(float restitution) {
    restitution_ = restitution;
}

float glimmer::RigidBody2DComponent::GetFriction() const {
    return friction_;
}

void glimmer::RigidBody2DComponent::CreateBody(const ResourceLocator *resourceLocator, const b2WorldId worldId,
                                               const WorldVector2D vector2d) {
    if (ready_) {
        return;
    }
    b2BodyDef bodyDef_ = b2DefaultBodyDef();
    bodyDef_.type = bodyType_;
    bodyDef_.position = Box2DUtils::ToMeters(vector2d);
    bodyDef_.enableSleep = allowBodySleep_;
    bodyDef_.fixedRotation = fixedRotation_;
    bodyId_ = b2CreateBody(worldId, &bodyDef_);
    b2Filter filter{};
    filter.categoryBits = filter_.categoryBits;
    filter.maskBits = filter_.maskBits;
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.filter = filter;
    shapeDef.density = density_;
    shapeDef.material.restitution = restitution_;
    shapeDef.material.friction = friction_;
    if (ShapeResource *shapeResource = resourceLocator->FindShape(shapeRef_); shapeResource != nullptr) {
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


    ready_ = true;
}

b2BodyId glimmer::RigidBody2DComponent::GetBodyId() const {
    return bodyId_;
}

bool glimmer::RigidBody2DComponent::IsReady() const {
    return ready_;
}

void glimmer::RigidBody2DComponent::SetBodyType(const b2BodyType bodyType) {
    if (ready_) {
        LogCat::d("Cannot change body type after creation.");
        return;
    }
    bodyType_ = bodyType;
}

void glimmer::RigidBody2DComponent::SetAllowBodySleep(bool enable) {
    if (ready_) {
        LogCat::d("Cannot enableSleep after creation.");
        return;
    }
    allowBodySleep_ = enable;
}

bool glimmer::RigidBody2DComponent::AllowBodySleep() const {
    return allowBodySleep_;
}

void glimmer::RigidBody2DComponent::SetFixedRotation(bool fixedRotation) {
    if (ready_) {
        LogCat::d("Cannot change fixedRotation after creation.");
        return;
    }
    fixedRotation_ = fixedRotation;
}

bool glimmer::RigidBody2DComponent::GetFixedRotation() const {
    return fixedRotation_;
}

bool glimmer::RigidBody2DComponent::IsDynamicBody() const {
    return bodyType_ == b2_dynamicBody;
}

bool glimmer::RigidBody2DComponent::IsKinematicBody() const {
    return bodyType_ == b2_kinematicBody;
}

bool glimmer::RigidBody2DComponent::IsStaticBody() const {
    return bodyType_ == b2_staticBody;
}

uint32_t glimmer::RigidBody2DComponent::GetId() {
    return COMPONENT_ID_RIGID_BODY_2D;
}

bool glimmer::RigidBody2DComponent::IsSerializable() {
    return false;
}
