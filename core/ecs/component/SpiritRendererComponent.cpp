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
#include "SpiritRendererComponent.h"

glimmer::SpiritRendererComponent::SpiritRendererComponent() = default;

void glimmer::SpiritRendererComponent::SetTextureRef(const ResourceRef &resourceRef) {
    resourceRef_ = resourceRef;
}

bool glimmer::SpiritRendererComponent::IsFlipH() const {
    return flipH_;
}

bool glimmer::SpiritRendererComponent::IsFlipV() const {
    return flipV_;
}

void glimmer::SpiritRendererComponent::SetFlipH(const bool flipH) {
    flipH_ = flipH;
}

void glimmer::SpiritRendererComponent::SetFlipV(const bool flipV) {
    flipV_ = flipV;
}

SDL_Texture *glimmer::SpiritRendererComponent::GetTexture(const ResourceLocator *resourceLocator) {
    if (texture_ == nullptr) {
        texture_ = resourceLocator->FindTexture(&resourceRef_);
    }
    return texture_.get();
}

void glimmer::SpiritRendererComponent::SetPosition(WorldVector2D position)
{
    position_ = position;
}


GameComponentTypeMessage glimmer::SpiritRendererComponent::GetComponentType() {
    return COMPONENT_SPIRIT_RENDERER;
}

const glimmer::WorldVector2D &glimmer::SpiritRendererComponent::GetPosition() const {
    return position_;
}
