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
#include "GameSystem.h"
#include "../world/WorldContext.h"


void glimmer::GameSystem::OnActivationChanged(const bool activeStatus)
{
}

void glimmer::GameSystem::AddActiveWatchComponent(GameComponentTypeMessage gameComponentType)
{
    activeWatchComponents_.emplace(gameComponentType);
}

void glimmer::GameSystem::RemoveActiveWatchComponent(GameComponentTypeMessage gameComponentType)
{
    activeWatchComponents_.erase(gameComponentType);
}

bool glimmer::GameSystem::IsAllWatchComponentsReady() const
{
    return activeWatchComponents_ == watchComponents_;
}

bool glimmer::GameSystem::CanActive() const
{
    return true;
}

void glimmer::GameSystem::WatchComponent(const GameComponentTypeMessage gameComponentType)
{
    if (lockWatchComponents_)
    {
        return;
    }
    watchComponents_.insert(gameComponentType);
    //When observing the changes of a certain component, a callback will be triggered first.
    //当开始观察某个组件的变化时，会优先进行一次回调。
    OnWatchedComponentChanged(gameComponentType, entityManager_->GetComponentCount(gameComponentType));
}

void glimmer::GameSystem::LockWatchComponent()
{
    lockWatchComponents_ = true;
}

void glimmer::GameSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
}


glimmer::GameSystem::GameSystem(WorldContext* worldContext) : worldContext_(worldContext)
{
    entityManager_ = worldContext_->GetEntityManager();
    entityShortCut_ = worldContext_->GetEntityShortCut();
}


bool glimmer::GameSystem::CanRunWhilePaused() const
{
    return false;
}

bool glimmer::GameSystem::IsWatchingComponent(GameComponentTypeMessage gameComponentType) const
{
    return watchComponents_.contains(gameComponentType);
}

bool glimmer::GameSystem::HandleEvent(const SDL_Event& event)
{
    return false;
}

void glimmer::GameSystem::OnConfigChanged(const Config* config)
{

}

void glimmer::GameSystem::OnWindowSizeChanged(int width, int height)
{

}

bool glimmer::GameSystem::OnBackPressed()
{
    return false;
}

void glimmer::GameSystem::Update(float delta)
{
}

uint8_t glimmer::GameSystem::GetRenderOrder()
{
    return 0;
}

void glimmer::GameSystem::Render(SDL_Renderer* renderer)
{
}
