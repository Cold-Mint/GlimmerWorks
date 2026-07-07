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
#include "WorldScene.h"

#include "core/saves/SavesManager.h"
#include "core/world/SystemScheduler.h"

glimmer::WorldScene::WorldScene(AppContext* context, std::unique_ptr<WorldContext> worldContext)
    : Scene(context)
{
    worldContext_ = std::move(worldContext);
    if (context != nullptr)
    {
        context->SetWindowTitle((PROJECT_NAME + " - " + worldContext_->GetMapManifest()->name).c_str());
    }
    Init();
}

void glimmer::WorldScene::OnFrameStart()
{
    if (worldContext_ == nullptr)
    {
        return;
    }
    worldContext_->GetSystemScheduler()->OnFrameStart();
}

bool glimmer::WorldScene::HandleEvent(const SDL_Event& event)
{
    if (worldContext_ == nullptr)
    {
        return false;
    }
    SystemScheduler* systemScheduler = worldContext_->GetSystemScheduler();
    if (systemScheduler == nullptr)
    {
        return false;
    }
    if (event.type == SDL_EVENT_KEY_DOWN && !systemScheduler->HasAnyModalGuiOpen())
    {
        //When a certain key is pressed and there is no system display currently active.
        //当按下某个键，且没有系统正在显示中时。
        if (event.key.scancode == SDL_SCANCODE_E)
        {
            systemScheduler->PushGuiSystemType(GameSystemType::InventoryCraftGUISystem);
        }
    }
    return systemScheduler->HandleEvent(event);
}

bool glimmer::WorldScene::OnBackPressed()
{
    if (worldContext_ == nullptr)
    {
        return false;
    }
    SystemScheduler* systemScheduler = worldContext_->GetSystemScheduler();
    if (systemScheduler == nullptr)
    {
        return false;
    }
    return systemScheduler->OnBackPressed();
}

void glimmer::WorldScene::OnWindowClose()
{
    if (worldContext_ == nullptr)
    {
        return;
    }
    worldContext_->SaveGame();
}

void glimmer::WorldScene::Update(float delta)
{
    if (worldContext_ == nullptr)
    {
        return;
    }
    const SystemScheduler* systemScheduler = worldContext_->GetSystemScheduler();
    if (systemScheduler == nullptr)
    {
        return;
    }
    systemScheduler->Update(delta);
}

void glimmer::WorldScene::OnWindowSizeChanged(const int& width, const int& height)
{
    if (worldContext_ == nullptr)
    {
        return;
    }
    const SystemScheduler* systemScheduler = worldContext_->GetSystemScheduler();
    if (systemScheduler == nullptr)
    {
        return;
    }
    systemScheduler->OnWindowSizeChanged(width, height);
}

void glimmer::WorldScene::OnConfigChanged(const Config* config)
{
    if (worldContext_ == nullptr)
    {
        return;
    }
    SystemScheduler* systemScheduler = worldContext_->GetSystemScheduler();
    if (systemScheduler == nullptr)
    {
        return;
    }
    systemScheduler->OnConfigChanged(config);
}

void glimmer::WorldScene::Render(SDL_Renderer* renderer)
{
    if (worldContext_ == nullptr)
    {
        return;
    }
    const SystemScheduler* systemScheduler = worldContext_->GetSystemScheduler();
    if (systemScheduler == nullptr)
    {
        return;
    }
    systemScheduler->Render(renderer);
}

void glimmer::WorldScene::RenderImGui(SDL_Renderer* renderer)
{
    if (worldContext_ == nullptr)
    {
        return;
    }
    const SystemScheduler* systemScheduler = worldContext_->GetSystemScheduler();
    if (systemScheduler == nullptr)
    {
        return;
    }
    systemScheduler->RenderImGui(renderer);
}
