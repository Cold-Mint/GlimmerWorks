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
#include "AppContext.h"

#include <random>

#include "core/log/LogCat.h"
#include "core/utils/LanguageUtils.h"
#include "core/utils/RandomUtils.h"
#include "core/utils/StringUtils.h"
#include "core/vfs/StdFileProvider.h"
#include "fmt/xchar.h"
#include "SDL3_image/SDL_image.h"
#include "toml11/find.hpp"
#include "toml11/parser.hpp"
#ifdef __ANDROID__
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "core/vfs/AndroidAssetsFileProvider.h"
#endif


void glimmer::AppContext::LoadLanguage(const std::string& data) const
{
    toml::value value = toml::parse_str(data, tomlVersion_);
    langsResources_->startGame = find<std::string>(value, "start_game");
    langsResources_->settings = find<std::string>(value, "settings");
    langsResources_->mods = find<std::string>(value, "mods");
    langsResources_->exitGame = find<std::string>(value, "exit_game");
    langsResources_->console = find<std::string>(value, "console");
    langsResources_->commandNotFound = find<std::string>(value, "command_not_found");
    langsResources_->executionFailed = find<std::string>(value, "execution_failed");
    langsResources_->executedSuccess = find<std::string>(value, "executed_success");
    langsResources_->commandIsEmpty = find<std::string>(value, "command_is_empty");
    langsResources_->createWorld = find<std::string>(value, "create_world");
    langsResources_->savedGames = find<std::string>(value, "saved_games");
    langsResources_->cancel = find<std::string>(value, "cancel");
    langsResources_->worldName = find<std::string>(value, "world_name");
    langsResources_->seed = find<std::string>(value, "seed");
    langsResources_->random = find<std::string>(value, "random");
    langsResources_->commandInfo = find<std::string>(value, "command_info");
    langsResources_->awakeBodyCount = find<std::string>(value, "awake_body_count");
    langsResources_->getActualPathError = find<std::string>(value, "get_actual_path_error");
    langsResources_->unknownAssetType = find<std::string>(value, "unknown_asset_type");
    langsResources_->unknownCommandParameters = find<std::string>(value, "unknown_command_parameters");
    langsResources_->worldContextIsNull = find<std::string>(value, "world_context_is_null");
    langsResources_->insufficientParameterLength = find<std::string>(value, "insufficient_parameter_length");
    langsResources_->entryCannotFoundInConfigurationFile = find<std::string>(
        value, "entry_cannot_found_in_configuration_file");
    langsResources_->configurationUpdate = find<std::string>(value, "configuration_update");
    langsResources_->itemIdNotFound = find<std::string>(value, "item_id_not_found");
    langsResources_->lootTableNotFound = find<std::string>(value, "loot_table_not_found");
    langsResources_->itemContainerIsNull = find<std::string>(value, "item_container_is_null");
    langsResources_->composableItemIsNull = find<std::string>(value, "composable_item_is_null");
    langsResources_->abilityItemIsNull = find<std::string>(value, "ability_item_is_null");
    langsResources_->itemResourceIsNull = find<std::string>(value, "item_resource_is_null");
    langsResources_->tileResourceIsNull = find<std::string>(value, "tile_resource_is_null");
    langsResources_->minXIsGreaterThanMaxX = find<std::string>(value, "min_x_is_greater_than_max_x");
    langsResources_->folderCreationFailed = find<std::string>(value, "folder_creation_failed");
    langsResources_->fileWritingFailed = find<std::string>(value, "file_writing_failed");
    langsResources_->failedToLoadLicense = find<std::string>(value, "failed_to_load_license");
    langsResources_->cantFindObject = find<std::string>(value, "cant_find_object");
    langsResources_->teleportEntity = find<std::string>(value, "teleport_entity");
    langsResources_->loadGame = find<std::string>(value, "load_game");
    langsResources_->deleteGame = find<std::string>(value, "delete_game");
    langsResources_->confirm = find<std::string>(value, "confirm");
    langsResources_->wantDeleteThisSave = find<std::string>(value, "want_delete_this_save");
    langsResources_->savesList = find<std::string>(value, "saves_list");
    langsResources_->pause = find<std::string>(value, "pause");
    langsResources_->restore = find<std::string>(value, "restore");
    langsResources_->saveAndExit = find<std::string>(value, "save_and_exit");
    langsResources_->screenshotSavedSuccess = find<std::string>(value, "screenshot_saved_success");
    langsResources_->screenshotSavedFailed = find<std::string>(value, "screenshot_saved_failed");
    langsResources_->areaMarkerTip = find<std::string>(value, "area_marker_tip");
    langsResources_->efficiencyTip = find<std::string>(value, "efficiency_tip");
    langsResources_->precisionMiningTip = find<std::string>(value, "precision_mining_tip");
    langsResources_->canMineBlockTip = find<std::string>(value, "can_mine_block_tip");
    langsResources_->canMineWallTip = find<std::string>(value, "can_mine_wall_tip");
    langsResources_->chainMiningTip = find<std::string>(value, "chain_mining_tip");
    langsResources_->flyEnable = find<std::string>(value, "fly_enable");
    langsResources_->flyDisable = find<std::string>(value, "fly_disable");
    langsResources_->tileDebugInfo = find<std::string>(value, "tile_debug_info");
    langsResources_->tileResDebugInfo = find<std::string>(value, "tile_res_debug_info");
    langsResources_->mousePosition = find<std::string>(value, "mouse_position");
    langsResources_->totalLight = find<std::string>(value, "total_light");
    langsResources_->noBiomeWasFound = find<std::string>(value, "no_biome_was_found");
    langsResources_->biomeHasFound = find<std::string>(value, "biome_has_found");
    langsResources_->configurationCommitSuccess = find<std::string>(value, "configuration_commit_success");
    langsResources_->configurationCommitFail = find<std::string>(value, "configuration_commit_fail");
    langsResources_->scancodeUnknown = find<std::string>(value, "scancode_unknown");
    langsResources_->hookCreateDuplicate = find<std::string>(value, "hook_create_duplicate");
    langsResources_->hookAddDuplicate = find<std::string>(value, "hook_add_duplicate");
    langsResources_->hookAddSuccess = find<std::string>(value, "hook_add_success");
    langsResources_->hookRemoveSuccess = find<std::string>(value, "hook_remove_success");
    langsResources_->hookIdNotExist = find<std::string>(value, "hook_id_not_exist");
    langsResources_->hookInfo = find<std::string>(value, "hook_info");
    langsResources_->lightInfo = find<std::string>(value, "light_info");
    langsResources_->lightContributionInfo = find<std::string>(value, "light_contribution_info");
    langsResources_->lightSourceInfo = find<std::string>(value, "light_source_info");
    langsResources_->lightMaskInfo = find<std::string>(value, "light_mask_info");
    langsResources_->notIncludeLighting = find<std::string>(value, "not_include_lighting");
    langsResources_->scancodeHookNotFound = find<std::string>(value, "scancode_hook_not_found");
    langsResources_->scancodeHookFoundCount = find<std::string>(value, "scancode_hook_found_count");
    langsResources_->worldNamePrefix = find<std::vector<std::string>>(value, "world_name_prefix");
    langsResources_->worldNameSuffix = find<std::vector<std::string>>(value, "world_name_suffix");
    langsResources_->slogans = find<std::vector<std::string>>(value, "slogans");
    langsResources_->cmdHookManagerNotFound = find<std::string>(value, "cmd_hook_manager_not_found");
    langsResources_->lightingInspectorEnable = find<std::string>(value, "lighting_inspector_enable");
    langsResources_->lightingInspectorDisable = find<std::string>(value, "lighting_inspector_disable");
    langsResources_->lightingInspectorEnableFail = find<std::string>(value, "lighting_inspector_enable_fail");
    langsResources_->lightingInspectorDisableFail = find<std::string>(value, "lighting_inspector_disable_fail");
    langsResources_->tileNameAir = find<std::string>(value, STRING_TILE_AIR_NAME);
    langsResources_->tileNameAirWall = find<std::string>(value, STRING_TILE_AIR_WALL_NAME);
    langsResources_->tileNameError = find<std::string>(value, STRING_TILE_ERROR_NAME);
    langsResources_->tileNameErrorWall = find<std::string>(value, STRING_TILE_ERROR_WALL_NAME);
    langsResources_->tileNameAccessDenied = find<std::string>(value, STRING_TILE_ACCESS_DENIED_NAME);
    langsResources_->tileNameAccessDeniedWall = find<std::string>(value, STRING_TILE_ACCESS_DENIED_WALL_NAME);
    langsResources_->tileNameBedrock = find<std::string>(value, STRING_TILE_BEDROCK_NAME);
    langsResources_->tileNameWater = find<std::string>(value, STRING_TILE_WATER_NAME);
    langsResources_->tileDescriptionAir = find<std::string>(value, STRING_TILE_AIR_DESCRIPTION);
    langsResources_->tileDescriptionAirWall = find<std::string>(value, STRING_TILE_AIR_WALL_DESCRIPTION);
    langsResources_->tileDescriptionError = find<std::string>(value, STRING_TILE_ERROR_DESCRIPTION);
    langsResources_->tileDescriptionErrorWall = find<std::string>(value, STRING_TILE_ERROR_WALL_DESCRIPTION);
    langsResources_->tileDescriptionAccessDenied = find<std::string>(value, STRING_TILE_ACCESS_DENIED_DESCRIPTION);
    langsResources_->tileDescriptionAccessDeniedWall = find<std::string>(
        value, STRING_TILE_ACCESS_DENIED_WALL_DESCRIPTION);
    langsResources_->tileDescriptionBedrock = find<std::string>(value, STRING_TILE_BEDROCK_DESCRIPTION);
    langsResources_->parallaxBackgroundClear = find<std::string>(value, "parallax_background_clear");
    langsResources_->parallaxBackgroundSet = find<std::string>(value, "parallax_background_set");
    langsResources_->parallaxBackgroundGet = find<std::string>(value, "parallax_background_get");
    langsResources_->parallaxBackgroundNone = find<std::string>(value, "parallax_background_none");
    langsResources_->biomeTemperatureInfo = find<std::string>(value, "biome_temperature_info");
    langsResources_->biomeHumidityInfo = find<std::string>(value, "biome_humidity_info");
    langsResources_->biomeElevationInfo = find<std::string>(value, "biome_elevation_info");
    langsResources_->biomeWeirdnessInfo = find<std::string>(value, "biome_weirdness_info");
    langsResources_->biomeErosionInfo = find<std::string>(value, "biome_erosion_info");
    langsResources_->biomeSurfaceProximityInfo = find<std::string>(value, "biome_surface_proximity_info");
    langsResources_->biomeTotalScore = find<std::string>(value, "biome_total_score");
    langsResources_->biomeScoreInspectorEnable = find<std::string>(value, "biome_score_inspector_enable");
    langsResources_->biomeScoreInspectorDisable = find<std::string>(value, "biome_score_inspector_disable");
    langsResources_->biomeScoreInspectorEnableFail = find<std::string>(value, "biome_score_inspector_enable_fail");
    langsResources_->biomeScoreInspectorDisableFail = find<
        std::string>(value, "biome_score_inspector_disable_fail");
    langsResources_->notEnabledSignVerify = find<std::string>(value, "not_enabled_sign_verify");
    langsResources_->unsignedPackage = find<std::string>(value, "unsigned");
    langsResources_->signatureVerificationSuccessful = find<
        std::string>(value, "signature_verification_successful");
    langsResources_->signatureVerificationFailed = find<std::string>(value, "signature_verification_failed");
    langsResources_->dataPackageCannotBeFound = find<std::string>(value, "data_package_cannot_be_found");
    langsResources_->tileSnapshotInfo = find<std::string>(value, "tile_snapshot_info");
    langsResources_->tileSnapshotInspectorEnable = find<std::string>(value, "tile_snapshot_inspector_enable");
    langsResources_->tileSnapshotInspectorDisable = find<std::string>(value, "tile_snapshot_inspector_disable");
    langsResources_->tileSnapshotInspectorEnableFail = find<std::string>(
        value, "tile_snapshot_inspector_enable_fail");
    langsResources_->tileSnapshotInspectorDisableFail = find<
        std::string>(value, "tile_snapshot_inspector_disable_fail");
    langsResources_->chunkHasNotBeenLoadedYet = find<std::string>(value, "chunk_has_not_been_loaded_yet");
    langsResources_->tileSnapshotsDoesNotExist = find<std::string>(value, "tile_snapshots_does_not_exist");
    langsResources_->itemEditorHoldItem = find<std::string>(value, "item_editor_hold_item");
    langsResources_->itemEditorReadAttr = find<std::string>(value, "item_editor_read_attr");
    langsResources_->itemEditorSetAttr = find<std::string>(value, "item_editor_set_attr");
    langsResources_->playerDoesNotExist = find<std::string>(value, "player_does_not_exist");
    langsResources_->craft = find<std::string>(value, "craft");
    langsResources_->technologyItem = find<std::string>(value, "technology_item");
    langsResources_->tagItem = find<std::string>(value, "tag_item");
    langsResources_->tagCannotFound = find<std::string>(value, "tag_cannot_found");
    langsResources_->noUnlockedRecipes = find<std::string>(value, "no_unlocked_recipes");
    langsResources_->recipesItem = find<std::string>(value, "recipes_item");
    langsResources_->debugChunkInfo = find<std::string>(value, "debug_chunk_info");
    langsResources_->fpsInfo = find<std::string>(value, "fps_info");
    langsResources_->lockedTip = find<std::string>(value, "locked_tip");
    langsResources_->timeH = find<std::string>(value, "time_h");
    langsResources_->timeM = find<std::string>(value, "time_m");
    langsResources_->timeS = find<std::string>(value, "time_s");
    langsResources_->savesDescription = find<std::string>(value, "saves_description");
}

