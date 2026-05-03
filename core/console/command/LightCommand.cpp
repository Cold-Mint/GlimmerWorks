//
// Created by coldmint on 2026/5/3.
//

#include "LightCommand.h"

#include "core/Langs.h"
#include "fmt/xchar.h"
#include "core/scene/AppContext.h"
#include "../../world/WorldContext.h"

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
}

bool glimmer::LightCommand::Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) {
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
        auto playerEntity = worldContext_->GetPlayerEntity();
        if (WorldContext::IsEmptyEntityId(playerEntity)) {
            onMessage(appContext_->GetLangsResources()->cantFindObject);
            return false;
        }
        auto transform2DComponent = worldContext_->GetComponent<Transform2DComponent>(playerEntity);
        TileVector2D tileVector2d = TileLayerComponent::WorldToTile({
            commandArgs.AsCoordinate(2, transform2DComponent->GetPosition().x),
            commandArgs.AsCoordinate(3, transform2DComponent->GetPosition().y)
        });
        const TileLightData *lightData = worldContext_->GetLightingBuffer()->GetTileLightData(tileVector2d);
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
        std::stringstream stringStream;
        const Color *finalColor = lightData->GetFinalLightColor();
        stringStream << fmt::format(fmt::runtime(langsResources->lightInfo), finalColor->r, finalColor->g,
                                    finalColor->b, finalColor->a, lightContributionStream.str());
        onMessage(stringStream.str());
    }
    return true;
}
