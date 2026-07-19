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
#include "ResourcePackManager.h"

#include <regex>
#include <algorithm>

#include "AudioResourceResult.h"
#include "ResourcePack.h"
#include "TextureResourceResult.h"
#include "core/Config.h"
#include "core/context/AppContext.h"
#include "core/log/LogCat.h"
#include "core/scene/MainThreadDispatcher.h"
#include "core/utils/StringUtils.h"
#include "SDL3_image/SDL_image.h"
#include "toml11/get.hpp"
#include "toml11/parser.hpp"
#include "toml11/spec.hpp"
#include "core/utils/TomlUtils.h"
#include "SDL3/SDL_render.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "core/world/PreloadColors.h"


bool glimmer::ResourcePackManager::IsResourcePackAvailable(const ResourcePack& pack) const
{
    const PackManifest& manifest = pack.GetManifest();
    if (manifest.id == RESOURCE_REF_CORE || manifest.id == RESOURCE_REF_SELF)
    {
        return false;
    }
    if (resourcePackMap_.contains(manifest.id))
    {
        return false;
    }
    if (manifest.minGameVersion > GAME_VERSION_NUMBER)
    {
        return false;
    }

    return manifest.resPack;
}

bool glimmer::ResourcePackManager::IsResourcePackEnabled(const ResourcePack& pack,
                                                         const std::vector<std::string>& enabledResourcePack)
{
    return std::ranges::find(enabledResourcePack, pack.GetManifest().id) != enabledResourcePack.end();
}

std::shared_ptr<glimmer::TextureResourceResult> glimmer::ResourcePackManager::CreateTextureResult(
    SDL_Texture* texture,
    const ResourcePack* resourcePack,
    const std::string& path)
{
    auto textureResourceResult = std::make_unique<TextureResourceResult>();
    textureResourceResult->SetResourcePack(resourcePack);
    textureResourceResult->SetResource(texture);
    auto deleter = [this, path](TextureResourceResult* textureResourceResult)
    {
        if (textureResourceResult != nullptr)
        {
            textureResourceResult->DestroyResource();
        }
        textureCache_.erase(path);
    };
    std::shared_ptr<TextureResourceResult> textureSharedPtr(textureResourceResult.release(), deleter);
    textureCache_[path] = textureSharedPtr;
    return textureSharedPtr;
}

