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
#include "core/mod/ResourceRef.h"

namespace glimmer {
    /**
     * DimensionSwitchEvent
     * 维度切换事件
     * Published before the world switches to a new dimension, while the old
     * dimension is still the current one. Subscribers (e.g. ChunkManager) can
     * save data to the old dimension folder before the switch happens.
     * 在世界切换到新维度之前发布，此时旧维度仍是当前维度。订阅者（如 ChunkManager）
     * 可在切换发生前把数据保存到旧维度文件夹。
     */
    class DimensionSwitchEvent {
        ResourceRef oldDimensionRef_;
        ResourceRef newDimensionRef_;

    public:
        /**
         * SetOldDimensionRef
         * 设置切换前的维度资源引用。
         */
        void SetOldDimensionRef(const ResourceRef &oldDimensionRef);

        /**
         * GetOldDimensionRef
         * 获取切换前的维度资源引用。
         */
        [[nodiscard]] const ResourceRef &GetOldDimensionRef() const;

        /**
         * SetNewDimensionRef
         * 设置切换后的维度资源引用。
         */
        void SetNewDimensionRef(const ResourceRef &newDimensionRef);

        /**
         * GetNewDimensionRef
         * 获取切换后的维度资源引用。
         */
        [[nodiscard]] const ResourceRef &GetNewDimensionRef() const;
    };
}
