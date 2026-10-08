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
#include "DecoratorNinePatchTexture.h"

#include "RmlUi/Core/ComputedValues.h"
#include "RmlUi/Core/Element.h"
#include "RmlUi/Core/ElementUtilities.h"
#include "RmlUi/Core/Geometry.h"
#include "RmlUi/Core/RenderBox.h"

bool glimmer::DecoratorNinePatchTexture::Initialise(Rml::Texture texture, float edgeTop, float edgeRight,
                                                    float edgeBottom, float edgeLeft)
{
    edgeTop_ = edgeTop;
    edgeRight_ = edgeRight;
    edgeBottom_ = edgeBottom;
    edgeLeft_ = edgeLeft;
    const int textureIndex = AddTexture(texture);
    return textureIndex >= 0;
}

Rml::DecoratorDataHandle glimmer::DecoratorNinePatchTexture::GenerateElementData(Rml::Element* element,
    Rml::BoxArea paint_area) const
{
    const Rml::Texture texture = GetTexture();
    const Rml::Vector2f textureDimensions(texture.GetDimensions());
    if (textureDimensions.x <= 0 || textureDimensions.y <= 0)
        return INVALID_DECORATORDATAHANDLE;

    const auto& computed = element->GetComputedValues();
    const Rml::RenderBox renderBox = element->GetRenderBox(paint_area);
    const Rml::Vector2f surfaceOffset = renderBox.GetFillOffset();
    const Rml::Vector2f surfaceDimensions = renderBox.GetFillSize();

    const Rml::ColourbPremultiplied quadColour = computed.image_color().ToPremultiplied(computed.opacity());

    // 边缘在表面上的位置（像素）
    const float dpRatio = Rml::ElementUtilities::GetDensityIndependentPixelRatio(element);
    const float edgeTopPx = edgeTop_ * dpRatio;
    const float edgeRightPx = edgeRight_ * dpRatio;
    const float edgeBottomPx = edgeBottom_ * dpRatio;
    const float edgeLeftPx = edgeLeft_ * dpRatio;

    // 纹理坐标归一化 [0, 1]
    const float texTop = edgeTop_ / textureDimensions.y;
    const float texBottom = (textureDimensions.y - edgeBottom_) / textureDimensions.y;
    const float texLeft = edgeLeft_ / textureDimensions.x;
    const float texRight = (textureDimensions.x - edgeRight_) / textureDimensions.x;

    // 表面上的4条分割线位置
    Rml::Vector2f surfacePos[4];
    surfacePos[0] = {0, 0};
    surfacePos[1] = {edgeLeftPx, edgeTopPx};
    surfacePos[2] = surfaceDimensions - Rml::Vector2f(edgeRightPx, edgeBottomPx);
    surfacePos[3] = surfaceDimensions;

    // 如果元素太小，按比例缩小角落
    const Rml::Vector2f surfaceCenterSize = surfacePos[2] - surfacePos[1];
    for (int i = 0; i < 2; i++)
    {
        if (surfaceCenterSize[i] < 0.0f)
        {
            const float topLeftSize = surfacePos[1][i] - surfacePos[0][i];
            const float bottomRightSize = surfacePos[3][i] - surfacePos[2][i];
            const float total = topLeftSize + bottomRightSize;
            if (total > 0.0f)
            {
                surfacePos[1][i] = topLeftSize / total * surfaceDimensions[i];
                surfacePos[2][i] = surfacePos[1][i];
            }
            else
            {
                surfacePos[1][i] = surfaceDimensions[i] * 0.5f;
                surfacePos[2][i] = surfacePos[1][i];
            }
        }
    }

    // 偏移
    for (Rml::Vector2f& pos : surfacePos)
        pos += surfaceOffset;

    // 取整
    surfacePos[1] = surfacePos[1].Round();
    surfacePos[2] = surfacePos[2].Round();

    // 纹理坐标的4条分割线
    const float texPosX[4] = {0.0f, texLeft, texRight, 1.0f};
    const float texPosY[4] = {0.0f, texTop, texBottom, 1.0f};

    // 生成顶点: 4x4 网格
    Rml::Mesh mesh;
    Rml::Vector<Rml::Vertex>& vertices = mesh.vertices;
    Rml::Vector<int>& indices = mesh.indices;

    vertices.resize(4 * 4);
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            Rml::Vertex& vertex = vertices[y * 4 + x];
            vertex.colour = quadColour;
            vertex.position = {surfacePos[x].x, surfacePos[y].y};
            vertex.tex_coord = {texPosX[x], texPosY[y]};
        }
    }

    // 9个矩形区域，每个2个三角形，每个三角形3个索引
    indices.resize(9 * 2 * 3);
    constexpr int topLeftIndices[9] = {0, 1, 2, 4, 5, 6, 8, 9, 10};
    for (int rect = 0; rect < 9; rect++)
    {
        const int i = rect * 6;
        const int tl = topLeftIndices[rect];
        indices[i] = tl;
        indices[i + 1] = tl + 4;
        indices[i + 2] = tl + 1;
        indices[i + 3] = tl + 1;
        indices[i + 4] = tl + 4;
        indices[i + 5] = tl + 5;
    }

    auto* data = new Rml::Geometry(element->GetRenderManager()->MakeGeometry(std::move(mesh)));
    return reinterpret_cast<Rml::DecoratorDataHandle>(data);
}

void glimmer::DecoratorNinePatchTexture::ReleaseElementData(Rml::DecoratorDataHandle element_data) const
{
    delete reinterpret_cast<Rml::Geometry*>(element_data);
}

void glimmer::DecoratorNinePatchTexture::RenderElement(Rml::Element* element,
                                                       const Rml::DecoratorDataHandle element_data) const
{
    auto* data = reinterpret_cast<Rml::Geometry*>(element_data);
    data->Render(element->GetAbsoluteOffset(Rml::BoxArea::Border), GetTexture());
}
