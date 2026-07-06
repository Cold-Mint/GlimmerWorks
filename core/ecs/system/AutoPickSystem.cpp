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
#include "AutoPickSystem.h"

#include <strstream>

#include "core/world/WorldContext.h"
#include "core/ecs/component/DroppedItemComponent.h"
#include "core/ecs/FlowingTextCreator.h"
#include "core/ecs/component/MagnetComponent.h"
#include "core/mod/resourcePack/AudioResourceResult.h"


void glimmer::AutoPickSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    const EntityManager* entityManager = GetEntityManager();
    if (gameComponentType == COMPONENT_AUTO_PICK)
    {
        autoPickCount_ = count;
    }
    if (gameComponentType == COMPONENT_MAGNET)
    {
        magnetCount_ = count;
    }
    if (gameComponentType == COMPONENT_ITEM_CONTAINER)
    {
        itemContainerCount_ = count;
    }
    if (autoPickCount_ > 0 && magnetCount_ > 0 && itemContainerCount_ > 0)
    {
        entities_ = entityManager->GetEntityIDWithComponents({
            COMPONENT_MAGNET, COMPONENT_ITEM_CONTAINER
        });
    }
}

glimmer::AutoPickSystem::AutoPickSystem(WorldContext* worldContext) : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_AUTO_PICK);
    WatchComponent(COMPONENT_MAGNET);
    WatchComponent(COMPONENT_ITEM_CONTAINER);
    const AppContext* appContext = worldContext->GetAppContext();
    if (appContext == nullptr)
    {
        return;
    }
    ResourceRef ref;
    ref.SetSelfPackageId(RESOURCE_REF_CORE);
    ref.SetResourceType(RESOURCE_AUDIO);
    ref.SetResourceKey("sfx/pick_item");
    pickItemSFXResult_ = appContext->GetResourceLocator()->FindAudio(&ref);
    audioManager_ = appContext->GetAudioManager();
    Init();
}

void glimmer::AutoPickSystem::TryMergeFlowingText()
{
    WorldContext* worldContext = GetWorldContext();
    EntityManager* entityManager = GetEntityManager();
    if (frameItemCounts_.empty())
    {
        return;
    }
    std::stringstream stringStream{};
    for (auto& [itemName, count] : frameItemCounts_)
    {
        stringStream << itemName;
        if (count <= 1)
        {
            continue;
        }
        stringStream << " * " << count << "\n";
    }

    FlowingTextCreator flowingTextCreator(worldContext, stringStream.str(), lastPosition);
    flowingTextCreator.LoadTemplateComponents(entityManager->AddEntity());
    frameItemCounts_.clear();
}

void glimmer::AutoPickSystem::ProcessMagnetEntity(GameEntityID entity)
{
    EntityManager* entityManager = GetEntityManager();
    const auto magnetComponent = entityManager->GetComponent<MagnetComponent>(entity);
    const auto containerComponent = entityManager->GetComponent<ItemContainerComponent>(entity);
    if (magnetComponent == nullptr || containerComponent == nullptr)
    {
        return;
    }
    const auto itemContainer = containerComponent->GetItemContainer();
    if (itemContainer == nullptr)
    {
        return;
    }
    for (auto& entities = magnetComponent->GetEntities(); uint32_t entityId : entities)
    {
        auto transform2DComponent = entityManager->GetComponent<Transform2DComponent>(entityId);
        if (transform2DComponent != nullptr)
        {
            lastPosition = transform2DComponent->GetPosition();
        }

        auto droppedItemComponent = entityManager->GetComponent<DroppedItemComponent>(entityId);
        if (droppedItemComponent == nullptr)
        {
            continue;
        }

        auto extractItem = droppedItemComponent->ExtractItem();
        if (extractItem == nullptr)
        {
            continue;
        }

        const std::string& itemName = extractItem->GetName();
        frameItemCounts_[itemName] += extractItem->GetAmount();

        auto item = itemContainer->AddItem(std::move(extractItem));
        if (item == nullptr || pickItemSFXResult_ == nullptr)
        {
            return;
        }
        MIX_Audio* audio = pickItemSFXResult_->GetResource();
        if (audio != nullptr)
        {
            audioManager_->TryPlayFree(AMBIENT, audio, 0);
        }
        entityManager->RemoveEntity(entityId);
    }
}

void glimmer::AutoPickSystem::Update(const float delta)
{
    if (audioManager_ == nullptr)
    {
        return;
    }
    if (remainingTime_ <= 0)
    {
        TryMergeFlowingText();
        remainingTime_ = MERGE_DURATION;
    }
    remainingTime_ -= delta;
    for (const auto entity : entities_)
    {
        ProcessMagnetEntity(entity);
    }
}

glimmer::GameSystemType glimmer::AutoPickSystem::GetGameSystemType() const
{
    return GameSystemType::AutoPickSystem;
}
