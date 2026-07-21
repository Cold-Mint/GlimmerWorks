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
#include "EntityManager.h"

#include "component/AutoPickComponent.h"
#include "core/log/LogCat.h"
#include "component/DebugDrawComponent.h"
#include "component/DroppedItemComponent.h"
#include "component/ItemContainerComponent.h"
#include "component/MagnetComponent.h"
#include "component/MagneticComponent.h"
#include "core/world/WorldContext.h"

ComponentFingerprint glimmer::EntityManager::GenComponentFingerprint(const GameEntityID gameEntityId,
                                                                     const GameComponentTypeMessage typeMessage)
{
    const auto rawType = static_cast<std::uint32_t>(typeMessage);
    return static_cast<std::uint64_t>(gameEntityId) << 32 | rawType;
}

void glimmer::EntityManager::Clear()
{
    entityMap_.clear();
    entityToGameComponentType_.clear();
    components_.clear();
    componentCount_.clear();
    onComponentCountChanged_.clear();
}

GameEntityID glimmer::EntityManager::AddEntity(GameEntityID gameEntityId)
{
    if (gameEntityId == GAME_ENTITY_ID_INVALID)
    {
        ++entityIndex_;
        if (entityIndex_ == GAME_ENTITY_ID_INVALID)
        {
            //It is prohibited to use illegal IDs.
            //禁止占用非法ID。
            ++entityIndex_;
        }
        entityMap_.emplace(entityIndex_, std::make_unique<GameEntity>(entityIndex_));
        LogCat::d("Entity created: id=", entityIndex_);
        return entityIndex_;
    }
    entityMap_.emplace(gameEntityId, std::make_unique<GameEntity>(gameEntityId));
    LogCat::d("Entity created: id=", gameEntityId);
    return gameEntityId;
}

std::vector<GameEntityID> glimmer::EntityManager::GetAllEntityIDs() const
{
    std::vector<GameEntityID> entityIds;
    entityIds.reserve(entityMap_.size());
    for (auto& entity : entityMap_)
    {
        entityIds.emplace_back(entity.first);
    }
    return entityIds;
}

uint32_t glimmer::EntityManager::RegisterOnComponentCountChanged(
    const std::function<void(GameComponentTypeMessage, uint32_t)>& onComponentCountChanged)
{
    ++onComponentCountChangedId_;
    std::pair<uint32_t, std::function<void(GameComponentTypeMessage, uint32_t)>> pair;
    pair.first = onComponentCountChangedId_;
    pair.second = onComponentCountChanged;
    onComponentCountChanged_.emplace_back(pair);
    return onComponentCountChangedId_;
}

void glimmer::EntityManager::UnRegisterOnComponentCountChanged(uint32_t id)
{
    auto it = std::remove_if(onComponentCountChanged_.begin(), onComponentCountChanged_.end(),
                             [id](const auto& item)
                             {
                                 return item.first == id;
                             });
    onComponentCountChanged_.erase(it, onComponentCountChanged_.end());
}

std::vector<glimmer::GameComponent*> glimmer::EntityManager::GetAllComponent(const GameEntityID gameEntityId)
{
    std::vector<GameComponent*> components;
    auto entityToGameComponentTypeIterator = entityToGameComponentType_.find(gameEntityId);
    if (entityToGameComponentTypeIterator != entityToGameComponentType_.end())
    {
        for (auto componentTypeMessage : entityToGameComponentTypeIterator->second)
        {
            auto componentFingerprint = GenComponentFingerprint(gameEntityId, componentTypeMessage);
            const auto componentIterator = components_.find(componentFingerprint);
            if (componentIterator != components_.end())
            {
                std::unique_ptr<GameComponent>& component = componentIterator->second;
                if (component == nullptr)
                {
                    continue;
                }
                components.emplace_back(component.get());
            }
        }
    }

    return components;
}

uint32_t glimmer::EntityManager::GetComponentCount(const GameComponentTypeMessage componentType)
{
    const auto iterator = componentCount_.find(componentType);
    if (iterator == componentCount_.end())
    {
        return 0;
    }
    return iterator->second;
}

void glimmer::EntityManager::RemoveEntity(const GameEntityID gameEntityId)
{
    LogCat::d("Entity removing: id=", gameEntityId);
    entityMap_.erase(gameEntityId);
    auto entityToGameComponentTypeIterator = entityToGameComponentType_.find(gameEntityId);
    if (entityToGameComponentTypeIterator != entityToGameComponentType_.end())
    {
        int componentCount = 0;
        for (auto componentTypeMessage : entityToGameComponentTypeIterator->second)
        {
            auto componentFingerprint = GenComponentFingerprint(gameEntityId, componentTypeMessage);
            const auto componentIterator = components_.find(componentFingerprint);
            if (componentIterator != components_.end())
            {
                components_.erase(componentIterator);
                componentCount++;
                auto componentCountIterator = componentCount_.find(componentTypeMessage);
                if (componentCountIterator != componentCount_.end())
                {
                    --componentCountIterator->second;
                    const uint32_t newCount = componentCountIterator->second;
                    if (newCount == 0)
                    {
                        componentCount_.erase(componentCountIterator);
                    }
                    for (auto& callBack : onComponentCountChanged_)
                    {
                        callBack.second(componentTypeMessage, newCount);
                    }
                }
            }
        }
        LogCat::d("Entity removed: id=", gameEntityId, ", components removed: ", componentCount);
    }
    entityIndex_++;
}

void glimmer::EntityManager::SetEntityIndex(const GameEntityID entityIndex)
{
    entityIndex_ = entityIndex;
}

