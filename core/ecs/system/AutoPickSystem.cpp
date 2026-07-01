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
        entities_ = entityManager_->GetEntityIDWithComponents({
            COMPONENT_MAGNET, COMPONENT_ITEM_CONTAINER
        });
    }
}

glimmer::AutoPickSystem::AutoPickSystem(WorldContext* worldContext) : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_AUTO_PICK);
    WatchComponent(COMPONENT_MAGNET);
    WatchComponent(COMPONENT_ITEM_CONTAINER);
    AppContext* appContext = worldContext->GetAppContext();
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

void glimmer::AutoPickSystem::Update(const float delta)
{
    if (audioManager_ == nullptr)
    {
        return;
    }
    if (remainingTime_ <= 0)
    {
        if (!frameItemCounts_.empty())
        {
            std::stringstream stringStream{};
            for (auto& pair : frameItemCounts_)
            {
                stringStream << pair.first;
                if (pair.second > 1)
                {
                    stringStream << " * " << pair.second << "\n";
                }
            }

            FlowingTextCreator flowingTextCreator(worldContext_, stringStream.str(), lastPosition);
            flowingTextCreator.LoadTemplateComponents(entityManager_->AddEntity());
            frameItemCounts_.clear();
        }
        remainingTime_ = MERGE_DURATION;
    }
    remainingTime_ -= delta;
    for (auto entity : entities_)
    {
        auto* magnetComponent = entityManager_->GetComponent<MagnetComponent>(entity);
        auto* containerComponent = entityManager_->GetComponent<ItemContainerComponent>(
            entity);
        auto& entities = magnetComponent->GetEntities();
        for (uint32_t entityId : entities)
        {
            auto* transform2DComponent = entityManager_->GetComponent<Transform2DComponent>(entityId);
            if (transform2DComponent == nullptr)
            {
                continue;
            }
            lastPosition = transform2DComponent->GetPosition();

            auto* droppedItemComponent = entityManager_->GetComponent<DroppedItemComponent>(entityId);
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

            auto item = containerComponent->GetItemContainer()->AddItem(std::move(extractItem));
            if (item == nullptr && pickItemSFXResult_ != nullptr)
            {
                MIX_Audio* audio = pickItemSFXResult_->GetResource();
                if (audio != nullptr)
                {
                    audioManager_->TryPlayFree(AMBIENT, audio, 0);
                }
            }
            entityManager_->RemoveEntity(entityId);
        }
    }
}

glimmer::GameSystemType glimmer::AutoPickSystem::GetGameSystemType() const
{
    return GameSystemType::AutoPickSystem;
}
