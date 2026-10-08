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
#pragma once
#include <string>
#include "src/core/game_component_type.pb.h"

namespace glimmer
{
    class AppContext;
    class WorldContext;

    class GameComponent
    {
    public:
        virtual ~GameComponent() = default;

        /**
         * The game attempts to call this function when saving the data of the components to the save file.
         * 游戏尝试将组件的数据保存到存档内时调用。
         *
         * Note that after you override this method, don't forget to add the corresponding judgment within the RecoveryComponent.
         * 注意，当你覆盖了这个方法后不要忘记在RecoveryComponent内添加对应的判断。
         * @return data  The default return value is std::nullopt, indicating that saving to the archive is not supported.
         * 数据 默认返回std::nullopt表示不支持保存到存档。
         */
        [[nodiscard]] virtual std::optional<std::string> Serialize();

        /**
         * Called when loading data from the archive.
         * 从存档加载数据时调用。
         * @param worldContext worldContext 世界上下文
         * @param data data 数据
         */
        virtual void Deserialize(WorldContext* worldContext, const std::string& data);

        [[nodiscard]] virtual GameComponentTypeMessage GetComponentType() = 0;
    };
}
