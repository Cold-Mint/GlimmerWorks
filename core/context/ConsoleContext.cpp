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
#include "ConsoleContext.h"

#include "core/console/command/AssetViewerCommand.h"
#include "core/console/command/BiomeScoreCommand.h"
#include "core/console/command/Box2DCommand.h"
#include "core/console/command/ClearCommand.h"
#include "core/console/command/ConfigCommand.h"
#include "core/console/command/EchoCommand.h"
#include "core/console/command/EcsCommand.h"
#include "core/console/command/FlyCommand.h"
#include "core/console/command/GiveCommand.h"
#include "core/console/command/HelpCommand.h"
#include "core/console/command/HookCommand.h"
#include "core/console/command/ItemEditorCommand.h"
#include "core/console/command/LicenseCommand.h"
#include "core/console/command/LightCommand.h"
#include "core/console/command/LocateCommand.h"
#include "core/console/command/LootCommand.h"
#include "core/console/command/PackVerifyCommand.h"
#include "core/console/command/ParallaxBackgroundCommand.h"
#include "core/console/command/PlaceCommand.h"
#include "core/console/command/PlayCommand.h"
#include "core/console/command/ScreenshotCommand.h"
#include "core/console/command/SeedCommand.h"
#include "core/console/command/SummonCommand.h"
#include "core/console/command/TagCommand.h"
#include "core/console/command/TechnologyCommand.h"
#include "core/console/command/TileSnapshotCommand.h"
#include "core/console/command/TpCommand.h"
#include "core/console/command/UnlockedRecipesCommand.h"
#include "core/console/command/VFSCommand.h"
#include "core/console/suggestion/AbilityItemDynamicSuggestions.h"
#include "core/console/suggestion/AllocStrategyTypeDynamicSuggestions.h"
#include "core/console/suggestion/AudioTrackDynamicSuggestions.h"
#include "core/console/suggestion/BiomeSuggestions.h"
#include "core/console/suggestion/BoolDynamicSuggestions.h"
#include "core/console/suggestion/BooleanToggleDynamicSuggestions.h"
#include "core/console/suggestion/CommandHookIdDynamicSuggestions.h"
#include "core/console/suggestion/CommandHookScopeDynamicSuggestions.h"
#include "core/console/suggestion/ComposableItemDynamicSuggestions.h"
#include "core/console/suggestion/ConfigSuggestions.h"
#include "core/console/suggestion/CoordinateDynamicSuggestions.h"
#include "core/console/suggestion/DataPackDynamicSuggestions.h"
#include "core/console/suggestion/EventTypeDynamicSuggestions.h"
#include "core/console/suggestion/LootSuggestions.h"
#include "core/console/suggestion/MaterialItemDynamicSuggestions.h"
#include "core/console/suggestion/MobDynamicSuggestions.h"
#include "core/console/suggestion/MouseButtonDynamicSuggestions.h"
#include "core/console/suggestion/ScanKeyDynamicSuggestions.h"
#include "core/console/suggestion/StructureDynamicSuggestions.h"
#include "core/console/suggestion/TileDynamicSuggestions.h"
#include "core/console/suggestion/VFSDynamicSuggestions.h"
#include "core/log/LogCat.h"
#include "AppContext.h"

void glimmer::ConsoleContext::RegisterCommands(AppContext* appContext) const
{
    commandManager_->RegisterCommand(std::make_unique<GiveCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<HelpCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<TpCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<SummonCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<PlaceCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<ClearCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<LootCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<PackVerifyCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<LicenseCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<SeedCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<FlyCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<EchoCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<ScreenshotCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<LocateCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<ItemEditorCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<PlayCommand>(appContext));
#if  !defined(NDEBUG)
    commandManager_->RegisterCommand(std::make_unique<HookCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<TileSnapshotCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<EcsCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<VFSCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<LightCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<Box2DCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<BiomeScoreCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<AssetViewerCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<ParallaxBackgroundCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<TechnologyCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<TagCommand>(appContext));
    commandManager_->RegisterCommand(std::make_unique<UnlockedRecipesCommand>(appContext));
#endif
    commandManager_->RegisterCommand(std::make_unique<ConfigCommand>(appContext));
}