std::string glimmer::AppContext::GetTimeFileName(const std::string& prefix, const std::string& ext)
{
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << prefix << "_"
        << std::put_time(&tm, "%Y%m%d_%H%M%S")
        << ext;

    return oss.str();
}

glimmer::AppContext::AppContext()
{
    windowContext_ = std::make_unique<WindowContext>();
    mainThreadDispatcher_ = std::make_unique<MainThreadDispatcher>();
    virtualFileSystem_ = std::make_unique<VirtualFileSystem>();
    VirtualFileSystem* virtualFileSystemPtr = virtualFileSystem_.get();
#ifdef __ANDROID__
    auto env = static_cast<JNIEnv*>(SDL_GetAndroidJNIEnv());
    if (env == nullptr)
    {
        initSuccess_ = false;
        LogCat::e(std::source_location::current(), "env is nullptr");
        return;
    }
    auto activity = static_cast<jobject>(SDL_GetAndroidActivity());
    if (activity == nullptr)
    {
        initSuccess_ = false;
        LogCat::e(std::source_location::current(), "activity is nullptr");
        return;
    }
    jclass activityClass = env->GetObjectClass(activity);
    if (activityClass == nullptr)
    {
        LogCat::e(std::source_location::current(), "activityClass is nullptr");
        return;
    }
    jmethodID getAssetsMethod = env->
        GetMethodID(activityClass, "getAssets", "()Landroid/content/res/AssetManager;");
    if (getAssetsMethod == nullptr)
    {
        LogCat::e(std::source_location::current(), "getAssetsMethod is nullptr");
        return;
    }
    jobject assetManagerJava = env->CallObjectMethod(activity, getAssetsMethod);
    if (assetManagerJava == nullptr)
    {
        LogCat::e(std::source_location::current(), "assetManagerJava is nullptr");
        return;
    }
    AAssetManager* assetManager = AAssetManager_fromJava(env, assetManagerJava);
    if (assetManager == nullptr)
    {
        LogCat::e(std::source_location::current(), "assetManager is nullptr");
        return;
    }
    auto assetsProvider = std::make_unique<AndroidAssetsFileProvider>(assetManager);
    std::optional<std::string> indexTomlOptional = assetsProvider->ReadFile("index.toml");
    if (!indexTomlOptional.has_value())
    {
        LogCat::e(std::source_location::current(), "assetManager is nullptr");
        return;
    }
    const toml::value tomlValue = toml::parse_str(indexTomlOptional.value(), tomlVersion_);
    auto assetsEntry = toml::get<std::vector<AndroidAssetEntry>>(tomlValue);
    assetsProvider->SetAssetEntryData(assetsEntry);
    jmethodID getDataDirMethod = env->GetMethodID(activityClass, "getFilesDir", "()Ljava/io/File;");
    if (getDataDirMethod == nullptr)
    {
        LogCat::e(std::source_location::current(), "getDataDirMethod is nullptr");
        return;
    }
    jobject dataDirFile = env->CallObjectMethod(activity, getDataDirMethod);
    if (dataDirFile == nullptr)
    {
        LogCat::e(std::source_location::current(), "dataDirFile is nullptr");
        return;
    }
    jclass fileClass = env->GetObjectClass(dataDirFile);
    if (fileClass == nullptr)
    {
        LogCat::e(std::source_location::current(), "fileClass is nullptr");
        return;
    }
    jmethodID getAbsolutePathMethod = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
    if (getAbsolutePathMethod == nullptr)
    {
        LogCat::e(std::source_location::current(), "getAbsolutePathMethod is nullptr");
        return;
    }
    auto absolutePathJStr = static_cast<jstring>(env->CallObjectMethod(dataDirFile, getAbsolutePathMethod));
    if (!absolutePathJStr)
    {
        LogCat::e(std::source_location::current(), "absolutePathJStr is nullptr");
        return;
    }
    const char* absolutePathCStr = env->GetStringUTFChars(absolutePathJStr, nullptr);
    if (!absolutePathCStr)
    {
        LogCat::e(std::source_location::current(), "absolutePathCStr is nullptr");
        return;
    }
    std::string dataDirPath(absolutePathCStr);
    env->ReleaseStringUTFChars(absolutePathJStr, absolutePathCStr);
    virtualFileSystem_->Mount(std::make_unique<StdFileProvider>(dataDirPath + "/assets"));
    virtualFileSystem_->Mount(std::unique_ptr<IFileProvider>(std::move(assetsProvider)));
#else
    virtualFileSystem_->Mount(
        std::make_unique<StdFileProvider>(std::filesystem::current_path().string()));
#endif
    language_ = LanguageUtils::getLanguage();
    std::filesystem::path langFile = "langs" / std::filesystem::path(language_);
    langFile.replace_extension("toml");
    if (!virtualFileSystem_->Exists(langFile))
    {
        LogCat::w(std::source_location::current(), "langFile not exist");
        langFile = "langs/default.toml";
    }
    const auto langData = virtualFileSystem_->ReadFileAsString(langFile);
    if (!langData.has_value())
    {
        LogCat::e(std::source_location::current(), "langData not has value");
        return;
    }
    langsResources_ = std::make_unique<LangsResources>();
    LoadLanguage(langData.value());
    audioContext_ = std::make_unique<AudioContext>();
    graphicsContext_ = std::make_unique<GraphicsContext>();
    sceneManager_ = std::make_unique<SceneManager>();
    config_ = std::make_unique<Config>();
    const std::optional<std::string> configData = virtualFileSystem_->ReadFileAsString(CONFIG_FILE_NAME);
    if (!configData.has_value())
    {
        LogCat::e(std::source_location::current(), "configData not has value");
        return;
    }
    configValue = std::make_unique<toml::value>(toml::parse_str(configData.value(), tomlVersion_));
    toml::value* configValuePtr = configValue.get();
    config_->LoadConfig(*configValuePtr);
    savesManager_ = std::make_unique<SavesManager>(virtualFileSystemPtr);
    savesManager_->LoadAllSaves(config_->runtimePath);
    modContext_ = std::make_unique<ModContext>();
    modContext_->Init(virtualFileSystemPtr, langsResources_.get());
    if (modContext_ == nullptr)
    {
        LogCat::e(std::source_location::current(), "modContext_ is nullptr");
        return;
    }
    consoleContext_ = std::make_unique<ConsoleContext>();
    if (!consoleContext_->Init(this, virtualFileSystemPtr, config_->runtimePath,
                               config_->console.maxHistoryEntries, configValuePtr))
    {
        LogCat::e(std::source_location::current(), "init consoleContext fail.");
        return;
    }

    DataPackManager* dataPackManager = modContext_->GetDataPackManager();
    if (dataPackManager == nullptr)
    {
        LogCat::e(std::source_location::current(), "dataPackManager is nullptr");
        return;
    }
    if (dataPackManager->Scan(this, tomlVersion_) == 0)
    {
        LogCat::e(std::source_location::current(), "The data package cannot be found.");
        return;
    }
    RecipeManager* recipeManager = modContext_->GetRecipeManager();
    if (recipeManager == nullptr)
    {
        LogCat::e(std::source_location::current(), "recipeManager is nullptr");
        return;
    }
    recipeManager->PreSortRecipes();
    resourcePackManager_ = std::make_unique<ResourcePackManager>(virtualFileSystemPtr);
    if (resourcePackManager_ == nullptr)
    {
        LogCat::e(std::source_location::current(), "resourcePackManager_ is nullptr");
        return;
    }
    if (resourcePackManager_->Scan(config_->mods.resourcePackPath, config_->mods.enabledResourcePack,
                                   tomlVersion_) == 0)
    {
        LogCat::e(std::source_location::current(), "The resource package cannot be found.");
        return;
    }
    const MobManager* mobManager = modContext_->GetMobManager();
    if (mobManager == nullptr)
    {
        LogCat::e(std::source_location::current(), "mobManager is nullptr");
        return;
    }
    if (const size_t number = mobManager->GetPlayerResourceList().size(); number == 0)
    {
        LogCat::e(std::source_location::current(), "At least one player resource is required.");
        return;
    }
    resourceLocator_ = std::make_unique<ResourceLocator>(this);
    graphicsContext_->Init(resourceLocator_.get());
    initSuccess_ = true;
}

