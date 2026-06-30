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
#include "PlayerComponent.h"
#include "fmt/xchar.h"


void glimmer::PlayerComponent::ResetTechnologyMap()
{
    technologyMap_.clear();
    technologyMap_[RecipeGroup::None] = 1;
}

void glimmer::PlayerComponent::SetTechnology(RecipeGroup recipeGroup, uint8_t technologyLevel)
{
    auto iterator = technologyMap_.find(recipeGroup);
    if (iterator == technologyMap_.end())
    {
        technologyMap_[recipeGroup] = technologyLevel;
    }
    else
    {
        if (technologyLevel > iterator->second)
        {
            //If the current technological level set is higher than the previous one.
            //如果当前设置的科技等级大于上次设置的科技等级。
            technologyMap_[recipeGroup] = technologyLevel;
        }
    }
}

const std::unordered_map<glimmer::RecipeGroup, uint8_t>& glimmer::PlayerComponent::GetTechnologyMap() const
{
    return technologyMap_;
}

std::string glimmer::PlayerComponent::ListTechnology(const std::string& technologyItem) const
{
    std::stringstream ss;
    for (auto& technology : technologyMap_)
    {
        ss << fmt::format(
            fmt::runtime(technologyItem),
            static_cast<int>(technology.first), static_cast<int>(technology.second));
        ss << '\n';
    }
    return ss.str();
}

GameComponentTypeMessage glimmer::PlayerComponent::GetComponentTypeStatic()
{
    return COMPONENT_PLAYER;
}

GameComponentTypeMessage glimmer::PlayerComponent::GetComponentType()
{
    return GetComponentTypeStatic();
}