glimmer::ConsoleContext::ConsoleContext() = default;

glimmer::ConsoleContext::~ConsoleContext() = default;

bool glimmer::ConsoleContext::Init(AppContext* appContext, VirtualFileSystem* vfs, const std::string& runtimePath,
                                   int maxHistoryEntries)
{
    const ModContext* modContext = appContext->GetModContext();
    if (modContext == nullptr)
    {
        LogCat::e(std::source_location::current(), "modContext == nullptr");
        return false;
    }
    commandHookManager_ = std::make_unique<CommandHookManager>();
    dynamicSuggestionsManager_ = std::make_unique<DynamicSuggestionsManager>();
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<BoolDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<CommandHookScopeDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<CommandHookIdDynamicSuggestions>(commandHookManager_.get()));
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<ScanKeyDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<MouseButtonDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<EventTypeDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<AllocStrategyTypeDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<AudioTrackDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<CoordinateDynamicSuggestions>(Y_DYNAMIC_SUGGESTIONS_NAME));
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<CoordinateDynamicSuggestions>(X_DYNAMIC_SUGGESTIONS_NAME));
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<MobDynamicSuggestions>(modContext->GetMobManager()));
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<BoolDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<BooleanToggleDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<VFSDynamicSuggestions>(vfs));
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<DataPackDynamicSuggestions>(modContext->GetDataPackManager()));
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<BiomeSuggestions>(modContext->GetBiomesManager()));
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<TileDynamicSuggestions>(modContext->GetTileResourceManager()));

    auto* im = modContext->GetItemManager();
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<ComposableItemDynamicSuggestions>(im));
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<AbilityItemDynamicSuggestions>(im));
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<MaterialItemDynamicSuggestions>(im));

    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<LootSuggestions>(modContext->GetLootTableManager()));
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<StructureDynamicSuggestions>(modContext->GetStructureManager()));

    dynamicSuggestionsManager_->
        RegisterDynamicSuggestions(std::make_unique<ConfigSuggestions>(appContext));

    commandManager_ = std::make_unique<CommandManager>();
    RegisterCommands(appContext);
    consoleWorker_ = std::make_unique<ConsoleWorker>(commandManager_.get());
    commandHistoryManager_ = std::make_unique<CommandHistoryManager>(runtimePath, vfs);
    if (maxHistoryEntries > 0)
    {
        commandHistoryManager_->Read();
    }
    return true;
}

void glimmer::ConsoleContext::StopConsoleWorker() const
{
    if (consoleWorker_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "consoleWorker_ == nullptr");
        return;
    }
    consoleWorker_->Stop();
}

void glimmer::ConsoleContext::SaveCommandHistory() const
{
    if (commandHistoryManager_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "commandHistoryManager_ == nullptr");
        return;
    }
    commandHistoryManager_->Save();
}

glimmer::CommandManager* glimmer::ConsoleContext::GetCommandManager() const
{
    if (commandManager_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "commandManager_ == nullptr");
        return nullptr;
    }
    return commandManager_.get();
}

glimmer::ConsoleWorker* glimmer::ConsoleContext::GetConsoleWorker() const
{
    if (consoleWorker_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "consoleWorker_ == nullptr");
        return nullptr;
    }
    return consoleWorker_.get();
}

glimmer::CommandHistoryManager* glimmer::ConsoleContext::GetCommandHistoryManager() const
{
    if (commandHistoryManager_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "commandHistoryManager_ == nullptr");
        return nullptr;
    }
    return commandHistoryManager_.get();
}

glimmer::CommandHookManager* glimmer::ConsoleContext::GetCommandHookManager() const
{
    if (commandHookManager_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "commandHookManager_ == nullptr");
        return nullptr;
    }
    return commandHookManager_.get();
}

glimmer::DynamicSuggestionsManager* glimmer::ConsoleContext::GetDynamicSuggestionsManager() const
{
    if (dynamicSuggestionsManager_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "dynamicSuggestionsManager_ == nullptr");
        return nullptr;
    }
    return dynamicSuggestionsManager_.get();
}
