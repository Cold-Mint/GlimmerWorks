//
// Created by Cold-Mint on 2025/11/28.
//

#include "VFSCommand.h"

#include "../../Constants.h"
#include "../../scene/AppContext.h"
#include "fmt/color.h"
#include "src/saves/chunk.pb.h"
#include "src/saves/map_manifest.pb.h"

void glimmer::VFSCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
    //List all the mount points
    //列出所有的挂载点
    suggestionsTree.AddChild("listMount");
    //Obtain the absolute path of a certain virtual path
    //获取某个虚拟路径的绝对路径
    suggestionsTree.AddChild("actualPath");
    //Check whether a certain path exists
    //检查某个路径是否存在
    suggestionsTree.AddChild("exists");
    //Read the content of a certain file and list it on the console
    //读取某个文件内容并将其列在控制台上
    suggestionsTree.AddChild("readFile");
}

std::string glimmer::VFSCommand::GetName() const {
    return VFS_COMMAND_NAME;
}

void glimmer::VFSCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
    strings.emplace_back("[operation type:string]");
    strings.emplace_back("[path:string]");
    if (commandArgs.GetSize() > 1) {
        std::string type = commandArgs.AsString(1);
        if (type == "readFile") {
            strings.emplace_back("[message type:string]");
        }
    }
}

bool glimmer::VFSCommand::Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) {
    const int size = commandArgs.GetSize();
    if (size < 2) {
        onMessage(fmt::format(
            fmt::runtime(appContext_->GetLangsResources()->insufficientParameterLength),
            2, size));
        return false;
    }
    const auto type = commandArgs.AsString(1);
    if (type == "listMount") {
        onMessage(virtualFileSystem_->ListMounts());
        return true;
    }
    if (size > 2 && type == "actualPath") {
        auto path = commandArgs.AsString(2);
        auto actualPath = virtualFileSystem_->GetActualPath(path);
        if (actualPath.has_value()) {
            onMessage(actualPath.value());
        } else {
            onMessage(appContext_->GetLangsResources()->getActualPathError);
        }
        return true;
    }
    if (size > 2 && type == "exists") {
        auto path = commandArgs.AsString(2);
        auto exists = virtualFileSystem_->Exists(path);
        if (exists) {
            onMessage("true");
        } else {
            onMessage("false");
        }
        return true;
    }
    if (size > 2 && type == "readFile") {
        auto path = commandArgs.AsString(2);
        auto text = virtualFileSystem_->ReadFile(path);
        if (text.has_value()) {
            if (size > 3) {
                auto messageType = commandArgs.AsString(3);
                if (messageType == "chunk") {
                    ChunkMessage chunkMessage;
                    chunkMessage.ParseFromString(text.value());
                    onMessage(chunkMessage.DebugString());
                } else if (messageType == "mapManifest") {
                    MapManifestMessage mapManifestMessage;
                    mapManifestMessage.ParseFromString(text.value());
                    onMessage(mapManifestMessage.DebugString());
                }else if (messageType == "entity") {
                    ChunkEntityMessage chunkEntityMessage;
                    chunkEntityMessage.ParseFromString(text.value());
                    onMessage(chunkEntityMessage.DebugString());
                }else if (messageType == "player") {
                    PlayerMessage playerMessage;
                    playerMessage.ParseFromString(text.value());
                    onMessage(playerMessage.DebugString());
                }
            } else {
                onMessage(text.value());
            }
        } else {
            onMessage("null");
        }
        return true;
    }
    return false;
}

glimmer::NodeTree<std::string> glimmer::VFSCommand::GetSuggestionsTree(const CommandArgs &commandArgs) {
    if (commandArgs.GetSize() > 1) {
        auto type = commandArgs.AsString(1);
        if (type == "readFile" || type == "actualPath" || type == "exists") {
            //Attach dynamic suggestion parameters to the second-level tree
            //为第二层树附加动态建议参数
            int size = suggestionsTree_.GetSize();
            for (int i = 0; i < size; i++) {
                auto child = suggestionsTree_.GetChild(i);
                child->ClearChildren();
                auto childNode = child->AddChild(
                    std::string(VFS_DYNAMIC_SUGGESTIONS_NAME) + ":" + commandArgs.AsString(2));
                if (type == "readFile") {
                    childNode->AddChild("mapManifest");
                    childNode->AddChild("chunk");
                    childNode->AddChild("entity");
                    childNode->AddChild("player");
                }
            }
        }
    }

    return suggestionsTree_;
}