std::shared_ptr<glimmer::TextureResourceResult> glimmer::ResourcePackManager::TryLoadTextureFromPack(
    const std::string& path,
    const ResourcePack* resourcePack,
    const std::vector<std::string>& supportedFormats)
{
    for (auto& supportedTextureFormat : supportedFormats)
    {
        std::filesystem::path texturePath = resourcePack->GetPath() / "textures" / path;
        texturePath.replace_extension(supportedTextureFormat);
        if (!virtualFileSystem_->Exists(texturePath))
        {
            continue;
        }

        auto actualTexturePath = virtualFileSystem_->GetActualPath(texturePath);
        if (!actualTexturePath.has_value())
        {
            continue;
        }

        SDL_Surface* surface = IMG_Load(actualTexturePath.value().string().c_str());
        if (surface == nullptr)
        {
            continue;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
        SDL_DestroySurface(surface);
        if (texture == nullptr)
        {
            continue;
        }
        return CreateTextureResult(texture, resourcePack, path);
    }
    return nullptr;
}

std::shared_ptr<glimmer::TextureResourceResult> glimmer::ResourcePackManager::ImplLoadTextureFromFile(
    const std::string& path,
    const Mods& modConfig)
{
    if (renderer_ == nullptr || path.empty())
    {
        return nullptr;
    }
    const auto cache = textureCache_.find(path);
    if (cache != textureCache_.end())
    {
        if (auto cacheTexture = cache->second.lock())
        {
            return cacheTexture;
        }
        //Cache has expired.
        //缓存过期。
        textureCache_.erase(cache);
    }

    for (const auto& packId : modConfig.enabledResourcePack)
    {
        auto resourcePackIterator = resourcePackMap_.find(packId);
        if (resourcePackIterator == resourcePackMap_.end())
        {
            continue;
        }
        const ResourcePack* resourcePack = resourcePackIterator->second.get();
        if (resourcePack == nullptr)
        {
            continue;
        }
        auto result = TryLoadTextureFromPack(path, resourcePack, modConfig.supportedTextureFormats);
        if (result != nullptr)
        {
            return result;
        }
    }
    return nullptr;
}

std::shared_ptr<glimmer::AudioResourceResult> glimmer::ResourcePackManager::ImplLoadAudioFromFile(
    const std::string& path,
    const Mods& modConfig)
{
    if (mixer_ == nullptr || path.empty())
    {
        return nullptr;
    }
    const auto cacheIt = audioMixCache_.find(path);
    if (cacheIt != audioMixCache_.end())
    {
        if (auto tex = cacheIt->second.lock())
        {
            return tex;
        }
    }

    for (const auto& packId : modConfig.enabledResourcePack)
    {
        auto it = resourcePackMap_.find(packId);
        if (it == resourcePackMap_.end())
        {
            continue;
        }

        const ResourcePack* pack = it->second.get();
        for (auto& supportedAudioFormat : modConfig.supportedAudioFormats)
        {
            std::filesystem::path audioPath = pack->GetPath() / "audios" / path;
            audioPath.replace_extension(supportedAudioFormat);
            if (!virtualFileSystem_->Exists(audioPath))
            {
                continue;
            }

            auto actualAudioPath = virtualFileSystem_->GetActualPath(audioPath);
            if (!actualAudioPath.has_value())
            {
                continue;
            }
            MIX_Audio* audio = MIX_LoadAudio(mixer_, actualAudioPath.value().c_str(), false);
            if (audio == nullptr)
            {
                continue;
            }


            auto audioResourceResult = std::make_unique<AudioResourceResult>();
            audioResourceResult->SetResourcePack(pack);
            audioResourceResult->SetResource(audio);
            auto deleter = [this, path](AudioResourceResult* audioResourceResult)
            {
                if (audioResourceResult != nullptr)
                {
                    audioResourceResult->DestroyResource();
                }
                audioMixCache_.erase(path);
            };
            std::shared_ptr<AudioResourceResult> audioSharedPtr(audioResourceResult.release(), deleter);
            audioMixCache_[path] = audioSharedPtr;
            return audioSharedPtr;
        }
    }

    return nullptr;
}

std::shared_ptr<glimmer::TextureResourceResult> glimmer::ResourcePackManager::LoadTextureFromFile(
    const AppContext* appContext, const ResourceRef* resourceRef)
{
    if (resourceRef->GetPackageId() == RESOURCE_REF_CORE)
    {
        const std::string resourceKey = resourceRef->GetResourceKey();
        if (resourceKey == ERROR_TEXTURE_KEY)
        {
            return errorTexture_;
        }
        if (resourceKey == ACCESS_DENIED_TEXTURE_KEY)
        {
            return accessDeniedTexture_;
        }
    }
    std::string path = resourceRef->GetPackageId() + "/" + resourceRef->GetResourceKey();
    MainThreadDispatcher* mainThreadDispatcher = appContext->GetMainThreadDispatcher();
    if (mainThreadDispatcher == nullptr)
    {
        return nullptr;
    }
    return mainThreadDispatcher->AddMainThreadTaskAwait(
        [this,appContext, path]
        {
            return ImplLoadTextureFromFile(path, appContext->GetConfig()->mods);
        }
    ).get();
}

glimmer::ResourcePackManager::ResourcePackManager(VirtualFileSystem* virtualFilesystem) : virtualFileSystem_(
    virtualFilesystem)
{
}

void glimmer::ResourcePackManager::SetMixer(MIX_Mixer* mixer)
{
    mixer_ = mixer;
}

void glimmer::ResourcePackManager::SetRenderer(SDL_Renderer* renderer, const PreloadColors* preloadColors)
{
    renderer_ = renderer;
    if (preloadColors == nullptr)
    {
        return;
    }
    errorTexture_ = CreateTexture(preloadColors->error.accentColor, preloadColors->error.baseColor);
    accessDeniedTexture_ = CreateTexture(preloadColors->accessDenied.accentColor,
                                         preloadColors->accessDenied.baseColor);
}

int glimmer::ResourcePackManager::Scan(const std::string& resourcePackPathString,
                                       const std::vector<std::string>& enabledResourcePack,
                                       const toml::spec& tomlVersion)
{
    resourcePackMap_.clear();
    const std::filesystem::path& resourcePackPath = resourcePackPathString;
    if (!virtualFileSystem_->Exists(resourcePackPath))
    {
        return 0;
    }


    int success = 0;
    std::vector<std::filesystem::path> files = virtualFileSystem_->ListFile(resourcePackPath, false);
    for (const auto& entry : files)
    {
        if (!virtualFileSystem_->IsFile(entry))
        {
            auto packPtr = std::make_unique<ResourcePack>(entry, virtualFileSystem_, tomlVersion);
            if (!packPtr->LoadManifest())
            {
                continue;
            }
            if (!IsResourcePackEnabled(*packPtr, enabledResourcePack))
            {
                continue;
            }
            if (!IsResourcePackAvailable(*packPtr))
            {
                continue;
            }
            packPtr->LoadResourceConfig();
            resourcePackMap_[packPtr->GetManifest().id] = std::move(packPtr);
            success++;
        }
    }
    return success;
}

std::optional<std::string> glimmer::ResourcePackManager::GetFontPath(
    const std::vector<std::string>& enabledResourcePack,
    const std::string& language)
{
    std::optional<std::string> defaultFontPath;

    for (const auto& packId : enabledResourcePack)
    {
        auto it = resourcePackMap_.find(packId);
        if (it == resourcePackMap_.end())
        {
            continue;
        }

        const ResourcePack* pack = it->second.get();

        std::filesystem::path fontsDir = pack->GetPath() / "fonts";

        // First, check language.ttf
        // 优先查 language.ttf
        std::filesystem::path langFont = fontsDir / language;
        langFont.replace_extension("ttf");
        if (virtualFileSystem_->Exists(langFont))
        {
            return langFont.string();
        }

        // Record the first default.ttf (for deferred use)
        // 记录第一个 default.ttf（延后使用）
        if (!defaultFontPath.has_value())
        {
            std::filesystem::path defaultFont = fontsDir / "default";
            defaultFont.replace_extension("ttf");
            if (virtualFileSystem_->Exists(defaultFont))
            {
                defaultFontPath = defaultFont.string();
            }
        }
    }

    if (defaultFontPath.has_value())
    {
        return defaultFontPath;
    }


    return std::nullopt;
}

std::unique_ptr<glimmer::RmlResourceResult> glimmer::ResourcePackManager::GetRmlFilePath(const AppContext* appContext,
    const ResourceRef* resourceRef)
{
    if (resourceRef == nullptr)
    {
        LogCat::w(std::source_location::current(), "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_RML_PATH)
    {
        LogCat::w(std::source_location::current(), "resourceRef->GetResourceType() != RESOURCE_RML_PATH");
        return nullptr;
    }
    for (const auto& packId : appContext->GetConfig()->mods.enabledResourcePack)
    {
        auto it = resourcePackMap_.find(packId);
        if (it == resourcePackMap_.end())
        {
            continue;
        }

        const ResourcePack* pack = it->second.get();
        std::filesystem::path rmlPath = pack->GetPath() / "layouts" / resourceRef->GetPackageId() / resourceRef->
            GetResourceKey();
        rmlPath.replace_extension("rml");
        if (!virtualFileSystem_->Exists(rmlPath))
        {
            continue;
        }
        auto resourceResult = std::make_unique<RmlResourceResult>();
        resourceResult->SetPath(rmlPath);
        return resourceResult;
    }
    return nullptr;
}


std::shared_ptr<SDL_Texture> glimmer::ResourcePackManager::CreateStringTexture(const std::string& string,
                                                                               const Color* color, int wrapWidth)
{
    if (renderer_ == nullptr || string.empty() || color == nullptr)
    {
        return nullptr;
    }
    uint64_t stringFingerprint = StringUtils::StringToUint64(string);
    uint64_t colorFingerprint = color->GetFingerprint();
    uint64_t totalFingerprint = stringFingerprint;
    totalFingerprint ^= colorFingerprint + 0x9e3779b97f4a7c15ULL + (totalFingerprint << 6) + (totalFingerprint >> 2);
    const auto cache = stringTextureCache_.find(totalFingerprint);
    if (cache != stringTextureCache_.end())
    {
        if (auto cacheTexture = cache->second.lock())
        {
            return cacheTexture;
        }
        //Cache has expired.
        //缓存过期。
        stringTextureCache_.erase(cache);
    }

    SDL_Surface* surface =
        TTF_RenderText_Blended_Wrapped(font_, string.c_str(), string.length(), color->ToSDLColor(), wrapWidth);
    if (surface == nullptr)
    {
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    SDL_DestroySurface(surface);
    if (texture == nullptr)
    {
        return nullptr;
    }
    auto deleter = [this,totalFingerprint](SDL_Texture* sdlTexture)
    {
        if (sdlTexture != nullptr)
        {
            SDL_DestroyTexture(sdlTexture);
        }
        stringTextureCache_.erase(totalFingerprint);
    };
    std::shared_ptr<SDL_Texture> texturePtr(texture, std::move(deleter));
    stringTextureCache_[totalFingerprint] = texturePtr;
    return texturePtr;
}


std::shared_ptr<glimmer::AudioResourceResult> glimmer::ResourcePackManager::LoadAudioFromFile(
    const AppContext* appContext,
    const ResourceRef* resourceRef)
{
    MainThreadDispatcher* mainThreadDispatcher = appContext->GetMainThreadDispatcher();
    if (mainThreadDispatcher == nullptr)
    {
        return nullptr;
    }
    std::string path = resourceRef->GetPackageId() + "/" + resourceRef->GetResourceKey();
    return mainThreadDispatcher->AddMainThreadTaskAwait(
        [this,appContext, path]
        {
            return ImplLoadAudioFromFile(path, appContext->GetConfig()->mods);
        }
    ).get();
}

glimmer::ColorResource* glimmer::ResourcePackManager::LoadColorResFromFile(const AppContext* appContext,
                                                                           const ResourceRef* resourceRef)
{
    uint64_t fingPrint = resourceRef->GetFingerprint();
    const auto cacheIt = colorCache_.find(fingPrint);
    if (cacheIt != colorCache_.end())
    {
        return cacheIt->second.get();
    }
    for (const auto& packId : appContext->GetConfig()->mods.enabledResourcePack)
    {
        auto it = resourcePackMap_.find(packId);
        if (it == resourcePackMap_.end())
        {
            continue;
        }

        const ResourcePack* pack = it->second.get();
        std::filesystem::path colorPath = pack->GetPath() / "colors" / resourceRef->GetPackageId();
        colorPath.replace_filename(resourceRef->GetResourceKey() + "." + DATA_FILE_TYPE_COLOR);
        colorPath.replace_extension("toml");
        if (!virtualFileSystem_->Exists(colorPath))
        {
            continue;
        }
        auto data =
            virtualFileSystem_->ReadFileAsString(colorPath);
        if (!data.has_value())
        {
            continue;
        }

        const toml::value value = toml::parse_str(data.value(), appContext->GetTomlVersion());
        colorCache_[fingPrint] = std::make_unique<ColorResource>(
            toml::get<ColorResource>(value)
        );
        return colorCache_[fingPrint].get();
    }
    return nullptr;
}

std::shared_ptr<glimmer::TextureResourceResult> glimmer::ResourcePackManager::CreateTexture(const Color& accent,
    const Color& base) const
{
    if (renderer_ == nullptr)
    {
        return nullptr;
    }

    SDL_Surface* surface =
        SDL_CreateSurface(TILE_SIZE, TILE_SIZE, SDL_PIXELFORMAT_RGBA32);
    if (!surface)
    {
        return nullptr;
    }

    const Uint32 accentValue = SDL_MapSurfaceRGBA(surface, accent.r, accent.g, accent.b, accent.a);
    const Uint32 baseValue = SDL_MapSurfaceRGBA(surface, base.r, base.g, base.b, base.a);

    for (int y = 0; y < TILE_SIZE; ++y)
    {
        for (int x = 0; x < TILE_SIZE; ++x)
        {
            const bool isAccentColor =
                (x < TILE_SIZE / 2 && y < TILE_SIZE / 2) ||
                (x >= TILE_SIZE / 2 && y >= TILE_SIZE / 2);
            const Uint32 color = isAccentColor ? accentValue : baseValue;
            Uint8* pixel =
                static_cast<Uint8*>(surface->pixels)
                + y * surface->pitch
                + x * 4;
            *reinterpret_cast<Uint32*>(pixel) = color;
        }
    }
    SDL_Texture* texture =
        SDL_CreateTextureFromSurface(renderer_, surface);
    SDL_DestroySurface(surface);
    auto textureResourceResult = std::make_unique<TextureResourceResult>();
    textureResourceResult->SetResource(texture);
    auto deleter = [](TextureResourceResult* textureResourceResult)
    {
        if (textureResourceResult == nullptr)
        {
            return;
        }
        textureResourceResult->DestroyResource();
    };
    return {textureResourceResult.release(), deleter};
}

std::string glimmer::ResourcePackManager::ListTextureCache() const
{
    std::ostringstream oss;
    for (const auto& [path, weakTex] : textureCache_)
    {
        if (auto shared = weakTex.lock())
        {
            oss << path
                << " -> "
                << "use_count="
                << shared.use_count() - 1;

            if (errorTexturePathSet_.contains(path))
            {
                oss << " (error texture)";
            }
        }
        oss << "\n";
    }
    return oss.str();
}

void glimmer::ResourcePackManager::SetFont(TTF_Font* font)
{
    font_ = font;
}

glimmer::ResourcePackManager::~ResourcePackManager()
{
    stringTextureCache_.clear();
}
