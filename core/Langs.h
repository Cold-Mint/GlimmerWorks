//
// Created by Cold-Mint on 2025/10/18.
//

#ifndef GLIMMERWORKS_LANGS_H
#define GLIMMERWORKS_LANGS_H
#include <string>
#include <vector>

namespace glimmer {
    struct LangsResources {
        std::string startGame;
        std::string settings;
        std::string mods;
        std::string exitGame;
        std::string console;
        std::string commandNotFound;
        std::string executionFailed;
        std::string executedSuccess;
        std::string commandIsEmpty;
        std::string createWorld;
        std::string cancel;
        std::string worldName;
        std::string seed;
        std::string random;
        std::string commandInfo;
        std::string awakeBodyCount;
        std::string getActualPathError;
        std::string unknownAssetType;
        std::string unknownCommandParameters;
        std::string worldContextIsNull;
        std::string insufficientParameterLength;
        std::string entryCannotFoundInConfigurationFile;
        std::string configurationUpdate;
        std::string itemIdNotFound;
        std::string tileResourceNotFound;
        std::string itemContainerIsNull;
        std::string composableItemIsNull;
        std::string abilityItemIsNull;
        std::string itemResourceNotFound;
        std::string tileResourceIsNull;
        std::string itemResourceIsNull;
        std::string minXIsGreaterThanMaxX;
        std::string folderCreationFailed;
        std::string fileWritingFailed;
        std::string failedToLoadLicense;
        std::string cantFindObject;
        std::string teleportEntity;
        std::string savedGames;
        std::string loadGame;
        std::string deleteGame;
        std::string confirm;
        std::string wantDeleteThisSave;
        std::string savesList;
        std::string pause;
        std::string restore;
        std::string saveAndExit;
        std::vector<std::string> worldNamePrefix;
        std::vector<std::string> worldNameSuffix;
    };
}

#endif //GLIMMERWORKS_LANGS_H
