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
#include "Transform2DComponent.h"

#include "core/Constants.h"
#include "src/saves/transform2d.pb.h"
#include "core/math/WorldVector2D.h"

glimmer::Transform2DComponent::Transform2DComponent(WorldVector2D position) : position_(position)
{
}

void glimmer::Transform2DComponent::SetPosition(WorldVector2D newPosition)
{
    position_ = newPosition;
}

void glimmer::Transform2DComponent::SetRotation(const float newRotation)
{
    rotation_ = newRotation;
}

float glimmer::Transform2DComponent::GetRotation() const
{
    return rotation_;
}

void glimmer::Transform2DComponent::Translate(WorldVector2D deltaPosition)
{
    position_ += deltaPosition;
}

glimmer::WorldVector2D glimmer::Transform2DComponent::GetPosition() const
{
    return position_;
}

std::optional<std::string> glimmer::Transform2DComponent::Serialize()
{
    Transform2dMessage transform2DMessage;
    auto pos = transform2DMessage.mutable_position();
    pos->set_x(position_.x);
    pos->set_y(position_.y);
    transform2DMessage.set_rotation(rotation_);
    return transform2DMessage.SerializeAsString();
}


void glimmer::Transform2DComponent::Deserialize(WorldContext* worldContext, const std::string& data)
{
    Transform2dMessage transform2DMessage;
    transform2DMessage.ParseFromString(data);
    position_.x = transform2DMessage.position().x();
    position_.y = transform2DMessage.position().y();
    rotation_ = transform2DMessage.rotation();
}

GameComponentTypeMessage glimmer::Transform2DComponent::GetComponentTypeStatic()
{
    return COMPONENT_TRANSFORM_2D;
}

GameComponentTypeMessage glimmer::Transform2DComponent::GetComponentType()
{
    return GetComponentTypeStatic();
}