void glimmer::AppContext::LoadMainMenuBGM() const
{
    audioContext_->LoadMainMenuBGM(resourceLocator_.get());
}

void glimmer::AppContext::PlayMainMenuBGM() const
{
    audioContext_->PlayMainMenuBGM();
}

const toml::spec& glimmer::AppContext::GetTomlVersion() const
{
    return tomlVersion_;
}

glimmer::AppContext::~AppContext()
{
    sceneManager_->ClearScenes();
}

bool glimmer::AppContext::InitSuccess() const
{
    return initSuccess_;
}

glimmer::WindowContext* glimmer::AppContext::GetWindowContext() const
{
    return windowContext_.get();
}

void glimmer::AppContext::AddUIMessage(const std::string& string)
{
    if (!gameUIMessages_.empty())
    {
        const uint64_t lastFingerprint = gameUIMessages_.back().GetFingerprint();
        const uint64_t stringFingerprint = StringUtils::StringToUint64(string);
        if (lastFingerprint == stringFingerprint)
        {
            return;
        }
    }
    if (resourcePackManager_ == nullptr)
    {
        return;
    }

    mainThreadDispatcher_->PostToNextMainFrame([this, string]
    {
        gameUIMessages_.emplace_back(resourcePackManager_.get(), string, SDL_GetTicks(),
                                     &graphicsContext_->GetPreloadColors()->textColor);
    });
}

