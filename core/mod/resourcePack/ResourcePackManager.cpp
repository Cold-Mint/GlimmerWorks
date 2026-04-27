//
// Created by Cold-Mint on 2025/10/15.
//

#include "ResourcePackManager.h"

#include <filesystem>
#include <regex>
#include <algorithm>
#include "ResourcePack.h"
#include "core/Config.h"
#include "core/log/LogCat.h"
#include "core/scene/AppContext.h"
#include "SDL3_image/SDL_image.h"
#include "toml11/get.hpp"
#include "toml11/parser.hpp"
#include "toml11/spec.hpp"
#include "core/utils/TomlUtils.h"
#include "SDL3/SDL_render.h"
#include "SDL3_mixer/SDL_mixer.h"


bool glimmer::ResourcePackManager::IsResourcePackAvailable(const ResourcePack &pack) const {
    const PackManifest &manifest = pack.getManifest();
    if (manifest.id == RESOURCE_REF_CORE || manifest.id == RESOURCE_REF_SELF) {
        return false;
    }
    if (resourcePackMap_.contains(manifest.id)) {
        LogCat::i("Duplicate package ID: ", manifest.id);
        return false;
    }
    if (manifest.minGameVersion > GAME_VERSION_NUMBER) {
        LogCat::e("ResourcePack ", manifest.id, " requires game version ",
                  manifest.minGameVersion, ", current version: ", GAME_VERSION_NUMBER);
        return false;
    }

    return manifest.resPack;
}

bool glimmer::ResourcePackManager::IsResourcePackEnabled(const ResourcePack &pack,
                                                         const std::vector<std::string> &enabledResourcePack) {
    return std::ranges::find(enabledResourcePack, pack.getManifest().id) != enabledResourcePack.end();
}

std::shared_ptr<SDL_Texture> glimmer::ResourcePackManager::ImplLoadTextureFromFile(const std::string &path,
    const Mods &modConfig) {
    if (renderer_ == nullptr || path.empty()) {
        return nullptr;
    }
    const auto cache = textureCache_.find(path);
    if (cache != textureCache_.end()) {
        if (auto cacheTexture = cache->second.lock()) {
            return cacheTexture;
        }
        //Cache has expired.
        //缓存过期。
        textureCache_.erase(cache);
    }

    for (const auto &packId: modConfig.enabledResourcePack) {
        auto resourcePackIterator = resourcePackMap_.find(packId);
        if (resourcePackIterator == resourcePackMap_.end()) {
            continue;
        }
        const ResourcePack *resourcePack = resourcePackIterator->second.get();
        if (resourcePack == nullptr) {
            continue;
        }
        for (auto &supportedTextureFormat: modConfig.supportedTextureFormats) {
            std::string texturePath = resourcePack->getPath() + "/textures/" + path + "." + supportedTextureFormat;

            if (!virtualFileSystem_->Exists(texturePath)) {
                continue;
            }

            auto actualTexturePath = virtualFileSystem_->GetActualPath(texturePath);
            if (!actualTexturePath.has_value()) {
                continue;
            }

            SDL_Surface *surface = IMG_Load(actualTexturePath.value().c_str());
            if (surface == nullptr) {
                continue;
            }

            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer_, surface);
            SDL_DestroySurface(surface);
            if (texture == nullptr) {
                continue;
            }
            auto deleter = [this,path](SDL_Texture *sdlTexture) {
                if (sdlTexture != nullptr) {
                    SDL_DestroyTexture(sdlTexture);
                }
                textureCache_.erase(path);
            };
            std::shared_ptr<SDL_Texture> texturePtr(texture, std::move(deleter));
            textureCache_[path] = texturePtr;
            return texturePtr;
        }
    }
    return nullptr;
}

