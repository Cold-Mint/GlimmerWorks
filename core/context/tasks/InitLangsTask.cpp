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
#include "InitLangsTask.h"

#include "core/utils/LangsResources.h"
#include "core/vfs/VirtualFileSystem.h"
#include "toml11/find.hpp"
#include "toml11/parser.hpp"

bool glimmer::InitLangsTask::Run(SystemBucket *systemBucket) {
    const toml::spec *tomlVersion = systemBucket->GetTomlVersion();
    if (tomlVersion == nullptr) {
        return false;
    }
    const VirtualFileSystem *virtualFileSystem = systemBucket->GetVirtualFileSystem();
    if (virtualFileSystem == nullptr) {
        return false;
    }
    std::filesystem::path langFile = "langs" / std::filesystem::path(systemBucket->GetLanguage());
    langFile.replace_extension("toml");
    if (!virtualFileSystem->Exists(langFile)) {
        langFile = "langs/default.toml";
    }
    const auto langData = virtualFileSystem->ReadFileAsString(langFile);
    if (!langData.has_value()) {
        return false;
    }
    auto tomlValue = toml::parse_str(langData.value(), *tomlVersion);
    auto langsResources = std::make_unique<LangsResources>();
    langsResources->savedGames = toml::find<std::string>(tomlValue, "saved_games");
    langsResources->commandInfo = toml::find<std::string>(tomlValue, "command_info");
    langsResources->awakeBodyCount = toml::find<std::string>(tomlValue, "awake_body_count");
    langsResources->getActualPathError = toml::find<std::string>(tomlValue, "get_actual_path_error");
    langsResources->unknownAssetType = toml::find<std::string>(tomlValue, "unknown_asset_type");
    langsResources->unknownCommandParameters = toml::find<std::string>(tomlValue, "unknown_command_parameters");
    langsResources->worldContextIsNull = toml::find<std::string>(tomlValue, "world_context_is_null");
    langsResources->cheatsNotAllowed = toml::find<std::string>(tomlValue, "cheats_not_allowed");
    langsResources->insufficientParameterLength = toml::find<std::string>(tomlValue, "insufficient_parameter_length");
    langsResources->configurationUpdate = toml::find<std::string>(tomlValue, "configuration_update");
    langsResources->itemIdNotFound = toml::find<std::string>(tomlValue, "item_id_not_found");
    langsResources->lootTableNotFound = toml::find<std::string>(tomlValue, "loot_table_not_found");
    langsResources->itemContainerIsNull = toml::find<std::string>(tomlValue, "item_container_is_null");
    langsResources->composableItemIsNull = toml::find<std::string>(tomlValue, "composable_item_is_null");
    langsResources->itemResourceIsNull = toml::find<std::string>(tomlValue, "item_resource_is_null");
    langsResources->tileResourceIsNull = toml::find<std::string>(tomlValue, "tile_resource_is_null");
    langsResources->failedToLoadLicense = toml::find<std::string>(tomlValue, "failed_to_load_license");
    langsResources->cantFindObject = toml::find<std::string>(tomlValue, "cant_find_object");
    langsResources->teleportEntity = toml::find<std::string>(tomlValue, "teleport_entity");
    langsResources->screenshotSavedSuccess = toml::find<std::string>(tomlValue, "screenshot_saved_success");
    langsResources->screenshotSavedFailed = toml::find<std::string>(tomlValue, "screenshot_saved_failed");
    langsResources->areaMarkerTip = toml::find<std::string>(tomlValue, "area_marker_tip");
    langsResources->efficiencyTip = toml::find<std::string>(tomlValue, "efficiency_tip");
    langsResources->precisionMiningTip = toml::find<std::string>(tomlValue, "precision_mining_tip");
    langsResources->canMineBlockTip = toml::find<std::string>(tomlValue, "can_mine_block_tip");
    langsResources->canMineWallTip = toml::find<std::string>(tomlValue, "can_mine_wall_tip");
    langsResources->chainMiningTip = toml::find<std::string>(tomlValue, "chain_mining_tip");
    langsResources->flyEnable = toml::find<std::string>(tomlValue, "fly_enable");
    langsResources->flyDisable = toml::find<std::string>(tomlValue, "fly_disable");
    langsResources->tileDebugInfo = toml::find<std::string>(tomlValue, "tile_debug_info");
    langsResources->tileResDebugInfo = toml::find<std::string>(tomlValue, "tile_res_debug_info");
    langsResources->mousePosition = toml::find<std::string>(tomlValue, "mouse_position");
    langsResources->totalLight = toml::find<std::string>(tomlValue, "total_light");
    langsResources->noBiomeWasFound = toml::find<std::string>(tomlValue, "no_biome_was_found");
    langsResources->biomeHasFound = toml::find<std::string>(tomlValue, "biome_has_found");
    langsResources->configurationCommitSuccess = toml::find<std::string>(tomlValue, "configuration_commit_success");
    langsResources->configurationCommitFail = toml::find<std::string>(tomlValue, "configuration_commit_fail");
    langsResources->scancodeUnknown = toml::find<std::string>(tomlValue, "scancode_unknown");
    langsResources->hookCreateDuplicate = toml::find<std::string>(tomlValue, "hook_create_duplicate");
    langsResources->hookAddDuplicate = toml::find<std::string>(tomlValue, "hook_add_duplicate");
    langsResources->hookAddSuccess = toml::find<std::string>(tomlValue, "hook_add_success");
    langsResources->hookRemoveSuccess = toml::find<std::string>(tomlValue, "hook_remove_success");
    langsResources->hookIdNotExist = toml::find<std::string>(tomlValue, "hook_id_not_exist");
    langsResources->hookInfo = toml::find<std::string>(tomlValue, "hook_info");
    langsResources->lightInfo = toml::find<std::string>(tomlValue, "light_info");
    langsResources->lightContributionInfo = toml::find<std::string>(tomlValue, "light_contribution_info");
    langsResources->lightSourceInfo = toml::find<std::string>(tomlValue, "light_source_info");
    langsResources->lightMaskInfo = toml::find<std::string>(tomlValue, "light_mask_info");
    langsResources->notIncludeLighting = toml::find<std::string>(tomlValue, "not_include_lighting");
    langsResources->scancodeHookNotFound = toml::find<std::string>(tomlValue, "scancode_hook_not_found");
    langsResources->scancodeHookFoundCount = toml::find<std::string>(tomlValue, "scancode_hook_found_count");
    langsResources->worldNamePrefix = toml::find<std::vector<std::string> >(tomlValue, "world_name_prefix");
    langsResources->worldNameSuffix = toml::find<std::vector<std::string> >(tomlValue, "world_name_suffix");
    langsResources->slogans = toml::find<std::vector<std::string> >(tomlValue, "slogans");
    langsResources->cmdHookManagerNotFound = toml::find<std::string>(tomlValue, "cmd_hook_manager_not_found");
    langsResources->lightingInspectorEnable = toml::find<std::string>(tomlValue, "lighting_inspector_enable");
    langsResources->lightingInspectorDisable = toml::find<std::string>(tomlValue, "lighting_inspector_disable");
    langsResources->lightingInspectorEnableFail = toml::find<std::string>(tomlValue, "lighting_inspector_enable_fail");
    langsResources->lightingInspectorDisableFail = toml::find<std::string>(
        tomlValue, "lighting_inspector_disable_fail");
    langsResources->tileNameAir = toml::find<std::string>(tomlValue, STRING_TILE_AIR_NAME);
    langsResources->tileNameAirWall = toml::find<std::string>(tomlValue, STRING_TILE_AIR_WALL_NAME);
    langsResources->tileNameError = toml::find<std::string>(tomlValue, STRING_TILE_ERROR_NAME);
    langsResources->tileNameErrorWall = toml::find<std::string>(tomlValue, STRING_TILE_ERROR_WALL_NAME);
    langsResources->tileNameAccessDenied = toml::find<std::string>(tomlValue, STRING_TILE_ACCESS_DENIED_NAME);
    langsResources->tileNameAccessDeniedWall = toml::find<std::string>(tomlValue, STRING_TILE_ACCESS_DENIED_WALL_NAME);
    langsResources->tileNameBedrock = toml::find<std::string>(tomlValue, STRING_TILE_BEDROCK_NAME);
    langsResources->tileNameVoidWall = toml::find<std::string>(tomlValue, STRING_TILE_VOID_WALL_NAME);
    langsResources->tileDescriptionVoidWall = toml::find<std::string>(tomlValue, STRING_TILE_VOID_WALL_DESCRIPTION);
    langsResources->tileNameWater = toml::find<std::string>(tomlValue, STRING_TILE_WATER_NAME);
    langsResources->tileDescriptionAir = toml::find<std::string>(tomlValue, STRING_TILE_AIR_DESCRIPTION);
    langsResources->tileDescriptionAirWall = toml::find<std::string>(tomlValue, STRING_TILE_AIR_WALL_DESCRIPTION);
    langsResources->tileDescriptionError = toml::find<std::string>(tomlValue, STRING_TILE_ERROR_DESCRIPTION);
    langsResources->tileDescriptionErrorWall = toml::find<std::string>(tomlValue, STRING_TILE_ERROR_WALL_DESCRIPTION);
    langsResources->tileDescriptionAccessDenied = toml::find<
        std::string>(tomlValue, STRING_TILE_ACCESS_DENIED_DESCRIPTION);
    langsResources->tileDescriptionAccessDeniedWall = toml::find<std::string>(
        tomlValue, STRING_TILE_ACCESS_DENIED_WALL_DESCRIPTION);
    langsResources->tileDescriptionBedrock = toml::find<std::string>(tomlValue, STRING_TILE_BEDROCK_DESCRIPTION);
    langsResources->parallaxBackgroundClear = toml::find<std::string>(tomlValue, "parallax_background_clear");
    langsResources->parallaxBackgroundSet = toml::find<std::string>(tomlValue, "parallax_background_set");
    langsResources->parallaxBackgroundGet = toml::find<std::string>(tomlValue, "parallax_background_get");
    langsResources->parallaxBackgroundNone = toml::find<std::string>(tomlValue, "parallax_background_none");
    langsResources->biomeTemperatureInfo = toml::find<std::string>(tomlValue, "biome_temperature_info");
    langsResources->biomeHumidityInfo = toml::find<std::string>(tomlValue, "biome_humidity_info");
    langsResources->biomeElevationInfo = toml::find<std::string>(tomlValue, "biome_elevation_info");
    langsResources->biomeWeirdnessInfo = toml::find<std::string>(tomlValue, "biome_weirdness_info");
    langsResources->biomeErosionInfo = toml::find<std::string>(tomlValue, "biome_erosion_info");
    langsResources->biomeSurfaceProximityInfo = toml::find<std::string>(tomlValue, "biome_surface_proximity_info");
    langsResources->biomeTotalScore = toml::find<std::string>(tomlValue, "biome_total_score");
    langsResources->biomeScoreInspectorEnable = toml::find<std::string>(tomlValue, "biome_score_inspector_enable");
    langsResources->biomeScoreInspectorDisable = toml::find<std::string>(tomlValue, "biome_score_inspector_disable");
    langsResources->biomeScoreInspectorEnableFail = toml::find<
        std::string>(tomlValue, "biome_score_inspector_enable_fail");
    langsResources->biomeScoreInspectorDisableFail = toml::find<
        std::string>(tomlValue, "biome_score_inspector_disable_fail");
    langsResources->notEnabledSignVerify = toml::find<std::string>(tomlValue, "not_enabled_sign_verify");
    langsResources->unsignedPackage = toml::find<std::string>(tomlValue, "unsigned");
    langsResources->signatureVerificationSuccessful = toml::find<
        std::string>(tomlValue, "signature_verification_successful");
    langsResources->signatureVerificationFailed = toml::find<std::string>(tomlValue, "signature_verification_failed");
    langsResources->dataPackageCannotBeFound = toml::find<std::string>(tomlValue, "data_package_cannot_be_found");
    langsResources->tileSnapshotInfo = toml::find<std::string>(tomlValue, "tile_snapshot_info");
    langsResources->tileSnapshotInspectorEnable = toml::find<std::string>(tomlValue, "tile_snapshot_inspector_enable");
    langsResources->tileSnapshotInspectorDisable = toml::find<std::string>(
        tomlValue, "tile_snapshot_inspector_disable");
    langsResources->tileSnapshotInspectorEnableFail = toml::find<std::string>(
        tomlValue, "tile_snapshot_inspector_enable_fail");
    langsResources->tileSnapshotInspectorDisableFail = toml::find<
        std::string>(tomlValue, "tile_snapshot_inspector_disable_fail");
    langsResources->chunkHasNotBeenLoadedYet = toml::find<std::string>(tomlValue, "chunk_has_not_been_loaded_yet");
    langsResources->tileSnapshotsDoesNotExist = toml::find<std::string>(tomlValue, "tile_snapshots_does_not_exist");
    langsResources->itemEditorHoldItem = toml::find<std::string>(tomlValue, "item_editor_hold_item");
    langsResources->itemEditorReadAttr = toml::find<std::string>(tomlValue, "item_editor_read_attr");
    langsResources->itemEditorSetAttr = toml::find<std::string>(tomlValue, "item_editor_set_attr");
    langsResources->playerDoesNotExist = toml::find<std::string>(tomlValue, "player_does_not_exist");
    langsResources->technologyItem = toml::find<std::string>(tomlValue, "technology_item");
    langsResources->tagItem = toml::find<std::string>(tomlValue, "tag_item");
    langsResources->tagCannotFound = toml::find<std::string>(tomlValue, "tag_cannot_found");
    langsResources->noUnlockedRecipes = toml::find<std::string>(tomlValue, "no_unlocked_recipes");
    langsResources->recipesItem = toml::find<std::string>(tomlValue, "recipes_item");
    langsResources->debugChunkInfo = toml::find<std::string>(tomlValue, "debug_chunk_info");
    langsResources->fpsInfo = toml::find<std::string>(tomlValue, "fps_info");
    langsResources->lockedTip = toml::find<std::string>(tomlValue, "locked_tip");
    langsResources->timeH = toml::find<std::string>(tomlValue, "time_h");
    langsResources->timeM = toml::find<std::string>(tomlValue, "time_m");
    langsResources->timeS = toml::find<std::string>(tomlValue, "time_s");
    langsResources->savesDescription = toml::find<std::string>(tomlValue, "saves_description");
    systemBucket->SetLangsResources(std::move(langsResources));
    systemBucket->SetLangsValue(std::make_unique<toml::value>(tomlValue));
    return true;
}

void glimmer::InitLangsTask::Rollback(SystemBucket *systemBucket) {
    systemBucket->SetLangsResources(nullptr);
    systemBucket->SetLangsValue(nullptr);
}
