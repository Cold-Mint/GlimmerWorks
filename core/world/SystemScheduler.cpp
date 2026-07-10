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
#include "SystemScheduler.h"

#include <ranges>
#include <utility>

#include "core/ecs/EntityManager.h"
#include "core/ecs/system/AreaMarkerSystem.h"
#include "core/ecs/system/AutoPickSystem.h"
#include "core/ecs/system/BiomeBGMSystem.h"
#include "core/ecs/system/BlueprintSystem.h"
#include "core/ecs/system/ButtonSystem.h"
#include "core/ecs/system/CameraSystem.h"
#include "core/ecs/system/ChunkSystem.h"
#include "core/ecs/system/CraftPreviewSlotSystem.h"
#include "core/ecs/system/DebugDrawBox2dSystem.h"
#include "core/ecs/system/DebugDrawSystem.h"
#include "core/ecs/system/DebugMultiMapSystem.h"
#include "core/ecs/system/DebugPanelSystem.h"
#include "core/ecs/system/DiggingSystem.h"
#include "core/ecs/system/DraggableSystem.h"
#include "core/ecs/system/DroppedItemSystem.h"
#include "core/ecs/system/FloatingTextSystem.h"
#include "core/ecs/system/HotBarGUISystem.h"
#include "core/ecs/system/InventoryCraftGUISystem.h"
#include "core/ecs/system/ItemSlotQuantitySystem.h"
#include "core/ecs/system/ItemSlotSystem.h"
#include "core/ecs/system/ItemToolTipSystem.h"
#include "core/ecs/system/Light2DSystem.h"
#include "core/ecs/system/MagnetSystem.h"
#include "core/ecs/system/MaterialSelectCraftUISystem.h"
#include "core/ecs/system/ParallaxBackgroundSystem.h"
#include "core/ecs/system/PauseSystem.h"
#include "core/ecs/system/PhysicsSystem.h"
#include "core/ecs/system/PlayerControlSystem.h"
#include "core/ecs/system/RayCast2DSystem.h"
#include "core/ecs/system/SpiritRendererSystem.h"
#include "core/ecs/system/TechProviderSystem.h"
#include "core/ecs/system/TileLayerSystem.h"
#include "core/log/LogCat.h"
#include "core/world/WorldContext.h"
#ifdef __ANDROID__
#include "core/ecs/system/AndroidControlSystem.h"
#endif

glimmer::SystemScheduler::SystemScheduler(WorldContext* worldContext) : worldContext_(worldContext)
{
    EntityManager* entityManager = worldContext_->GetEntityManager();
    if (entityManager != nullptr)
    {
        onComponentCountChangedId_ = entityManager->RegisterOnComponentCountChanged(
            [this](GameComponentTypeMessage type, uint32_t count)
            {
                OnWatchedComponentChanged(type, count);
            });
    }
}

glimmer::SystemScheduler::~SystemScheduler()
{
    if (onComponentCountChangedId_ != 0 && worldContext_ != nullptr)
    {
        EntityManager* entityManager = worldContext_->GetEntityManager();
        if (entityManager != nullptr)
        {
            entityManager->UnRegisterOnComponentCountChanged(onComponentCountChangedId_);
        }
    }
}

void glimmer::SystemScheduler::PushGuiSystemType(GameSystemType systemType)
{
    activeSystemStack_.emplace(systemType);
}

void glimmer::SystemScheduler::PushPersistentGuiSystem(GameSystemType systemType)
{
    persistentGuiSystemCount_++;
    activeSystemStack_.emplace(systemType);
}

glimmer::GameSystemType glimmer::SystemScheduler::GetGuiSystemType() const
{
    if (activeSystemStack_.empty())
    {
        return GameSystemType::None;
    }
    return activeSystemStack_.top();
}


void glimmer::SystemScheduler::PopGuiSystemType()
{
    if (activeSystemStack_.size() > persistentGuiSystemCount_)
    {
        activeSystemStack_.pop();
    }
}