GameEntityID glimmer::EntityManager::GetEntityIndex() const
{
    return entityIndex_;
}


void glimmer::EntityManager::RemoveComponent(const GameEntityID gameEntityId,
                                             const GameComponentTypeMessage typeMessage)
{
    auto componentFingerprint = GenComponentFingerprint(gameEntityId, typeMessage);
    const auto componentIterator = components_.find(componentFingerprint);
    if (componentIterator == components_.end())
    {
        return;
    }
    const auto& componentPtr = componentIterator->second;
    if (componentPtr == nullptr)
    {
        //Remove the invalid pointers.
        //移除失效的指针。
        components_.erase(componentIterator);
        return;
    }

    components_.erase(componentIterator);
    LogCat::d("Component removed: entityId=", gameEntityId, ", type=", static_cast<int>(typeMessage));
    auto entityToGameComponentTypeIterator = entityToGameComponentType_.find(gameEntityId);
    if (entityToGameComponentTypeIterator != entityToGameComponentType_.end())
    {
        auto& unorderedSet = entityToGameComponentTypeIterator->second;
        unorderedSet.erase(typeMessage);
        if (unorderedSet.empty())
        {
            entityToGameComponentType_.erase(entityToGameComponentTypeIterator);
        }
    }
    const auto countIterator = componentCount_.find(typeMessage);
    if (countIterator != componentCount_.end())
    {
        --countIterator->second;
        const uint32_t newCount = countIterator->second;
        // When the count reaches zero, invalid keys in the hash table will be cleared.
        // 计数归零则清理哈希表无效键
        if (newCount == 0)
        {
            componentCount_.erase(countIterator);
        }
        for (auto& callBack : onComponentCountChanged_)
        {
            callBack.second(typeMessage, newCount);
        }
    }
}

bool glimmer::EntityManager::SetPersistable(const GameEntityID gameEntityId, const bool persistable)
{
    auto iterator = entityMap_.find(gameEntityId);
    if (iterator == entityMap_.end())
    {
        return false;
    }
    iterator->second->SetPersistable(persistable);
    return true;
}

bool glimmer::EntityManager::SetResourceRef(const GameEntityID gameEntityId, const ResourceRef& resourceRef)
{
    auto iterator = entityMap_.find(gameEntityId);
    if (iterator == entityMap_.end())
    {
        return false;
    }
    iterator->second->SetResourceRef(resourceRef);
    return true;
}

const glimmer::ResourceRef* glimmer::EntityManager::GetResourceRef(const GameEntityID gameEntityId)
{
    auto iterator = entityMap_.find(gameEntityId);
    if (iterator == entityMap_.end())
    {
        return nullptr;
    }
    return &iterator->second->GetResourceRef();
}

bool glimmer::EntityManager::IsPersistable(const GameEntityID gameEntityId)
{
    auto iterator = entityMap_.find(gameEntityId);
    if (iterator == entityMap_.end())
    {
        return false;
    }
    return iterator->second->IsPersistable();
}

bool glimmer::EntityManager::HasComponent(GameEntityID gameEntityId, const GameComponentTypeMessage typeMessage) const
{
    return components_.contains(GenComponentFingerprint(gameEntityId, typeMessage));
}

std::vector<GameEntityID> glimmer::EntityManager::GetEntityIDWithComponents(
    const std::vector<GameComponentTypeMessage>& targetComponentType) const
{
    std::vector<GameEntityID> result;
    const size_t number = targetComponentType.size();
    for (auto& entityToGameComponentType : entityToGameComponentType_)
    {
        const std::unordered_set<GameComponentTypeMessage>& gameComponentTypeMessageSet = entityToGameComponentType.
            second;
        if (gameComponentTypeMessageSet.size() < number)
        {
            //If the number of elements in the Set is less than the number we are looking for, then skip this Set.
            //如果Set内的数量比我们要查找的数量少，那么跳过这个Set。
            continue;
        }
        bool missComponent = false;
        for (auto targetComponent : targetComponentType)
        {
            if (!gameComponentTypeMessageSet.contains(targetComponent))
            {
                //Found the missing component.
                //发现缺失的组件。
                missComponent = true;
                break;
            }
        }
        if (missComponent)
        {
            continue;
        }
        result.push_back(entityToGameComponentType.first);
    }
    return result;
}

void glimmer::EntityManager::RecoveryComponent(WorldContext* worldContext, GameEntityID gameEntityId,
                                               const ComponentMessage& componentMessage)
{
    //Note: Within this method, this part of the code only includes components that can be serialized. That is, components that override the Serialize() method.
    //注意：在这个方法内，这部分代码只写可被序列化的组件。即覆盖了Serialize()方法的组件。
    GameComponent* gameComponent = nullptr;
    switch (componentMessage.type())
    {
    case COMPONENT_DROPPED_ITEM:
        gameComponent = AddComponent<DroppedItemComponent>(gameEntityId);
        break;
    case COMPONENT_ITEM_CONTAINER:
        gameComponent = AddComponent<ItemContainerComponent>(gameEntityId);
        break;
    case COMPONENT_TRANSFORM_2D:
        gameComponent = AddComponent<Transform2DComponent>(gameEntityId);
        break;
    default:
        LogCat::w(std::source_location::current(), "Irrecoverable component type ",
                  std::to_underlying(componentMessage.type()));
        return;
    }
    if (gameComponent == nullptr)
    {
        LogCat::w(std::source_location::current(), "When restoring the component, an empty object was returned. ");
        return;
    }
    gameComponent->Deserialize(worldContext, componentMessage.data());
}
