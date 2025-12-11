//
// Created by Cold-Mint on 2025/10/15.
//

#include "ResourcePackManager.h"

#include <filesystem>
#include <regex>
#include <algorithm>
#include "ResourcePack.h"
#include "../../log/LogCat.h"
#include "SDL3_image/SDL_image.h"

bool glimmer::ResourcePackManager::IsResourcePackAvailable(const ResourcePack &pack) {
    const PackManifest &manifest = pack.getManifest();
    static const std::regex uuidPattern(
        "^[0-9a-fA-F]{8}-"
        "[0-9a-fA-F]{4}-"
        "[0-9a-fA-F]{4}-"
        "[0-9a-fA-F]{4}-"
        "[0-9a-fA-F]{12}$"
    );

    if (!std::regex_match(manifest.id, uuidPattern)) {
        LogCat::e("Invalid ResourcePack id format: ", manifest.id);
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

void glimmer::ResourcePackManager::SetRenderer(SDL_Renderer *renderer) {
    renderer_ = renderer;
}

int glimmer::ResourcePackManager::Scan(const std::string &path, const std::vector<std::string> &enabledResourcePack) {
    resourcePackMap.clear();
    try {
        if (!virtualFileSystem_->Exists(path)) {
            LogCat::e("ResourcePackManager: Path does not exist -> ", path);
            return 0;
        }

        LogCat::i("Scanning resources packs in: ", path);
        int success = 0;
        std::vector<std::string> files = virtualFileSystem_->ListFile(path);
        for (const auto &entry: files) {
            if (!virtualFileSystem_->IsFile(entry)) {
                LogCat::d("Found resource pack folder: ", entry);
                auto packPtr = std::make_unique<ResourcePack>(entry, virtualFileSystem_);
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
    } catch (const std::exception &e) {
        LogCat::e("ResourcePackManager::scan failed: ", e.what());
        return 0;
    }
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

std::shared_ptr<SDL_Texture> glimmer::ResourcePackManager::LoadTextureFromFile(
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

    //Cache null values when loading fails (to prevent repeated attempts)
    //加载失败时缓存空值（防止重复尝试）
    textureCache[path] = std::weak_ptr<SDL_Texture>();

    LogCat::e("Texture not found in any enabled resource pack: ", path);
    return nullptr;
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