std::vector<glimmer::GameSystemType> glimmer::SystemScheduler::GetAllActiveSystemType() const
{
    std::vector<GameSystemType> result;
    for (auto& activeSystem : activeSystems_)
    {
        if (activeSystem == nullptr)
        {
            continue;
        }
        result.emplace_back(activeSystem->GetGameSystemType());
    }
    return result;
}

void glimmer::SystemScheduler::OnWatchedComponentChanged(const GameComponentTypeMessage type,
                                                         const uint32_t count)
{
    onComponentCountChangeBuffer_[type] = count;
}

bool glimmer::SystemScheduler::HasAnyModalGuiOpen() const
{
    return activeSystemStack_.size() > persistentGuiSystemCount_;
}

bool glimmer::SystemScheduler::HandleEvent(const SDL_Event& event) const
{
    bool handled = false;
    for (auto& system : activeSystems_)
    {
        if (system == nullptr)
        {
            continue;
        }
        if (!worldContext_->IsRuning() && !system->CanRunWhilePaused())
        {
            continue;
        }
        if (system->HandleEvent(event))
        {
            handled = true;
        }
    }
    return handled;
}

void glimmer::SystemScheduler::Update(const float delta) const
{
    for (auto& system : activeSystems_)
    {
        if (system == nullptr)
        {
            continue;
        }
        if (!worldContext_->IsRuning() && !system->CanRunWhilePaused())
        {
            continue;
        }
        system->Update(delta);
    }
}

bool glimmer::SystemScheduler::OnBackPressed()
{
    if (activeSystemStack_.size() > persistentGuiSystemCount_)
    {
        activeSystemStack_.pop();
        return true;
    }
    bool handled = false;
    for (const auto& system : activeSystems_)
    {
        if (system == nullptr)
        {
            continue;
        }
        if (!worldContext_->IsRuning() && !system->CanRunWhilePaused())
        {
            continue;
        }
        if (system->OnBackPressed())
        {
            handled = true;
        }
    }
    return handled;
}

void glimmer::SystemScheduler::Render(SDL_Renderer* renderer) const
{
    for (const std::unique_ptr<GameSystem>& system : activeSystems_)
    {
#if  defined(NDEBUG)
        system->Render(renderer);
#else
        SDL_Color oldColor;
        SDL_GetRenderDrawColor(renderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);
        system->Render(renderer);
        SDL_Color newColor;
        SDL_GetRenderDrawColor(renderer, &newColor.r, &newColor.g, &newColor.b, &newColor.a);
        if (oldColor.a != newColor.a || oldColor.r != newColor.r || oldColor.g != newColor.g || oldColor.b != newColor.
            b)
        {
            LogCat::e("The color of the renderer has been changed by the game system.",
                      std::to_underlying(system->GetGameSystemType()),
                      " invoke AppContext::RestoreColorRenderer(renderer);");
            assert(false);
        }
#endif
    }
}

void glimmer::SystemScheduler::RenderImGui(SDL_Renderer* renderer) const
{
    for (const std::unique_ptr<GameSystem>& system : activeSystems_)
    {
#if  defined(NDEBUG)
        system->RenderImGui(renderer);
#else
        SDL_Color oldColor;
        SDL_GetRenderDrawColor(renderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);
        system->RenderImGui(renderer);
        SDL_Color newColor;
        SDL_GetRenderDrawColor(renderer, &newColor.r, &newColor.g, &newColor.b, &newColor.a);
        if (oldColor.a != newColor.a || oldColor.r != newColor.r || oldColor.g != newColor.g || oldColor.b != newColor.
            b)
        {
            LogCat::e("The color of the renderImGui has been changed by the game system.",
                      std::to_underlying(system->GetGameSystemType()),
                      " invoke AppContext::RestoreColorRenderer(renderer);");
            assert(false);
        }
#endif
    }
}

void glimmer::SystemScheduler::NotifySystemsOfComponentChange(const GameComponentTypeMessage gameComponentType,
                                                              const uint32_t count)
{
    NotifyActiveSystems(gameComponentType, count);
    NotifyInactiveSystems(gameComponentType, count);
}

