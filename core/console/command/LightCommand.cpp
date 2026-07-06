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
#if  !defined(NDEBUG)
#include "LightCommand.h"

#include "core/LangsResources.h"
#include "fmt/xchar.h"
#include "core/scene/AppContext.h"
#include "core/world/WorldContext.h"
#include "core/console/CommandSender.h"
#include "core/math/CoordinateTransformer.h"

glimmer::LightCommand::LightCommand(AppContext* appContext) : Command(appContext)
{
}

void glimmer::LightCommand::InitSuggestions(NodeTree<std::string>* suggestionsTree)
{
    if (suggestionsTree == nullptr)
    {
        return;
    }
    suggestionsTree->AddChild("inspector");
    suggestionsTree->AddChild("info")->AddChild(X_DYNAMIC_SUGGESTIONS_NAME)->AddChild(Y_DYNAMIC_SUGGESTIONS_NAME);
}

bool glimmer::LightCommand::RequiresWorldContext() const
{
    return true;
}

const std::string& glimmer::LightCommand::GetName() const
{
    return LIGHT_COMMAND_NAME;
}

void glimmer::LightCommand::PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings)
{
    if (commandArgs == nullptr || strings == nullptr)
    {
        return;
    }
    strings->emplace_back("[operation:string]");
    if (commandArgs->GetSize() >= 2)
    {
        std::string operation = commandArgs->AsString(1);
        if (operation == "info")
        {
            strings->emplace_back("[x:int]");
            strings->emplace_back("[y:int]");
        }
    }
}

bool glimmer::LightCommand::ExecuteInspector(AppContext* appContext,
                                             const std::function<void(const std::string& text)>& onMessageRef,
                                             const LangsResources* langsResources)
{
    CommandHookManager* commandHookManager = appContext->GetCommandHookManager();
    if (commandHookManager == nullptr)
    {
        onMessageRef(langsResources->cmdHookManagerNotFound);
        return false;
    }
    if (commandHookManager->Contains(LIGHT_INSPECTOR_ID))
    {
        if (commandHookManager->Unregister(LIGHT_INSPECTOR_ID))
        {
            onMessageRef(langsResources->lightingInspectorDisable);
            return true;
        }
        onMessageRef(langsResources->lightingInspectorDisableFail);
    }
    else
    {
        auto commandHookEntry = std::make_unique<CommandHookEntry>();
        commandHookEntry->hookId = LIGHT_INSPECTOR_ID;
        commandHookEntry->scope = CommandHookScope::SESSION;
        commandHookEntry->code = SDL_BUTTON_LEFT;
        commandHookEntry->command = LIGHT_COMMAND_NAME + " info ~ ~";
        commandHookEntry->eventType = SDL_EVENT_MOUSE_BUTTON_DOWN;
        if (commandHookManager->Register(std::move(commandHookEntry)))
        {
            onMessageRef(langsResources->lightingInspectorEnable);
            return true;
        }
        onMessageRef(langsResources->lightingInspectorEnableFail);
    }
    return false;
}

std::string glimmer::LightCommand::BuildLightContributionString(const TileLightData* lightData,
                                                                const LangsResources* langsResources)
{
    std::stringstream lightContributionStream;
    const std::unordered_map<TileLayerType, std::vector<std::unique_ptr<LightContribution>>>* lightContributions =
        lightData->GetLightContributions();
    if (lightContributions == nullptr)
    {
        return lightContributionStream.str();
    }
    for (const auto& [layerType,contributionList] : *lightContributions)
    {
        for (const auto& lightPtr : contributionList)
        {
            if (lightPtr == nullptr)
            {
                continue;
            }
            LightContribution* contribution = lightPtr.get();
            const Color* lightColor = contribution->GetLightColor();
            const TileVector2D lightPosition = lightPtr->GetLightSource()->GetCenter();
            lightContributionStream << '\n';
            lightContributionStream << fmt::format(fmt::runtime(langsResources->lightContributionInfo),
                                                   std::to_underlying(layerType), lightColor->r,
                                                   lightColor->g, lightColor->b, lightColor->a,
                                                   contribution->GetRayIndex(), lightPosition.x,
                                                   lightPosition.y);
        }
    }
    return lightContributionStream.str();
}

