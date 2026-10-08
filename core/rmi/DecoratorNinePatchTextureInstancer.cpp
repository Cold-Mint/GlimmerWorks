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
#include "DecoratorNinePatchTextureInstancer.h"

#include "DecoratorNinePatchTexture.h"
#include "RmlUi/Core/PropertyDefinition.h"

void glimmer::DecoratorNinePatchTextureInstancer::RegisterProperties()
{
    srcId_ = RegisterProperty("src", "").AddParser("string").GetId();
    edgeIds_[0] = RegisterProperty("edge-top", "0px").AddParser("number_length_percent").GetId();
    edgeIds_[1] = RegisterProperty("edge-right", "0px").AddParser("number_length_percent").GetId();
    edgeIds_[2] = RegisterProperty("edge-bottom", "0px").AddParser("number_length_percent").GetId();
    edgeIds_[3] = RegisterProperty("edge-left", "0px").AddParser("number_length_percent").GetId();

    RegisterShorthand("edge", "edge-top, edge-right, edge-bottom, edge-left", Rml::ShorthandType::Box);
    RegisterShorthand("decorator", "src, edge?", Rml::ShorthandType::RecursiveCommaSeparated);
}

Rml::SharedPtr<Rml::Decorator> glimmer::DecoratorNinePatchTextureInstancer::InstanceDecorator(const Rml::String& name,
    const Rml::PropertyDictionary& properties, const Rml::DecoratorInstancerInterface& instancer_interface)
{
    const auto textureName = properties.GetProperty(srcId_)->Get<Rml::String>();

    Rml::Texture texture = instancer_interface.GetTexture(textureName);
    if (!texture)
    {
        Rml::Log::Message(Rml::Log::LT_WARNING, "Could not load texture '%s' in ninepatch-texture decorator.",
                          textureName.c_str());
        return nullptr;
    }

    float edges[4] = {0, 0, 0, 0};
    for (int i = 0; i < 4; i++)
    {
        const Rml::Property* prop = properties.GetProperty(edgeIds_[i]);
        if (prop->unit == Rml::Unit::PX)
            edges[i] = prop->Get<float>();
        else if (prop->unit == Rml::Unit::PERCENT)
        {
            // 百分比值将在 GenerateElementData 中通过 dpRatio 处理
            edges[i] = prop->Get<float>();
        }
        else
            edges[i] = prop->Get<float>();
    }

    auto decorator = Rml::MakeShared<DecoratorNinePatchTexture>();
    if (!decorator->Initialise(texture, edges[0], edges[1], edges[2], edges[3]))
        return nullptr;

    return decorator;
}
