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

#include "core/console/command/AssetViewerCommand.h"
#include "core/console/command/BiomeScoreCommand.h"
#include "core/console/command/Box2DCommand.h"
#include "core/console/command/EcsCommand.h"
#include "core/console/command/GiveCommand.h"
#include "core/console/command/HelpCommand.h"
#include "core/console/command/LicenseCommand.h"
#include "core/console/command/SeedCommand.h"
#include "core/console/command/TpCommand.h"
#include "core/console/command/VFSCommand.h"
#include "core/console/suggestion/AbilityItemDynamicSuggestions.h"
#include "core/console/suggestion/BoolDynamicSuggestions.h"
#include "core/console/suggestion/ComposableItemDynamicSuggestions.h"
#include "core/console/suggestion/TileDynamicSuggestions.h"
#include "core/console/suggestion/VFSDynamicSuggestions.h"
#include "core/log/LogCat.h"
#include "core/utils/LanguageUtils.h"
#include "core/vfs/StdFileProvider.h"
#include "core/console/command/ClearCommand.h"
#include "core/console/command/ConfigCommand.h"
#include "core/console/command/EchoCommand.h"
#include "core/console/command/FlyCommand.h"
#include "core/console/command/HookCommand.h"
#include "core/console/command/ItemEditorCommand.h"
#include "core/console/command/LightCommand.h"
#include "core/console/command/LocateCommand.h"
#include "core/console/command/LootCommand.h"
#include "core/console/command/PackVerifyCommand.h"
#include "core/console/command/ParallaxBackgroundCommand.h"
#include "core/console/command/PlaceCommand.h"
#include "core/console/command/PlayCommand.h"
#include "core/console/command/ScreenshotCommand.h"
#include "core/console/command/SummonCommand.h"
#include "core/console/command/TagCommand.h"
#include "core/console/command/TechnologyCommand.h"
#include "core/console/command/TileSnapshotCommand.h"
#include "core/console/command/UnlockedRecipesCommand.h"
#include "core/console/suggestion/AllocStrategyTypeDynamicSuggestions.h"
#include "core/console/suggestion/AudioTrackDynamicSuggestions.h"
#include "core/console/suggestion/BiomeSuggestions.h"
#include "core/console/suggestion/BooleanToggleDynamicSuggestions.h"
#include "core/console/suggestion/CommandHookIdDynamicSuggestions.h"
#include "core/console/suggestion/CommandHookScopeDynamicSuggestions.h"
#include "core/console/suggestion/ConfigSuggestions.h"
#include "core/console/suggestion/CoordinateDynamicSuggestions.h"
#include "core/console/suggestion/DataPackDynamicSuggestions.h"
#include "core/console/suggestion/DynamicSuggestionsManager.h"
#include "core/console/suggestion/EventTypeDynamicSuggestions.h"
#include "core/console/suggestion/LootSuggestions.h"
#include "core/console/suggestion/MaterialItemDynamicSuggestions.h"
#include "core/console/suggestion/MobDynamicSuggestions.h"
#include "core/console/suggestion/MouseButtonDynamicSuggestions.h"
#include "core/console/suggestion/ScanKeyDynamicSuggestions.h"
#include "core/console/suggestion/StructureDynamicSuggestions.h"
#include "core/mod/resourcePack/AudioResourceResult.h"
#include "core/mod/templateCommand/InsertTemplateCommand.h"
#include "core/mod/templateCommand/SetTemplateCommand.h"
#include "core/mod/templateCommand/UnSetTemplateCommand.h"
#include "core/saves/SavesManager.h"
#include "core/utils/RandomUtils.h"
#include "core/utils/StringUtils.h"
#include "core/world/generator/FillBiomeDecorator.h"
#include "core/world/generator/MineralBiomeDecorator.h"
#include "core/world/generator/SurfaceBiomeDecorator.h"
#include "core/world/structure/BiomeStructureConditionProcessor.h"
#include "core/world/structure/HeightStructureConditionProcessor.h"
#include "core/world/structure/HorizontalSpacingStructureConditionProcessor.h"
#include "core/world/structure/StaticStructureGenerator.h"
#include "core/world/structure/SurfaceStructureConditionProcessor.h"
#include "core/world/structure/TreeStructureGenerator.h"
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
    langsResources_->biomeScoreInspectorDisableFail = find<std::string>(value, "biome_score_inspector_disable_fail");
    langsResources_->notEnabledSignVerify = find<std::string>(value, "not_enabled_sign_verify");
    langsResources_->unsignedPackage = find<std::string>(value, "unsigned");
    langsResources_->signatureVerificationSuccessful = find<std::string>(value, "signature_verification_successful");
    langsResources_->signatureVerificationFailed = find<std::string>(value, "signature_verification_failed");
    langsResources_->dataPackageCannotBeFound = find<std::string>(value, "data_package_cannot_be_found");
    langsResources_->tileSnapshotInfo = find<std::string>(value, "tile_snapshot_info");
    langsResources_->tileSnapshotInspectorEnable = find<std::string>(value, "tile_snapshot_inspector_enable");
    langsResources_->tileSnapshotInspectorDisable = find<std::string>(value, "tile_snapshot_inspector_disable");
    langsResources_->tileSnapshotInspectorEnableFail = find<std::string>(value, "tile_snapshot_inspector_enable_fail");
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
    localtime_s(&tm, &t); // Windows 安全函数
