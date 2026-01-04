//
// Created by Cold-Mint on 2025/10/15.
//

#ifndef GLIMMERWORKS_COMMANDARGS_H
#define GLIMMERWORKS_COMMANDARGS_H
#include <sstream>
#include <string>
#include <vector>
#include <optional>

#include "../mod/ResourceRef.h"

namespace glimmer {
    class CommandArgs {
        std::vector<std::string> tokens_;
        std::string command_;

    public:
        explicit CommandArgs(const std::string &command) {
            command_ = command;
            std::istringstream iss(command);
            std::string token;
            while (iss >> token) {
                tokens_.push_back(token);
            }
        }


        /**
         * Obtain the Token position corresponding to the cursor position
         * 获取光标位置对应的Token位置
         * @param cursorPos cursorPos 光标位置
         * @return
         */
        [[nodiscard]] int GetTokenIndexAtCursor(int cursorPos) const;

        /**
         * Get the keyword corresponding to the cursor position
         * 获取光标位置对应的关键字
         * @param cursorPos cursorPos 光标位置
         * @return
         */
        [[nodiscard]] std::string GetKeywordAtCursor(int cursorPos) const;

        [[nodiscard]] int GetSize() const;

        [[nodiscard]] bool AsBool(int index) const;

        [[nodiscard]] int AsInt(int index) const;

        [[nodiscard]] float AsFloat(int index) const;

        [[nodiscard]] double AsDouble(int index) const;

        [[nodiscard]] std::string AsString(int index) const;

        [[nodiscard]] std::optional<ResourceRef> AsResourceRef(int index,int resourceType) const;
    };
}

#endif //GLIMMERWORKS_COMMANDARGS_H
