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

#include "RmlUi/Core/Decorator.h"

namespace glimmer
{
    /**
     * Create a custom ninepatch decorator that directly accepts the texture URL and edge size (in pixels).
     * 自定义 ninepatch decorator，直接接受纹理 URL 和边缘尺寸（px）。
     *
     * RCSS 语法:
     *   decorator: ninepatch-texture(url(texture://@core:gui/button), 2px, 2px, 2px, 2px);
     *   参数: src, edge-top, edge-right, edge-bottom, edge-left
     *   简写: decorator: ninepatch-texture(src, edge-top, edge-right, edge-bottom, edge-left);
     */
    class DecoratorNinePatchTexture : public Rml::Decorator
    {
        float edgeTop_ = 0;
        float edgeRight_ = 0;
        float edgeBottom_ = 0;
        float edgeLeft_ = 0;

    public:
        bool Initialise(Rml::Texture texture, float edgeTop, float edgeRight, float edgeBottom, float edgeLeft);

        Rml::DecoratorDataHandle GenerateElementData(Rml::Element* element, Rml::BoxArea paint_area) const override;

        void ReleaseElementData(Rml::DecoratorDataHandle element_data) const override;

        void RenderElement(Rml::Element* element, Rml::DecoratorDataHandle element_data) const override;
    };
}