std::vector<glimmer::GameUIMessage>& glimmer::AppContext::GetGameUIMessages()
{
    return gameUIMessages_;
}

void glimmer::AppContext::ExitApp() const
{
    consoleContext_->StopConsoleWorker();
    if (config_->console.maxHistoryEntries > 0)
    {
        consoleContext_->SaveCommandHistory();
    }
    windowContext_->Exit();
}

void glimmer::AppContext::CreateScreenshot(const std::function<void(const std::string& text)>* onMessage) const
{
    if (onMessage == nullptr)
    {
        return;
    }
    const std::function<void(const std::string& text)>& onMessageRef = *onMessage;
    SDL_Renderer* renderer_ = windowContext_->GetRenderer();
    if (!renderer_)
    {
        onMessageRef(fmt::format(
            fmt::runtime(GetLangsResources()->screenshotSavedFailed),
            "renderer is null failed"));
        return;
    }
    const std::filesystem::path screenshotsFolder = std::filesystem::path(config_->runtimePath) / "screenshots";
    if (!virtualFileSystem_->Exists(screenshotsFolder) && !virtualFileSystem_->CreateFolder(screenshotsFolder))
    {
        onMessageRef(fmt::format(
            fmt::runtime(GetLangsResources()->screenshotSavedFailed),
            "CreateFolder failed"));
        return;
    }
    const auto actualPath = virtualFileSystem_->GetActualPath(
        screenshotsFolder / GetTimeFileName());
    if (!actualPath.has_value())
    {
        onMessageRef(fmt::format(
            fmt::runtime(GetLangsResources()->screenshotSavedFailed),
            "GetActualPath failed"));
        return;
    }
    int width = 0;
    int height = 0;
    if (!SDL_GetRenderOutputSize(renderer_, &width, &height))
    {
        onMessageRef(fmt::format(
            fmt::runtime(GetLangsResources()->screenshotSavedFailed),
            "SDL_GetRenderOutputSize failed"));
        return;
    }
    auto sdlRect = SDL_Rect();
    sdlRect.x = 0;
    sdlRect.y = 0;
    sdlRect.w = width;
    sdlRect.h = height;
    SDL_Surface* surface = SDL_RenderReadPixels(
        renderer_, &sdlRect);
    if (surface == nullptr)
    {
        SDL_DestroySurface(surface);
        onMessageRef(fmt::format(
            fmt::runtime(GetLangsResources()->screenshotSavedFailed),
            "SDL_RenderReadPixels failed"));
        return;
    }
    const bool result = IMG_SavePNG(surface, actualPath.value().c_str());
    SDL_DestroySurface(surface);
    if (result)
    {
        onMessageRef(fmt::format(
            fmt::runtime(GetLangsResources()->screenshotSavedSuccess),
            actualPath.value().string()));
    }
    else
    {
        onMessageRef(
            fmt::format(
                fmt::runtime(GetLangsResources()->screenshotSavedFailed),
                "IMG_SavePNG Failed"));
    }
}

