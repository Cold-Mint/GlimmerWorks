//
// Created by coldmint on 2026/5/3.
//

#include "LightCommand.h"

#include "core/Langs.h"
#include "fmt/xchar.h"
#include "core/scene/AppContext.h"
#include "../../world/WorldContext.h"
#include "core/console/CommandSender.h"

glimmer::LightCommand::LightCommand(AppContext *appContext) : Command(appContext) {
}

void glimmer::LightCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
    suggestionsTree.AddChild("inspector");
    suggestionsTree.AddChild("info")->AddChild(X_DYNAMIC_SUGGESTIONS_NAME)->AddChild(Y_DYNAMIC_SUGGESTIONS_NAME);
}

bool glimmer::LightCommand::RequiresWorldContext() const {
    return true;
}

std::string glimmer::LightCommand::GetName() const {
    return LIGHT_COMMAND_NAME;
}

void glimmer::LightCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
    strings.emplace_back("[operation：string]");
    if (commandArgs.GetSize() >= 2) {
        std::string operation = commandArgs.AsString(1);
        if (operation == "info") {
            strings.emplace_back("[x:int]");
            strings.emplace_back("[y:int]");
        }
    }
}

bool glimmer::LightCommand::Execute(const CommandSender *commandSender, CommandArgs commandArgs,
                                    std::function<void(const std::string &text)> onMessage) {
    if (appContext_ == nullptr) {
        return false;
    }
    if (worldContext_ == nullptr) {
        onMessage(appContext_->GetLangsResources()->worldContextIsNull);
        return false;
    }
    const LangsResources *langsResources = appContext_->GetLangsResources();
    if (langsResources == nullptr) {
        return false;
    }
    const int size = commandArgs.GetSize();
    if (size < 2) {
        onMessage(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            2, size));
        return false;
    }
    std::string operation = commandArgs.AsString(1);
    if (operation == "inspector") {
    } else if (operation == "info") {
        if (size < 4) {
            onMessage(fmt::format(
                fmt::runtime(langsResources->insufficientParameterLength),
                4, size));
            return false;
        }
        const WorldVector2D commandSenderPosition = commandSender->GetPosition();
        const TileVector2D tileVector2D = TileLayerComponent::WorldToTile(WorldVector2D(
            commandArgs.AsCoordinate(1, commandSenderPosition.x),
            commandArgs.AsCoordinate(
                2, commandSenderPosition.y)));
        const TileLightData *lightData = worldContext_->GetLightingBuffer()->GetTileLightData(
            tileVector2D);
        if (lightData == nullptr) {
            onMessage(appContext_->GetLangsResources()->notIncludeLighting);
            return false;
        }
        std::stringstream lightContributionStream;
        const std::unordered_map<TileLayerType, std::vector<std::unique_ptr<LightContribution> > > *lightContributions =
                lightData->GetLightContributions();
        if (lightContributions != nullptr) {
            for (const auto &layerPair: *lightContributions) {
                TileLayerType layerType = layerPair.first;
                const auto &contributionList = layerPair.second;
                for (const auto &lightPtr: contributionList) {
                    if (lightPtr == nullptr) {
                        continue;
                    }
                    LightContribution *contribution = lightPtr.get();
                    const Color *lightColor = contribution->GetLightColor();
                    const TileVector2D lightPosition = lightPtr->GetLightSource()->GetCenter();
                    lightContributionStream << '\n';
                    lightContributionStream << fmt::format(fmt::runtime(langsResources->lightContributionInfo),
                                                           static_cast<uint8_t>(layerType), lightColor->r,
                                                           lightColor->g, lightColor->b, lightColor->a,
                                                           contribution->GetRayIndex(), lightPosition.x,
                                                           lightPosition.y);
                }
            }
        }
        const Color *finalColor = lightData->GetFinalLightColor();
        if (finalColor == nullptr) {
            onMessage(lightContributionStream.str());
        } else {
            std::stringstream stringStream;
            stringStream << fmt::format(fmt::runtime(langsResources->lightInfo), tileVector2D.x, tileVector2D.y,
                                        finalColor->r, finalColor->g,
                                        finalColor->b, finalColor->a, lightContributionStream.str());
            onMessage(stringStream.str());
        }
    }
    return true;
}
