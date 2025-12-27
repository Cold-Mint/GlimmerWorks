//
// Created by Cold-Mint on 2025/12/14.
//

#include "HeightMapCommand.h"

#include "../../Constants.h"
#include "../../world/WorldContext.h"
#include "fmt/color.h"

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
    if (size_t size = commandArgs.GetSize(); size < 3) {
        onMessage(fmt::format(
            fmt::runtime(appContext_->GetLangsResources()->insufficientParameterLength),
            3, size));
        return false;
    }
    const int minX = commandArgs.AsInt(1);
    const int maxX = commandArgs.AsInt(2);
    if (minX > maxX) {
        onMessage(appContext_->GetLangsResources()->minXIsGreaterThanMaxX);
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
            if (!virtualFileSystem_->CreateFolder(DEBUG_FOLDER_NAME)) {
                onMessage(fmt::format(
                    fmt::runtime(appContext_->GetLangsResources()->folderCreationFailed),
                    DEBUG_FOLDER_NAME));
                return false;
            }
        }
        std::string path = DEBUG_FOLDER_NAME + "/heightMap_" + std::to_string(worldContext_->GetSeed()) + "_" + fileName
                           + ".json";
        const bool write = virtualFileSystem_->WriteFile(
            path,
            ss.str());
        if (!write) {
            onMessage(fmt::format(
                fmt::runtime(appContext_->GetLangsResources()->fileWritingFailed),
                path));
            return false;
        }
    }
    return true;
}
