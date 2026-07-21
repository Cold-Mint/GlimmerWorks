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
#include "core/log/LogCat.h"

namespace glimmer
{

    SceneCommand::SceneCommand(AppContext* appContext) : Command(appContext)
    {
    }

    const std::string& SceneCommand::GetName() const
    {
        return SCENE_COMMAND_NAME;
    }

    void SceneCommand::PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings)
    {
        if (commandArgs == nullptr || strings == nullptr)
        {
            return;
        }
        strings->emplace_back("[push|replace|pop|list]");
        strings->emplace_back("[sceneName:string]");
    }

    bool SceneCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
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

        std::string action = commandArgs->AsString(1);

        if (action == "list")
        {
            onMessageRef("Available scenes:");
            onMessageRef("  - main: Main menu scene");
            onMessageRef("  - splash: Splash screen scene");
            onMessageRef("  - savedGames: Saved games scene");
            onMessageRef("  - createWorld: Create world scene");
            onMessageRef("Note: world scene cannot be created directly, use createWorld scene instead");
            onMessageRef("Scene stack count: " + std::to_string(appContext->GetSceneManager()->GetSceneCount()));
            return true;
        }

        if (action == "pop")
        {
            appContext->GetMainThreadDispatcher()->PostToNextMainFrame([appContext]()
            {
                appContext->GetSceneManager()->PopScene();
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

        std::string sceneName = commandArgs->AsString(2);
        SceneManager* sceneManager = appContext->GetSceneManager();

        if (action == "push")
        {
            MainThreadDispatcher* dispatcher = appContext->GetMainThreadDispatcher();
            
            if (sceneName == "main")
            {
                dispatcher->PostToNextMainFrame([appContext]()
                {
                    appContext->GetSceneManager()->PushScene(std::make_unique<MainScene>(appContext));
                });
            }
            else if (sceneName == "splash")
            {
                dispatcher->PostToNextMainFrame([appContext]()
                {
                    appContext->GetSceneManager()->PushScene(std::make_unique<SplashScene>(appContext));
                });
            }
            else if (sceneName == "savedGames")
            {
                dispatcher->PostToNextMainFrame([appContext]()
                {
                    appContext->GetSceneManager()->PushScene(std::make_unique<SavedGamesScene>(appContext));
                });
            }
            else if (sceneName == "createWorld")
            {
                dispatcher->PostToNextMainFrame([appContext]()
                {
                    appContext->GetSceneManager()->PushScene(std::make_unique<CreateWorldScene>(appContext));
                });
            }
            else if (sceneName == "world")
            {
                onMessageRef("World scene cannot be created directly. Use createWorld scene and click 'Create' button.");
                return false;
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
            
            if (sceneName == "main")
            {
                dispatcher->PostToNextMainFrame([appContext]()
                {
                    appContext->GetSceneManager()->ReplaceScene(std::make_unique<MainScene>(appContext));
                });
            }
            else if (sceneName == "splash")
            {
                dispatcher->PostToNextMainFrame([appContext]()
                {
                    appContext->GetSceneManager()->ReplaceScene(std::make_unique<SplashScene>(appContext));
                });
            }
            else if (sceneName == "savedGames")
            {
                dispatcher->PostToNextMainFrame([appContext]()
                {
                    appContext->GetSceneManager()->ReplaceScene(std::make_unique<SavedGamesScene>(appContext));
                });
            }
            else if (sceneName == "createWorld")
            {
                dispatcher->PostToNextMainFrame([appContext]()
                {
                    appContext->GetSceneManager()->ReplaceScene(std::make_unique<CreateWorldScene>(appContext));
                });
            }
            else if (sceneName == "world")
            {
                onMessageRef("World scene cannot be created directly. Use createWorld scene and click 'Create' button.");
                return false;
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
}