//
// Created by Cold-Mint on 2025/10/15.
//

#ifndef GLIMMERWORKS_RESOURCEPACKMANAGER_H
#define GLIMMERWORKS_RESOURCEPACKMANAGER_H
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "ResourcePack.h"
#include "SDL3/SDL_render.h"

namespace glimmer {
    class ResourcePackManager {
        std::unordered_map<std::string, std::unique_ptr<ResourcePack> > resourcePackMap;

        static bool IsResourcePackAvailable(const ResourcePack &pack);

        static bool IsResourcePackEnabled(const ResourcePack &pack,
                                          const std::vector<std::string> &enabledResourcePack);

        std::unordered_map<std::string, std::weak_ptr<SDL_Texture> > textureCache;

    public:
        int Scan(const std::string &path, const std::vector<std::string> &enabledResourcePack);

        std::optional<std::string> GetFontPath(const std::vector<std::string> &enabledResourcePack,
                                               const std::string &language);

        std::shared_ptr<SDL_Texture> LoadTextureFromFile(const std::vector<std::string> &enabledResourcePack,
                                                         SDL_Renderer &renderer,
                                                         const std::string &path);

        /**
         * ListTextureCache
         * 列出纹理缓存
         * @param includeExpired Whether to list the expired texture cache 是否列出过期的纹理缓存
         * @return
         */
        std::string ListTextureCache(bool includeExpired) const;
    };
}

#endif //GLIMMERWORKS_RESOURCEPACKMANAGER_H
