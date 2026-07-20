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


Rml::ElementDocument* glimmer::RmlContext::LoadDocument(const AppContext* appContext, const ResourceRef* resourceRef)
{
    if (context_ == nullptr || resourcePackManager_ == nullptr || resourceRef == nullptr)
    {
        return nullptr;
    }
    const uint64_t fingerprint = resourceRef->GetFingerprint();
    auto iterator = elementDocumentCache_.find(fingerprint);
    if (iterator == elementDocumentCache_.end())
    {
        const std::unique_ptr<RmlResourceResult> rmlResourceResult = resourcePackManager_->GetRmlFilePath(
            appContext, resourceRef);
        if (rmlResourceResult == nullptr)
        {
            LogCat::w(std::source_location::current(), "rmlResourceResult == nullptr");
            return nullptr;
        }
        auto rmlPath = rmlResourceResult->GetResource();
        if (rmlPath == nullptr)
        {
            LogCat::w(std::source_location::current(), "path == nullptr");
            return nullptr;
        }
        Rml::ElementDocument* elementDocument = context_->LoadDocument(rmlPath->string());
        if (elementDocument == nullptr)
        {
            LogCat::w(std::source_location::current(), "elementDocument == nullptr");
            return nullptr;
        }
        elementDocumentCache_[fingerprint] = elementDocument;
        elementDocument->Show();
        return elementDocument;
    }
    Rml::ElementDocument* elementDocument = iterator->second;
    elementDocument->Show();
    return elementDocument;
}

void glimmer::RmlContext::CloseDocument(Rml::ElementDocument* document)
{
    if (document == nullptr)
    {
        return;
    }
    for (auto iterator = elementDocumentCache_.begin(); iterator != elementDocumentCache_.end(); ++iterator)
    {
        if (iterator->second == document)
        {
            document->Close();
            elementDocumentCache_.erase(iterator);
            return;
        }
    }
    document->Close();
}

Rml::Context* glimmer::RmlContext::GetRmlContext() const
{
    if (context_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "context_ == nullptr");
        return nullptr;
    }
    return context_;
}

bool glimmer::RmlContext::Init(VirtualFileSystem* virtualFileSystem, SDL_Renderer* renderer,
                               ResourcePackManager* resourcePackManager, ResourceLocator* resourceLocator,
                               toml::value* langsValuePtr, int width,
                               int height)
{
    resourcePackManager_ = resourcePackManager;
    systemInterfaceSDL3_ = std::make_unique<SystemInterfaceSDL3>(langsValuePtr);
    Rml::SetSystemInterface(systemInterfaceSDL3_.get());
    renderInterfaceSDL3_ = std::make_unique<RenderInterfaceSDL3>(renderer, resourcePackManager, resourceLocator);
    Rml::SetRenderInterface(renderInterfaceSDL3_.get());
    gameFileInterface_ = std::make_unique<GameFileInterface>(virtualFileSystem);
    Rml::SetFileInterface(gameFileInterface_.get());
    gameFontEngineInterface_ = std::make_unique<GameFontEngineInterface>();
    Rml::SetFontEngineInterface(gameFontEngineInterface_.get());
    Rml::Initialise();
    context_ = Rml::CreateContext("glimmerGui", Rml::Vector2i(width, height));
    return true;
}

bool glimmer::RmlContext::LoadFont(const VirtualFileSystem* virtualFileSystem, const std::filesystem::path& path)
{
    if (virtualFileSystem == nullptr)
    {
        return false;
    }
    std::unique_ptr<std::istream> stream = virtualFileSystem->ReadFileAsStream(path);
    if (stream == nullptr)
    {
        return false;
    }
    std::vector<Rml::byte> buffer;
    stream->seekg(0, std::ios::end);
    std::streamsize size = stream->tellg();
    if (size <= 0)
    {
        return false;
    }
    stream->seekg(0, std::ios::beg);

    buffer.resize(static_cast<size_t>(size));
    if (!stream->read(reinterpret_cast<std::istream::char_type*>(buffer.data()), size))
    {
        return false;
    }
    fontDataBuffers_.push_back(std::move(buffer));
    const auto& storedData = fontDataBuffers_.back();
    bool success = Rml::LoadFontFace(
        Rml::Span(storedData.data(), storedData.size()),
        "core",
        Rml::Style::FontStyle::Normal
    );
    if (!success)
    {
        fontDataBuffers_.pop_back();
    }
    return success;
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
