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
#include "ConsoleOverlay.h"

#include "core/context/AppContext.h"
#include "core/log/LogCat.h"
#include "RmlUi/Core/Elements/ElementFormControlInput.h"

void glimmer::ConsoleOverlay::StartInput() const
{
    const AppContext* appContext = GetAppContext();
    if (appContext == nullptr)
    {
        LogCat::e(std::source_location::current(), "appContext== nullptr");
        return;
    }
    const WindowContext* windowContext = appContext->GetWindowContext();
    if (windowContext == nullptr)
    {
        LogCat::e(std::source_location::current(), "appContext== nullptr");
        return;
    }
    if (!windowContext->StartInput())
    {
        LogCat::e(std::source_location::current(), "Input opening failed.");
    }
}

void glimmer::ConsoleOverlay::StopInput() const
{
    const AppContext* appContext = GetAppContext();
    if (appContext == nullptr)
    {
        LogCat::e(std::source_location::current(), "appContext== nullptr");
        return;
    }
    const WindowContext* windowContext = appContext->GetWindowContext();
    if (windowContext == nullptr)
    {
        LogCat::e(std::source_location::current(), "appContext== nullptr");
        return;
    }
    if (!windowContext->StopInput())
    {
        LogCat::e(std::source_location::current(), "Input closure failed.");
    }
}


void glimmer::ConsoleOverlay::ShowConsole()
{
    consoleDocument_->Show();
    if (consoleWorker_ != nullptr)
    {
        consoleWorker_->PushOnMessage(
            std::make_unique<std::function<void(const std::string&)>>([this](const std::string& text)
            {
                consoleMessages_.emplace_back(text);
                consoleModelHandle_.DirtyVariable("console_messages");
            })
        );
    }
    StartInput();
}

void glimmer::ConsoleOverlay::HideConsole() const
{
    consoleDocument_->Hide();
    if (consoleWorker_ != nullptr)
    {
        consoleWorker_->PopOnMessage();
    }
    StopInput();
}

void glimmer::ConsoleOverlay::OnTextChanged(const std::string& text, int cursorPos)
{
    if (text.empty() || !text.starts_with('/'))
    {
        commandStructure_.clear();
        consoleModelHandle_.DirtyVariable("command_structure");
        consolePlaceholder_ = "";
        consoleModelHandle_.DirtyVariable("console_placeholder");
        commandSuggestions_.clear();
        consoleModelHandle_.DirtyVariable("command_suggestions");
        return;
    }
    int commandCursorPos = cursorPos - 1;
    //Get the command name.
    //获取命令名称。
    auto blankIndex = text.find_first_of(' ');
    std::stringstream commandNameStream;
    if (blankIndex == std::string::npos)
    {
        commandNameStream << text.substr(1);
    }
    else
    {
        commandNameStream << text.substr(1, blankIndex - 1);
    }
    std::string commandName = commandNameStream.str();
    std::string commandLine = text.substr(1);
    CommandArgs commandArgs(commandLine);
    commandSuggestions_.clear();
    std::vector<std::string> suggestion = commandManager_->GetSuggestions(
        dynamicSuggestionsManager_, commandArgs, commandCursorPos);
    std::ranges::sort(suggestion);
    for (auto& string : suggestion)
    {
        commandSuggestions_.emplace_back(CommandSuggestions(string));
    }
    consoleModelHandle_.DirtyVariable("command_suggestions");
    if (const Command* command = commandManager_->GetCommand(commandName); command == nullptr)
    {
        commandStructure_.clear();
        consoleModelHandle_.DirtyVariable("command_structure");
        consolePlaceholder_ = "";
        consoleModelHandle_.DirtyVariable("console_placeholder");
        return;
    }

    consolePlaceholder_ = "";
    consoleModelHandle_.DirtyVariable("console_placeholder");
    const std::vector<std::string> commandStructure = commandManager_->GetCommandStructure(&commandArgs);
    commandStructure_.clear();
    for (auto& string : commandStructure)
    {
        commandStructure_.emplace_back(CommandStructure(string));
    }
    consoleModelHandle_.DirtyVariable("command_structure");
}

