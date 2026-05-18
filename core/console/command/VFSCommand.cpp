//
// Created by Cold-Mint on 2025/11/28.
//
#if  !defined(NDEBUG)
#include "VFSCommand.h"

#include "../../Constants.h"
#include "../../scene/AppContext.h"
#include "fmt/color.h"

void glimmer::VFSCommand::InitSuggestions(NodeTree<std::string> *suggestionsTree) {
    if (suggestionsTree == nullptr) {
        return;
    }
    //List all the mount points
    //列出所有的挂载点
    suggestionsTree->AddChild("listMount");
    //Obtain the absolute path of a certain virtual path
    //获取某个虚拟路径的绝对路径
    suggestionsTree->AddChild("actualPath");
    //Check whether a certain path exists
    //检查某个路径是否存在
    suggestionsTree->AddChild("exists");
    //Read the content of a certain file and list it on the console
    //读取某个文件内容并将其列在控制台上
    suggestionsTree->AddChild("readFile");
}

glimmer::VFSCommand::VFSCommand(AppContext *appContext)
    : Command(appContext) {
}

std::string glimmer::VFSCommand::GetName() const {
    return VFS_COMMAND_NAME;
}

void glimmer::VFSCommand::PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) {
    if (commandArgs == nullptr || strings == nullptr) {
        return;
    }
    strings->emplace_back("[operation type:string]");
    strings->emplace_back("[path:string]");
    if (commandArgs->GetSize() > 1) {
        const std::string type = commandArgs->AsString(1);
        if (type == "readFile") {
            strings->emplace_back("[message type:string]");
        }
    }
}

glimmer::NodeTree<std::string> *glimmer::VFSCommand::GetSuggestionsTree(const CommandArgs *commandArgs) {
    if (commandArgs == nullptr) {
        return &suggestionsTree_;
    }
    if (commandArgs->GetSize() > 1) {
        auto type = commandArgs->AsString(1);
        if (type == "readFile" || type == "actualPath" || type == "exists") {
            //Attach dynamic suggestion parameters to the second-level tree
            //为第二层树附加动态建议参数
            int size = suggestionsTree_.GetSize();
            for (int i = 0; i < size; i++) {
                auto child = suggestionsTree_.GetChild(i);
                child->ClearChildren();
                child->AddChild(
                    std::string(VFS_DYNAMIC_SUGGESTIONS_NAME) + ":" + commandArgs->AsString(2));
            }
        }
    }

    return &suggestionsTree_;
}


bool glimmer::VFSCommand::Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                                  const std::function<void(const std::string &text)> *onMessage) {
    if (appContext_ == nullptr || commandArgs == nullptr || onMessage == nullptr) {
        return false;
    }
    const std::function<void(const std::string &text)> &onMessageRef = *onMessage;
    const int size = commandArgs->GetSize();
    if (size < 2) {
        onMessageRef(fmt::format(
            fmt::runtime(appContext_->GetLangsResources()->insufficientParameterLength),
            2, size));
        return false;
    }
    VirtualFileSystem *virtualFileSystem = appContext_->GetVirtualFileSystem();
    const auto type = commandArgs->AsString(1);
    if (type == "listMount") {
        onMessageRef(virtualFileSystem->ListMounts());
        return true;
    }
    if (size > 2 && type == "actualPath") {
        auto path = commandArgs->AsString(2);
        auto actualPath = virtualFileSystem->GetActualPath(path);
        if (actualPath.has_value()) {
            onMessageRef(actualPath.value());
        } else {
            onMessageRef(appContext_->GetLangsResources()->getActualPathError);
        }
        return true;
    }
    if (size > 2 && type == "exists") {
        auto path = commandArgs->AsString(2);
        if (virtualFileSystem->Exists(path)) {
            onMessageRef("true");
        } else {
            onMessageRef("false");
        }
        return true;
    }
    if (size > 2 && type == "readFile") {
        auto path = commandArgs->AsString(2);
        auto text = virtualFileSystem->ReadFile(path);
        if (text.has_value()) {
            onMessageRef(text.value());
        } else {
            onMessageRef("null");
        }
        return true;
    }
    return false;
}
#endif