std::string glimmer::LightCommand::BuildLightSourceString(const TileLightData* lightData,
                                                          const LangsResources* langsResources,
                                                          const TileVector2D& tileVector2D)
{
    std::stringstream lightSourceStream;
    const auto lightSources = lightData->GetLightSources();
    if (lightSources == nullptr)
    {
        return lightSourceStream.str();
    }
    for (const auto& [layerType,lightSource] : *lightSources)
    {
        lightSourceStream << '\n';
        const Color* emissionColor = lightSource->GetEmissionColor();
        lightSourceStream << fmt::format(fmt::runtime(langsResources->lightSourceInfo),
                                         std::to_underlying(layerType), lightSource->GetMaxRadius(),
                                         emissionColor->r, emissionColor->g, emissionColor->b, emissionColor->a,
                                         tileVector2D.x, tileVector2D.y);
    }
    return lightSourceStream.str();
}

std::string glimmer::LightCommand::BuildLightMaskString(const TileLightData* lightData,
                                                        const LangsResources* langsResources)
{
    std::stringstream lightMaskStream;
    AppendMaskList(lightMaskStream, lightData->GetSideLightMasks(), true, langsResources);
    AppendMaskList(lightMaskStream, lightData->GetBackLightMasks(), false, langsResources);
    return lightMaskStream.str();
}

void glimmer::LightCommand::AppendMaskList(std::stringstream& stream,
                                           const std::unordered_map<TileLayerType, std::unique_ptr<LightMask>>* masks,
                                           bool isSide,
                                           const LangsResources* langsResources)
{
    if (masks == nullptr)
    {
        return;
    }
    for (const auto& [layerType, mask] : *masks)
    {
        if (mask == nullptr)
        {
            continue;
        }
        stream << '\n';
        const Color* maskColor = mask->GetLightMaskColor();
        stream << fmt::format(fmt::runtime(langsResources->lightMaskInfo),
                              isSide,
                              std::to_underlying(layerType), maskColor->r,
                              maskColor->g,
                              maskColor->b, maskColor->a,
                              mask->GetTintFactor());
    }
}


bool glimmer::LightCommand::ExecuteInfo(const CommandSender* commandSender, const CommandArgs* commandArgs, int size,
                                        const std::function<void(const std::string& text)>& onMessageRef,
                                        const LangsResources* langsResources, WorldContext* worldContext)
{
    if (size < 4)
    {
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            4, size));
        return false;
    }
    const WorldVector2D commandSenderPosition = commandSender->GetPosition();
    const TileVector2D tileVector2D = CoordinateTransformer::WorldToTile(WorldVector2D(
        commandArgs->AsCoordinate(2, commandSenderPosition.x),
        commandArgs->AsCoordinate(
            3, commandSenderPosition.y)));
    const TileLightData* lightData = worldContext->GetLightingBuffer()->GetTileLightData(
        tileVector2D);
    if (lightData == nullptr)
    {
        onMessageRef(fmt::format(fmt::runtime(langsResources->notIncludeLighting), tileVector2D.x, tileVector2D.y));
        return false;
    }
    const std::string lightContributionStr = BuildLightContributionString(lightData, langsResources);
    const std::string lightSourceStr = BuildLightSourceString(lightData, langsResources, tileVector2D);
    const std::string lightMaskStr = BuildLightMaskString(lightData, langsResources);
    const Color* finalColor = lightData->GetFinalLightColor();
    std::stringstream stringStream;
    if (finalColor == nullptr)
    {
        stringStream << fmt::format(fmt::runtime(langsResources->lightInfo), tileVector2D.x, tileVector2D.y,
                                    -1, -1,
                                    -1, -1, lightContributionStr,
                                    lightSourceStr, lightMaskStr);
    }
    else
    {
        stringStream << fmt::format(fmt::runtime(langsResources->lightInfo), tileVector2D.x, tileVector2D.y,
                                    finalColor->r, finalColor->g,
                                    finalColor->b, finalColor->a, lightContributionStr,
                                    lightSourceStr, lightMaskStr);
    }
    onMessageRef(stringStream.str());
    return true;
}

bool glimmer::LightCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                                    const std::function<void(const std::string& text)>* onMessage)
{
    AppContext* appContext = GetAppContext();
    WorldContext* worldContext = GetWorldContext();
    if (appContext == nullptr || commandArgs == nullptr || onMessage == nullptr)
    {
        return false;
    }
    const std::function<void(const std::string& text)>& onMessageRef = *onMessage;
    if (worldContext == nullptr)
    {
        onMessageRef(appContext->GetLangsResources()->worldContextIsNull);
        return false;
    }
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
    std::string operation = commandArgs->AsString(1);
    if (operation == "inspector")
    {
        return ExecuteInspector(appContext, onMessageRef, langsResources);
    }
    if (operation == "info")
    {
        return ExecuteInfo(commandSender, commandArgs, size, onMessageRef, langsResources, worldContext);
    }
    return false;
}
#endif
