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
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ResourcePack.h"
#include "core/vfs/VirtualFileSystem.h"
#include "core/Config.h"
#include "core/mod/ResourceLocator.h"
#include "SDL3/SDL_render.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "toml11/spec.hpp"

namespace glimmer
{
    class AppContext;
    struct PreloadColors;

    class ResourcePackManager
    {
        friend class ResourceLocator;

        std::vector<std::string> packIdVector_;
        std::unordered_map<std::string, std::unique_ptr<ResourcePack>> resourcePackMap_;
        TTF_Font* font_ = nullptr;
        VirtualFileSystem* virtualFileSystem_ = nullptr;
        SDL_Renderer* renderer_ = nullptr;
        MIX_Mixer* mixer_ = nullptr;
        /**
         * Placeholder texture Path Set
         * 用于存储加载失败的纹理路径
         */
        std::unordered_set<std::string> errorTexturePathSet_{};

        bool IsResourcePackAvailable(const ResourcePack& pack) const;

        static bool IsResourcePackEnabled(const ResourcePack& pack,
                                          const std::vector<std::string>& enabledResourcePack);

        std::unordered_map<std::string, std::weak_ptr<SDL_Texture>> textureCache_;

        std::unordered_map<uint64_t, std::weak_ptr<SDL_Texture>> stringTextureCache_;

        std::unordered_map<std::string, std::weak_ptr<MIX_Audio>> audioMixCache_;

        std::unordered_map<std::string, std::unique_ptr<ColorResource>> colorCache_;


        std::shared_ptr<SDL_Texture> ImplLoadTextureFromFile(const std::string& path, const Mods& modConfig);

        std::shared_ptr<MIX_Audio> ImplLoadAudioFromFile(const std::string& path, const Mods& modConfig);

        /**
         * Load texture from file
         * 从文件加载纹理
         * @param appContext appContext 应用上下文环境
         * @param resourceRef resourceRef 资源引用
         * @return Not found, return null. 找不到返回null
         */
        std::shared_ptr<SDL_Texture> LoadTextureFromFile(AppContext* appContext, const ResourceRef* resourceRef);

        std::shared_ptr<MIX_Audio> LoadAudioFromFile(AppContext* appContext, const ResourceRef* resourceRef);

        ColorResource* LoadColorResFromFile(const AppContext* appContext, const ResourceRef* resourceRef);

        std::shared_ptr<SDL_Texture> CreateTexture(Color accent,
                                                   Color base) const;


        std::shared_ptr<SDL_Texture> errorTexture_;
        std::shared_ptr<SDL_Texture> accessDeniedTexture_;

    public:
        explicit ResourcePackManager(VirtualFileSystem* virtualFilesystem);

        void SetMixer(MIX_Mixer* mixer);

        void SetRenderer(SDL_Renderer* renderer, const PreloadColors* preloadColors);

        int Scan(const std::string& path, const std::vector<std::string>& enabledResourcePack,
                 const toml::spec& tomlVersion);

        std::optional<std::string> GetFontPath(const std::vector<std::string>& enabledResourcePack,
                                               const std::string& language);

        std::shared_ptr<SDL_Texture> CreateStringTexture(const std::string& string, const Color* color,
                                                         int wrapWidth = 0);

        std::string ListTextureCache() const;

        void SetFont(TTF_Font* font);
    };
}
