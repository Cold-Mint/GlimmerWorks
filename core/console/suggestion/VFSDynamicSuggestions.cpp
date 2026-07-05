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
#include "VFSDynamicSuggestions.h"

#include "core/Constants.h"
#include "core/log/LogCat.h"

glimmer::VFSDynamicSuggestions::VFSDynamicSuggestions(VirtualFileSystem* virtualFileSystem) : virtualFileSystem_(
    virtualFileSystem)
{
}

std::string glimmer::VFSDynamicSuggestions::GetId() const
{
    return VFS_DYNAMIC_SUGGESTIONS_NAME;
}

bool glimmer::VFSDynamicSuggestions::Match(std::string keyword, std::string param)
{
    return virtualFileSystem_->Exists(keyword);
}

std::vector<std::string> glimmer::VFSDynamicSuggestions::GetSuggestions(std::optional<std::string> param)
{
    if (!param.has_value())
    {
        return {};
    }
    const std::string& paramValue = param.value();
    std::string directory;
    std::string keyword;
    // 判断是否包含 '/'
    auto pos = paramValue.find_last_of('/');
    if (pos != std::string::npos)
    {
        // 目录 = 最后一个 '/' 之前的部分
        directory = paramValue.substr(0, pos);
        if (directory.empty()) directory = "/"; // 根目录情况

        // 关键字 = '/' 之后的部分
        keyword = paramValue.substr(pos + 1);
    }
    else
    {
        // 不包含 '/'
        directory = "";
        keyword = paramValue;
    }

    // 目录不存在 → 返回空
    if (!virtualFileSystem_->Exists(directory))
    {
        return {};
    }

    // 列出目录下所有文件
    auto files = virtualFileSystem_->ListFile(directory, false);
    std::vector<std::string> result;

    // 根据关键字过滤（关键字为空则全部返回）
    for (auto& f : files)
    {
        if (keyword.empty() || f.find(keyword) != std::string::npos)
        {
            result.push_back(f);
        }
    }
    return result;
}
