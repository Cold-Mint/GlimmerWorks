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
#include "core/utils/StringUtils.h"
#include "RmlUi/Core/Elements/ElementFormControlInput.h"

void glimmer::ConsoleOverlay::UpdateCommandSuggestions()
{
    commandSuggestions_.clear();
    std::vector<std::string> suggestionVector = commandManager_->GetSuggestions(
        dynamicSuggestionsManager_, commandArgs_, tokenIndex_);
    std::ranges::sort(suggestionVector);
    std::string keyword = commandArgs_.AsString(tokenIndex_);
    for (auto& suggestion : suggestionVector)
    {
        size_t keywordIndex = suggestion.find(keyword);
        CommandSuggestions commandSuggestions;
        if (keywordIndex == std::string::npos)
        {
            commandSuggestions.message = suggestion;
            commandSuggestions.suffix = "";
            commandSuggestions.prefix = StringUtils::MakeRawText(suggestion);
            commandSuggestions.keyword = "";
            commandSuggestions_.emplace_back(commandSuggestions);
            continue;
        }
        commandSuggestions.message = suggestion;
        commandSuggestions.prefix = StringUtils::MakeRawText(suggestion.substr(0, keywordIndex));
        commandSuggestions.suffix = StringUtils::MakeRawText(suggestion.substr(keywordIndex + keyword.length()));
        commandSuggestions.keyword = StringUtils::MakeRawText(keyword);
        commandSuggestions_.emplace_back(commandSuggestions);
    }
    consoleModelHandle_.DirtyVariable("command_suggestions");
}

void glimmer::ConsoleOverlay::UpdateTokenIndex()
{
    int selectionStart = 0;
    int selectionEnd = 0;
    Rml::String selectedText;
    consoleInputElement_->GetSelection(&selectionStart, &selectionEnd, &selectedText);
    tokenIndex_ = commandArgs_.GetTokenIndexAtCursor(selectionEnd - 1);
}

void glimmer::ConsoleOverlay::UpdateCommandStructure()
{
    commandStructure_.clear();
    const std::string commandName = commandArgs_.AsString(0);
    if (const Command* command = commandManager_->GetCommand(commandName); command == nullptr)
    {
        consoleModelHandle_.DirtyVariable("command_structure");
        return;
    }
    const std::vector<std::string> commandStructureVector = commandManager_->GetCommandStructure(&commandArgs_);
    int structureIndex = 0;
    for (auto& string : commandStructureVector)
    {
        CommandStructure commandStructure(StringUtils::MakeRawText(string));
        commandStructure.selected = structureIndex == tokenIndex_;
        commandStructure_.emplace_back(commandStructure);
        structureIndex++;
    }
    consoleModelHandle_.DirtyVariable("command_structure");
}

void glimmer::ConsoleOverlay::UpdateCommandPlaceholder(const std::string& text)
{
    if (commandHistoryMessage_ == nullptr)
    {
        consolePlaceholder_.clear();
        consoleModelHandle_.DirtyVariable("console_placeholder");
        return;
    }
    // Search history in reverse order so the most recent match wins.
    // 倒序搜索历史记录，使最近的匹配优先。
    for (int i = commandHistoryMessage_->history_size() - 1; i >= 0; --i)
    {
        const std::string& historyItem = commandHistoryMessage_->history(i);
        if (historyItem.starts_with(text) && historyItem.length() > text.length())
        {
            consolePlaceholder_ = historyItem;
            consoleModelHandle_.DirtyVariable("console_placeholder");
            return;
        }
    }
    consolePlaceholder_.clear();
    consoleModelHandle_.DirtyVariable("console_placeholder");
}

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

void glimmer::ConsoleOverlay::OnSuggestClick(Rml::DataModelHandle handle, Rml::Event& event, const Rml::VariantList& args)
{
    if (args.empty())
    {
        LogCat::w(std::source_location::current(), "args.empty()");
        return;
    }
    if (consoleInputElement_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "consoleInputElement_ == nullptr");
        return;
    }
    auto message = args[0].Get<std::string>();

    std::string newCommand;
    const int tokenCount = commandArgs_.GetSize();
    for (int i = 0; i < tokenCount; ++i)
    {
        if (i > 0)
        {
            newCommand += " ";
        }
        if (i == tokenIndex_)
        {
            newCommand += message;
        }
        else
        {
            newCommand += commandArgs_.AsString(i);
        }
    }

    if (tokenIndex_ >= tokenCount)
    {
        if (!newCommand.empty())
        {
            newCommand += ' ';
        }
        newCommand += message;
    }

    newCommand += ' ';

    int cursorPos = 1;
    for (int i = 0; i < tokenCount; ++i)
    {
        if (i >= tokenIndex_)
        {
            break;
        }
        cursorPos += commandArgs_.AsString(i).length() + 1;
    }
    if (tokenIndex_ >= tokenCount && !newCommand.empty())
    {
        cursorPos += 1;
    }
    cursorPos += message.length() + 1;

    commandArgs_.SetCommand(newCommand);

    consoleInputElement_->SetValue("/" + newCommand);

    consoleInputElement_->SetSelectionRange(cursorPos, cursorPos);

    UpdateTokenIndex();
    UpdateCommandStructure();
    UpdateCommandSuggestions();
}

