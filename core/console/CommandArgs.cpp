//
// Created by Cold-Mint on 2025/10/15.
//

#include "CommandArgs.h"

#include <stdexcept>

#include "../log/LogCat.h"

int glimmer::CommandArgs::GetTokenIndexAtCursor(const int cursorPos) const {
    const int commandLen = static_cast<int>(command_.size());
    if (commandLen == 0 || cursorPos < 0 || cursorPos > commandLen) {
        return -1;
    }

    int tokenIndex = 0;
    bool inToken = false;
    int maxIndex = commandLen - 1;
    for (int i = 0; i < cursorPos; ++i) {
        const char c = command_[i];
        if (c == ' ') {
            // Encountering a space indicates that the token has ended
            // 遇到空格，说明 token 结束
            if (inToken) {
                //如果不是末尾的空格
                if (i < maxIndex) {
                    tokenIndex++;
                }
                inToken = false;
            }
        } else {
            if (!inToken) {
                inToken = true;
            }
        }
    }

    // If the cursor stops in the middle of the token, it belongs to the current token.
    // If it stops after the space at the end of the token, it is not counted as a new token
    // 若光标停在 token 中间，则属于当前这个 token，若停在 token 尾部空格之后，不计入新的 token
    if (inToken) {
        return tokenIndex;
    }

    // If the cursor is after the space, it should return "Next token index".
    // 光标在空格后，则应该返回 “下一个 token index”
    return tokenIndex;
}

std::string glimmer::CommandArgs::GetKeywordAtCursor(const int cursorPos) const {
    const int index = GetTokenIndexAtCursor(cursorPos);
    if (index < 0) {
        return "";
    }
    return tokens_[index];
}


int glimmer::CommandArgs::GetSize() const {
    return static_cast<int>(tokens_.size());
}

bool glimmer::CommandArgs::AsBool(const int index) const {
    if (index < 0 || index >= static_cast<int>(tokens_.size())) {
        throw std::out_of_range("CommandArgs::asBool index out of range");
    }
    const std::string &val = tokens_[index];
    if (val == "1" || val == "true" || val == "yes") return true;
    if (val == "0" || val == "false" || val == "no") return false;
    throw std::invalid_argument("Cannot convert to bool: " + val);
}

int glimmer::CommandArgs::AsInt(const int index) const {
    if (index < 0 || index >= static_cast<int>(tokens_.size())) {
        throw std::out_of_range("CommandArgs::asInt index out of range");
    }
    return std::stoi(tokens_[index]);
}

float glimmer::CommandArgs::AsFloat(const int index) const {
    if (index < 0 || index >= static_cast<int>(tokens_.size())) {
        throw std::out_of_range("CommandArgs::asFloat index out of range");
    }
    return std::stof(tokens_[index]);
}

double glimmer::CommandArgs::AsDouble(const int index) const {
    if (index < 0 || index >= static_cast<int>(tokens_.size())) {
        throw std::out_of_range("CommandArgs::asDouble index out of range");
    }
    return std::stod(tokens_[index]);
}

std::string glimmer::CommandArgs::AsString(const int index) const {
    if (index < 0 || index >= static_cast<int>(tokens_.size())) {
        throw std::out_of_range("CommandArgs::asString index out of range");
    }
    return tokens_[index];
}
