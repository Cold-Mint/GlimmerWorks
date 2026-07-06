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
#include "AssetViewerCommand.h"

#include "core/Constants.h"
#include "core/scene/AppContext.h"

#include "core/console/asset_enumerator/StringAssetEnumerator.h"
#include "core/console/asset_enumerator/TextureAssetEnumerator.h"
#include "core/console/asset_enumerator/TileAssetEnumerator.h"
#include "core/console/asset_enumerator/BiomesAssetEnumerator.h"
#include "core/console/asset_enumerator/ComposableItemsAssetEnumerator.h"
#include "core/console/asset_enumerator/AbilityItemsAssetEnumerator.h"
#include "core/console/asset_enumerator/MaterialItemsAssetEnumerator.h"
#include "core/console/asset_enumerator/LootTablesAssetEnumerator.h"
#include "core/console/asset_enumerator/StartInvAssetEnumerator.h"
#include "core/console/asset_enumerator/StructuresAssetEnumerator.h"
#include "core/console/asset_enumerator/MobsAssetEnumerator.h"
#include "core/console/asset_enumerator/ShapesAssetEnumerator.h"
#include "core/console/asset_enumerator/BiomeDecorsAssetEnumerator.h"
#include "core/console/asset_enumerator/FixedColorAssetEnumerator.h"
#include "core/console/asset_enumerator/LightMaskAssetEnumerator.h"
#include "core/console/asset_enumerator/LightSourceAssetEnumerator.h"
#include "core/console/asset_enumerator/RecipesAssetEnumerator.h"

void glimmer::AssetViewerCommand::InitSuggestions(NodeTree<std::string>* suggestionsTree)
{
    if (suggestionsTree == nullptr)
    {
        return;
    }
    for (const auto& enumerator : assetEnumerators_ | std::views::values)
    {
        suggestionsTree->AddChild(std::string(enumerator->GetAssetType()));
    }
}

void glimmer::AssetViewerCommand::AddAssetEnumerator(std::unique_ptr<IAssetEnumerator> assetEnumeratorPtr)
{
    if (assetEnumeratorPtr == nullptr)
    {
        return;
    }
    assetEnumerators_.emplace(assetEnumeratorPtr->GetAssetType(), std::move(assetEnumeratorPtr));
}

glimmer::AssetViewerCommand::AssetViewerCommand(AppContext* appContext)
    : Command(appContext)
{
    AddAssetEnumerator(std::make_unique<StringAssetEnumerator>());
    AddAssetEnumerator(std::make_unique<TextureAssetEnumerator>());
    AddAssetEnumerator(std::make_unique<TileAssetEnumerator>());
    AddAssetEnumerator(std::make_unique<BiomesAssetEnumerator>());
    AddAssetEnumerator(std::make_unique<ComposableItemsAssetEnumerator>());
    AddAssetEnumerator(std::make_unique<AbilityItemsAssetEnumerator>());
    AddAssetEnumerator(std::make_unique<MaterialItemsAssetEnumerator>());
    AddAssetEnumerator(std::make_unique<LootTablesAssetEnumerator>());
    AddAssetEnumerator(std::make_unique<StartInvAssetEnumerator>());
    AddAssetEnumerator(std::make_unique<StructuresAssetEnumerator>());
    AddAssetEnumerator(std::make_unique<MobsAssetEnumerator>());
    AddAssetEnumerator(std::make_unique<ShapesAssetEnumerator>());
    AddAssetEnumerator(std::make_unique<BiomeDecorsAssetEnumerator>());
    AddAssetEnumerator(std::make_unique<FixedColorAssetEnumerator>());
    AddAssetEnumerator(std::make_unique<LightMaskAssetEnumerator>());
    AddAssetEnumerator(std::make_unique<LightSourceAssetEnumerator>());
    AddAssetEnumerator(std::make_unique<RecipesAssetEnumerator>());
}

const std::string& glimmer::AssetViewerCommand::GetName() const
{
    return ASSET_VIEWER_COMMAND_NAME;
}

bool glimmer::AssetViewerCommand::RequiresWorldContext() const
{
    return false;
}

void glimmer::AssetViewerCommand::
PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings)
{
    if (strings == nullptr)
    {
        return;
    }
    strings->emplace_back("[asset type:string]");
}


bool glimmer::AssetViewerCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                                          const std::function<void(const std::string& text)>* onMessage)
{
    const AppContext* appContext = GetAppContext();
    if (appContext == nullptr || commandArgs == nullptr || onMessage == nullptr)
    {
        return false;
    }
    const std::function<void(const std::string& text)>& onMessageRef = *onMessage;
    const auto type = commandArgs->AsString(1);
    auto it = assetEnumerators_.find(type);
    if (it != assetEnumerators_.end())
    {
        auto result = it->second->ListAsset(appContext);
        if (result.has_value())
        {
            onMessageRef(result.value());
            return true;
        }
        return false;
    }
    const LangsResources* langsResources = appContext->GetLangsResources();
    if (langsResources == nullptr)
    {
        return false;
    }
    onMessageRef(langsResources->unknownAssetType);
    return false;
}
#endif
