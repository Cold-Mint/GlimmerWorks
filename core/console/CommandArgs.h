//
// Created by Cold-Mint on 2025/10/15.
//

#ifndef GLIMMERWORKS_COMMANDARGS_H
#define GLIMMERWORKS_COMMANDARGS_H
#include <sstream>
#include <string>
#include <vector>

namespace glimmer {
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

        [[nodiscard]] bool AsBool(int index) const;

        [[nodiscard]] int AsInt(int index) const;

        [[nodiscard]] float AsFloat(int index) const;

        [[nodiscard]] double AsDouble(int index) const;

        [[nodiscard]] std::string AsString(int index) const;
    };
}

#endif //GLIMMERWORKS_COMMANDARGS_H
