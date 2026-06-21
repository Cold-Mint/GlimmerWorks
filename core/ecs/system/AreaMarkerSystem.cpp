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
#include "AreaMarkerSystem.h"

#include "core/ecs/component/AreaMarkerComponent.h"
#include "core/math/CoordinateTransformer.h"
#include "../../world/WorldContext.h"
#include "core/utils/StringUtils.h"
#include "fmt/color.h"

void glimmer::AreaMarkerSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    if (gameComponentType == COMPONENT_CAMERA && cameraComponent_ == nullptr)
    {
        cameraComponent_ = entityShortCut_->GetCameraComponent();
    }
    if (gameComponentType == COMPONENT_TRANSFORM_2D && cameraTransform2DComponent_ == nullptr)
    {
        cameraTransform2DComponent_ = entityShortCut_->GetCameraTransform2DComponent();
    }
    if (gameComponentType == COMPONENT_AREA_MARKER && areaMarkerComponent_ == nullptr)
    {
        areaMarkerComponent_ = entityShortCut_->GetAreaMarkerComponent();
    }
}

glimmer::AreaMarkerSystem::AreaMarkerSystem(WorldContext* worldContext) : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_AREA_MARKER);
    WatchComponent(COMPONENT_TRANSFORM_2D);
    WatchComponent(COMPONENT_CAMERA);
    appContext_ = worldContext->GetAppContext();
    resourcePackManager_ = appContext_->GetResourcePackManager();
    preloadColors_ = appContext_->GetPreloadColors();
    Init();
}


void glimmer::AreaMarkerSystem::Update(const float delta)
{
    if (areaMarkerComponent_ == nullptr)
    {
        return;;
    }
    if (!areaMarkerComponent_->CanDraw())
    {
        return;
    }
    areaMarkerComponent_->SetRemainingTime(areaMarkerComponent_->GetRemainingTime() - delta);
    if (areaMarkerComponent_->IsExpired())
    {
        areaMarkerComponent_->Reset();
    }
}

void glimmer::AreaMarkerSystem::Render(SDL_Renderer* renderer)
{
    if (appContext_ == nullptr)
    {
        return;
    }
    if (cameraComponent_ == nullptr)
    {
        return;
    }
    if (cameraTransform2DComponent_ == nullptr)
    {
        return;
    }
    if (areaMarkerComponent_ == nullptr)
    {
        return;;
    }
    if (!areaMarkerComponent_->CanDraw())
    {
        return;
    }
    WorldVector2D cameraPosition = cameraTransform2DComponent_->GetPosition();
    const auto& areaMarkerFullColor =
        preloadColors_->
        areaMarkerColor;
    const auto& areaMarkerBorderColor = preloadColors_->
        areaMarkerBorderColor;
    const TileVector2D& startPointTileVector2D = areaMarkerComponent_->GetStartPoint();
    const TileVector2D& endPointTileVector2D = areaMarkerComponent_->GetEndPoint();
    const WorldVector2D startPoint = CoordinateTransformer::TileToWorld(startPointTileVector2D);
    const WorldVector2D endPoint = CoordinateTransformer::TileToWorld(endPointTileVector2D);
    int tileWidth = std::abs(endPointTileVector2D.x - startPointTileVector2D.x) + 1;
    int tileHeight = std::abs(endPointTileVector2D.y - startPointTileVector2D.y) + 1;
    int tileArea = tileWidth * tileHeight;
    const float minWorldX = std::min(startPoint.x, endPoint.x);
    const float maxWorldX = std::max(startPoint.x, endPoint.x);
    const float minWorldY = std::min(startPoint.y, endPoint.y);
    const float maxWorldY = std::max(startPoint.y, endPoint.y);
    WorldVector2D rectWorldMin;
    rectWorldMin.x = minWorldX - HALF_TILE_SIZE;
    rectWorldMin.y = minWorldY - HALF_TILE_SIZE;
    WorldVector2D rectWorldMax;
    rectWorldMax.x = maxWorldX + HALF_TILE_SIZE;
    rectWorldMax.y = maxWorldY + HALF_TILE_SIZE;
    if (cameraComponent_->IsPointInViewport(cameraPosition, rectWorldMin) ||
        cameraComponent_->IsPointInViewport(cameraPosition, rectWorldMax))
    {
        ScreenVector2D camMin = CoordinateTransformer::WorldToScreen(cameraPosition, rectWorldMin,
                                                                     cameraComponent_->GetSize(),
                                                                     cameraComponent_->GetZoom());
        ScreenVector2D camMax = CoordinateTransformer::WorldToScreen(cameraPosition, rectWorldMax,
                                                                     cameraComponent_->GetSize(),
                                                                     cameraComponent_->GetZoom());
        SDL_FRect rect;
        rect.x = std::min(camMin.x, camMax.x);
        rect.y = std::min(camMin.y, camMax.y);
        rect.w = std::abs(camMax.x - camMin.x);
        rect.h = std::abs(camMax.y - camMin.y);
        SDL_SetRenderDrawColor(renderer, areaMarkerFullColor.r,
                               areaMarkerFullColor.g, areaMarkerFullColor.b,
                               areaMarkerFullColor.a);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, areaMarkerBorderColor.r,
                               areaMarkerBorderColor.g, areaMarkerBorderColor.b,
                               areaMarkerBorderColor.a);
        SDL_RenderRect(renderer, &rect);

        std::string areaMarkerTip = fmt::format(fmt::runtime(appContext_->GetLangsResources()->areaMarkerTip),
                                                tileWidth, tileHeight, tileArea);
        uint64_t areaMarkerTipFingerprint = StringUtils::StringToUint64(areaMarkerTip);
        if (areaMarkerTipFingerprint != areaMarkerTipFingerprint_)
        {
            areaMarkerTipTexture_ = resourcePackManager_->
                CreateStringTexture(areaMarkerTip, &preloadColors_->textColor);
        }

        if (areaMarkerTipTexture_ == nullptr)
        {
            return;
        }
        const float textX = rect.x + (rect.w - areaMarkerTipTexture_->w) / 2.0F;
        const float textY = rect.y + (rect.h - areaMarkerTipTexture_->h) / 2.0F;
        SDL_FRect rectCenter = {
            textX, textY,
            static_cast<float>(areaMarkerTipTexture_->w),
            static_cast<float>(areaMarkerTipTexture_->h)
        };
        SDL_RenderTexture(renderer, areaMarkerTipTexture_.get(), nullptr, &rectCenter);
    }
    AppContext::RestoreColorRenderer(renderer);
}

glimmer::GameSystemType glimmer::AreaMarkerSystem::GetGameSystemType() const
{
    return GameSystemType::AreaMarkerSystem;
}

uint8_t glimmer::AreaMarkerSystem::GetRenderOrder()
{
    return RENDER_ORDER_AREA_MARKER;
}
