//
// Created by Cold-Mint on 2025/12/14.
//

#include "HeightMapCommand.h"

#include "../../Constants.h"
#include "../../world/WorldContext.h"

void glimmer::HeightMapCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
}

std::string glimmer::HeightMapCommand::GetName() const {
    return HEIGHT_MAP_COMMAND_NAME;
}

bool glimmer::HeightMapCommand::RequiresWorldContext() const {
    return true;
}

void glimmer::HeightMapCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
    strings.emplace_back("[minX:int]");
    strings.emplace_back("[maxX:int]");
    strings.emplace_back("[fileName:string]");
}

glimmer::NodeTree<std::string> glimmer::HeightMapCommand::GetSuggestionsTree(const CommandArgs &commandArgs) {
    return Command::GetSuggestionsTree(commandArgs);
}

bool glimmer::HeightMapCommand::Execute(CommandArgs commandArgs,
                                        std::function<void(const std::string &text)> onMessage) {
    if (worldContext_ == nullptr) {
        onMessage(appContext_->GetLangsResources()->worldContextIsNull);
        return false;
    }
    if (commandArgs.GetSize() < 3) {
        return false;
    }
    int minX = commandArgs.AsInt(1);
    int maxX = commandArgs.AsInt(2);
    if (minX > maxX) {
        onMessage("X is greater than maxX");
        return false;
    }
    std::ostringstream ss;

    ss << "{\n";
    ss << "  \"seed\": " << worldContext_->GetSeed() << ",\n";
    ss << R"(  "range": { "minX": )" << minX
            << ", \"maxX\": " << maxX << " },\n";
    ss << "  \"heightMap\": [\n";

    for (int x = minX; x <= maxX; ++x) {
        const int height = worldContext_->GetHeight(x);

        ss << "    { \"x\": " << x
                << ", \"height\": " << height << " }";

        if (x != maxX)
            ss << ",";

        ss << "\n";
    }
    ss << "  ]\n";
    ss << "}\n";
    onMessage(ss.str());
    LogCat::d(ss.str());
    if (commandArgs.GetSize() >= 4) {
        const std::string fileName = commandArgs.AsString(3);
        if (!virtualFileSystem_->Exists(DEBUG_FOLDER_NAME)) {
            bool createFolder = virtualFileSystem_->CreateFolder(DEBUG_FOLDER_NAME);
            if (!createFolder) {
                LogCat::e("Directories cannot be created: ", DEBUG_FOLDER_NAME);
                return false;
            }
        }
        bool write = virtualFileSystem_->WriteFile(
            DEBUG_FOLDER_NAME + "/heightMap_" + std::to_string(worldContext_->GetSeed()) + "_" + fileName + ".json",
            ss.str());
        if (!write) {
            LogCat::w("Failed to write the file.");
            return false;
        }
    }
    return true;
}
