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
#pragma once
#include "EcsTypes.h"
#include "component/AreaMarkerComponent.h"
#include "component/BlueprintComponent.h"
#include "component/CameraComponent.h"
#include "component/DiggingComponent.h"
#include "component/HotBarComonent.h"
#include "component/ParallaxBackgroundComponent.h"
#include "component/Transform2DComponent.h"
#include "core/Constants.h"

namespace glimmer
{
    class EntityShortCut
    {
        GameEntityID player_ = GAME_ENTITY_ID_INVALID;
        CameraComponent* cameraComponent_ = nullptr;
        Transform2DComponent* cameraTransform2DComponent_ = nullptr;
        HotBarComponent* hotBarComponent_ = nullptr;
        BlueprintComponent* blueprintComponent_ = nullptr;
        DiggingComponent* diggingComponent_ = nullptr;
        AreaMarkerComponent* areaMarkerComponent_ = nullptr;
        ParallaxBackgroundComponent* parallaxBackgroundComponent_ = nullptr;

    public:
        void SetPlayer(GameEntityID player);

        void SetParallaxBackgroundComponent(ParallaxBackgroundComponent* parallaxBackgroundComponent);

        [[nodiscard]] ParallaxBackgroundComponent* GetParallaxBackgroundComponent() const;

        [[nodiscard]] GameEntityID GetPlayer() const;


        void SetDiggingComponent(DiggingComponent* diggingComponent);

        [[nodiscard]] DiggingComponent* GetDiggingComponent() const;


        void SetBlueprintComponent(BlueprintComponent* blueprintComponent);

        [[nodiscard]] BlueprintComponent* GetBlueprintComponent() const;

        void SetHotBarComponent(HotBarComponent* hotBarComponent);

        [[nodiscard]] HotBarComponent* GetHotBarComponent() const;

        void SetCameraComponent(CameraComponent* cameraComponent);

        void SetAreaMarkerComponent(AreaMarkerComponent* areaMarkerComponent);

        [[nodiscard]] AreaMarkerComponent* GetAreaMarkerComponent() const;

        [[nodiscard]] CameraComponent* GetCameraComponent() const;

        void SetCameraTransform2DComponent(Transform2DComponent* cameraTransform2DComponent);

        [[nodiscard]] Transform2DComponent* GetCameraTransform2DComponent() const;
    };
}
