//
// Created by Cold-Mint on 2025/10/15.
//

#include "CommandArgs.h"

#include "../mod/Resource.h"
#include "../mod/ResourceRef.h"
#include <optional>

glimmer::CommandArgs::CommandArgs(const std::string &command) {
    command_ = command;
    std::istringstream iss(command);
    std::string token;
    while (iss >> token) {
        tokens_.push_back(token);
    }
}

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
            if (inToken) {
                if (i < maxIndex) {
                    if (const char nextChar = command_[i + 1]; nextChar != ' ') {
                        tokenIndex++;
                        inToken = false;
                    }
                } else {
                    tokenIndex++;
                    inToken = false;
                }
            }
        } else {
            if (!inToken) {
                inToken = true;
            }
        }
    }
    return tokenIndex;
}

std::string glimmer::CommandArgs::GetKeywordAtCursor(const int cursorPos) const {
    const int index = GetTokenIndexAtCursor(cursorPos);
    if (index < 0 || index >= tokens_.size()) {
        return "";
    }
    return tokens_[index];
}


int glimmer::CommandArgs::GetSize() const {
    return static_cast<int>(tokens_.size());
}

bool glimmer::CommandArgs::AsBool(const int index) const {
    if (index < 0 || index >= static_cast<int>(tokens_.size())) {
        return false;
    }
    const std::string &val = tokens_[index];
    if (val == "1" || val == "true" || val == "yes" || val == "y") {
        return true;
    }
    if (val == "0" || val == "false" || val == "no" || val == "n") {
        return false;
    }
    return false;
}

int glimmer::CommandArgs::AsInt(const int index) const {
    if (index < 0 || index >= static_cast<int>(tokens_.size())) {
        return 0;
    }
    return std::stoi(tokens_[index]);
}

float glimmer::CommandArgs::AsFloat(const int index) const {
    if (index < 0 || index >= static_cast<int>(tokens_.size())) {
        return 0.0F;
    }
    return std::stof(tokens_[index]);
}

double glimmer::CommandArgs::AsDouble(const int index) const {
    if (index < 0 || index >= static_cast<int>(tokens_.size())) {
        return 0.0F;
    }
    return std::stod(tokens_[index]);
}

std::string glimmer::CommandArgs::AsString(const int index) const {
    if (index < 0 || index >= static_cast<int>(tokens_.size())) {
        return "";
    }
    return tokens_[index];
}

std::optional<glimmer::ResourceRef> glimmer::CommandArgs::AsResourceRef(const int index, const int resourceType) const {
    return Resource::ParseFromId(AsString(index), resourceType);
}
