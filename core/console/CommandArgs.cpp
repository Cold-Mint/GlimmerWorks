/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "CommandArgs.h"

#include "core/mod/Resource.h"
#include "core/mod/ResourceRef.h"
#include <optional>

glimmer::CommandArgs::CommandArgs(const std::string& command)
{
    command_ = command;
    std::istringstream iss(command);
    std::string token;
    while (iss >> token)
    {
        tokens_.push_back(token);
    }
}

int glimmer::CommandArgs::GetTokenIndexAtCursor(const int cursorPos) const
{
    const int commandLen = static_cast<int>(command_.size());
    if (commandLen == 0 || cursorPos < 0 || cursorPos > commandLen)
    {
        return -1;
    }

    int tokenIndex = 0;
    bool inToken = false;
    int maxIndex = commandLen - 1;
    for (int i = 0; i < cursorPos; ++i)
    {
        const char c = command_[i];
        if (c == ' ')
        {
            if (inToken)
            {
                if (i < maxIndex)
                {
                    if (const char nextChar = command_[i + 1]; nextChar != ' ')
                    {
                        tokenIndex++;
                        inToken = false;
                    }
                }
                else
                {
                    tokenIndex++;
                    inToken = false;
                }
            }
        }
        else
        {
            if (!inToken)
            {
                inToken = true;
            }
        }
    }
    return tokenIndex;
}

std::string glimmer::CommandArgs::GetKeywordAtCursor(const int cursorPos) const
{
    const int index = GetTokenIndexAtCursor(cursorPos);
    if (index < 0 || index >= tokens_.size())
    {
        return "";
    }
    return tokens_[index];
}


int glimmer::CommandArgs::GetSize() const
{
    return static_cast<int>(tokens_.size());
}

bool glimmer::CommandArgs::AsBool(const int index) const
{
    if (index < 0 || index >= static_cast<int>(tokens_.size()))
    {
        return false;
    }
    const std::string& val = tokens_[index];
    if (val == "1" || val == "true" || val == "yes" || val == "y")
    {
        return true;
    }
    if (val == "0" || val == "false" || val == "no" || val == "n")
    {
        return false;
    }
    return false;
}

glimmer::BoolOrToggle glimmer::CommandArgs::AsBoolOrToggle(int index) const
{
    using enum BoolOrToggle;
    if (index < 0 || index >= static_cast<int>(tokens_.size()))
    {
        return FALSE;
    }
    const std::string& val = tokens_[index];
    if (val == "1" || val == "true" || val == "yes" || val == "y")
    {
        return TRUE;
    }
    if (val == "0" || val == "false" || val == "no" || val == "n")
    {
        return FALSE;
    }
    if (val == TOGGLE_KEY_WORD)
    {
        return TOGGLE;
    }
    return FALSE;
}

int glimmer::CommandArgs::AsInt(const int index) const
{
    if (index < 0 || index >= static_cast<int>(tokens_.size()))
    {
        return 0;
    }
    return std::stoi(tokens_[index]);
}

float glimmer::CommandArgs::AsCoordinate(const int index, const float origin) const
{
    if (index < 0 || index >= static_cast<int>(tokens_.size()))
    {
        return 0.0F;
    }
    const std::string& token = tokens_[index];
    if (token == "~")
    {
        return origin;
    }
    //Parsing ~ + number or ~ - number format
    //解析~+数字 或 ~-数字 格式
    if (!token.empty() && token[0] == '~')
    {
        // The extracted part (symbols + numbers)
        // 提取~后的部分（符号+数字）
        std::string suffix = token.substr(1);
        if (suffix.empty())
        {
            return origin;
        }

        // Set the symbol (default is plus sign)
        // 确定符号（默认正号）
        float sign = 1.0F;
        size_t numStart = 0; //The starting position of the numbers 数字开始的位置

        // Check for the presence of the plus/minus symbol
        // 检查是否有+/-符号
        if (suffix[0] == '+' || suffix[0] == '-')
        {
            sign = suffix[0] == '-' ? -1.0F : 1.0F;
            numStart = 1; // The numbers start after the symbols. 数字从符号后开始
        }

        //Extract the numerical part and convert it
        //提取数字部分并转换
        try
        {
            //Check if the numeric part is valid (with at least one number)
            //检查数字部分是否有效（至少有一个数字）
            if (numStart >= suffix.size() || std::isdigit(static_cast<unsigned char>(suffix[numStart])) == 0)
            {
                return origin; //No valid digits, returning the original value 无有效数字，返回原始值
            }
            float offset = std::stof(suffix.substr(numStart));
            //Calculate the final value: Original value + Offset * Sign * Tile size
            //计算最终值：原始值 + 偏移量 * 符号 * 瓦片大小
            return origin + sign * offset * TILE_SIZE;
        }
        catch (const std::invalid_argument&)
        {
            //Digital conversion failed (e.g. ~+abc), return original value
            //数字转换失败（如~+abc），返回原始值
            return origin;
        }
        catch (const std::out_of_range&)
        {
            //The number is out of range. Returning the original value.
            //数字超出范围，返回原始值
            return origin;
        }
    }
    return std::stof(token) * TILE_SIZE;
}

float glimmer::CommandArgs::AsFloat(const int index) const
{
    if (index < 0 || index >= static_cast<int>(tokens_.size()))
    {
        return 0.0F;
    }
    return std::stof(tokens_[index]);
}

double glimmer::CommandArgs::AsDouble(const int index) const
{
    if (index < 0 || index >= static_cast<int>(tokens_.size()))
    {
        return 0.0F;
    }
    return std::stod(tokens_[index]);
}

std::string glimmer::CommandArgs::AsString(const int index) const
{
    if (index < 0 || index >= static_cast<int>(tokens_.size()))
    {
        return "";
    }
    return tokens_[index];
}

std::optional<glimmer::ResourceRef> glimmer::CommandArgs::AsResourceRef(int index,
                                                                        ResourceTypeMessage resourceType) const
{
    return ResourceRef::ParseFromId(AsString(index), resourceType);
}
