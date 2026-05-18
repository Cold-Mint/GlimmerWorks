//
// Created by Cold-Mint on 2025/12/14.
//
#if  !defined(NDEBUG)
#include "HeightMapCommand.h"

#include "../../Constants.h"
#include "../../world/WorldContext.h"
#include "fmt/color.h"

void glimmer::HeightMapCommand::InitSuggestions(NodeTree<std::string> *suggestionsTree) {
}

glimmer::HeightMapCommand::HeightMapCommand(AppContext *appContext) : Command(appContext) {
}

std::string glimmer::HeightMapCommand::GetName() const {
    return HEIGHT_MAP_COMMAND_NAME;
}

bool glimmer::HeightMapCommand::RequiresWorldContext() const {
    return true;
}

void glimmer::HeightMapCommand::PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) {
    if (commandArgs == nullptr || strings == nullptr) {
        return;
    }
    strings->emplace_back("[minX:int]");
    strings->emplace_back("[maxX:int]");
    strings->emplace_back("[fileName:string]");
}


bool glimmer::HeightMapCommand::Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                                        const std::function<void(const std::string &text)> *onMessage) {
    if (appContext_ == nullptr || commandArgs == nullptr || onMessage == nullptr) {
        return false;
    }
    const std::function<void(const std::string &text)> &onMessageRef = *onMessage;
    if (worldContext_ == nullptr) {
        onMessageRef(appContext_->GetLangsResources()->worldContextIsNull);
        return false;
    }
    if (size_t size = commandArgs->GetSize(); size < 3) {
        onMessageRef(fmt::format(
            fmt::runtime(appContext_->GetLangsResources()->insufficientParameterLength),
            3, size));
        return false;
    }
    const int minX = commandArgs->AsInt(1);
    const int maxX = commandArgs->AsInt(2);
    if (minX > maxX) {
        onMessageRef(appContext_->GetLangsResources()->minXIsGreaterThanMaxX);
        return false;
    }
    std::ostringstream ss;
    ss << "{\n";
    ss << "  \"seed\": " << worldContext_->GetWorldSeed() << ",\n";
    ss << R"(  "range": { "minX": )" << minX
            << ", \"maxX\": " << maxX << " },\n";
    ss << "  \"heightMap\": [\n";

    for (int x = minX; x <= maxX; ++x) {
        const int height = worldContext_->GetChunkGenerator()->GetFirstTileTerrainY(x);
        ss << "    { \"x\": " << x
                << ", \"height\": " << height << " }";

        if (x != maxX)
            ss << ",";

        ss << "\n";
    }
    ss << "  ]\n";
    ss << "}\n";
    onMessageRef(ss.str());
    LogCat::d(ss.str());
    if (commandArgs->GetSize() >= 4) {
        const VirtualFileSystem *virtualFileSystem = appContext_->GetVirtualFileSystem();
        const std::string fileName = commandArgs->AsString(3);
        if (!virtualFileSystem->Exists(DEBUG_FOLDER_NAME)) {
            if (!virtualFileSystem->CreateFolder(DEBUG_FOLDER_NAME)) {
                onMessageRef(fmt::format(
                    fmt::runtime(appContext_->GetLangsResources()->folderCreationFailed),
                    DEBUG_FOLDER_NAME));
                return false;
            }
        }
        std::string path = DEBUG_FOLDER_NAME + "/heightMap_" + std::to_string(worldContext_->GetWorldSeed()) + "_" +
                           fileName
                           + ".json";
        const bool write = virtualFileSystem->WriteFile(
            path,
            ss.str());
        if (!write) {
            onMessageRef(fmt::format(
                fmt::runtime(appContext_->GetLangsResources()->fileWritingFailed),
                path));
            return false;
        }
    }
    return true;
}
#endif