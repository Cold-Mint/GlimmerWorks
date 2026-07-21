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
#include "GuiCommand.h"

#include <string>

#include "fmt/xchar.h"
#include "core/LangsResources.h"
#include "core/context/AppContext.h"
#include "core/scene/SceneManager.h"
#include "core/scene/Scene.h"
#include "core/scene/MainThreadDispatcher.h"
#include "core/scene/CreateWorldScene.h"
#include "core/Constants.h"
#include "RmlUi/Core/Elements/ElementFormControlInput.h"
#include <SDL3/SDL.h>

namespace glimmer
{

    GuiCommand::GuiCommand(AppContext* appContext) : Command(appContext)
    {
    }

    const std::string& GuiCommand::GetName() const
    {
        return GUI_COMMAND_NAME;
    }

    void GuiCommand::PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings)
    {
        if (commandArgs == nullptr || strings == nullptr)
        {
            return;
        }
        strings->emplace_back("[find|text|click|list|focus]");
        strings->emplace_back("[elementId|attr:value|text:content]");
        strings->emplace_back("[text:string]");
    }

    bool GuiCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
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
        SceneManager* sceneManager = appContext->GetSceneManager();

        if (action == "list")
        {
            onMessageRef("GUI commands:");
            onMessageRef("  - gui find <id|attr:name=value|text:content>: Find element");
            onMessageRef("  - gui text <id|attr:name=value|text:content> <text>: Set text to input");
            onMessageRef("  - gui click <id|attr:name=value|text:content>: Simulate click");
            onMessageRef("  - gui position <id|attr:name=value|text:content>: Get element position");
            onMessageRef("  - gui focus <id|attr:name=value|text:content>: Focus on element");
            onMessageRef("  - gui inspect: List all elements in current scene");
            onMessageRef("  - gui list: Show this help");
            return true;
        }

        if (action == "inspect")
        {
            onMessageRef("Inspecting all elements:");
            
            std::function<void(Rml::Element*, int, const std::function<void(const std::string&)>&)> inspectElement;
            inspectElement = [&](Rml::Element* element, int depth, const std::function<void(const std::string&)>& msg) -> void {
                if (element == nullptr) return;
                std::string indent(depth * 2, ' ');
                std::string tag = element->GetTagName().c_str();
                std::string inner = element->GetInnerRML().c_str();
                if (inner.size() > 50) inner = inner.substr(0, 50) + "...";
                msg(indent + "- <" + tag + ">: " + inner);
                
                int childCount = element->GetNumChildren();
                for (int i = 0; i < childCount; ++i)
                {
                    Rml::Element* child = element->GetChild(i);
                    inspectElement(child, depth + 1, msg);
                }
            };
            
            auto inspectScene = [&](Scene* scene, const std::string& sceneName) {
                if (scene == nullptr) {
                    onMessageRef("  Scene " + sceneName + " is null");
                    return;
                }
                
                const auto& docs = scene->GetAllDocuments();
                onMessageRef("  Scene " + sceneName + ": " + std::to_string(docs.size()) + " documents");
                
                for (size_t i = 0; i < docs.size(); ++i)
                {
                    Rml::ElementDocument* doc = docs[i];
                    onMessageRef("    Document " + std::to_string(i) + ": " + 
                                std::string(doc->GetSourceURL().c_str()));
                    inspectElement(doc, 3, onMessageRef);
                }
            };

            const auto& overlayScenes = sceneManager->GetOverlayScenes();
            onMessageRef("  Overlay scenes count: " + std::to_string(overlayScenes.size()));
            for (size_t i = 0; i < overlayScenes.size(); ++i)
            {
                inspectScene(overlayScenes[i], "Overlay " + std::to_string(i));
            }
            
            if (Scene* topScene = sceneManager->GetTopScene(); topScene != nullptr)
            {
                inspectScene(topScene, "Top Scene");
            }
            else
            {
                onMessageRef("  Top Scene is null");
            }
            
            return true;
        }

        if (size < 3)
        {
            onMessageRef(fmt::format(
                fmt::runtime(langsResources->insufficientParameterLength),
                3, size));
            return false;
        }

        std::string elementSpec = commandArgs->AsString(2);

        Rml::Element* targetElement = nullptr;
        std::string foundBy = "";
        std::string elementBy = "";
        std::string attrName = "";
        std::string attrValue = "";

        auto findInScenes = [&](const std::function<Rml::Element*(Scene*)>& finder) -> bool {
            const auto& overlayScenes = sceneManager->GetOverlayScenes();
            for (const auto overlayScene : overlayScenes)
            {
                if (overlayScene != nullptr)
                {
                    targetElement = finder(overlayScene);
                    if (targetElement != nullptr)
                    {
                        return true;
                    }
                }
            }
            if (Scene* topScene = sceneManager->GetTopScene(); topScene != nullptr)
            {
                targetElement = finder(topScene);
                if (targetElement != nullptr)
                {
                    return true;
                }
            }
            return false;
        };

        if (elementSpec.starts_with("attr:"))
        {
            std::string attrPart = elementSpec.substr(5);
            size_t eqPos = attrPart.find('=');
            if (eqPos != std::string::npos)
            {
                attrName = attrPart.substr(0, eqPos);
                attrValue = attrPart.substr(eqPos + 1);
                elementBy = "attribute";
                findInScenes([&](Scene* scene) {
                    return scene->FindElementByAttribute(attrName, attrValue);
                });
                foundBy = "attribute " + attrName + "=" + attrValue;
            }
        }
        else if (elementSpec.starts_with("text:"))
        {
            std::string textContent = elementSpec.substr(5);
            elementBy = "text";
            findInScenes([&](Scene* scene) {
                return scene->FindElementByText(textContent);
            });
            foundBy = "text '" + textContent + "'";
        }
        else
        {
            elementBy = "id";
            findInScenes([&](Scene* scene) {
                return scene->FindElementById(elementSpec);
            });
            foundBy = "id '" + elementSpec + "'";
        }

        if (targetElement == nullptr)
        {
            onMessageRef("Element not found by " + foundBy);
            return false;
        }

        if (action == "find")
        {
            onMessageRef("Found element by " + foundBy);
            onMessageRef("  - Tag: " + std::string(targetElement->GetTagName().c_str()));
            
            Rml::String value = targetElement->GetAttribute<Rml::String>("value", "");
            if (!value.empty())
            {
                onMessageRef("  - Value: " + std::string(value.c_str()));
            }
            
            Rml::String innerRml = targetElement->GetInnerRML();
            if (!innerRml.empty())
            {
                onMessageRef("  - InnerRML: " + std::string(innerRml.c_str()));
            }
            
            return true;
        }

        if (action == "position")
        {
            Rml::Vector2f pos = targetElement->GetAbsoluteOffset(Rml::BoxArea::Content);
            int x = static_cast<int>(pos.x);
            int y = static_cast<int>(pos.y);
            int width = static_cast<int>(targetElement->GetOffsetWidth());
            int height = static_cast<int>(targetElement->GetOffsetHeight());
            int centerX = x + width / 2;
            int centerY = y + height / 2;
            
            onMessageRef("Element position:");
            onMessageRef("  - X: " + std::to_string(x));
            onMessageRef("  - Y: " + std::to_string(y));
            onMessageRef("  - Width: " + std::to_string(width));
            onMessageRef("  - Height: " + std::to_string(height));
            onMessageRef("  - Center: (" + std::to_string(centerX) + ", " + std::to_string(centerY) + ")");
            onMessageRef("  - Click command: input mouse click left " + std::to_string(centerX) + " " + std::to_string(centerY));
            return true;
        }

        if (action == "text")
        {
            if (size < 4)
            {
                onMessageRef(fmt::format(
                    fmt::runtime(langsResources->insufficientParameterLength),
                    4, size));
                return false;
            }
            std::string text = commandArgs->AsString(3);

            std::string tag = targetElement->GetTagName().c_str();
            if (tag == "input" || tag == "textarea")
            {
                Rml::ElementFormControlInput* inputElement = dynamic_cast<Rml::ElementFormControlInput*>(targetElement);
                if (inputElement != nullptr)
                {
                    inputElement->SetValue(text.c_str());
                }
                
                if (elementBy == "attribute" && attrName == "data-value")
                {
                    Scene* topScene = sceneManager->GetTopScene();
                    if (topScene != nullptr)
                    {
                        CreateWorldScene* createWorldScene = dynamic_cast<CreateWorldScene*>(topScene);
                        if (createWorldScene != nullptr)
                        {
                            if (attrValue == "world_name")
                            {
                                createWorldScene->worldName_ = text;
                                onMessageRef("Updated CreateWorldScene.worldName_ to: " + text);
                            }
                            else if (attrValue == "seed")
                            {
                                createWorldScene->seedStr_ = text;
                                onMessageRef("Updated CreateWorldScene.seedStr_ to: " + text);
                            }
                        }
                    }
                }
            }
            else
            {
                targetElement->SetAttribute("value", text.c_str());
            }
            
            onMessageRef("Set text to element by " + foundBy + ": " + text);
            return true;
        }

        if (action == "click")
        {
            Rml::Vector2f pos = targetElement->GetAbsoluteOffset(Rml::BoxArea::Content);
            int x = static_cast<int>(pos.x);
            int y = static_cast<int>(pos.y);
            int width = static_cast<int>(targetElement->GetOffsetWidth());
            int height = static_cast<int>(targetElement->GetOffsetHeight());
            int centerX = x + width / 2;
            int centerY = y + height / 2;
            
            MainThreadDispatcher* dispatcher = appContext->GetMainThreadDispatcher();
            dispatcher->PostToNextMainFrame([centerX, centerY]() {
                SDL_Event moveEvent{};
                moveEvent.type = SDL_EVENT_MOUSE_MOTION;
                moveEvent.motion.x = static_cast<float>(centerX);
                moveEvent.motion.y = static_cast<float>(centerY);
                SDL_PushEvent(&moveEvent);
                
                SDL_Event downEvent{};
                downEvent.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
                downEvent.button.button = SDL_BUTTON_LEFT;
                downEvent.button.x = static_cast<float>(centerX);
                downEvent.button.y = static_cast<float>(centerY);
                SDL_PushEvent(&downEvent);
                
                SDL_Event upEvent{};
                upEvent.type = SDL_EVENT_MOUSE_BUTTON_UP;
                upEvent.button.button = SDL_BUTTON_LEFT;
                upEvent.button.x = static_cast<float>(centerX);
                upEvent.button.y = static_cast<float>(centerY);
                SDL_PushEvent(&upEvent);
            });
            
            onMessageRef("Mouse click scheduled for next frame at position (" + std::to_string(centerX) + ", " + std::to_string(centerY) + ") on element by " + foundBy);
            return true;
        }

        if (action == "focus")
        {
            targetElement->Focus();
            onMessageRef("Focused on element by " + foundBy);
            return true;
        }

        onMessageRef("Unknown action: " + action);
        return false;
    }
}