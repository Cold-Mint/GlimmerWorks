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
#include <unordered_set>

#include "EcsTypes.h"
#include "GameComponent.h"
#include "GameEntity.h"
#include "core/Constants.h"
#include "core/log/LogCat.h"
#include "src/core/game_component_type.pb.h"
#include "src/saves/component.pb.h"

namespace glimmer
{
    class EntityManager
    {
        GameEntityID entityIndex_ = GAME_ENTITY_ID_INVALID;
        std::unordered_map<GameEntityID, std::unique_ptr<GameEntity>> entityMap_;
        std::unordered_map<GameEntityID, std::unordered_set<GameComponentTypeMessage>> entityToGameComponentType_;
        std::unordered_map<ComponentFingerprint, std::unique_ptr<GameComponent>> components_;
        std::unordered_map<GameComponentTypeMessage, u_int32_t> componentCount_;
        std::vector<std::pair<uint32_t, std::function<void(GameComponentTypeMessage, uint32_t)>>>
        onComponentCountChanged_;
        uint32_t onComponentCountChangedId_ = 0;


        static ComponentFingerprint GenComponentFingerprint(GameEntityID gameEntityId,
                                                            GameComponentTypeMessage typeMessage);

    public:

        void Clear();
        /**
         * CreateEntity
         * 创建实体
         * @param gameEntityId Specify the game entity ID 指定游戏实体Id
         * @return
         */
        GameEntityID AddEntity(GameEntityID gameEntityId = GAME_ENTITY_ID_INVALID);

        std::vector<GameEntityID> GetAllEntityIDs() const;


        [[nodiscard]] uint32_t RegisterOnComponentCountChanged(
            const std::function<void(GameComponentTypeMessage, uint32_t)>& onComponentCountChanged);

        void UnRegisterOnComponentCountChanged(uint32_t id);


        std::vector<GameComponent*> GetAllComponent(GameEntityID gameEntityId);


        /**
         * GetComponentCount
         * 获取组件总数
         * @param componentType
         * @return
         */
        [[nodiscard]] uint32_t GetComponentCount(GameComponentTypeMessage componentType);

        /**
         * RemoveEntity
         * 移除实体
         * @param gameEntityId
         */
        void RemoveEntity(GameEntityID gameEntityId);

        void SetEntityIndex(GameEntityID entityIndex);

        [[nodiscard]] GameEntityID GetEntityIndex() const;

        /**
         * AddComponent
         * 为实体添加组件
         * @tparam TComponent TComponent 组件类型
         * @tparam Args
         * @param gameEntityId gameEntityId 实体id
         * @param args Args 参数列表
         * @return
         */
        template <typename TComponent, typename... Args>
        TComponent* AddComponent(GameEntityID gameEntityId, Args&&... args);


        /**
         * RemoveComponent
         * 移除组件
         * @param gameEntityId
         * @param typeMessage
         */
        void RemoveComponent(GameEntityID gameEntityId, GameComponentTypeMessage typeMessage);


        /**
         * Set whether a certain object is persistent.
         * 设置某个对象是否为可持久化。
         * @param gameEntityId
         * @param persistable
         * @return
         */
        bool SetPersistable(GameEntityID gameEntityId, bool persistable);

        /**
         * Set the resource reference of a certain object.
         * 设置某个对象的资源引用。
         * @param gameEntityId
         * @param resourceRef
         * @return
         */
        bool SetResourceRef(GameEntityID gameEntityId, const ResourceRef& resourceRef);

        const ResourceRef* GetResourceRef(GameEntityID gameEntityId);

        bool IsPersistable(GameEntityID gameEntityId);

        template <typename TComponent>
        TComponent* GetComponent(GameEntityID gameEntityId);


        /**
         * HasComponent
         * 是否有某个组件
         * @param gameEntityId gameEntityId 实体ID
         * @param typeMessage typeMessage 组件类型
         * @return
         */
        bool HasComponent(GameEntityID gameEntityId, GameComponentTypeMessage typeMessage) const;

        /**
         * Obtain the entity that contains the specific component.
         * 获取带有特定组件的实体。
         * Note: This method does not guarantee the order of the vector.
         * 注意：此方法不保证vector的顺序。
         * @param gameComponentTypeMessages
         * @return
         */
        std::vector<GameEntityID> GetEntityIDWithComponents(
            const std::vector<GameComponentTypeMessage>& gameComponentTypeMessages) const;

        /**
         * RecoveryComponent
         * 恢复组件
         * @param worldContext worldContext 世界上下文
         * @param gameEntityId id 游戏实体id
         * @param componentMessage componentMessage 组件消息
         * @return
         */
        void RecoveryComponent(WorldContext* worldContext, GameEntityID gameEntityId,
                               const ComponentMessage& componentMessage);
    };

    template <typename TComponent, typename... Args>
    TComponent* EntityManager::AddComponent(GameEntityID gameEntityId, Args&&... args)
    {
#if  !defined(NDEBUG)
        // Only in the debugging mode, check if any components are attached to empty entities.
        //仅在调试模式检查是否有向空实体挂载组件。
        const auto entityIterator = entityMap_.find(gameEntityId);
        if (entityIterator == entityMap_.end())
        {
            LogCat::e("Entity ", gameEntityId, " does not exist.");
            assert(false);
            return nullptr;
        }
#endif
        const GameComponentTypeMessage typeMessage = TComponent::GetComponentTypeStatic();
        auto componentFingerprint = GenComponentFingerprint(gameEntityId, typeMessage);
        const auto componentIterator = components_.find(componentFingerprint);
        if (componentIterator == components_.end())
        {
            auto gameComponentPtr = std::make_unique<TComponent>(std::forward<Args>(args)...);
            if (gameComponentPtr == nullptr)
            {
                return nullptr;
            }
            TComponent* ptr = gameComponentPtr.get();
            components_.emplace(componentFingerprint, std::move(gameComponentPtr));
            auto entityToGameComponentTypeIterator = entityToGameComponentType_.find(gameEntityId);
            if (entityToGameComponentTypeIterator == entityToGameComponentType_.end())
            {
                entityToGameComponentType_.emplace(gameEntityId,
                                                   std::unordered_set{typeMessage});
            }
            else
            {
                entityToGameComponentTypeIterator->second.insert(typeMessage);
            }
            auto [it, inserted] = componentCount_.try_emplace(typeMessage, 0U);
            const uint32_t count = ++it->second;
            for (auto& callBack : onComponentCountChanged_)
            {
                callBack.second(typeMessage, count);
            }
            return ptr;
        }
        const std::unique_ptr<GameComponent>& gameComponentPtr = componentIterator->second;
        if (gameComponentPtr == nullptr)
        {
            return nullptr;
        }
        return static_cast<TComponent*>(gameComponentPtr.get());
    }

    template <typename TComponent>
    TComponent* EntityManager::GetComponent(GameEntityID gameEntityId)
    {
        const GameComponentTypeMessage typeMessage = TComponent::GetComponentTypeStatic();
        auto componentFingerprint = GenComponentFingerprint(gameEntityId, typeMessage);
        const auto componentIterator = components_.find(componentFingerprint);
        if (componentIterator == components_.end())
        {
            return nullptr;
        }
        const std::unique_ptr<GameComponent>& uniquePtr = componentIterator->second;
        if (uniquePtr == nullptr)
        {
            return nullptr;
        }
        return static_cast<TComponent*>(uniquePtr.get());
    }
}