std::shared_ptr<MIX_Audio> glimmer::ResourcePackManager::ImplLoadAudioFromFile(const std::string &path,
                                                                               const Mods &modConfig) {
    if (mixer_ == nullptr || path.empty()) {
        return nullptr;
    }
    const auto cacheIt = audioMixCache_.find(path);
    if (cacheIt != audioMixCache_.end()) {
        if (auto tex = cacheIt->second.lock()) {
            return tex;
        }
        LogCat::w("Cached audio expired, reloading: ", path);
    }

    for (const auto &packId: modConfig.enabledResourcePack) {
        auto it = resourcePackMap_.find(packId);
        if (it == resourcePackMap_.end()) {
            LogCat::w("Resource pack not found: ", packId);
            continue;
        }

        const ResourcePack *pack = it->second.get();
        for (auto &supportedAudioFormat: modConfig.supportedAudioFormats) {
            std::string audioPath = pack->getPath() + "/audios/" + path + "." + supportedAudioFormat;
            if (!virtualFileSystem_->Exists(audioPath)) {
                continue;
            }

            auto actualAudioPath = virtualFileSystem_->GetActualPath(audioPath);
            if (!actualAudioPath.has_value()) {
                continue;
            }
            MIX_Audio *audio = MIX_LoadAudio(mixer_, actualAudioPath.value().c_str(), false);
            if (audio == nullptr) {
                continue;
            }

            LogCat::d("Loaded audio from pack '", packId, "': ", audioPath);
            auto deleter = [](MIX_Audio *audio) {
                if (audio != nullptr) {
                    MIX_DestroyAudio(audio);
                }
            };

            std::shared_ptr<MIX_Audio> audioPtr(audio, deleter);
            audioMixCache_[path] = audioPtr;
            return audioPtr;
        }
    }
    LogCat::w("Audio not found in any enabled resource pack: ", path);
    return nullptr;
}

glimmer::ResourcePackManager::ResourcePackManager(VirtualFileSystem *virtualFilesystem) : virtualFileSystem_(
        virtualFilesystem), renderer_(nullptr),
    mixer_(nullptr) {
}

void glimmer::ResourcePackManager::SetMixer(MIX_Mixer *mixer) {
    mixer_ = mixer;
}

void glimmer::ResourcePackManager::SetRenderer(SDL_Renderer *renderer, Color accent, Color base) {
    renderer_ = renderer;
    errorTexture_ = CreateErrorTexture(accent, base);
}

int glimmer::ResourcePackManager::Scan(const std::string &path, const std::vector<std::string> &enabledResourcePack,
                                       const toml::spec &tomlVersion) {
    resourcePackMap_.clear();
    if (!virtualFileSystem_->Exists(path)) {
        LogCat::e("ResourcePackManager: Path does not exist -> ", path);
        return 0;
    }

    LogCat::i("Scanning resources packs in: ", path);
    int success = 0;
    std::vector<std::string> files = virtualFileSystem_->ListFile(path, false);
    for (const auto &entry: files) {
        if (!virtualFileSystem_->IsFile(entry)) {
            LogCat::d("Found resource pack folder: ", entry);
            auto packPtr = std::make_unique<ResourcePack>(entry, virtualFileSystem_, tomlVersion);
            if (!packPtr->loadManifest()) {
                continue;
            }
            if (!IsResourcePackEnabled(*packPtr, enabledResourcePack)) {
                LogCat::w("Resource pack not enabled: ", packPtr->getManifest().id);
                continue;
            }
            if (!IsResourcePackAvailable(*packPtr)) {
                continue;
            }
            resourcePackMap_[packPtr->getManifest().id] = std::move(packPtr);
            success++;
        }
    }
    return success;
}

std::optional<std::string> glimmer::ResourcePackManager::GetFontPath(
    const std::vector<std::string> &enabledResourcePack,
    const std::string &language) {
    std::optional<std::string> defaultFontPath;

    for (const auto &packId: enabledResourcePack) {
        auto it = resourcePackMap_.find(packId);
        if (it == resourcePackMap_.end()) {
            LogCat::w("Resource pack not found: ", packId);
            continue;
        }

        const ResourcePack *pack = it->second.get();

        std::string fontsDir = pack->getPath() + "/fonts/";

        // First, check language.ttf
        // 优先查 language.ttf
        std::string langFont = fontsDir + language + ".ttf";
        if (virtualFileSystem_->Exists(langFont)) {
            LogCat::d("Found font for language '", language, "' in pack '", packId,
                      "': ", langFont);
            return langFont;
        }

        // Record the first default.ttf (for deferred use)
        // 记录第一个 default.ttf（延后使用）
        if (!defaultFontPath.has_value()) {
            std::string defaultFont = fontsDir + "default.ttf";
            if (virtualFileSystem_->Exists(defaultFont)) {
                LogCat::d("Found default font in pack '", packId, "': ", defaultFont);
                defaultFontPath = defaultFont;
            }
        }
    }

    if (defaultFontPath.has_value()) {
        LogCat::d("Using default font: ", *defaultFontPath);
        return defaultFontPath;
    }

    LogCat::e("No font found for language '", language, "' in any enabled resource pack");
    return std::nullopt;
}

