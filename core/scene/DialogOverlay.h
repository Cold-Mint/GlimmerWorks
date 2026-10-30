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
#include <optional>
#include <string>
#include <vector>

#include "Scene.h"
#include "core/ui/Dialog.h"
#include "RmlUi/Core/DataModelHandle.h"

namespace glimmer {
    /**
     * DialogOverlay
     * 通用对话框叠加层
     *
     * A persistent overlay that presents a generic modal dialog (GNOME / Adwaita
     * styled) through an RmlUi data model. It follows the same pattern as
     * UIMessageOverlay: the visual presentation lives entirely in a resource-pack
     * RML document (overridable by mods), while this scene only owns the dialog
     * state and lifetime.
     * 常驻叠加层，通过 RmlUi 数据模型呈现通用模态对话框（GNOME / Adwaita 风格）。
     * 它沿用 UIMessageOverlay 的模式：视觉呈现完全由资源包的 RML 文档决定（可被
     * 模组覆盖），本场景只负责对话框状态与生命周期。
     */
    class DialogOverlay final : public Scene {
        std::string title_;
        std::string message_;
        std::vector<DialogButton> buttons_;
        std::function<void(int)> onResult_;
        int cancelResult_ = -1;
        Rml::DataModelHandle modelHandle_;
        Rml::ElementDocument *dialogDocument_ = nullptr;

        void OnButtonClick(Rml::DataModelHandle handle, Rml::Event &event, const Rml::VariantList &args);

        void HideAndClear();

        void InvokeResult(int result);

        void ApplyToModel();

    public:
        explicit DialogOverlay(AppContext *context);

        /**
         * Show the dialog described by `request`. If a dialog is already visible,
         * it is replaced by the new one.
         * 显示 `request` 描述的对话框。如果已有可见对话框，则被新的替换。
         */
        void ShowDialog(DialogRequest request);

        /**
         * Whether the dialog document is currently visible.
         * 对话框文档当前是否可见。
         */
        [[nodiscard]] bool IsVisible() const;

        void LoadDocuments() override;

        void OnCreateDataModels() override;

        bool HandleEvent(const SDL_Event &event) override;

        bool OnBackPressed() override;
    };
} // namespace glimmer
