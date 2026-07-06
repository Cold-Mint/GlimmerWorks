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
#include "ConfigSuggestions.h"

#include "core/Constants.h"

void glimmer::ConfigSuggestions::ParseTable(const toml::value::table_type& table, std::vector<std::string>& fields,
                                            const std::string& prefix)
{
    for (const auto& [key, val] : table)
    {
        std::string fullKey;
        if (prefix.empty())
        {
            fullKey = key;
        }
        else
        {
            fullKey.reserve(prefix.size() + 1 + key.size());
            fullKey = prefix;
            fullKey.push_back('.');
            fullKey += key;
        }
        if (val.is_table())
        {
            ParseTable(val.as_table(), fields, fullKey);
        }
        else
        {
            fields.push_back(fullKey);
        }
    }
}

glimmer::ConfigSuggestions::ConfigSuggestions(toml::value* configValue) : configValue_(configValue)
{
}

bool glimmer::ConfigSuggestions::Match(const std::string& keyword, const std::string& param)
{
    std::vector<std::string> fields;
    ParseTable(configValue_->as_table(), fields);
    return std::find(fields.begin(), fields.end(), keyword) != fields.end();
}

std::string glimmer::ConfigSuggestions::GetId() const
{
    return CONFIG_DYNAMIC_SUGGESTIONS_NAME;
}

std::vector<std::string> glimmer::ConfigSuggestions::GetSuggestions(const std::optional<std::string>& param)
{
    std::vector<std::string> fields;
    ParseTable(configValue_->as_table(), fields);
    return fields;
}