glimmer::ModContext* glimmer::AppContext::GetModContext() const
{
    if (modContext_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "modContext is null");
        return nullptr;
    }
    return modContext_.get();
}

glimmer::ConsoleContext* glimmer::AppContext::GetConsoleContext() const
{
    if (consoleContext_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "consoleContext is null");
        return nullptr;
    }
    return consoleContext_.get();
}

glimmer::GraphicsContext* glimmer::AppContext::GetGraphicsContext() const
{
    if (graphicsContext_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "graphicsContext is null");
        return nullptr;
    }
    return graphicsContext_.get();
}

glimmer::AudioContext* glimmer::AppContext::GetAudioContext() const
{
    if (audioContext_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "audioContext is null");
        return nullptr;
    }
    return audioContext_.get();
}

glimmer::MainThreadDispatcher* glimmer::AppContext::GetMainThreadDispatcher() const
{
    if (mainThreadDispatcher_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "mainThreadDispatcher is null");
        return nullptr;
    }
    return mainThreadDispatcher_.get();
}

glimmer::Config* glimmer::AppContext::GetConfig() const
{
    if (config_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "config is null");
        return nullptr;
    }
    return config_.get();
}

glimmer::LangsResources* glimmer::AppContext::GetLangsResources() const
{
    if (langsResources_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "langsResources is null");
        return nullptr;
    }
    return langsResources_.get();
}