glimmer::ConsoleOverlay::ConsoleOverlay(AppContext* context)
    : Scene(context)
{
    Init();
    Rml::DataModelConstructor* constructor = CreateDataModel("console_scene");
    if (constructor != nullptr)
    {
        auto messageStruct = constructor->RegisterStruct<ConsoleMessage>();
        messageStruct.RegisterMember("message", &ConsoleMessage::message);
        constructor->RegisterArray<std::vector<ConsoleMessage>>();
        constructor->Bind("console_messages", &consoleMessages_);
        auto commandStructure = constructor->RegisterStruct<CommandStructure>();
        commandStructure.RegisterMember("message", &CommandStructure::message);
        constructor->RegisterArray<std::vector<CommandStructure>>();
        constructor->Bind("command_structure", &commandStructure_);
        consolePlaceholder_ = "console_placeholder";
        constructor->Bind("console_placeholder", &consolePlaceholder_);
        auto commandSuggestions = constructor->RegisterStruct<CommandSuggestions>();
        commandSuggestions.RegisterMember("message", &CommandSuggestions::message);
        constructor->RegisterArray<std::vector<CommandSuggestions>>();
        constructor->Bind("command_suggestions", &commandSuggestions_);
        consoleModelHandle_ = constructor->GetModelHandle();
    }
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_RML_PATH);
    resourceRef.SetResourceKey("console/console");
    consoleDocument_ = LoadDocument(&resourceRef);
    consoleDocument_->Hide();
    Rml::Element* consoleInput = consoleDocument_->GetElementById("console_input");
    if (consoleInput == nullptr)
    {
        LogCat::e(std::source_location::current(), "consoleInput== nullptr");
        return;
    }
    consoleInput->AddEventListener(Rml::EventId::Keydown, this);
    consoleInput->AddEventListener(Rml::EventId::Change, this);

    const AppContext* appContext = GetAppContext();
    if (appContext == nullptr)
    {
        LogCat::e(std::source_location::current(), "appContext== nullptr");
        return;
    }
    ConsoleContext* consoleContext = appContext->GetConsoleContext();
    if (consoleContext == nullptr)
    {
        LogCat::w(std::source_location::current(), "consoleContext== nullptr");
        return;
    }
    commandManager_ = consoleContext->GetCommandManager();
    if (commandManager_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "commandManager== nullptr");
        return;
    }
    consoleWorker_ = consoleContext->GetConsoleWorker();
    if (consoleWorker_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "consoleWorker== nullptr");
        return;
    }
    dynamicSuggestionsManager_ = consoleContext->GetDynamicSuggestionsManager();
    if (dynamicSuggestionsManager_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "dynamicSuggestionsManager== nullptr");
        return;
    }
}

void glimmer::ConsoleOverlay::ProcessEvent(Rml::Event& event)
{
    if (event.GetType() == "keydown" && event.GetParameter("key_identifier", Rml::Input::KI_UNKNOWN) ==
        Rml::Input::KeyIdentifier::KI_RETURN)
    {
        Rml::Element* consoleInput = event.GetTargetElement();
        if (consoleInput == nullptr)
        {
            LogCat::w(std::source_location::current(), "consoleInput== nullptr");
            return;
        }
        auto text = consoleInput->GetAttribute<Rml::String>("value", "");
        if (text.empty())
        {
            LogCat::w(std::source_location::current(), "text.empty()");
            return;
        }
        consoleInput->SetAttribute("value", "");
        commandSuggestions_.clear();
        consoleModelHandle_.DirtyVariable("command_suggestions");
        commandStructure_.clear();
        consoleModelHandle_.DirtyVariable("command_structure");
        if (!text.starts_with('/'))
        {
            //General Message
            //普通消息
            consoleMessages_.emplace_back(text);
            consoleModelHandle_.DirtyVariable("console_messages");
            return;
        }
        if (consoleWorker_ == nullptr || commandManager_ == nullptr)
        {
            LogCat::w(std::source_location::current(), "consoleWorker_ == nullptr || commandManager_ == nullptr");
            return;
        }
        consoleWorker_->CreateRequest(text.substr(1),
                                      commandManager_->GetDefaultCommandSender());
    }
    else if (event.GetType() == "change")
    {
        const Rml::Element* consoleInput = event.GetTargetElement();
        if (consoleInput == nullptr)
        {
            LogCat::w(std::source_location::current(), "consoleInput== nullptr");
            return;
        }
        auto text = consoleInput->GetAttribute<Rml::String>("value", "");
        if (text.empty())
        {
            LogCat::w(std::source_location::current(), "text.empty()");
            commandSuggestions_.clear();
            consoleModelHandle_.DirtyVariable("command_suggestions");
            consolePlaceholder_ = "console_placeholder";
            consoleModelHandle_.DirtyVariable("console_placeholder");
            return;
        }
        const Rml::ElementFormControlInput* consoleInputElement = rmlui_dynamic_cast<const Rml::ElementFormControlInput
            *>(consoleInput);

        int selectionStart = 0;
        int selectionEnd = 0;
        Rml::String selectedText;
        if (consoleInputElement != nullptr)
        {
            consoleInputElement->GetSelection(&selectionStart, &selectionEnd, &selectedText);
        }
        OnTextChanged(consoleInput->GetAttribute<Rml::String>("value", ""), selectionEnd);
    }
}

bool glimmer::ConsoleOverlay::OnBackPressed()
{
    if (consoleDocument_ == nullptr)
    {
        return false;
    }
    if (consoleDocument_->IsVisible())
    {
        HideConsole();
        return true;
    }
    return false;
}

bool glimmer::ConsoleOverlay::HandleEvent(const SDL_Event& event)
{
    if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat && consoleDocument_ != nullptr && event.key.scancode ==
        SDL_SCANCODE_F1)
    {
        if (consoleDocument_->
            IsVisible())
        {
            HideConsole();
        }
        else
        {
            ShowConsole();
        }
        return true;
    }
    return Scene::HandleEvent(event);
}
