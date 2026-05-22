//
// Created by Cold-Mint on 2025/10/15.
//

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
#include "toml11/spec.hpp"

namespace glimmer {
    class AppContext;
    struct PreloadColors;

    class ResourcePackManager {
        friend class ResourceLocator;
        VirtualFileSystem *virtualFileSystem_;
        SDL_Renderer *renderer_;
        MIX_Mixer *mixer_;
        std::vector<std::string> packIdVector_;
        std::unordered_map<std::string, std::unique_ptr<ResourcePack> > resourcePackMap_;
        /**
         * Placeholder texture Path Set
         * 用于存储加载失败的纹理路径
         */
        std::unordered_set<std::string> errorTexturePathSet_{};

        bool IsResourcePackAvailable(const ResourcePack &pack) const;

        static bool IsResourcePackEnabled(const ResourcePack &pack,
                                          const std::vector<std::string> &enabledResourcePack);

        std::unordered_map<std::string, std::weak_ptr<SDL_Texture> > textureCache_;

        std::unordered_map<std::string, std::weak_ptr<MIX_Audio> > audioMixCache_;

        std::unordered_map<std::string, std::unique_ptr<ColorResource> > colorCache_;


        std::shared_ptr<SDL_Texture> ImplLoadTextureFromFile(const std::string &path, const Mods &modConfig);

        std::shared_ptr<MIX_Audio> ImplLoadAudioFromFile(const std::string &path, const Mods &modConfig);

        std::shared_ptr<SDL_Texture> LoadTextureFromFile(AppContext *appContext, const ResourceRef *resourceRef);

        std::shared_ptr<SDL_Texture> LoadTextureFromFileRaw(AppContext *appContext, const ResourceRef *resourceRef);

        std::shared_ptr<MIX_Audio> LoadAudioFromFile(AppContext *appContext, const ResourceRef *resourceRef);

        ColorResource *LoadColorResFromFile(const AppContext *appContext, const ResourceRef *resourceRef);

        std::shared_ptr<SDL_Texture> CreateTexture(Color accent,
                                                   Color base) const;


        std::shared_ptr<SDL_Texture> errorTexture_;
        std::shared_ptr<SDL_Texture> accessDeniedTexture_;

    public:
        explicit ResourcePackManager(VirtualFileSystem *virtualFilesystem);

        void SetMixer(MIX_Mixer *mixer);

        void SetRenderer(SDL_Renderer *renderer, const PreloadColors *preloadColors);

        int Scan(const std::string &path, const std::vector<std::string> &enabledResourcePack,
                 const toml::spec &tomlVersion);

        std::optional<std::string> GetFontPath(const std::vector<std::string> &enabledResourcePack,
                                               const std::string &language);

        std::string ListTextureCache() const;
    };
}
