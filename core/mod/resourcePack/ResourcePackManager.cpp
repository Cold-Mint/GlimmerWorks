//
// Created by Cold-Mint on 2025/10/15.
//

#include "ResourcePackManager.h"

#include <filesystem>
#include <regex>
#include <algorithm>
#include "ResourcePack.h"
#include "../../Config.h"
#include "../../log/LogCat.h"
#include "../../scene/AppContext.h"
#include "SDL3_image/SDL_image.h"

bool glimmer::ResourcePackManager::IsResourcePackAvailable(const ResourcePack &pack) const {
    const PackManifest &manifest = pack.getManifest();
    if (manifest.id == RESOURCE_REF_CORE || manifest.id == RESOURCE_REF_SELF) {
        return false;
    }
    if (resourcePackMap.contains(manifest.id)) {
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

std::shared_ptr<SDL_Texture> glimmer::ResourcePackManager::ImplLoadTextureFromFile(
    const std::vector<std::string> &enabledResourcePack, const std::string &path) {
    if (renderer_ == nullptr) {
        return nullptr;
    }
    if (path.empty()) {
        LogCat::e("Invalid texture path (empty).");
        return nullptr;
    }

    const auto cacheIt = textureCache.find(path);
    if (cacheIt != textureCache.end()) {
        if (auto tex = cacheIt->second.lock()) {
            LogCat::d("Texture loaded from cache: ", path);
            return tex;
        }
        LogCat::w("Cached texture expired, reloading: ", path);
    }

    for (const auto &packId: enabledResourcePack) {
        auto it = resourcePackMap.find(packId);
        if (it == resourcePackMap.end()) {
            LogCat::w("Resource pack not found: ", packId);
            continue;
        }

        const ResourcePack *pack = it->second.get();
        std::string texturePath = pack->getPath() + "/textures/" + path;

        if (!virtualFileSystem_->Exists(texturePath)) {
            continue;
        }

        auto actualTexturePath = virtualFileSystem_->GetActualPath(texturePath);
        if (!actualTexturePath.has_value()) {
            continue;
        }

        SDL_Surface *surface = IMG_Load(actualTexturePath.value().c_str());
        if (surface == nullptr) {
            LogCat::w("IMG_Load failed for ", texturePath, ": ", SDL_GetError());
            continue;
        }

        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer_, surface);
        SDL_DestroySurface(surface);
        if (!texture) {
            LogCat::w("SDL_CreateTextureFromSurface failed for ", texturePath,
                      ": ", SDL_GetError());
            continue;
        }

        LogCat::d("Loaded texture from pack '", packId, "': ", texturePath);

        auto deleter = [path](SDL_Texture *tex) {
            LogCat::d("Destroying texture from cache: ", path);
            if (tex) SDL_DestroyTexture(tex);
        };

        std::shared_ptr<SDL_Texture> texturePtr(texture, deleter);
        textureCache[path] = texturePtr; // 自动转换为 weak_ptr
        LogCat::d("add use_count=", texturePtr.use_count());
        return texturePtr;
    }
    LogCat::w("Texture not found in any enabled resource pack: ", path);
    return nullptr;
}

std::shared_ptr<SDL_Texture>
glimmer::ResourcePackManager::CreateErrorTexture() const {
    if (renderer_ == nullptr) {
        return nullptr;
    }

    SDL_Surface *surface =
            SDL_CreateSurface(TILE_SIZE, TILE_SIZE, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        LogCat::e("SDL_CreateSurface failed: ", SDL_GetError());
        return nullptr;
    }

    Uint32 purple = SDL_MapSurfaceRGBA(surface, 255, 0, 255, 255);
    Uint32 black = SDL_MapSurfaceRGBA(surface, 0, 0, 0, 255);

    for (int y = 0; y < TILE_SIZE; ++y) {
        for (int x = 0; x < TILE_SIZE; ++x) {
            const bool isPurple =
                    (x < TILE_SIZE / 2 && y < TILE_SIZE / 2) ||
                    (x >= TILE_SIZE / 2 && y >= TILE_SIZE / 2);
            const Uint32 color = isPurple ? purple : black;
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

glimmer::ResourcePackManager::ResourcePackManager(VirtualFileSystem *virtualFilesystem) : virtualFileSystem_(
        virtualFilesystem), renderer_(nullptr) {
}

void glimmer::ResourcePackManager::SetRenderer(SDL_Renderer *renderer) {
    renderer_ = renderer;
    errorTexture_ = CreateErrorTexture();
}

int glimmer::ResourcePackManager::Scan(const std::string &path, const std::vector<std::string> &enabledResourcePack,
                                       const toml::spec &tomlVersion) {
    resourcePackMap.clear();
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
            resourcePackMap[packPtr->getManifest().id] = std::move(packPtr);
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
        auto it = resourcePackMap.find(packId);
        if (it == resourcePackMap.end()) {
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
            auto result = ImplLoadTextureFromFile(appContext->GetConfig()->mods.enabledResourcePack, path);
            if (result == nullptr) {
                result = errorTexture_;
                errorTexturePathSet_.insert(path);
                textureCache[path] = errorTexture_;
            }
            return result;
        }
    ).get();
}

std::string glimmer::ResourcePackManager::ListTextureCache(const bool includeExpired) const {
    std::string result;

    for (const auto &pair: textureCache) {
        const auto &path = pair.first;
        const auto &weakTex = pair.second;


        if (auto shared = weakTex.lock()) {
            result += path;
            result += " -> ";
            result += "alive (use_count=";
            result += std::to_string(shared.use_count());
            result += ")";
            if (errorTexturePathSet_.contains(path)) {
                result += " (error texture)";
            }
        } else {
            if (includeExpired) {
                result += path;
                result += " -> ";
                result += "expired";
            }
        }

        result += "\n";
    }

    return result;
}