#else
    localtime_r(&t, &tm); // Linux/macOS
#endif
    std::ostringstream oss;
    oss << prefix << "_"
        << std::put_time(&tm, "%Y%m%d_%H%M%S")
        << ext;

    return oss.str();
}

glimmer::AppContext::AppContext()
{
    mainThreadId_ = std::this_thread::get_id();
    virtualFileSystem_ = std::make_unique<VirtualFileSystem>();
#ifdef __ANDROID__
    auto env = static_cast<JNIEnv*>(SDL_GetAndroidJNIEnv());
    if (env == nullptr)
    {
        LogCat::e("Failed to get JNIEnv!");
        initSuccess_ = false;
        return;
    }
    auto activity = static_cast<jobject>(SDL_GetAndroidActivity());
    if (activity == nullptr)
    {
        LogCat::e("Failed to get Android activity!");
        initSuccess_ = false;
        return;
    }
    jclass activityClass = env->GetObjectClass(activity);
    if (activityClass == nullptr)
    {
        LogCat::e("Android: Failed to get activity class!");
        return;
    }
    jmethodID getAssetsMethod = env->GetMethodID(activityClass, "getAssets", "()Landroid/content/res/AssetManager;");
    if (getAssetsMethod == nullptr)
    {
        LogCat::e("Android: Failed to find 'getAssets' method!");
        return;
    }
    jobject assetManagerJava = env->CallObjectMethod(activity, getAssetsMethod);
    if (assetManagerJava == nullptr)
    {
        LogCat::e("Android: Failed to get AssetManager instance!");
        return;
    }
    AAssetManager* assetManager = AAssetManager_fromJava(env, assetManagerJava);
    if (assetManager == nullptr)
    {
        LogCat::e("Android: Failed to convert to native AAssetManager!");
        return;
    }
    auto assetsProvider = std::make_unique<AndroidAssetsFileProvider>(assetManager);
    std::optional<std::string> indexTomlOptional = assetsProvider->ReadFile("index.toml");
    if (!indexTomlOptional.has_value())
    {
        LogCat::e("Android: 'index.toml' not found or could not be read!");
        return;
    }
    const toml::value tomlValue = toml::parse_str(indexTomlOptional.value(), tomlVersion_);
    auto assetsEntry = toml::get<std::vector<AndroidAssetEntry>>(tomlValue);
    assetsProvider->SetAssetEntryData(assetsEntry);
    jmethodID getDataDirMethod = env->GetMethodID(activityClass, "getFilesDir", "()Ljava/io/File;");
    if (getDataDirMethod == nullptr)
    {
        LogCat::e("Android: Failed to find 'getFilesDir' method!");
        return;
    }
    jobject dataDirFile = env->CallObjectMethod(activity, getDataDirMethod);
    if (dataDirFile == nullptr)
    {
        LogCat::e("Android: Failed to get data directory File object!");
        return;
    }
    jclass fileClass = env->GetObjectClass(dataDirFile);
    if (fileClass == nullptr)
    {
        LogCat::e("Android: Failed to get File class!");
        return;
    }
    jmethodID getAbsolutePathMethod = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
    if (getAbsolutePathMethod == nullptr)
    {
        LogCat::e("Android: Failed to find 'getAbsolutePath' method!");
        return;
    }
    auto absolutePathJStr = static_cast<jstring>(env->CallObjectMethod(dataDirFile, getAbsolutePathMethod));
    if (!absolutePathJStr)
    {
        LogCat::e("Android: Failed to get absolute path string!");
        return;
    }
    const char* absolutePathCStr = env->GetStringUTFChars(absolutePathJStr, nullptr);
    if (!absolutePathCStr)
    {
        LogCat::e("Android: Failed to convert JString to UTF-8!");
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
    LogCat::i("Load the built-in language file.");
    std::filesystem::path langFile = "langs" / std::filesystem::path(language_);
    langFile.replace_extension("toml");
    LogCat::i("Try to load language file:", langFile);
    if (!virtualFileSystem_->Exists(langFile))
    {
        LogCat::w("Not found, fall back to default.toml");
        langFile = "langs/default.toml";
    }
    VirtualFileSystem* vfs = virtualFileSystem_.get();
    const auto langData = virtualFileSystem_->ReadFileAsString(langFile);
    if (!langData.has_value())
    {
        LogCat::e("Failed to load language file!");
        return;
    }
    langsResources_ = std::make_unique<LangsResources>();
    contributorManager_ = std::make_unique<ContributorManager>();
    recipeManager_ = std::make_unique<RecipeManager>();
    mobManager_ = std::make_unique<MobManager>();
    shapeManager_ = std::make_unique<ShapeManager>();
    audioManager_ = std::make_unique<AudioManager>();
    lightSourceManager_ = std::make_unique<LightSourceManager>();
    lightMaskManager_ = std::make_unique<LightMaskManager>();
    fixedColorManager_ = std::make_unique<FixedColorManager>();
    tomlTemplateExpander_ = std::make_unique<TomlTemplateExpander>();
    tomlTemplateExpander_->Register(std::make_unique<InsertTemplateCommand>());
    tomlTemplateExpander_->Register(std::make_unique<SetTemplateCommand>());
    tomlTemplateExpander_->Register(std::make_unique<UnSetTemplateCommand>());
    LoadLanguage(langData.value());
    dynamicSuggestionsManager_ = std::make_unique<DynamicSuggestionsManager>();
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<BoolDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<CommandHookScopeDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<CommandHookIdDynamicSuggestions>(commandHookManager_.get()));
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<ScanKeyDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<MouseButtonDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<EventTypeDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<AllocStrategyTypeDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<AudioTrackDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<CoordinateDynamicSuggestions>(Y_DYNAMIC_SUGGESTIONS_NAME));
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<CoordinateDynamicSuggestions>(X_DYNAMIC_SUGGESTIONS_NAME));
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<MobDynamicSuggestions>(mobManager_.get()));
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<BoolDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<BooleanToggleDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<VFSDynamicSuggestions>(vfs));
    dataPackManager_ = std::make_unique<DataPackManager>(vfs, tomlTemplateExpander_.get());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<DataPackDynamicSuggestions>(dataPackManager_.get()));
    resourcePackManager_ = std::make_unique<ResourcePackManager>(vfs);
    resourceLocator_ = std::make_unique<ResourceLocator>(this);
    sceneManager_ = std::make_unique<SceneManager>();
    stringManager_ = std::make_unique<StringManager>();
    stringManager_->LoadLangsString(langsResources_.get());
    biomesManager_ = std::make_unique<BiomesManager>();
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<BiomeSuggestions>(biomesManager_.get()));
    tileResourceManager_ = std::make_unique<TileResourceManager>();
    structurePlacementConditionsManager_ = std::make_unique<StructurePlacementConditionsManager>();
    structurePlacementConditionsManager_->AddConditionProcessor(std::make_unique<SurfaceStructureConditionProcessor>());
    structurePlacementConditionsManager_->AddConditionProcessor(std::make_unique<HeightStructureConditionProcessor>());
    structurePlacementConditionsManager_->AddConditionProcessor(
        std::make_unique<HorizontalSpacingStructureConditionProcessor>());
    structurePlacementConditionsManager_->AddConditionProcessor(std::make_unique<BiomeStructureConditionProcessor>());
    initialInventoryManager_ = std::make_unique<InitialInventoryManager>();
    tileResourceManager_->InitBuiltinTiles();
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<TileDynamicSuggestions>(tileResourceManager_.get()));
    commandManager_ = std::make_unique<CommandManager>();
    commandManager_->RegisterCommand(std::make_unique<GiveCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<HelpCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<TpCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<SummonCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<PlaceCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<ClearCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<LootCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<PackVerifyCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<LicenseCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<SeedCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<FlyCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<EchoCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<ScreenshotCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<LocateCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<ItemEditorCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<PlayCommand>(this));
