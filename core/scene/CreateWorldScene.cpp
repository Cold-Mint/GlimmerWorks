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
#include "CreateWorldScene.h"

#include <filesystem>
#include <random>

#include "MainScene.h"
#include "WorldScene.h"
#include "core/Config.h"
#include "core/saves/Saves.h"
#include "core/saves/SavesManager.h"
#include "core/utils/RandomUtils.h"
#include "core/utils/StringUtils.h"
#include "core/utils/TimeUtils.h"
#include "SDL3/SDL_log.h"
namespace fs = std::filesystem;

glimmer::CreateWorldScene::CreateWorldScene(AppContext* context) : Scene(context)
{
    RandomizeWorld();
    Init();
}

void glimmer::CreateWorldScene::OnCreateDataModels()
{
    Rml::DataModelConstructor* constructor = CreateDataModel("create_world_scene");
    if (constructor != nullptr)
    {
        constructor->Bind("world_name", &worldName_);
        constructor->Bind("seed", &seedStr_);
        constructor->BindEventCallback(
            "on_create_world_click",
            &CreateWorldScene::OnCreateWorldClick,
            this
        );
        constructor->BindEventCallback(
            "on_back_click",
            &CreateWorldScene::OnBackClick,
            this
        );
        constructor->BindEventCallback(
            "on_random_click",
            &CreateWorldScene::OnRandomClick,
            this
        );
        modelHandle_ = constructor->GetModelHandle();
    }
}

void glimmer::CreateWorldScene::OnPauseScene()
{
    Scene::OnPauseScene();
    modelHandle_ = nullptr;
}


void glimmer::CreateWorldScene::LoadDocuments()
{
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_RML_PATH);
    resourceRef.SetResourceKey("create_world/create_world");
    LoadSingleDocument(&resourceRef);
}

void glimmer::CreateWorldScene::RandomizeWorld()
{
    RandomizeName();
    RandomizeSeed();
}

void glimmer::CreateWorldScene::RandomizeName()
{
    auto op = RandomName();
    if (op.has_value())
    {
        worldName_ = op.value();
    }
}

void glimmer::CreateWorldScene::RandomizeSeed()
{
    const int newSeed = RandomUtils::Random<int>();
    seedStr_ = std::to_string(newSeed);
}

void glimmer::CreateWorldScene::OnRandomClick(Rml::DataModelHandle handle, Rml::Event& event,
                                              const Rml::VariantList& args)
{
    if (args.empty())
    {
        RandomizeWorld();
        modelHandle_.DirtyVariable("world_name");
        modelHandle_.DirtyVariable("seed");
        return;
    }
    auto type = args[0].Get<Rml::String>();
    if (type == "name")
    {
        RandomizeName();
        modelHandle_.DirtyVariable("world_name");
    }
    else if (type == "seed")
    {
        RandomizeSeed();
        modelHandle_.DirtyVariable("seed");
    }
}

void glimmer::CreateWorldScene::OnCreateWorldClick(Rml::DataModelHandle handle, Rml::Event& event,
                                                   const Rml::VariantList& args)
{
    CreateWorld();
}

void glimmer::CreateWorldScene::OnBackClick(Rml::DataModelHandle handle, Rml::Event& event,
                                            const Rml::VariantList& args)
{
    GetAppContext()->GetSceneManager()->ReplaceScene(std::make_unique<MainScene>(GetAppContext()));
}

void glimmer::CreateWorldScene::CreateWorld() const
{
    std::string name = worldName_;
    if (name.empty())
    {
        SDL_Log("The name of the world cannot be empty!");
        return;
    }
    const std::string seedInput = seedStr_;
    int seedValue = 0;
    if (StringUtils::IsInteger(seedInput))
    {
        seedValue = std::stoi(seedInput);
    }
    else
    {
        seedValue = static_cast<int>(StringUtils::StringToUint64Blake3(seedInput));
    }

    MapManifest manifest;
    manifest.seed = seedValue;
    manifest.name = name;
    manifest.gameVersionName = GAME_VERSION_STRING;
    manifest.gameVersionNumber = GAME_VERSION_NUMBER;
    manifest.createTime = TimeUtils::GetCurrentTimeMs();
    manifest.lastPlayedTime = manifest.createTime;
    manifest.totalPlayTime = 0;
    auto savesManager = GetAppContext()->GetSavesManager();
    Saves* saves =
        savesManager->
        Create(GetAppContext()->GetConfig()->runtimePath, manifest);
    if (saves == nullptr)
    {
        return;
    }
    GetAppContext()->GetSceneManager()->
                     ReplaceScene(std::make_unique<WorldScene>(
                         GetAppContext(), std::make_unique<WorldContext>(
                             GetAppContext(), savesManager->GetMapManifest(savesManager->GetSavesListSize() - 1),
                             saves)));
}

void glimmer::CreateWorldScene::OnConfigChanged(const Config* config)
{
    uiScale_ = config->window.uiScale;
}

std::optional<std::string> glimmer::CreateWorldScene::RandomName() const
{
    const std::vector<std::string>& prefixList = GetAppContext()->GetLangsResources()->worldNamePrefix;
    if (prefixList.empty())
    {
        return std::nullopt;
    }
    const std::vector<std::string>& suffixList = GetAppContext()->GetLangsResources()->worldNameSuffix;
    if (suffixList.empty())
    {
        return std::nullopt;
    }
    const auto randomPrefixIdx = RandomUtils::Random<size_t>(0, prefixList.size() - 1);
    const auto randomSuffixIdx = RandomUtils::Random<size_t>(0, suffixList.size() - 1);
    if (GetAppContext()->GetLanguage().compare(0, 2, "en") == 0)
    {
        return prefixList[randomPrefixIdx] + " " + suffixList[randomSuffixIdx];
    }
    return prefixList[randomPrefixIdx] + suffixList[randomSuffixIdx];
}
