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
#include "RmlContext.h"

#include "core/log/LogCat.h"
#include "RmlUi/Core/Context.h"
#include "RmlUi/Core/Core.h"

bool glimmer::RmlContext::Init(SDL_Renderer* renderer, const int width, const int height)
{
    systemInterfaceSDL3_ = std::make_unique<SystemInterfaceSDL3>();
    Rml::SetSystemInterface(systemInterfaceSDL3_.get());
    renderInterfaceSDL3_ = std::make_unique<RenderInterfaceSDL3>(renderer);
    Rml::SetRenderInterface(renderInterfaceSDL3_.get());
    Rml::Initialise();
    context_ = Rml::CreateContext("glimmerGui", Rml::Vector2i(width, height));
    return true;
}

bool glimmer::RmlContext::LoadFont(const std::filesystem::path& fontPath)
{
    return Rml::LoadFontFace(fontPath);
}

Rml::Context* glimmer::RmlContext::GetRmlContext() const
{
    return context_;
}

bool glimmer::RmlContext::LoadDocument(const std::filesystem::path& documentPath)
{
    if (context_ == nullptr)
    {
        return false;
    }
    document_ = context_->LoadDocument(documentPath);
    if (document_ != nullptr)
    {
        document_->Show();
    }
    return true;
}

Rml::ElementDocument* glimmer::RmlContext::GetDocument() const
{
    return document_;
}

void glimmer::RmlContext::UpdateContext() const
{
    if (context_ == nullptr)
    {
        return;
    }
    context_->Update();
}

void glimmer::RmlContext::RenderContext() const
{
    if (context_ == nullptr)
    {
        return;
    }
    context_->Render();
}


glimmer::RmlContext::RmlContext() = default;

glimmer::RmlContext::~RmlContext()
{
    LogCat::d("Destroy rmlContext");
    Rml::Shutdown();
}
