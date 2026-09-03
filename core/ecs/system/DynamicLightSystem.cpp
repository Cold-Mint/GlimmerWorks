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
#include "DynamicLightSystem.h"

#include "core/config/Constants.h"
#include "core/ecs/EntityManager.h"
#include "core/ecs/component/LightComponent.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/math/CoordinateTransformer.h"
#include "core/mod/Resource.h"
#include "core/mod/ResourceLocator.h"
#include "core/world/LightBuffer.h"
#include "core/world/WorldContext.h"
#include "core/world/generator/TileLayerType.h"

glimmer::DynamicLightSystem::DynamicLightSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    Init();
}

void glimmer::DynamicLightSystem::Update(const float delta) {
    WorldContext *worldContext = GetWorldContext();
    EntityManager *entityManager = GetEntityManager();
    if (worldContext == nullptr || entityManager == nullptr) {
        return;
    }
    const AppContext *appContext = worldContext->GetAppContext();
    LightBuffer *lightBuffer = worldContext->GetLightingBuffer();
    ResourceLocator *resourceLocator = appContext != nullptr ? appContext->GetResourceLocator() : nullptr;
    if (lightBuffer == nullptr || resourceLocator == nullptr) {
        return;
    }

    const std::vector<GameEntityID> lightEntities = entityManager->GetEntityIDWithComponents(
        {COMPONENT_LIGHT, COMPONENT_TRANSFORM_2D});

    std::unordered_set<GameEntityID> currentEntities;
    for (const GameEntityID entityId: lightEntities) {
        auto *lightComponent = entityManager->GetComponent<LightComponent>(entityId);
        auto *transform = entityManager->GetComponent<Transform2DComponent>(entityId);
        if (lightComponent == nullptr || transform == nullptr) {
            continue;
        }
        const ResourceRef &lightRef = lightComponent->GetLightSourceRef();
        if (!lightRef.IsValid()) {
            continue;
        }
        LightSourceResource *lightResource = resourceLocator->FindLightSource(&lightRef);
        if (lightResource == nullptr) {
            continue;
        }
        const std::unique_ptr<Color> lightColor = resourceLocator->FindColor(&lightResource->lightColor);
        if (lightColor == nullptr || lightColor->a == 0) {
            continue;
        }
        const TileVector2D tile = CoordinateTransformer::WorldToTile(transform->GetPosition());
        lightBuffer->SetDynamicLight(entityId, tile, TileLayerType::Ground,
                                     std::make_unique<LightSource>(tile, lightResource->lightRadius, *lightColor));
        currentEntities.insert(entityId);
    }

    for (const GameEntityID entityId: lastLightEntities_) {
        if (!currentEntities.contains(entityId)) {
            lightBuffer->RemoveDynamicLight(entityId);
        }
    }
    lastLightEntities_ = std::move(currentEntities);
}

uint8_t glimmer::DynamicLightSystem::GetExecutionOrder() {
    return EXECUTION_ORDER_DYNAMIC_LIGHT;
}

glimmer::GameSystemType glimmer::DynamicLightSystem::GetGameSystemType() const {
    return GameSystemType::DynamicLightSystem;
}