#if  !defined(NDEBUG)
    commandManager_->RegisterCommand(std::make_unique<HookCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<TileSnapshotCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<EcsCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<VFSCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<LightCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<Box2DCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<BiomeScoreCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<AssetViewerCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<ParallaxBackgroundCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<TechnologyCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<TagCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<UnlockedRecipesCommand>(this));
#endif
    consoleWorker_ = std::make_unique<ConsoleWorker>(commandManager_.get());
    biomeDecoratorManager_ = std::make_unique<BiomeDecoratorManager>();
    biomeDecoratorResourcesManager_ = std::make_unique<BiomeDecoratorResourcesManager>();
    itemManager_ = std::make_unique<ItemManager>();
    ItemManager* im = itemManager_.get();
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<ComposableItemDynamicSuggestions>(im));
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<AbilityItemDynamicSuggestions>(im));
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<MaterialItemDynamicSuggestions>(im));
    biomeDecoratorManager_->RegisterBiomeDecorator(std::make_unique<FillBiomeDecorator>());
    biomeDecoratorManager_->RegisterBiomeDecorator(std::make_unique<SurfaceBiomeDecorator>());
    biomeDecoratorManager_->RegisterBiomeDecorator(std::make_unique<MineralBiomeDecorator>());
    config_ = std::make_unique<Config>();
    LogCat::i("Loading ",CONFIG_FILE_NAME, "...");
    const std::optional<std::string> configData = vfs->ReadFileAsString(CONFIG_FILE_NAME);
    if (!configData.has_value())
    {
        LogCat::e("Failed to read ",CONFIG_FILE_NAME, " file!");
    }
    configValue = std::make_unique<toml::value>(toml::parse_str(configData.value(), tomlVersion_));
    toml::value* configValuePtr = configValue.get();
    config_->LoadConfig(*configValuePtr);
    commandHookManager_ = std::make_unique<CommandHookManager>();
    commandManager_->RegisterCommand(std::make_unique<ConfigCommand>(this, configValuePtr));
    dynamicSuggestionsManager_->
        RegisterDynamicSuggestions(std::make_unique<ConfigSuggestions>(configValuePtr));
    LogCat::i("windowHeight = ", config_->window.height);
    LogCat::i("windowWidth = ", config_->window.width);
    LogCat::i("dataPackPath = ", config_->mods.dataPackPath);
    LogCat::i("resourcePackPath = ", config_->mods.resourcePackPath);
    LogCat::i("framerate = ", config_->window.normalTargetFps);
    LogCat::i("The ",CONFIG_FILE_NAME, " load was successful.");
    savesManager_ = std::make_unique<SavesManager>(vfs);
    savesManager_->LoadAllSaves(config_->runtimePath);
    lootTableManager_ = std::make_unique<LootTableManager>();
    structureGeneratorManager_ = std::make_unique<StructureGeneratorManager>();
    structureGeneratorManager_->RegisterStructureGenerator(std::make_unique<StaticStructureGenerator>());
    structureGeneratorManager_->RegisterStructureGenerator(std::make_unique<TreeStructureGenerator>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<LootSuggestions>(lootTableManager_.get()));
    structureManager_ = std::make_unique<StructureManager>();
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<StructureDynamicSuggestions>(structureManager_.get()));
    LogCat::i("GAME_VERSION_NUMBER = ", GAME_VERSION_NUMBER);
    LogCat::i("GAME_VERSION_STRING = ", GAME_VERSION_STRING);
    LogCat::i("Starting GlimmerWorks...");
    if (dataPackManager_->Scan(this, tomlVersion_) == 0)
    {
        LogCat::e("Failed to load dataPack");
        return;
    }
    recipeManager_->PreSortRecipes();
    if (resourcePackManager_->Scan(config_->mods.resourcePackPath, config_->mods.enabledResourcePack,
                                   tomlVersion_) == 0)
    {
        LogCat::e("Failed to load resourcePack");
        return;
    }
    preloadColors_ = std::make_unique<PreloadColors>();
    preloadColors_->LoadAllColors(resourceLocator_.get());
    const size_t number = mobManager_->GetPlayerResourceList().size();
    if (number == 0)
    {
        LogCat::e("At least one of the mob files must have the \"isPlayer = true\" setting.");
        return;
    }
    commandHistoryManager_ = std::make_unique<CommandHistoryManager>(config_->runtimePath, virtualFileSystem_.get());
    if (config_->console.maxHistoryEntries > 0)
    {
        commandHistoryManager_->Read();
    }
    LogCat::i("Starting the app...");
    initSuccess_ = true;
}

