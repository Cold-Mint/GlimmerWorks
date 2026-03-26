//
// Created by Cold-Mint on 2026/3/3.
//

#include "SummonCommand.h"
#include "fmt/color.h"
#include "../../scene/AppContext.h"
#include "../../world/WorldContext.h"
#include "core/ecs/MobEntityCreator.h"
#include "core/ecs/component/RigidBody2DComponent.h"

void glimmer::SummonCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
    suggestionsTree.AddChild(X_DYNAMIC_SUGGESTIONS_NAME)->AddChild(Y_DYNAMIC_SUGGESTIONS_NAME)->AddChild(
        MOB_DYNAMIC_SUGGESTIONS_NAME);
}

glimmer::SummonCommand::SummonCommand(AppContext *appContext) : Command(appContext) {
}

bool glimmer::SummonCommand::RequiresWorldContext() const {
    return true;
}

bool glimmer::SummonCommand::Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) {
    if (appContext_ == nullptr) {
        return false;
    }
    if (worldContext_ == nullptr) {
        onMessage(appContext_->GetLangsResources()->worldContextIsNull);
        return false;
    }
    const size_t size = commandArgs.GetSize();
    if (size < 1) {
        onMessage(fmt::format(
            fmt::runtime(appContext_->GetLangsResources()->insufficientParameterLength),
            1, size));
        return false;
    }
    auto playerEntity = worldContext_->GetPlayerEntity();
    if (WorldContext::IsEmptyEntityId(playerEntity)) {
        onMessage(appContext_->GetLangsResources()->cantFindObject);
        return false;
    }
    auto resourceRefOptional = commandArgs.AsResourceRef(3, RESOURCE_TYPE_MOB);
    if (!resourceRefOptional.has_value()) {
        return false;
    }
    ResourceRef &resourceRef = resourceRefOptional.value();
    MobResource *mobResource = appContext_->GetResourceLocator()->FindMob(resourceRef);
    if (mobResource == nullptr) {
        return false;
    }
    const auto transform2DComponent = worldContext_->GetComponent<Transform2DComponent>(playerEntity);
    const GameEntity::ID modId = worldContext_->CreateEntity();
    MobEntityCreator mobEntityCreator{worldContext_};
    mobEntityCreator.LoadTemplateComponents(modId, resourceRef);
    mobEntityCreator.MergeEntityItemMessage(modId,
                                            MobEntityCreator::GetEntityItemMessage(
                                                WorldVector2D(
                                                    commandArgs.AsCoordinate(1, transform2DComponent->GetPosition().x),
                                                    commandArgs.AsCoordinate(
                                                        2, transform2DComponent->GetPosition().y))));
    return true;
}

std::string glimmer::SummonCommand::GetName() const {
    return SUMMON_COMMAND_NAME;
}

void glimmer::SummonCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
    strings.emplace_back("[x:int]");
    strings.emplace_back("[y:int]");
    strings.emplace_back("[mob_type:string]");
}
