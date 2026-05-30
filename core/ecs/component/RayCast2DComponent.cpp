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
#include "RayCast2DComponent.h"

const glimmer::WorldVector2D &glimmer::RayCast2DComponent::GetOrigin() const {
    return origin_;
}

void glimmer::RayCast2DComponent::SetOrigin(const WorldVector2D &origin) {
    origin_ = origin;
}

const b2QueryFilter &glimmer::RayCast2DComponent::GetFilter() const {
    return filter_;
}

void glimmer::RayCast2DComponent::SetFilter(const b2QueryFilter &filter) {
    filter_ = filter;
}

void glimmer::RayCast2DComponent::SetTransform(const WorldVector2D &transform) {
    translation_ = transform;
}

const glimmer::WorldVector2D &glimmer::RayCast2DComponent::GetHitPoint() const {
    return hitPoint_;
}

void glimmer::RayCast2DComponent::SetHitPoint(const WorldVector2D &hitPoint) {
    hitPoint_ = hitPoint;
}

const b2ShapeId &glimmer::RayCast2DComponent::GetHitShape() const {
    return hitShapeId_;
}

void glimmer::RayCast2DComponent::SetHitShape(const b2ShapeId &hitShapeId) {
    hitShapeId_ = hitShapeId;
}

const glimmer::WorldVector2D &glimmer::RayCast2DComponent::GetHitNormal() const {
    return hitNormal_;
}

void glimmer::RayCast2DComponent::SetHitNormal(const WorldVector2D &hitNormal) {
    hitNormal_ = hitNormal;
}

bool glimmer::RayCast2DComponent::IsHit() const {
    return hit_;
}

void glimmer::RayCast2DComponent::SetHit(bool hit) {
    hit_ = hit;
}

GameComponentTypeMessage glimmer::RayCast2DComponent::GetComponentType() {
    return COMPONENT_RAY_CAST_2D;
}


const glimmer::WorldVector2D &glimmer::RayCast2DComponent::GetTranslation() const {
    return translation_;
}

void glimmer::RayCast2DComponent::SetTransform2DEntity(GameEntity::ID id) {
    transform2DEntity_ = id;
}

glimmer::GameEntity::ID glimmer::RayCast2DComponent::GetTransform2DEntity() const {
    return transform2DEntity_;
}

void glimmer::RayCast2DComponent::SetTransform2D(const WorldVector2D &transform2D) {
    translation_ = transform2D;
}
