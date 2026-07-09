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
#include "core/world/WorldContext.h"


void glimmer::GameSystem::OnActivationChanged(const bool activeStatus)
{
    //Here, events related to the disabling or enabling of the system are handled.
    //在这里处理系统被禁用或启用的事件。
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

glimmer::WorldContext* glimmer::GameSystem::GetWorldContext() const
{
    return worldContext_;
}

glimmer::EntityManager* glimmer::GameSystem::GetEntityManager() const
{
    return entityManager_;
}

glimmer::EntityShortCut* glimmer::GameSystem::GetEntityShortCut() const
{
    return entityShortCut_;
}

void glimmer::GameSystem::LockWatchComponent()
{
    lockWatchComponents_ = true;
}

void glimmer::GameSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    //You could consider updating the values of the member variables here.
    //可以考虑在这里将更新成员变量的值。
}


glimmer::GameSystem::GameSystem(WorldContext* worldContext) : worldContext_(worldContext)
{
    entityManager_ = worldContext_->GetEntityManager();
    entityShortCut_ = worldContext_->GetEntityShortCut();
}

void glimmer::GameSystem::Init()
{
    initSubclassFinish_ = true;
    const AppContext* appContext = worldContext_->GetAppContext();
    if (appContext == nullptr)
    {
        return;
    }
    OnWindowSizeChanged(appContext->GetWindowContext()->GetWindowWidth(), appContext->GetWindowContext()->GetWindowHeight());
    const Config* config = appContext->GetConfig();
    if (config != nullptr)
    {
        OnConfigChanged(config);
    }
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
    //Rewrite this function to handle configuration changes.
    //重写这个函数，处理配置变更。
}

void glimmer::GameSystem::OnWindowSizeChanged(const int& width, const int& height)
{
    //Rewrite this function so that it performs certain actions when the window size changes.
    //重写这个函数，以便在窗口尺寸发生改变时，做些什么。
}

bool glimmer::GameSystem::OnBackPressed()
{
    return false;
}

void glimmer::GameSystem::OnFrameStart()
{
    //Carry out the preparatory work before processing each frame.
    //处理每帧开始前的工作。
}

void glimmer::GameSystem::Update(const float delta)
{
#if  !defined(NDEBUG)
    if (!initSubclassFinish_)
    {
        initTimeOut_ += delta;
        if (initTimeOut_ > 2)
        {
            LogCat::e("The game system subclass did not call the Init method within its constructor.");
            assert(false);
        }
    }
#endif
}

uint8_t glimmer::GameSystem::GetRenderOrder()
{
    return 0;
}

void glimmer::GameSystem::RenderImGui(SDL_Renderer* renderer)
{
    //Render the ImGui components within this method.
    //渲染Imgui组件在这个方法内。
}

void glimmer::GameSystem::Render(SDL_Renderer* renderer)
{
    //Here, it is drawn using SDL.
    //在这里使用SDL绘制。
}