void glimmer::SystemScheduler::NotifyActiveSystems(const GameComponentTypeMessage gameComponentType,
                                                   const uint32_t count) const
{
    for (auto& system : activeSystems_)
    {
        if (system == nullptr)
        {
            continue;
        }
        if (!system->IsWatchingComponent(gameComponentType))
        {
            continue;
        }
        if (count == 0)
        {
            system->RemoveActiveWatchComponent(gameComponentType);
        }
        system->OnWatchedComponentChanged(gameComponentType, count);
    }
}

void glimmer::SystemScheduler::NotifyInactiveSystems(const GameComponentTypeMessage gameComponentType,
                                                     const uint32_t count)
{
    for (auto& system : inactiveSystems_)
    {
        if (system == nullptr)
        {
            continue;
        }
        if (!system->IsWatchingComponent(gameComponentType))
        {
            continue;
        }
        system->AddActiveWatchComponent(gameComponentType);
        system->OnWatchedComponentChanged(gameComponentType, count);
    }
}

void glimmer::SystemScheduler::OnFrameStart()
{
    std::queue<GameSystem*> toActivate;
    std::queue<GameSystem*> toDeactivate;
    bool changed = false;
    for (auto& buffer : onComponentCountChangeBuffer_)
    {
        const GameComponentTypeMessage gameComponentType = buffer.first;
        const uint32_t count = buffer.second;
        NotifySystemsOfComponentChange(gameComponentType, count);
    }
    for (auto& system : inactiveSystems_)
    {
        if (system == nullptr)
        {
            continue;
        }
        if (system->IsAllWatchComponentsReady() && system->CanActive())
        {
            toActivate.emplace(system.get());
            changed = true;
        }
    }
    for (auto& system : activeSystems_)
    {
        if (system == nullptr)
        {
            continue;
        }
        if (!system->IsAllWatchComponentsReady() || !system->CanActive())
        {
            toDeactivate.emplace(system.get());
            changed = true;
        }
    }
    onComponentCountChangeBuffer_.clear();
    MoveSystemsToActive(toActivate);
    MoveSystemsToInactive(toDeactivate);
    if (changed)
    {
        std::ranges::stable_sort(activeSystems_,
                                 [](const std::unique_ptr<GameSystem>& a, const std::unique_ptr<GameSystem>& b)
                                 {
                                     return a->GetRenderOrder() < b->GetRenderOrder();
                                 });
    }
    for (auto& system : activeSystems_)
    {
        if (system == nullptr)
        {
            continue;
        }
        system->OnFrameStart();
    }
}

void glimmer::SystemScheduler::MoveSystemsToActive(std::queue<GameSystem*>& toActivate)
{
    while (!toActivate.empty())
    {
        const GameSystem* system = toActivate.front();
        toActivate.pop();
        auto it = std::ranges::find_if(inactiveSystems_,
                                       [system](auto& inactiveSystem) { return inactiveSystem.get() == system; });
        if (it == inactiveSystems_.end())
        {
            continue;
        }
        if (GameSystem* systemPtr = it->get(); systemPtr != nullptr)
        {
            systemPtr->OnActivationChanged(true);
        }
        activeSystems_.emplace_back(std::move(*it));
        inactiveSystems_.erase(it);
    }
}

void glimmer::SystemScheduler::MoveSystemsToInactive(std::queue<GameSystem*>& toDeactivate)
{
    while (!toDeactivate.empty())
    {
        const GameSystem* system = toDeactivate.front();
        toDeactivate.pop();
        auto it = std::ranges::find_if(activeSystems_,
                                       [system](auto& activeSystem) { return activeSystem.get() == system; });
        if (it == activeSystems_.end())
        {
            continue;
        }
        if (GameSystem* systemPtr = it->get(); systemPtr != nullptr)
        {
            systemPtr->OnActivationChanged(false);
        }
        inactiveSystems_.emplace_back(std::move(*it));
        activeSystems_.erase(it);
    }
}

