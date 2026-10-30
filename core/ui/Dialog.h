/*
 * Copyright (C) 2025-2026  Cold-Mint <cold_mint@qq.com>
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
 * 版权(C) 2025-2026  Cold-Mint <cold_mint@qq.com>
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
#include <functional>
#include <string>
#include <vector>

namespace glimmer {
    /**
     * DialogButton
     * 对话框按钮
     *
     * One of the action buttons shown in a generic dialog. Only `label`,
     * `result`, `destructive` and `suggested` are bound to the RmlUi data
     * model; `result` is the value delivered to DialogRequest::onResult.
     * 通用对话框中显示的操作按钮之一。只有 `label`、`result`、`destructive`、
     * `suggested` 绑定到 RmlUi 数据模型；`result` 是传给 DialogRequest::onResult
     * 的值。
     */
    struct DialogButton {
        /**
         * The button text. Either a localization key (translated by RmlUi) or a
         * raw string produced by StringUtils::MakeRawText.
         * 按钮文本。可以是本地化 key（由 RmlUi 翻译），或由 StringUtils::MakeRawText
         * 产生的原始字符串。
         */
        std::string label;

        /**
         * The value delivered to DialogRequest::onResult when this button is clicked.
         * 点击此按钮时传给 DialogRequest::onResult 的值。
         */
        int result = 0;

        /**
         * Whether the button is a destructive action (rendered with the danger style).
         * 是否是破坏性操作（以危险样式渲染）。
         */
        bool destructive = false;

        /**
         * Whether the button is the suggested action (rendered with the accent style).
         * 是否是建议操作（以强调样式渲染）。
         */
        bool suggested = false;
    };

    /**
     * DialogRequest
     * 对话框请求
     *
     * Describes a generic modal dialog, modelled after Android's AlertDialog:
     * an optional title, an optional message and up to three buttons. The
     * result of the user's choice is delivered asynchronously through
     * `onResult`.
     * 描述一个通用模态对话框，参考 Android 的 AlertDialog：可选标题、可选消息和
     * 最多三个按钮。用户的选择结果通过 `onResult` 异步回传。
     */
    struct DialogRequest {
        /// The dialog title. Optional; empty means no title. 对话框标题，可选，为空则不显示。
        std::string title;

        /// The dialog message. Optional; empty means no message. 对话框消息，可选，为空则不显示。
        std::string message;

        /// The buttons (1 to 3). 按钮（1 到 3 个）。
        std::vector<DialogButton> buttons;

        /**
         * Invoked when a button is clicked, or when the dialog is dismissed
         * through the back key / Escape key. The argument is the `result` of the
         * clicked button, or `cancelResult` when dismissed.
         * 当按钮被点击，或通过返回键/Escape 键关闭对话框时被调用。参数为被点击按钮的
         * `result`，关闭时为 `cancelResult`。
         */
        std::function<void(int)> onResult;

        /**
         * The value delivered to `onResult` when the dialog is dismissed through
         * the back key / Escape key. A value of -1 means the callback is not
         * invoked at all on dismissal.
         * 通过返回键/Escape 键关闭对话框时传给 `onResult` 的值。值为 -1 表示关闭时
         * 不调用回调。
         */
        int cancelResult = -1;
    };
} // namespace glimmer
