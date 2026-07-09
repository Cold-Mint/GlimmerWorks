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
#include "BiomeScoreCommand.h"
#include "core/LangsResources.h"
#include "core/math/CoordinateTransformer.h"
#include "fmt/xchar.h"
#include "core/scene/AppContext.h"
#include "core/world/WorldContext.h"

glimmer::BiomeScoreCommand::BiomeScoreCommand(AppContext* appContext) : Command(appContext)
{
}

void glimmer::BiomeScoreCommand::InitSuggestions(NodeTree<std::string>* suggestionsTree)
{
    if (suggestionsTree == nullptr)
    {
        return;
    }
    suggestionsTree->AddChild("inspector");
    suggestionsTree->AddChild("info")->AddChild(X_DYNAMIC_SUGGESTIONS_NAME)->AddChild(Y_DYNAMIC_SUGGESTIONS_NAME);
}

const std::string& glimmer::BiomeScoreCommand::GetName() const
{
    return BIOME_SCORE_COMMAND_NAME;
}

bool glimmer::BiomeScoreCommand::RequiresWorldContext() const
{
    return true;
}

void glimmer::BiomeScoreCommand::
PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings)
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

std::string glimmer::BiomeScoreCommand::CalculateAndFormatBiomeScores(const TileVector2D& tileVector2D,
                                                                      ChunkGenerator* chunkGenerator,
                                                                      BiomesManager* biomesManager,
                                                                      const LangsResources* langsResources)
{
    std::stringstream biomeStream;
    for (auto biomeResource : biomesManager->GetBiomeVector())
    {
        float total = 0;
        std::string biomeId = Resource::GenerateId(*biomeResource);
        const float elevation = ChunkGenerator::GetElevation(tileVector2D.y);
        float elevationScore = BiomesManager::CalculateBiomeScoreDelta(
            biomeResource->elevation, elevation, biomeResource->strictnessElevation);
        total += elevationScore;
        biomeStream << fmt::format(fmt::runtime(langsResources->biomeElevationInfo), biomeId,
                                   biomeResource->elevation, elevation, biomeResource->strictnessElevation,
                                   elevationScore);
        biomeStream << '\n';
        const auto humidity = chunkGenerator->GetHumidity(tileVector2D);
        float humidityScore = BiomesManager::CalculateBiomeScoreDelta(
            biomeResource->humidity, humidity, biomeResource->strictnessHumidity);
        total += humidityScore;
        biomeStream << fmt::format(fmt::runtime(langsResources->biomeHumidityInfo), biomeId,
                                   biomeResource->humidity, humidity, biomeResource->strictnessHumidity,
                                   humidityScore);
        biomeStream << '\n';
        const auto temperature = chunkGenerator->GetTemperature(tileVector2D, elevation);
        float temperatureScore = BiomesManager::CalculateBiomeScoreDelta(
            biomeResource->temperature, temperature, biomeResource->strictnessTemperature);
        total += temperatureScore;
        biomeStream << fmt::format(fmt::runtime(langsResources->biomeTemperatureInfo), biomeId,
                                   biomeResource->temperature, temperature, biomeResource->strictnessTemperature,
                                   temperatureScore);
        biomeStream << '\n';
        const auto weirdness = chunkGenerator->GetWeirdness(tileVector2D);
        float weirdnessScore = BiomesManager::CalculateBiomeScoreDelta(
            biomeResource->weirdness, weirdness, biomeResource->strictnessWeirdness);
        total += weirdnessScore;
        biomeStream << fmt::format(fmt::runtime(langsResources->biomeWeirdnessInfo), biomeId,
                                   biomeResource->weirdness, weirdness, biomeResource->strictnessWeirdness,
                                   weirdnessScore);
        biomeStream << '\n';
        const auto erosion = chunkGenerator->GetErosion(tileVector2D);
        float erosionScore = BiomesManager::CalculateBiomeScoreDelta(
            biomeResource->erosion, erosion, biomeResource->strictnessErosion);
        total += erosionScore;
        biomeStream << fmt::format(fmt::runtime(langsResources->biomeErosionInfo), biomeId,
                                   biomeResource->erosion, erosion, biomeResource->strictnessErosion,
                                   erosionScore);
        biomeStream << '\n';
        const auto surfaceProximity = ChunkGenerator::GetSurfaceProximity(
            chunkGenerator->GetFirstTileTerrainY(tileVector2D.x), tileVector2D.y);
        float surfaceProximityScore = BiomesManager::CalculateBiomeScoreDelta(
            biomeResource->surfaceProximity, surfaceProximity, biomeResource->strictnessSurfaceProximity);
        total += surfaceProximityScore;
        biomeStream << fmt::format(fmt::runtime(langsResources->biomeSurfaceProximityInfo), biomeId,
                                   biomeResource->surfaceProximity, surfaceProximity,
                                   biomeResource->strictnessSurfaceProximity,
                                   surfaceProximityScore);
        biomeStream << '\n';
        biomeStream << fmt::format(fmt::runtime(langsResources->biomeTotalScore), biomeId, total);
        biomeStream << '\n';
    }
    return biomeStream.str();
}

bool glimmer::BiomeScoreCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                                         const std::function<void(const std::string& text)>* onMessage)
{
    const AppContext* appContext = GetAppContext();
    const WorldContext* worldContext = GetWorldContext();
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
        CommandHookManager* commandHookManager = appContext->GetConsoleContext()->GetCommandHookManager();
        if (commandHookManager == nullptr)
        {
            onMessageRef(langsResources->cmdHookManagerNotFound);
            return false;
        }
        if (commandHookManager->Contains(BIOME_SCORE_INSPECTOR_ID))
        {
            if (commandHookManager->Unregister(BIOME_SCORE_INSPECTOR_ID))
            {
                onMessageRef(langsResources->biomeScoreInspectorDisable);
                return true;
            }
            onMessageRef(langsResources->biomeScoreInspectorDisableFail);
        }
        else
        {
            auto commandHookEntry = std::make_unique<CommandHookEntry>();
            commandHookEntry->hookId = BIOME_SCORE_INSPECTOR_ID;
            commandHookEntry->scope = CommandHookScope::SESSION;
            commandHookEntry->code = SDL_BUTTON_LEFT;
            commandHookEntry->command = BIOME_SCORE_COMMAND_NAME + " info ~ ~";
            commandHookEntry->eventType = SDL_EVENT_MOUSE_BUTTON_DOWN;
            if (commandHookManager->Register(std::move(commandHookEntry)))
            {
                onMessageRef(langsResources->biomeScoreInspectorEnable);
                return true;
            }
            onMessageRef(langsResources->biomeScoreInspectorEnableFail);
        }
    }
    if (operation == "info")
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
        ChunkGenerator* chunkGenerator = worldContext->GetChunkGenerator();
        if (chunkGenerator == nullptr)
        {
            return false;
        }
        BiomesManager* biomesManager = appContext->GetModContext()->GetBiomesManager();
        if (biomesManager == nullptr)
        {
            return false;
        }
        onMessageRef(CalculateAndFormatBiomeScores(tileVector2D, chunkGenerator, biomesManager, langsResources));
        return true;
    }
    return false;
}
#endif
