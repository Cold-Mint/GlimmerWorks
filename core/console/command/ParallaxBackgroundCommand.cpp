//
// Created by coldmint on 2026/5/10.
//

#include "ParallaxBackgroundCommand.h"

#include "core/ecs/component/ParallaxBackgroundComponent.h"
#include "core/scene/AppContext.h"
#include "core/world/WorldContext.h"
#include "fmt/xchar.h"


glimmer::ParallaxBackgroundCommand::ParallaxBackgroundCommand(AppContext *appContext) : Command(appContext) {
}

void glimmer::ParallaxBackgroundCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
    suggestionsTree.AddChild("clear");
    suggestionsTree.AddChild("set");
    suggestionsTree.AddChild("get");
}

std::string glimmer::ParallaxBackgroundCommand::GetName() const {
    return PARALLAX_BACKGROUND_COMMAND_NAME;
}

bool glimmer::ParallaxBackgroundCommand::RequiresWorldContext() const {
    return true;
}

void glimmer::ParallaxBackgroundCommand::PutCommandStructure(const CommandArgs &commandArgs,
                                                             std::vector<std::string> &strings) {
    strings.emplace_back("[operation:string]");
}

bool glimmer::ParallaxBackgroundCommand::Execute(const CommandSender *commandSender, CommandArgs commandArgs,
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
    ParallaxBackgroundComponent *parallaxBackgroundComponent = worldContext_->GetParallaxBackgroundComponent();
    if (parallaxBackgroundComponent == nullptr) {
        return false;
    }
    if (operation == "clear") {
        parallaxBackgroundComponent->ClearTexture();
        onMessage(langsResources->parallaxBackgroundClear);
        return true;
    }
    if (operation == "set") {
        const ResourceLocator *resourceLocator = appContext_->GetResourceLocator();
        if (resourceLocator == nullptr) {
            return false;
        }
        if (size < 3) {
            onMessage(fmt::format(
                fmt::runtime(langsResources->insufficientParameterLength),
                3, size));
            return false;
        }
        const std::optional<ResourceRef> texturesResourceRefOptional = commandArgs.AsResourceRef(
            2, RESOURCE_TYPE_TEXTURES);
        if (!texturesResourceRefOptional.has_value()) {
            return false;
        }
        parallaxBackgroundComponent->SetTextureResourceRef(texturesResourceRefOptional.value());
        onMessage(langsResources->parallaxBackgroundSet);
        return true;
    }
    if (operation == "get") {
        const ResourceLocator *resourceLocator = appContext_->GetResourceLocator();
        if (resourceLocator == nullptr) {
            return false;
        }
        const SDL_Texture *texture = parallaxBackgroundComponent->GetTexture(resourceLocator);
        if (texture == nullptr) {
            onMessage(langsResources->parallaxBackgroundNone);
            return false;
        }

        const ResourceRef &resourceRef = parallaxBackgroundComponent->GetTextureResourceRef();
        onMessage(fmt::format(
            fmt::runtime(langsResources->parallaxBackgroundGet),
            Resource::GenerateId(resourceRef.GetPackageId(), resourceRef.GetResourceKey())));
        return true;
    }
    return false;
}
