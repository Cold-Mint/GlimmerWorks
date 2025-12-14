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
}

glimmer::NodeTree<std::string> glimmer::HeightMapCommand::GetSuggestionsTree(const CommandArgs &commandArgs) {
    return Command::GetSuggestionsTree(commandArgs);
}

bool glimmer::HeightMapCommand::Execute(CommandArgs commandArgs,
                                        std::function<void(const std::string &text)> onOutput) {
    if (worldContext_ == nullptr) {
        onOutput("WorldContext is nullptr");
        return false;
    }
    if (commandArgs.GetSize() < 3) {
        return false;
    }
    int minX = commandArgs.AsInt(1);
    int maxX = commandArgs.AsInt(2);
    if (minX > maxX) {
        onOutput("X is greater than maxX");
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
    onOutput(ss.str());
    LogCat::d(ss.str());
    return true;
}
