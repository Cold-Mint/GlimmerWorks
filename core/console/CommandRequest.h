//
// Created by coldmint on 2026/5/13.
//

#ifndef GLIMMERWORKS_COMMANDREQUEST_H
#define GLIMMERWORKS_COMMANDREQUEST_H
#include <string>

namespace glimmer {
    class CommandSender;

    class CommandRequest {
        u_int32_t id_ = 0;
        std::string command_;
        CommandSender *commandSender_ = nullptr;

    public:
        CommandRequest(u_int32_t id, const std::string &command, CommandSender *commandSender);

        [[nodiscard]] u_int32_t GetId() const;

        const std::string &GetCommand();

        [[nodiscard]] const CommandSender *GetCommandSender() const;
    };
}

#endif //GLIMMERWORKS_COMMANDREQUEST_H
