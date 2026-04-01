//
// Created by coldmint on 2026/4/1.
//

#ifndef GLIMMERWORKS_COMMANDHISTORYMANAGER_H
#define GLIMMERWORKS_COMMANDHISTORYMANAGER_H
#include "core/vfs/VirtualFileSystem.h"
#include "src/saves/command_history.pb.h"

namespace glimmer {
    class CommandHistoryManager {
        CommandHistoryMessage commandHistoryMessage_;

        VirtualFileSystem *virtualFileSystem_;

        std::string commandHistoryPath_;

    public:
        explicit CommandHistoryManager(const std::string &runtimePath, VirtualFileSystem *virtualFileSystem);

        /**
         * Save
         * 保存命令历史到管理器内
         */
        void Save() const;

        /**
         * Obtain command history information
         * 获取命令历史消息
         * @return
         */
        [[nodiscard]] CommandHistoryMessage &GetCommandHistoryMessage();

        /**
         * Read
         * 读取
         */
        void Read();
    };
}


#endif //GLIMMERWORKS_COMMANDHISTORYMANAGER_H
