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
#include "TimeUtils.h"

#include <chrono>
#include <format>
#include <iomanip>
#include <sstream>


long glimmer::TimeUtils::GetCurrentTimeMs()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

std::string glimmer::TimeUtils::FormatTimeMs(const LangsResources* langsResources, uint64_t ms)
{
    if (ms < 1000)
    {
        double seconds = static_cast<double>(ms) / 1000.0F;
        return std::format("{:.2f}{}", seconds, langsResources->timeS);
    }

    uint64_t totalSeconds = ms / 1000;
    if (totalSeconds < 60)
    {
        return std::format("{}{}", totalSeconds, langsResources->timeS);
    }

    uint64_t minutes = totalSeconds / 60;
    uint64_t seconds = totalSeconds % 60;

    if (minutes < 60)
    {
        return std::format("{}{} {}{}", minutes, langsResources->timeM, seconds, langsResources->timeS);
    }
    uint64_t hours = minutes / 60;
    minutes = minutes % 60;
    return std::format("{}{} {}{} {}{}", hours, langsResources->timeH, minutes, langsResources->timeM, seconds,
                       langsResources->timeS);
}
