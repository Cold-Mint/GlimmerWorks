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

glimmer::BoolOrToggle glimmer::CommandArgs::AsBoolOrToggle(int index) const {
    if (index < 0 || index >= static_cast<int>(tokens_.size())) {
        return FALSE;
    }
    const std::string &val = tokens_[index];
    if (val == "1" || val == "true" || val == "yes" || val == "y") {
        return TRUE;
    }
    if (val == "0" || val == "false" || val == "no" || val == "n") {
        return FALSE;
    }
    if (val == TOGGLE_KEY_WORD) {
        return TOGGLE;
    }
    return FALSE;
}

int glimmer::CommandArgs::AsInt(const int index) const {
    if (index < 0 || index >= static_cast<int>(tokens_.size())) {
        return 0;
    }
    return std::stoi(tokens_[index]);
}

float glimmer::CommandArgs::AsCoordinate(const int index, const float origin) const {
    if (index < 0 || index >= static_cast<int>(tokens_.size())) {
        return 0.0F;
    }
    const std::string &token = tokens_[index];
    if (token == "~") {
        return origin;
    }
    //Parsing ~ + number or ~ - number format
    //解析~+数字 或 ~-数字 格式
    if (!token.empty() && token[0] == '~') {
        // The extracted part (symbols + numbers)
        // 提取~后的部分（符号+数字）
        std::string suffix = token.substr(1);
        if (suffix.empty()) {
            return origin;
        }

        // Set the symbol (default is plus sign)
        // 确定符号（默认正号）
        float sign = 1.0f;
        size_t numStart = 0; //The starting position of the numbers 数字开始的位置

        // Check for the presence of the plus/minus symbol
        // 检查是否有+/-符号
        if (suffix[0] == '+' || suffix[0] == '-') {
            sign = suffix[0] == '-' ? -1.0f : 1.0f;
            numStart = 1; // The numbers start after the symbols. 数字从符号后开始
        }

        //Extract the numerical part and convert it
        //提取数字部分并转换
        try {
            //Check if the numeric part is valid (with at least one number)
            //检查数字部分是否有效（至少有一个数字）
            if (numStart >= suffix.size() || !std::isdigit(static_cast<unsigned char>(suffix[numStart]))) {
                return origin; //No valid digits, returning the original value 无有效数字，返回原始值
            }
            float offset = std::stof(suffix.substr(numStart));
            //Calculate the final value: Original value + Offset * Sign * Tile size
            //计算最终值：原始值 + 偏移量 * 符号 * 瓦片大小
            return origin + sign * offset * TILE_SIZE;
        } catch (const std::invalid_argument &) {
            //Digital conversion failed (e.g. ~+abc), return original value
            //数字转换失败（如~+abc），返回原始值
            return origin;
        } catch (const std::out_of_range &) {
            //The number is out of range. Returning the original value.
            //数字超出范围，返回原始值
            return origin;
        }
    }
    return std::stof(token) * TILE_SIZE;
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
    return ResourceRef::ParseFromId(AsString(index), resourceType);
}