std::shared_ptr<SDL_Texture> glimmer::ResourcePackManager::LoadTextureFromFile(AppContext *appContext,
                                                                               const ResourceRef &resourceRef) {
    if (resourceRef.GetPackageId() == RESOURCE_REF_CORE && resourceRef.GetResourceKey() == ERROR_TEXTURE_KEY) {
        return errorTexture_;
    }
    std::string path = resourceRef.GetPackageId() + "/" + resourceRef.GetResourceKey();
    return appContext->AddMainThreadTaskAwait(
        [this,appContext, path] {
            auto result = ImplLoadTextureFromFile(path, appContext->GetConfig()->mods);
            if (result == nullptr) {
                result = errorTexture_;
                errorTexturePathSet_.insert(path);
                textureCache_[path] = errorTexture_;
            }
            return result;
        }
    ).get();
}

std::shared_ptr<MIX_Audio> glimmer::ResourcePackManager::LoadAudioFromFile(AppContext *appContext,
                                                                           const ResourceRef &resourceRef) {
    std::string path = resourceRef.GetPackageId() + "/" + resourceRef.GetResourceKey();
    return appContext->AddMainThreadTaskAwait(
        [this,appContext, path] {
            return ImplLoadAudioFromFile(path, appContext->GetConfig()->mods);
        }
    ).get();
}

glimmer::ColorResource *glimmer::ResourcePackManager::LoadColorResFromFile(const AppContext *appContext,
                                                                           const ResourceRef &resourceRef) {
    std::string path = resourceRef.GetPackageId() + "/" + resourceRef.GetResourceKey();
    const auto cacheIt = colorCache_.find(path);
    if (cacheIt != colorCache_.end()) {
        return cacheIt->second.get();
    }
    for (const auto &packId: appContext->GetConfig()->mods.enabledResourcePack) {
        auto it = resourcePackMap_.find(packId);
        if (it == resourcePackMap_.end()) {
            LogCat::w("Resource pack not found: ", packId);
            continue;
        }

        const ResourcePack *pack = it->second.get();
        std::string colorPath = pack->getPath() + "/colors/" + path + "." + DATA_FILE_TYPE_COLOR + ".toml";

        if (!virtualFileSystem_->Exists(colorPath)) {
            continue;
        }
        auto data =
                virtualFileSystem_->ReadFile(colorPath);
        if (!data.has_value()) {
            LogCat::e("Failed to load toml file: ", colorPath);
            continue;
        }

        const toml::value value = toml::parse_str(data.value(), appContext->GetTomlVersion());
        colorCache_[path] = std::make_unique<ColorResource>(
            toml::get<ColorResource>(value)
        );
        return colorCache_[path].get();
    }
    return nullptr;
}

std::shared_ptr<SDL_Texture> glimmer::ResourcePackManager::CreateErrorTexture(const Color accent, const Color base) const {
    if (renderer_ == nullptr) {
        return nullptr;
    }

    SDL_Surface *surface =
            SDL_CreateSurface(TILE_SIZE, TILE_SIZE, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        LogCat::e("SDL_CreateSurface failed: ", SDL_GetError());
        return nullptr;
    }

    const Uint32 accentValue = SDL_MapSurfaceRGBA(surface, accent.r, accent.g, accent.b, accent.a);
    const Uint32 baseValue = SDL_MapSurfaceRGBA(surface, base.r, base.g, base.b, base.a);

    for (int y = 0; y < TILE_SIZE; ++y) {
        for (int x = 0; x < TILE_SIZE; ++x) {
            const bool isAccentColor =
                    (x < TILE_SIZE / 2 && y < TILE_SIZE / 2) ||
                    (x >= TILE_SIZE / 2 && y >= TILE_SIZE / 2);
            const Uint32 color = isAccentColor ? accentValue : baseValue;
            Uint8 *pixel =
                    static_cast<Uint8 *>(surface->pixels)
                    + y * surface->pitch
                    + x * 4;
            *reinterpret_cast<Uint32 *>(pixel) = color;
        }
    }
    SDL_Texture *texture =
            SDL_CreateTextureFromSurface(renderer_, surface);
    auto deleter = [](SDL_Texture *tex) {
        LogCat::d("Destroying error texture");
        if (tex == nullptr) {
            SDL_DestroyTexture(tex);
        }
    };
    SDL_DestroySurface(surface);
    return std::shared_ptr<SDL_Texture>(texture, deleter);
}

std::string glimmer::ResourcePackManager::ListTextureCache() const {
    std::ostringstream oss;
    for (const auto &pair: textureCache_) {
        const auto &path = pair.first;
        const auto &weakTex = pair.second;
        if (auto shared = weakTex.lock()) {
            oss << path
                    << " -> "
                    << "use_count="
                    << shared.use_count() - 1;

            if (errorTexturePathSet_.contains(path)) {
                oss << " (error texture)";
            }
        }
        oss << "\n";
    }
    return oss.str();
}