void glimmer::ConsoleOverlay::OnConsoleKeydown(Rml::DataModelHandle handle, Rml::Event& event, const Rml::VariantList& args)
{
    if (consoleInputElement_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "consoleInputElement== nullptr");
        return;
    }
    Rml::Input::KeyIdentifier keyIdentifier = event.GetParameter("key_identifier", Rml::Input::KI_UNKNOWN);
    if (keyIdentifier == Rml::Input::KeyIdentifier::KI_RETURN)
    {
        auto text = consoleInputElement_->GetAttribute<Rml::String>("value", "");
        if (text.empty())
        {
            LogCat::w(std::source_location::current(), "text.empty()");
            return;
        }
        consoleInputElement_->SetAttribute("value", "");
        commandSuggestions_.clear();
        consoleModelHandle_.DirtyVariable("command_suggestions");
        commandStructure_.clear();
        consoleModelHandle_.DirtyVariable("command_structure");
        consolePlaceholder_.clear();
        consoleModelHandle_.DirtyVariable("console_placeholder");
        if (!text.starts_with('/'))
        {
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
        if (commandHistoryMessage_ != nullptr)
        {
            commandHistoryMessage_->add_history(text);
        }
    }
    if (keyIdentifier == Rml::Input::KI_LEFT)
    {
        UpdateTokenIndex();
        UpdateCommandStructure();
        UpdateCommandSuggestions();
    }
    if (keyIdentifier == Rml::Input::KI_RIGHT)
    {
        // Accept placeholder completion when the cursor is at the end of the input.
        // 当光标在输入末尾时，接受占位符补全。
        if (!consolePlaceholder_.empty() && consolePlaceholder_ != "console_placeholder")
        {
            auto text = consoleInputElement_->GetAttribute<Rml::String>("value", "");
            int selectionStart = 0;
            int selectionEnd = 0;
            Rml::String selectedText;
            consoleInputElement_->GetSelection(&selectionStart, &selectionEnd, &selectedText);
            if (selectionEnd == static_cast<int>(text.length()))
            {
                const std::string completedText = consolePlaceholder_;
                consoleInputElement_->SetValue(completedText);
                consoleInputElement_->SetSelectionRange(static_cast<int>(completedText.length()),
                                                        static_cast<int>(completedText.length()));
                consolePlaceholder_.clear();
                consoleModelHandle_.DirtyVariable("console_placeholder");
                commandArgs_.SetCommand(completedText.substr(1));
                UpdateTokenIndex();
                UpdateCommandStructure();
                UpdateCommandSuggestions();
                return;
            }
        }
        UpdateTokenIndex();
        UpdateCommandStructure();
        UpdateCommandSuggestions();
    }
}

void glimmer::ConsoleOverlay::OnConsoleChange(Rml::DataModelHandle handle, Rml::Event& event, const Rml::VariantList& args)
{
    if (consoleInputElement_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "consoleInputElement== nullptr");
        return;
    }
    auto text = consoleInputElement_->GetAttribute<Rml::String>("value", "");
    bool isCommand = text.starts_with('/');
    bool textEmpty = text.empty();
    if (textEmpty || !isCommand)
    {
        commandArgs_.SetCommand("");
    }
    else
    {
        commandArgs_.SetCommand(text.substr(1));
    }
    if (commandArgs_.GetSize() == 0)
    {
        commandSuggestions_.clear();
        consoleModelHandle_.DirtyVariable("command_suggestions");
        commandStructure_.clear();
        consoleModelHandle_.DirtyVariable("command_structure");
        if (textEmpty)
        {
            consolePlaceholder_ = "console_placeholder";
        }
        else if (isCommand)
        {
            UpdateCommandPlaceholder(text);
        }
        else
        {
            consolePlaceholder_.clear();
        }
        consoleModelHandle_.DirtyVariable("console_placeholder");
        if (!isCommand)
        {
            return;
        }
    }
    else
    {
        if (isCommand)
        {
            UpdateCommandPlaceholder(text);
        }
        else
        {
            consolePlaceholder_.clear();
            consoleModelHandle_.DirtyVariable("console_placeholder");
        }
    }
    UpdateTokenIndex();
    UpdateCommandStructure();
    UpdateCommandSuggestions();
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
        commandStructure.RegisterMember("selected", &CommandStructure::selected);
        constructor->RegisterArray<std::vector<CommandStructure>>();
        constructor->Bind("command_structure", &commandStructure_);
        consolePlaceholder_ = "console_placeholder";
        constructor->Bind("console_placeholder", &consolePlaceholder_);
        auto commandSuggestions = constructor->RegisterStruct<CommandSuggestions>();
        commandSuggestions.RegisterMember("message", &CommandSuggestions::message);
        commandSuggestions.RegisterMember("suffix", &CommandSuggestions::suffix);
        commandSuggestions.RegisterMember("keyword", &CommandSuggestions::keyword);
        commandSuggestions.RegisterMember("prefix", &CommandSuggestions::prefix);
        constructor->RegisterArray<std::vector<CommandSuggestions>>();
        constructor->Bind("command_suggestions", &commandSuggestions_);
        constructor->BindEventCallback(
            "on_suggest_click",
            &ConsoleOverlay::OnSuggestClick,
            this
        );
        constructor->BindEventCallback(
            "on_console_keydown",
            &ConsoleOverlay::OnConsoleKeydown,
            this
        );
        constructor->BindEventCallback(
            "on_console_change",
            &ConsoleOverlay::OnConsoleChange,
            this
        );
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
    consoleInputElement_ = rmlui_dynamic_cast<Rml::ElementFormControlInput*>(consoleInput);
    if (consoleInputElement_ == nullptr)
    {
        LogCat::e(std::source_location::current(), "consoleInputElement== nullptr");
        return;
    }
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
    commandHistoryMessage_ = consoleContext->GetCommandHistoryMessage();
    if (commandHistoryMessage_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "commandHistoryMessage == nullptr");
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
