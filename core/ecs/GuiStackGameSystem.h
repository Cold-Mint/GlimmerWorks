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
#include "GameSystem.h"
#include "GuiGameSystem.h"

namespace glimmer {
    /**
     * Gui system based on Stack (stack).
     * 基于Stack（栈）的Gui系统。
     *
     * The switch is automatically scheduled by SystemScheduler.
     * 由SystemScheduler自动调度开关。
     */
    class GuiStackGameSystem : public GuiGameSystem {
        SDL_Scancode hotKey_ = SDL_SCANCODE_UNKNOWN;

    protected:
        void SetAndHideElementDocument(Rml::ElementDocument *document);

    public:
        explicit GuiStackGameSystem(WorldContext *worldContext);

        void OnActivationChanged(bool activeStatus) override;

        /**
         * Get the hotkey for opening the Gui system
         * 获取打开Gui系统的热键
         * If you need to set hotkeys, then override this function.
         * 如果需要设置热键，那么覆盖此函数。
         * @return
         */
        virtual SDL_Scancode GetHotKey() const;

        /**
         * Add the shortcut key to the list of compatible shortcuts. The first shortcut key that players encounter when entering the game is E (to open the inventory). Once players get used to using E to close the UI, even if the system's key for closing the UI is ESC, they will press E to open the inventory UI. The expectation is to press E to close. This method indicates whether to add the shortcut key to the compatible list to accommodate players' habits.
         * 将快捷键添加到关闭兼容快捷键列表内，玩家进入游戏接触到的第一个快捷键是E（打开背包），玩家就会习惯使用E关闭UI，即使系统关闭UI的键为ESC，玩家使用E键打开背包UI，预期为按E关闭。这个方法是表示是否将快捷键加入到兼容列表内，以兼容玩家的习惯。
         * @return
         */
        virtual bool IsCloseKeyCompatible() const;

        /**
         * This method achieves automatic start-stop scheduling.
         * 这个方法实现了自动启停调度。
         * @return
         */
        bool CanActive() const override;
    };
}
