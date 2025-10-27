//
// Created by Cold-Mint on 2025/10/15.
//

#include "ResourcePackManager.h"

#include <filesystem>
#include <regex>
#include "ResourcePack.h"
#include "../../log/LogCat.h"
#include "SDL3_image/SDL_image.h"
namespace fs = std::filesystem;


bool glimmer::ResourcePackManager::isResourcePackAvailable(const ResourcePack &pack) {
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

bool glimmer::ResourcePackManager::isResourcePackEnabled(const ResourcePack &pack,
                                                         const std::vector<std::string> &enabledResourcePack) {
    return std::ranges::find(enabledResourcePack, pack.getManifest().id) != enabledResourcePack.end();
}

int glimmer::ResourcePackManager::scan(const std::string &path, const std::vector<std::string> &enabledResourcePack) {
    resourcePackMap.clear();
    try {
        if (!fs::exists(path)) {
            LogCat::e("ResourcePackManager: Path does not exist -> ", path);
            return 0;
        }

        LogCat::i("Scanning resources packs in: ", path);
        int success = 0;
        for (const auto &entry: fs::directory_iterator(path)) {
            if (entry.is_directory()) {
                const std::string folderPath = entry.path().string();
                LogCat::d("Found resource pack folder: ", folderPath);
                auto packPtr = std::make_unique<ResourcePack>(folderPath);
                if (!packPtr->loadManifest()) {
                    continue;
                }
                if (!isResourcePackEnabled(*packPtr, enabledResourcePack)) {
                    LogCat::w("Resource pack not enabled: ", packPtr->getManifest().id);
                    continue;
                }
                if (!isResourcePackAvailable(*packPtr)) {
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

std::optional<std::string> glimmer::ResourcePackManager::getFontPath(
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
        fs::path fontsDir = fs::path(pack->getPath().data()) / "fonts";

        // First, check language.ttf
        // 优先查 language.ttf
        fs::path langFont = fontsDir / (language + ".ttf");
        if (fs::exists(langFont)) {
            LogCat::d("Found font for language '", language, "' in pack '", packId,
                      "': ", langFont.string());
            return langFont.string();
        }

        // Record the first default.ttf (for deferred use)
        // 记录第一个 default.ttf（延后使用）
        if (!defaultFontPath.has_value()) {
            fs::path defaultFont = fontsDir / "default.ttf";
            if (fs::exists(defaultFont)) {
                LogCat::d("Found default font in pack '", packId, "': ", defaultFont.string());
                defaultFontPath = defaultFont.string();
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

std::shared_ptr<SDL_Texture> glimmer::ResourcePackManager::loadTextureFromFile(
    const std::vector<std::string> &enabledResourcePack, SDL_Renderer &renderer, const std::string &path) {
    namespace fs = std::filesystem;

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
        fs::path baseTexturesDir = fs::path(pack->getPath()) / "textures";
        fs::path texturePath = baseTexturesDir / path;

        fs::path canonicalBase;
        fs::path canonicalTarget;

        try {
            canonicalBase = fs::weakly_canonical(baseTexturesDir);
            canonicalTarget = fs::weakly_canonical(texturePath);
        } catch (const fs::filesystem_error &e) {
            LogCat::w("Invalid texture path in pack '", packId, "': ", e.what());
            continue;
        }

        if (canonicalTarget.string().find(canonicalBase.string()) != 0) {
            LogCat::w("Texture path escapes textures dir in pack '", packId,
                      "': ", canonicalTarget.string());
            continue;
        }

        if (!fs::exists(canonicalTarget)) {
            // The current resource package does not have this texture. Check the next one
            // 当前资源包无此纹理，查下一个
            continue;
        }


        SDL_Surface *surface = IMG_Load(canonicalTarget.string().c_str());
        if (surface == nullptr) {
            LogCat::w("IMG_Load failed for ", canonicalTarget.string(), ": ", SDL_GetError());
            continue;
        }

        SDL_Texture *texture = SDL_CreateTextureFromSurface(&renderer, surface);
        SDL_DestroySurface(surface);
        if (!texture) {
            LogCat::w("SDL_CreateTextureFromSurface failed for ", canonicalTarget.string(),
                      ": ", SDL_GetError());
            continue;
        }

        LogCat::d("Loaded texture from pack '", packId, "': ", canonicalTarget.string());

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
