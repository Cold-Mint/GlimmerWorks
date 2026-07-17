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
#include "SystemInterfaceSDL3.h"

#include "core/Constants.h"
#include "core/log/LogCat.h"
#include "toml11/find.hpp"

glimmer::SystemInterfaceSDL3::SystemInterfaceSDL3(toml::value* langsValuePtr) : langsValuePtr_(langsValuePtr)
{
}

int glimmer::SystemInterfaceSDL3::TranslateString(Rml::String& translated, const Rml::String& input)
{
    if (input.empty() || input == "\n")
    {
        return 0;
    }
    if (input.starts_with(RAW_TEXT_PREFIX))
    {
        translated = input.substr(RAW_TEXT_PREFIX.length());
        return 1;
    }
    if (langsValuePtr_ == nullptr)
    {
        return 0;
    }
    auto translatedResult = toml::find_or_default<std::string>(*langsValuePtr_, input);
    if (translatedResult.empty())
    {
        translated = input;
        return 0;
    }
    translated = translatedResult;
    return 1;
}

bool glimmer::SystemInterfaceSDL3::LogMessage(const Rml::Log::Type type, const Rml::String& message)
{
    if (type == Rml::Log::LT_ERROR)
    {
        LogCat::e(std::source_location::current(), "From rml :", message.c_str());
    }
    if (type == Rml::Log::LT_WARNING)
    {
        LogCat::w(std::source_location::current(), "From rml :", message.c_str());
    }
    if (type == Rml::Log::LT_INFO)
    {
        LogCat::i("From rml :", message.c_str());
    }
    if (type == Rml::Log::LT_DEBUG)
    {
        LogCat::d("From rml :", message.c_str());
    }
    return true;
}
