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
#include "ParallaxBackgroundCommand.h"

#include "core/ecs/component/ParallaxBackgroundComponent.h"
#include "core/scene/AppContext.h"
#include "core/world/WorldContext.h"
#include "fmt/xchar.h"


glimmer::ParallaxBackgroundCommand::ParallaxBackgroundCommand(AppContext* appContext) : Command(appContext)
{
}

void glimmer::ParallaxBackgroundCommand::InitSuggestions(NodeTree<std::string>* suggestionsTree)
{
    if (suggestionsTree == nullptr)
    {
        return;
    }
    suggestionsTree->AddChild("clear");
    suggestionsTree->AddChild("set");
    suggestionsTree->AddChild("get");
}


const std::string& glimmer::ParallaxBackgroundCommand::GetName() const
{
    return PARALLAX_BACKGROUND_COMMAND_NAME;
}

bool glimmer::ParallaxBackgroundCommand::RequiresWorldContext() const
{
    return true;
}

void glimmer::ParallaxBackgroundCommand::PutCommandStructure(const CommandArgs* commandArgs,
                                                             std::vector<std::string>* strings) //skipcq: CXX-C2014
{
    if (commandArgs == nullptr || strings == nullptr)
    {
        return;
    }
    strings->emplace_back("[operation:string]");
}

bool glimmer::ParallaxBackgroundCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                                                 const std::function<void(const std::string& text)>* onMessage)
{
    if (appContext_ == nullptr || commandArgs == nullptr || onMessage == nullptr)
    {
        return false;
    }
    const std::function<void(const std::string& text)>& onMessageRef = *onMessage;
    if (worldContext_ == nullptr)
    {
        onMessageRef(appContext_->GetLangsResources()->worldContextIsNull);
        return false;
    }
    const LangsResources* langsResources = appContext_->GetLangsResources();
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
    EntityShortCut* entityShortCut = worldContext_->GetEntityShortCut();
    if (entityShortCut == nullptr)
    {
        return false;
    }
    ParallaxBackgroundComponent* parallaxBackgroundComponent = entityShortCut
        ->
        GetParallaxBackgroundComponent();
    if (parallaxBackgroundComponent == nullptr)
    {
        return false;
    }
    if (operation == "clear")
    {
        parallaxBackgroundComponent->ClearTexture();
        onMessageRef(langsResources->parallaxBackgroundClear);
        return true;
    }
    if (operation == "set")
    {
        const ResourceLocator* resourceLocator = appContext_->GetResourceLocator();
        if (resourceLocator == nullptr)
        {
            return false;
        }
        if (size < 3)
        {
            onMessageRef(fmt::format(
                fmt::runtime(langsResources->insufficientParameterLength),
                3, size));
            return false;
        }
        const std::optional<ResourceRef> texturesResourceRefOptional = commandArgs->AsResourceRef(
            2, RESOURCE_TEXTURE);
        if (!texturesResourceRefOptional.has_value())
        {
            return false;
        }
        parallaxBackgroundComponent->SetTextureResourceRef(texturesResourceRefOptional.value());
        onMessageRef(langsResources->parallaxBackgroundSet);
        return true;
    }
    if (operation == "get")
    {
        const ResourceLocator* resourceLocator = appContext_->GetResourceLocator();
        if (resourceLocator == nullptr)
        {
            return false;
        }
        const SDL_Texture* texture = parallaxBackgroundComponent->GetTexture(resourceLocator);
        if (texture == nullptr)
        {
            onMessageRef(langsResources->parallaxBackgroundNone);
            return false;
        }

        const ResourceRef& resourceRef = parallaxBackgroundComponent->GetTextureResourceRef();
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->parallaxBackgroundGet),
            Resource::GenerateId(resourceRef.GetPackageId(), resourceRef.GetResourceKey())));
        return true;
    }
    return false;
}
#endif
