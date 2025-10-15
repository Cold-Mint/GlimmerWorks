//
// Created by Cold-Mint on 2025/10/15.
//

#ifndef GLIMMERWORKS_COMMANDARGS_H
#define GLIMMERWORKS_COMMANDARGS_H
#include <sstream>
#include <string>
#include <vector>

namespace Glimmer {
    class CommandArgs {
        std::vector<std::string> tokens;

    public:
        explicit CommandArgs(const std::string &command) {
            std::istringstream iss(command);
            std::string token;
            while (iss >> token) {
                tokens.push_back(token);
            }
        }

        [[nodiscard]] bool asBool(int index) const;

        [[nodiscard]] int asInt(int index) const;

        [[nodiscard]] float asFloat(int index) const;

        [[nodiscard]] double asDouble(int index) const;

        [[nodiscard]] std::string asString(int index) const;
    };
}

#endif //GLIMMERWORKS_COMMANDARGS_H
