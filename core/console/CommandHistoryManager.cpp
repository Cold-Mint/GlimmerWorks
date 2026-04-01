//
// Created by coldmint on 2026/4/1.
//

#include "CommandHistoryManager.h"


glimmer::CommandHistoryManager::CommandHistoryManager(const std::string &runtimePath,
                                                      VirtualFileSystem *virtualFileSystem) {
    virtualFileSystem_ = virtualFileSystem;
    commandHistoryPath_ = runtimePath + "/command_history.bin";
}

void glimmer::CommandHistoryManager::Save() const {
    (void) virtualFileSystem_->WriteFile(commandHistoryPath_, commandHistoryMessage_.SerializeAsString());
}


CommandHistoryMessage &glimmer::CommandHistoryManager::GetCommandHistoryMessage() {
    return commandHistoryMessage_;
}

void glimmer::CommandHistoryManager::Read() {
    if (!virtualFileSystem_->Exists(commandHistoryPath_)) {
        return;
    }
    const auto text = virtualFileSystem_->ReadFile(commandHistoryPath_);
    if (!text.has_value()) {
        return;
    }
    (void) commandHistoryMessage_.ParseFromString(text.value());
}
