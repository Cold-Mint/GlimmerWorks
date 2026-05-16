//
// Created by coldmint on 2026/5/13.
//

#ifndef GLIMMERWORKS_COMMANDREQUEST_H
#define GLIMMERWORKS_COMMANDREQUEST_H
#include <cstdint>
#include <string>

namespace glimmer {
    class CommandSender;

    class CommandRequest {
        uint32_t id_ = 0;
        std::string command_;
        CommandSender *commandSender_ = nullptr;

    public:
        CommandRequest(uint32_t id, const std::string &command, CommandSender *commandSender);

        [[nodiscard]] uint32_t GetId() const;

        const std::string &GetCommand();

        [[nodiscard]] const CommandSender *GetCommandSender() const;
    };
}

#endif //GLIMMERWORKS_COMMANDREQUEST_H
