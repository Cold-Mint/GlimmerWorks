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
#include <filesystem>
#include <memory>

#include "core/rmi/GameFileInterface.h"
#include "core/rmi/GameFontEngineInterface.h"
#include "core/rmi/RenderInterfaceSDL3.h"
#include "core/rmi/SystemInterfaceSDL3.h"
#include "RmlUi/Core/ElementDocument.h"

namespace glimmer
{
    class RmlContext
    {
        friend class Scene;
        friend class App;
        friend class AppEventLoop;
        std::unique_ptr<SystemInterfaceSDL3> systemInterfaceSDL3_ = nullptr;
        std::unique_ptr<RenderInterfaceSDL3> renderInterfaceSDL3_ = nullptr;
        ResourcePackManager* resourcePackManager_ = nullptr;
        std::unique_ptr<GameFileInterface> gameFileInterface_ = nullptr;
        std::unique_ptr<GameFontEngineInterface> gameFontEngineInterface_ = nullptr;
        Rml::Context* context_ = nullptr;
        std::unordered_map<uint64_t, Rml::ElementDocument*> elementDocumentCache_;
        std::vector<std::vector<Rml::byte>> fontDataBuffers_;

        [[nodiscard]] Rml::ElementDocument* LoadDocument(const AppContext* appContext,
                                                         const ResourceRef* resourceRef);

        void CloseDocument(Rml::ElementDocument* document);

        [[nodiscard]] Rml::Context* GetRmlContext() const;

    public:
        bool Init(VirtualFileSystem* virtualFileSystem, SDL_Renderer* renderer,
                  ResourcePackManager* resourcePackManager, ResourceLocator* resourceLocator,
                  toml::value* langsValuePtr, int width, int height);

        bool LoadFont(const VirtualFileSystem* virtualFileSystem, const std::filesystem::path& path);

        void UpdateContext() const;

        void RenderContext() const;

        RmlContext();

        ~RmlContext();
    };
}
