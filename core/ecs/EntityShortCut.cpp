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
#include "EntityShortCut.h"

#include "core/log/LogCat.h"

void glimmer::EntityShortCut::SetPlayer(const GameEntityID player)
{
    player_ = player;
}

void glimmer::EntityShortCut::SetParallaxBackgroundComponent(ParallaxBackgroundComponent* parallaxBackgroundComponent)
{
    parallaxBackgroundComponent_ = parallaxBackgroundComponent;
}

glimmer::ParallaxBackgroundComponent* glimmer::EntityShortCut::GetParallaxBackgroundComponent() const
{
    if (parallaxBackgroundComponent_ == nullptr)
    {
        LogCat::w(std::source_location::current(),
                  "EntityShortCut: ParallaxBackgroundComponent pointer is null, component not initialized");
        return nullptr;
    }
    return parallaxBackgroundComponent_;
}

GameEntityID glimmer::EntityShortCut::GetPlayer() const
{
    return player_;
}

glimmer::ItemToolTipComponent* glimmer::EntityShortCut::GetItemToolTipComponent() const
{
    if (itemToolTipComponent_ == nullptr)
    {
        LogCat::w(std::source_location::current(),
                  "EntityShortCut: ItemToolTipComponent pointer is null, tooltip ui not ready");
        return nullptr;
    }
    return itemToolTipComponent_;
}

void glimmer::EntityShortCut::SetItemToolTipComponent(ItemToolTipComponent* itemToolTipComponent)
{
    itemToolTipComponent_ = itemToolTipComponent;
}

void glimmer::EntityShortCut::SetDiggingComponent(DiggingComponent* diggingComponent)
{
    diggingComponent_ = diggingComponent;
}

glimmer::DiggingComponent* glimmer::EntityShortCut::GetDiggingComponent() const
{
    if (diggingComponent_ == nullptr)
    {
        LogCat::w(std::source_location::current(),
                  "EntityShortCut: DiggingComponent pointer is null, digging system uninitialized");
        return nullptr;
    }
    return diggingComponent_;
}

void glimmer::EntityShortCut::SetBlueprintComponent(BlueprintComponent* blueprintComponent)
{
    blueprintComponent_ = blueprintComponent;
}

glimmer::BlueprintComponent* glimmer::EntityShortCut::GetBlueprintComponent() const
{
    if (blueprintComponent_ == nullptr)
    {
        LogCat::w(std::source_location::current(),
                  "EntityShortCut: BlueprintComponent pointer is null, build blueprint ui missing");
        return nullptr;
    }
    return blueprintComponent_;
}

void glimmer::EntityShortCut::SetItemContainerComponent(ItemContainerComponent* itemContainerComponent)
{
    itemContainerComponent_ = itemContainerComponent;
}

glimmer::ItemContainerComponent* glimmer::EntityShortCut::GetItemContainerComponent() const
{
    if (itemContainerComponent_ == nullptr)
    {
        LogCat::w(std::source_location::current(),
                  "EntityShortCut: ItemContainerComponent pointer is null, inventory container unavailable");
        return nullptr;
    }
    return itemContainerComponent_;
}

void glimmer::EntityShortCut::SetCameraComponent(CameraComponent* cameraComponent)
{
    cameraComponent_ = cameraComponent;
}

glimmer::CameraComponent* glimmer::EntityShortCut::GetCameraComponent() const
{
    if (cameraComponent_ == nullptr)
    {
        LogCat::w(std::source_location::current(),
                  "EntityShortCut: CameraComponent pointer is null, main camera not initialized");
        return nullptr;
    }
    return cameraComponent_;
}

void glimmer::EntityShortCut::SetAreaMarkerComponent(AreaMarkerComponent* areaMarkerComponent)
{
    areaMarkerComponent_ = areaMarkerComponent;
}

glimmer::AreaMarkerComponent* glimmer::EntityShortCut::GetAreaMarkerComponent() const
{
    if (areaMarkerComponent_ == nullptr)
    {
        LogCat::w(std::source_location::current(),
                  "EntityShortCut: AreaMarkerComponent pointer is null, region marker render missing");
        return nullptr;
    }
    return areaMarkerComponent_;
}

void glimmer::EntityShortCut::SetCameraTransform2DComponent(Transform2DComponent* cameraTransform2DComponent)
{
    cameraTransform2DComponent_ = cameraTransform2DComponent;
}

glimmer::Transform2DComponent* glimmer::EntityShortCut::GetCameraTransform2DComponent() const
{
    if (cameraTransform2DComponent_ == nullptr)
    {
        LogCat::w(std::source_location::current(),
                  "EntityShortCut: Camera Transform2DComponent pointer is null, camera transform unbound");
        return nullptr;
    }
    return cameraTransform2DComponent_;
}
