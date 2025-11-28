//
// Created by coldmint on 2025/11/28.
//

#include "VFSCommand.h"

#include "../../Constants.h"
#include "../../scene/AppContext.h"

void glimmer::VFSCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
    suggestionsTree.AddChild("listMount"); //列出所有的挂载点
    suggestionsTree.AddChild("actualPath"); //获取某个虚拟路径的绝对路径
    suggestionsTree.AddChild("exists"); //检查某个路径是否存在
    suggestionsTree.AddChild("readFile"); //读取某个文件内容并将其列在控制台上
}

std::string glimmer::VFSCommand::GetName() const {
    return VFS_COMMAND_NAME;
}

void glimmer::VFSCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
    strings.emplace_back("[operation type:string]");
    strings.emplace_back("[path:string]");
}

bool glimmer::VFSCommand::Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onOutput) {
    const int size = commandArgs.GetSize();
    if (size < 2) {
        return false;
    }
    const auto type = commandArgs.AsString(1);
    if (type == "listMount") {
        onOutput(virtualFileSystem_->ListMounts());
        return true;
    }
    if (size > 2 && type == "actualPath") {
        auto path = commandArgs.AsString(2);
        auto actualPath = virtualFileSystem_->GetActualPath(path);
        if (actualPath.has_value()) {
            onOutput(actualPath.value());
        } else {
            onOutput(appContext_->langs_->getActualPathError);
        }
        return true;
    }
    if (size > 2 && type == "exists") {
        auto path = commandArgs.AsString(2);
        auto exists = virtualFileSystem_->Exists(path);
        if (exists) {
            onOutput("true");
        } else {
            onOutput("false");
        }
        return true;
    }
    if (size > 2 && type == "readFile") {
        auto path = commandArgs.AsString(2);
        auto text = virtualFileSystem_->ReadFile(path);
        if (text.has_value()) {
            onOutput(text.value());
        } else {
            onOutput("null");
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
                child->AddChild(std::string(VFS_DYNAMIC_SUGGESTIONS_NAME) + ":" + commandArgs.AsString(2));
            }
        }
    }

    return suggestionsTree_;
}