void glimmer::AppContext::LoadMainMenuBGM()
{
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_AUDIO);
    resourceRef.SetResourceKey("bgm/main_menu");
    mainMenuBGM_ = resourceLocator_->FindAudio(&resourceRef);
}

void glimmer::AppContext::PlayMainMenuBGM() const
{
    if (audioManager_ == nullptr || mainMenuBGM_ == nullptr)
    {
        return;
    }
    MIX_Audio* audio = mainMenuBGM_->GetResource();
    if (audio == nullptr)
    {
        return;
    }
    audioManager_->ForcePlayReplace(BGM, audio, -1);
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

void glimmer::AppContext::SetWindowWidth(const int width)
{
    windowWidth_ = width;
}

void glimmer::AppContext::SetWindowHeight(const int height)
{
    windowHeight_ = height;
}

void glimmer::AppContext::SetWindowTitle(const char* title) const
{
    SDL_SetWindowTitle(window_, title);
}

int glimmer::AppContext::GetWindowWidth() const
{
    return windowWidth_;
}

int glimmer::AppContext::GetWindowHeight() const
{
    return windowHeight_;
}


void glimmer::AppContext::SetWindow(SDL_Window* window)
{
    this->window_ = window;
}

void glimmer::AppContext::SetRenderer(SDL_Renderer* renderer)
{
    this->renderer_ = renderer;
}

void glimmer::AppContext::CreateScreenshot(const std::function<void(const std::string& text)>* onMessage) const
{
    if (onMessage == nullptr)
    {
        return;
    }
    const std::function<void(const std::string& text)>& onMessageRef = *onMessage;
    if (!renderer_)
    {
        onMessageRef(fmt::format(
            fmt::runtime(GetLangsResources()->screenshotSavedFailed),
            "renderer is null failed"));
        return;
    }
    const std::filesystem::path screenshotsFolder = std::filesystem::path(config_->runtimePath) / "screenshots";
    if (!virtualFileSystem_->Exists(screenshotsFolder))
    {
        if (!virtualFileSystem_->CreateFolder(screenshotsFolder))
        {
            onMessageRef(fmt::format(
                fmt::runtime(GetLangsResources()->screenshotSavedFailed),
                "CreateFolder failed"));
            return;
        }
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

bool glimmer::AppContext::Running() const
{
    return isRunning;
}

void glimmer::AppContext::AddUIMessage(const std::string& string)
{
    LogCat::d(string);
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

    PostToNextMainFrame([this, string]
    {
        gameUIMessages_.emplace_back(resourcePackManager_.get(), string, SDL_GetTicks(), &preloadColors_->textColor);
    });
}

std::vector<glimmer::GameUIMessage>& glimmer::AppContext::GetGameUIMessages()
{
    return gameUIMessages_;
}

void glimmer::AppContext::ExitApp()
{
    if (consoleWorker_ != nullptr)
    {
        consoleWorker_->Stop();
    }
    if (config_->console.maxHistoryEntries > 0)
    {
        commandHistoryManager_->Save();
    }
    isRunning = false;
}

glimmer::PreloadColors* glimmer::AppContext::GetPreloadColors() const
{
    return preloadColors_.get();
}

glimmer::Config* glimmer::AppContext::GetConfig() const
{
    return config_.get();
}

glimmer::InitialInventoryManager* glimmer::AppContext::GetInitialInventoryManager() const
{
    return initialInventoryManager_.get();
}

glimmer::CommandManager* glimmer::AppContext::GetCommandManager() const
{
    return commandManager_.get();
}

glimmer::AudioManager* glimmer::AppContext::GetAudioManager() const
{
    return audioManager_.get();
}

glimmer::ConsoleWorker* glimmer::AppContext::GetConsoleWorker() const
{
    return consoleWorker_.get();
}

glimmer::LightMaskManager* glimmer::AppContext::GetLightMaskManager() const
{
    return lightMaskManager_.get();
}

glimmer::LightSourceManager* glimmer::AppContext::GetLightSourceManager() const
{
    return lightSourceManager_.get();
}

glimmer::FixedColorManager* glimmer::AppContext::GetFixedColorManager() const
{
    return fixedColorManager_.get();
}

glimmer::RecipeManager* glimmer::AppContext::GetRecipeManager() const
{
    return recipeManager_.get();
}

glimmer::StringManager* glimmer::AppContext::GetStringManager() const
{
    return stringManager_.get();
}

glimmer::CommandHookManager* glimmer::AppContext::GetCommandHookManager() const
{
    return commandHookManager_.get();
}

glimmer::BiomeDecoratorResourcesManager* glimmer::AppContext::GetBiomeDecoratorResourcesManager() const
{
    return biomeDecoratorResourcesManager_.get();
}

glimmer::TomlTemplateExpander* glimmer::AppContext::GetTomlTemplateExpander() const
{
    return tomlTemplateExpander_.get();
}

glimmer::DynamicSuggestionsManager* glimmer::AppContext::GetDynamicSuggestionsManager() const
{
    return dynamicSuggestionsManager_.get();
}

const std::string& glimmer::AppContext::GetLanguage() const
{
    return language_;
}

glimmer::CommandHistoryManager* glimmer::AppContext::GetCommandHistoryManager() const
{
    return commandHistoryManager_.get();
}

glimmer::LangsResources* glimmer::AppContext::GetLangsResources() const
{
    return langsResources_.get();
}

glimmer::ResourcePackManager* glimmer::AppContext::GetResourcePackManager() const
{
    return resourcePackManager_.get();
}

glimmer::StructurePlacementConditionsManager* glimmer::AppContext::GetStructurePlacementConditionsManager() const
{
    return structurePlacementConditionsManager_.get();
}

glimmer::TileResourceManager* glimmer::AppContext::GetTileResourceManager() const
{
    return tileResourceManager_.get();
}

glimmer::DataPackManager* glimmer::AppContext::GetDataPackManager() const
{
    return dataPackManager_.get();
}

glimmer::LootTableManager* glimmer::AppContext::GetLootTableManager() const
{
    return lootTableManager_.get();
}

glimmer::ContributorManager* glimmer::AppContext::GetContributorManager() const
{
    return contributorManager_.get();
}

glimmer::StructureManager* glimmer::AppContext::GetStructureManager() const
{
    return structureManager_.get();
}

glimmer::StructureGeneratorManager* glimmer::AppContext::GetStructureGeneratorManager() const
{
    return structureGeneratorManager_.get();
}

glimmer::BiomeDecoratorManager* glimmer::AppContext::GetBiomeDecoratorManager() const
{
    return biomeDecoratorManager_.get();
}

glimmer::BiomesManager* glimmer::AppContext::GetBiomesManager() const
{
    return biomesManager_.get();
}

glimmer::ShapeManager* glimmer::AppContext::GetShapeManager() const
{
    return shapeManager_.get();
}

glimmer::ResourceLocator* glimmer::AppContext::GetResourceLocator() const
{
    return resourceLocator_.get();
}

glimmer::VirtualFileSystem* glimmer::AppContext::GetVirtualFileSystem() const
{
    return virtualFileSystem_.get();
}

glimmer::ItemManager* glimmer::AppContext::GetItemManager() const
{
    return itemManager_.get();
}

glimmer::SceneManager* glimmer::AppContext::GetSceneManager() const
{
    return sceneManager_.get();
}

void glimmer::AppContext::SetRandomSlogan() const
{
    if (window_ == nullptr || langsResources_ == nullptr)
    {
        SDL_SetWindowTitle(window_, PROJECT_NAME.c_str());
        return;
    }
    const std::vector<std::string>& slogans = langsResources_->slogans;
    if (slogans.empty())
    {
        SDL_SetWindowTitle(window_, PROJECT_NAME.c_str());
    }
    else
    {
        const int idx = RandomUtils::Random(0, static_cast<int>(slogans.size()) - 1);
        const std::string& random_str = slogans[idx];
        SDL_SetWindowTitle(window_, random_str.c_str());
    }
}

glimmer::SavesManager* glimmer::AppContext::GetSavesManager() const
{
    return savesManager_.get();
}

glimmer::MobManager* glimmer::AppContext::GetMobManager() const
{
    return mobManager_.get();
}

bool glimmer::AppContext::IsMainThread() const
{
    return std::this_thread::get_id() == mainThreadId_;
}

void glimmer::AppContext::ProcessMainThreadTasks()
{
    std::queue<std::function<void()>> tasks;
    {
        std::lock_guard lock(mainThreadMutex_);
        std::swap(tasks, mainThreadTasks_);
    }

    while (!tasks.empty())
    {
        tasks.front()();
        tasks.pop();
    }
}

void glimmer::AppContext::RestoreColorRenderer(SDL_Renderer* sdlRenderer)
{
    //Set the default renderer color to black.
    //设置默认渲染器颜色为黑色。
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
}

void glimmer::AppContext::RunOnMainThread(std::function<void()> task)
{
    if (IsMainThread())
    {
        task();
        return;
    }
    std::lock_guard lock(mainThreadMutex_);
    mainThreadTasks_.push(std::move(task));
}

void glimmer::AppContext::PostToNextMainFrame(std::function<void()> task)
{
    std::lock_guard lock(mainThreadMutex_);
    mainThreadTasks_.push(std::move(task));
}
