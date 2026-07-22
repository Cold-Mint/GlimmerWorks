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
#pragma once
#include <string>
#include <vector>

namespace glimmer
{
    struct LangsResources
    {
        std::string commandInfo;
        std::string awakeBodyCount;
        std::string getActualPathError;
        std::string unknownAssetType;
        std::string unknownCommandParameters;
        std::string worldContextIsNull;
        std::string cheatsNotAllowed;
        std::string insufficientParameterLength;
        std::string configurationUpdate;
        std::string itemIdNotFound;
        std::string lootTableNotFound;
        std::string itemContainerIsNull;
        std::string composableItemIsNull;
        std::string tileResourceIsNull;
        std::string itemResourceIsNull;
        std::string failedToLoadLicense;
        std::string cantFindObject;
        std::string teleportEntity;
        std::string savedGames;
        std::string screenshotSavedFailed;
        std::string screenshotSavedSuccess;
        std::string areaMarkerTip;
        std::string efficiencyTip;
        std::string precisionMiningTip;
        std::string canMineBlockTip;
        std::string canMineWallTip;
        std::string chainMiningTip;
        std::string configurationCommitSuccess;
        std::string configurationCommitFail;
        std::string flyEnable;
        std::string flyDisable;
        std::string tileDebugInfo;
        std::string tileResDebugInfo;
        std::string totalLight;
        std::string mousePosition;
        std::string noBiomeWasFound;
        std::string biomeHasFound;
        std::vector<std::string> worldNamePrefix;
        std::vector<std::string> worldNameSuffix;
        std::vector<std::string> slogans;
        std::string scancodeUnknown;
        std::string hookCreateDuplicate;
        std::string hookAddDuplicate;
        std::string hookAddSuccess;
        std::string hookRemoveSuccess;
        std::string hookIdNotExist;
        std::string scancodeHookNotFound;
        std::string scancodeHookFoundCount;
        std::string hookInfo;
        std::string lightInfo;
        std::string lightContributionInfo;
        std::string lightSourceInfo;
        std::string lightMaskInfo;
        std::string notIncludeLighting;
        std::string cmdHookManagerNotFound;
        std::string lightingInspectorEnable;
        std::string lightingInspectorDisable;
        std::string lightingInspectorEnableFail;
        std::string lightingInspectorDisableFail;
        std::string tileNameAir;
        std::string tileNameAirWall;
        std::string tileNameError;
        std::string tileNameErrorWall;
        std::string tileNameAccessDenied;
        std::string tileNameAccessDeniedWall;
        std::string tileNameBedrock;
        std::string tileNameWater;
        std::string tileDescriptionAir;
        std::string tileDescriptionAirWall;
        std::string tileDescriptionError;
        std::string tileDescriptionErrorWall;
        std::string tileDescriptionAccessDenied;
        std::string tileDescriptionAccessDeniedWall;
        std::string tileDescriptionBedrock;
        std::string parallaxBackgroundClear;
        std::string parallaxBackgroundSet;
        std::string parallaxBackgroundGet;
        std::string parallaxBackgroundNone;
        std::string biomeTemperatureInfo;
        std::string biomeHumidityInfo;
        std::string biomeElevationInfo;
        std::string biomeWeirdnessInfo;
        std::string biomeErosionInfo;
        std::string biomeSurfaceProximityInfo;
        std::string biomeTotalScore;
        std::string biomeScoreInspectorEnable;
        std::string biomeScoreInspectorDisable;
        std::string biomeScoreInspectorEnableFail;
        std::string biomeScoreInspectorDisableFail;
        std::string notEnabledSignVerify;
        std::string unsignedPackage;
        std::string signatureVerificationSuccessful;
        std::string signatureVerificationFailed;
        std::string dataPackageCannotBeFound;
        std::string tileSnapshotInfo;
        std::string tileSnapshotInspectorEnable;
        std::string tileSnapshotInspectorDisable;
        std::string tileSnapshotInspectorEnableFail;
        std::string tileSnapshotInspectorDisableFail;
        std::string chunkHasNotBeenLoadedYet;
        std::string tileSnapshotsDoesNotExist;
        std::string itemEditorHoldItem;
        std::string itemEditorReadAttr;
        std::string itemEditorSetAttr;
        std::string playerDoesNotExist;
        std::string technologyItem;
        std::string tagItem;
        std::string tagCannotFound;
        std::string noUnlockedRecipes;
        std::string recipesItem;
        std::string debugChunkInfo;
        std::string fpsInfo;
        std::string lockedTip;
        std::string timeS;
        std::string timeM;
        std::string timeH;
        std::string savesDescription;
    };
}
