//
// Created by Cold-Mint on 2025/11/6.
//

#include "RigidBody2DComponent.h"

#include "../../log/LogCat.h"
#include "../../utils/Box2DUtils.h"
#include "box2d/box2d.h"
#include "core/Constants.h"
#include "src/saves/rigidbody_2d.pb.h"


glimmer::RigidBody2DComponent::~RigidBody2DComponent() {
    if (ready_) {
        b2DestroyBody(bodyId_);
    }
}

void glimmer::RigidBody2DComponent::SetCategoryBits(const uint64_t categoryBits) {
    categoryBits_ = categoryBits;
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

void glimmer::RigidBody2DComponent::SetMaskBits(const uint64_t maskBits) {
    maskBits_ = maskBits;
}

void glimmer::RigidBody2DComponent::CreateBody(const b2WorldId worldId, const b2Vec2 position) {
    if (ready_) {
        return;
    }
    b2BodyDef bodyDef_ = b2DefaultBodyDef();
    bodyDef_.type = bodyType_;
    bodyDef_.position = position;
    bodyDef_.enableSleep = false;
    bodyDef_.fixedRotation = fixedRotation_;
    bodyId_ = b2CreateBody(worldId, &bodyDef_);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    b2Filter filter{};
    filter.categoryBits = categoryBits_;
    filter.maskBits = maskBits_;
    shapeDef.filter = filter;
    shapeDef.density = 1.0F;
    shapeDef.material.friction = 0.3F;
    const b2Polygon shape = b2MakeBox(
        Box2DUtils::ToMeters(width_ * 0.5F),
        Box2DUtils::ToMeters(height_ * 0.5F)
    );

    b2CreatePolygonShape(bodyId_, &shapeDef, &shape);
    ready_ = true;
}

b2BodyId glimmer::RigidBody2DComponent::GetBodyId() const {
    return bodyId_;
}

void glimmer::RigidBody2DComponent::SetWidth(const float width) {
    if (ready_) {
        LogCat::d("Cannot change width after creation.");
        return;
    }
    width_ = width;
}

void glimmer::RigidBody2DComponent::SetHeight(const float height) {
    if (ready_) {
        LogCat::d("Cannot change height after creation.");
        return;
    }
    height_ = height;
}

float glimmer::RigidBody2DComponent::GetWidth() const {
    return width_;
}

float glimmer::RigidBody2DComponent::GetHeight() const {
    return height_;
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

void glimmer::RigidBody2DComponent::SetEnableSleep(bool enable) {
    if (ready_) {
        LogCat::d("Cannot enableSleep after creation.");
        return;
    }
    enableSleep_ = enable;
}

bool glimmer::RigidBody2DComponent::GetEnableSleep() const {
    return enableSleep_;
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

u_int32_t glimmer::RigidBody2DComponent::GetId() {
    return COMPONENT_ID_RIGID_BODY_2D;
}

bool glimmer::RigidBody2DComponent::IsSerializable() {
    return true;
}

std::string glimmer::RigidBody2DComponent::Serialize() {
    RigidBody2dMessage rigidBody2dMessage;
    rigidBody2dMessage.set_categorybits(categoryBits_);
    rigidBody2dMessage.set_maskbits(maskBits_);
    switch (bodyType_) {
        case b2_dynamicBody:
            rigidBody2dMessage.set_type(DYNAMIC);
            break;
        case b2_kinematicBody:
            rigidBody2dMessage.set_type(KINEMATIC);
            break;
        case b2_staticBody:
            rigidBody2dMessage.set_type(STATIC);
            break;
        default:
            break;
    }
    rigidBody2dMessage.set_enablesleep(enableSleep_);
    rigidBody2dMessage.set_width(width_);
    rigidBody2dMessage.set_height(height_);
    rigidBody2dMessage.set_fixedrotation(fixedRotation_);
    rigidBody2dMessage.set_enabled(enabled_);
    return rigidBody2dMessage.SerializeAsString();
}

void glimmer::RigidBody2DComponent::Deserialize(WorldContext *worldContext, const std::string &data) {
    RigidBody2dMessage rigidBody2dMessage;
    rigidBody2dMessage.ParseFromString(data);
    categoryBits_ = rigidBody2dMessage.categorybits();
    maskBits_ = rigidBody2dMessage.maskbits();
    switch (rigidBody2dMessage.type()) {
        case DYNAMIC:
            bodyType_ = b2_dynamicBody;
            break;
        case KINEMATIC:
            bodyType_ = b2_kinematicBody;
            break;
        case STATIC:
            bodyType_ = b2_staticBody;
            break;
        default:
            break;
    }
    enableSleep_ = rigidBody2dMessage.enablesleep();
    width_ = rigidBody2dMessage.width();
    height_ = rigidBody2dMessage.height();
    fixedRotation_ = rigidBody2dMessage.fixedrotation();
    enabled_ = rigidBody2dMessage.enabled();
}