glimmer::ResourcePackManager* glimmer::AppContext::GetResourcePackManager() const
{
    if (resourcePackManager_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "resourcePackManager is null");
        return nullptr;
    }
    return resourcePackManager_.get();
}

glimmer::ResourceLocator* glimmer::AppContext::GetResourceLocator() const
{
    if (resourceLocator_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "resourceLocator is null");
        return nullptr;
    }
    return resourceLocator_.get();
}

glimmer::VirtualFileSystem* glimmer::AppContext::GetVirtualFileSystem() const
{
    if (virtualFileSystem_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "virtualFileSystem is null");
        return nullptr;
    }
    return virtualFileSystem_.get();
}

glimmer::SceneManager* glimmer::AppContext::GetSceneManager() const
{
    if (sceneManager_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "sceneManager is null");
        return nullptr;
    }
    return sceneManager_.get();
}

glimmer::SavesManager* glimmer::AppContext::GetSavesManager() const
{
    if (savesManager_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "savesManager is null");
        return nullptr;
    }
    return savesManager_.get();
}

void glimmer::AppContext::SetRandomSlogan() const
{
    if (windowContext_->GetWindow() == nullptr || langsResources_ == nullptr)
    {
        windowContext_->SetWindowTitle(PROJECT_NAME.c_str());
        return;
    }
    const std::vector<std::string>& slogans = langsResources_->slogans;
    if (slogans.empty())
    {
        windowContext_->SetWindowTitle(PROJECT_NAME.c_str());
    }
    else
    {
        const int idx = RandomUtils::Random(0, static_cast<int>(slogans.size()) - 1);
        const std::string& random_str = slogans[idx];
        windowContext_->SetWindowTitle(random_str.c_str());
    }
}

void glimmer::AppContext::RestoreColorRenderer(SDL_Renderer* sdlRenderer)
{
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
}

const std::string& glimmer::AppContext::GetLanguage() const
{
    return language_;
}
