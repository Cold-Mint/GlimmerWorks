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
#include "RenderInterfaceSDL3.h"

#include "core/log/LogCat.h"
#include "core/utils/StringUtils.h"


glimmer::RenderInterfaceSDL3::RenderInterfaceSDL3(SDL_Renderer* renderer, ResourcePackManager* resourcePackManager,
                                                  ResourceLocator* resourceLocator) : RenderInterface_SDL(renderer)
{
    resourcePackManager_ = resourcePackManager;
    resourceLocator_ = resourceLocator;
}

Rml::TextureHandle glimmer::RenderInterfaceSDL3::LoadTexture(Rml::Vector2i& texture_dimensions,
                                                             const Rml::String& source)
{
    if (!source.starts_with("texture://"))
    {
        LogCat::w(std::source_location::current(),
                  "Only textures with the loading path starting with \'texture://\' are supported.");
        return {};
    }
    LogCat::i("LoadTexture = ", source);
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_TEXTURE);
    resourceRef.SetResourceKey(source.substr(std::strlen("texture://")));
    std::shared_ptr<TextureResourceResult> textureResourceResult = resourceLocator_->FindTexture(&resourceRef);
    if (textureResourceResult == nullptr)
    {
        LogCat::w(std::source_location::current(), "textureResourceResult == nullptr");
        return {};
    }
    SDL_Texture* sdlTexture = textureResourceResult->GetResource();
    const auto textureHandle = reinterpret_cast<Rml::TextureHandle>(sdlTexture);
    if (sdlTexture == nullptr)
    {
        LogCat::w(std::source_location::current(), "sdlTexture == nullptr");
        return {};
    }
    texture_dimensions = {sdlTexture->w, sdlTexture->h};
    textureMap_[textureHandle] = textureResourceResult;
    return textureHandle;
}

void glimmer::RenderInterfaceSDL3::ReleaseTexture(Rml::TextureHandle texture_handle)
{
    textureMap_.erase(texture_handle);
}

glimmer::RenderInterfaceSDL3::~RenderInterfaceSDL3() = default;