void glimmer::SystemScheduler::InitSystem()
{
    allowRegisterSystem = true;
    RegisterSystem(std::make_unique<CameraSystem>(worldContext_));
    RegisterSystem(std::make_unique<PlayerControlSystem>(worldContext_));
    RegisterSystem(std::make_unique<TileLayerSystem>(worldContext_));
    RegisterSystem(std::make_unique<ChunkSystem>(worldContext_));
    RegisterSystem(std::make_unique<PhysicsSystem>(worldContext_));
    RegisterSystem(std::make_unique<ItemSlotSystem>(worldContext_));
    RegisterSystem(std::make_unique<MagnetSystem>(worldContext_));
    RegisterSystem(std::make_unique<ParallaxBackgroundSystem>(worldContext_));
    RegisterSystem(std::make_unique<FloatingTextSystem>(worldContext_));
    RegisterSystem(std::make_unique<DroppedItemSystem>(worldContext_));
    RegisterSystem(std::make_unique<AutoPickSystem>(worldContext_));
    RegisterSystem(std::make_unique<AreaMarkerSystem>(worldContext_));
    RegisterSystem(std::make_unique<DiggingSystem>(worldContext_));
    RegisterSystem(std::make_unique<DraggableSystem>(worldContext_));
    RegisterSystem(std::make_unique<SpiritRendererSystem>(worldContext_));
    RegisterSystem(std::make_unique<PauseSystem>(worldContext_));
    RegisterSystem(std::make_unique<RayCast2DSystem>(worldContext_));
    RegisterSystem(std::make_unique<BiomeBGMSystem>(worldContext_));
    RegisterSystem(std::make_unique<Light2DSystem>(worldContext_));
    RegisterSystem(std::make_unique<BlueprintSystem>(worldContext_));
    RegisterSystem(std::make_unique<HotBarGUISystem>(worldContext_));
    RegisterSystem(std::make_unique<InventoryCraftGUISystem>(worldContext_));
    RegisterSystem(std::make_unique<CraftPreviewSlotSystem>(worldContext_));
    RegisterSystem(std::make_unique<MaterialSelectCraftUISystem>(worldContext_));
    RegisterSystem(std::make_unique<ItemToolTipSystem>(worldContext_));
    RegisterSystem(std::make_unique<ItemSlotQuantitySystem>(worldContext_));
    RegisterSystem(std::make_unique<ButtonSystem>(worldContext_));
    RegisterSystem(std::make_unique<TechProviderSystem>(worldContext_));
#if  !defined(NDEBUG)
    RegisterSystem(std::make_unique<DebugDrawSystem>(worldContext_));
    RegisterSystem(std::make_unique<DebugDrawBox2dSystem>(worldContext_));
    RegisterSystem(std::make_unique<DebugPanelSystem>(worldContext_));
    RegisterSystem(std::make_unique<DebugMultiMapSystem>(worldContext_));
#endif
#ifdef __ANDROID__
    RegisterSystem(std::make_unique<AndroidControlSystem>(worldContext_));
#endif
    allowRegisterSystem = false;
    PushPersistentGuiSystem(GameSystemType::HotBarGUISystem);
}

void glimmer::SystemScheduler::OnConfigChanged(const Config* config)
{
    for (const auto& activeSystem : activeSystems_)
    {
        activeSystem->OnConfigChanged(config);
    }
    for (const auto& inactiveSystem : inactiveSystems_)
    {
        inactiveSystem->OnConfigChanged(config);
    }
}

void glimmer::SystemScheduler::RegisterSystem(std::unique_ptr<GameSystem> system)
{
    if (allowRegisterSystem)
    {
        system->LockWatchComponent();
        inactiveSystems_.emplace_back(std::move(system));
    }
    else
    {

    }
}

void glimmer::SystemScheduler::OnWindowSizeChanged(const int& width, const int& height) const
{
    for (auto& activeSystem : activeSystems_)
    {
        activeSystem->OnWindowSizeChanged(width, height);
    }
    for (auto& inactiveSystem : inactiveSystems_)
    {
        inactiveSystem->OnWindowSizeChanged(width, height);
    }
}
