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
#include "SceneCommand.h"
#if  !defined(NDEBUG)
#include <string>

#include "fmt/xchar.h"
#include "core/LangsResources.h"
#include "core/context/AppContext.h"
#include "core/scene/MainScene.h"
#include "core/scene/SplashScene.h"
#include "core/scene/SavedGamesScene.h"
#include "core/scene/CreateWorldScene.h"
#include "core/scene/SceneManager.h"
#include "core/scene/MainThreadDispatcher.h"
#include "core/Constants.h"

void glimmer::SceneCommand::InitSuggestions(NodeTree<std::string>* suggestionsTree)
{
    suggestionsTree->AddChild("push")->AddChild(SCENE_DYNAMIC_SUGGESTIONS_NAME);
    suggestionsTree->AddChild("replace")->AddChild(SCENE_DYNAMIC_SUGGESTIONS_NAME);
    suggestionsTree->AddChild("pop")->AddChild(SCENE_DYNAMIC_SUGGESTIONS_NAME);
    suggestionsTree->AddChild("list");
}

glimmer::SceneCommand::SceneCommand(AppContext* appContext) : Command(appContext)
{
}

const std::string& glimmer::SceneCommand::GetName() const
{
    return SCENE_COMMAND_NAME;
}

void glimmer::SceneCommand::PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings)
{
    if (commandArgs == nullptr || strings == nullptr)
    {
        return;
    }
    strings->emplace_back("[action:string]");
    const int size = commandArgs->GetSize();
    if (size > 1)
    {
        const std::string action = commandArgs->AsString(1);
        if (action != "list")
        {
            strings->emplace_back("[sceneName:string]");
        }
    }
}

bool glimmer::SceneCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                                    const std::function<void(const std::string& text)>* onMessage)
{
    AppContext* appContext = GetAppContext();
    if (appContext == nullptr || commandArgs == nullptr || onMessage == nullptr)
    {
        return false;
    }
    const std::function<void(const std::string& text)>& onMessageRef = *onMessage;
    const LangsResources* langsResources = appContext->GetLangsResources();
    if (langsResources == nullptr)
    {
        return false;
    }

    const int size = commandArgs->GetSize();
    if (size < 2)
    {
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            2, size));
        return false;
    }

    const std::string action = commandArgs->AsString(1);
    SceneManager* sceneManager = appContext->GetSceneManager();
    if (sceneManager == nullptr)
    {
        return false;
    }
    if (action == "list")
    {
        onMessageRef("Available scenes:");
        onMessageRef("Scene stack count: " + std::to_string(sceneManager->GetSceneCount()));
        return true;
    }

    if (action == "pop")
    {
        appContext->GetMainThreadDispatcher()->PostToNextMainFrame([sceneManager]
        {
            sceneManager->PopScene();
        });
        onMessageRef("Scene pop scheduled for next frame");
        return true;
    }

    if (size < 3)
    {
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            3, size));
        return false;
    }
    const std::string sceneName = commandArgs->AsString(2);
    if (action == "push")
    {
        MainThreadDispatcher* dispatcher = appContext->GetMainThreadDispatcher();

        if (sceneName == SCENE_NAME_MAIN)
        {
            dispatcher->PostToNextMainFrame([appContext, sceneManager]
            {
                sceneManager->PushScene(std::make_unique<MainScene>(appContext));
            });
        }
        else if (sceneName == SCENE_NAME_SPLASH)
        {
            dispatcher->PostToNextMainFrame([appContext, sceneManager]
            {
                sceneManager->PushScene(std::make_unique<SplashScene>(appContext));
            });
        }
        else if (sceneName == SCENE_NAME_SAVED_GAMES)
        {
            dispatcher->PostToNextMainFrame([appContext, sceneManager]
            {
                sceneManager->PushScene(std::make_unique<SavedGamesScene>(appContext));
            });
        }
        else if (sceneName == SCENE_NAME_CREATE_WORLD)
        {
            dispatcher->PostToNextMainFrame([appContext, sceneManager]
            {
                sceneManager->PushScene(std::make_unique<CreateWorldScene>(appContext));
            });
        }
        else
        {
            onMessageRef("Unknown scene: " + sceneName);
            return false;
        }
        onMessageRef("Scene push scheduled for next frame: " + sceneName);
        return true;
    }

    if (action == "replace")
    {
        MainThreadDispatcher* dispatcher = appContext->GetMainThreadDispatcher();

        if (sceneName == SCENE_NAME_MAIN)
        {
            dispatcher->PostToNextMainFrame([appContext, sceneManager]
            {
                sceneManager->ReplaceScene(std::make_unique<MainScene>(appContext));
            });
        }
        else if (sceneName == SCENE_NAME_SPLASH)
        {
            dispatcher->PostToNextMainFrame([appContext, sceneManager]
            {
                sceneManager->ReplaceScene(std::make_unique<SplashScene>(appContext));
            });
        }
        else if (sceneName == SCENE_NAME_SAVED_GAMES)
        {
            dispatcher->PostToNextMainFrame([appContext, sceneManager]
            {
                sceneManager->ReplaceScene(std::make_unique<SavedGamesScene>(appContext));
            });
        }
        else if (sceneName == SCENE_NAME_CREATE_WORLD)
        {
            dispatcher->PostToNextMainFrame([appContext, sceneManager]
            {
                sceneManager->ReplaceScene(std::make_unique<CreateWorldScene>(appContext));
            });
        }
        else
        {
            onMessageRef("Unknown scene: " + sceneName);
            return false;
        }
        onMessageRef("Scene replace scheduled for next frame: " + sceneName);
        return true;
    }
    onMessageRef("Unknown action: " + action);
    return false;
}
#endif
