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

namespace Glimmer {
    class ResourcePackManager {
        std::unordered_map<std::string, std::unique_ptr<ResourcePack> > resourcePackMap;

        static bool isResourcePackAvailable(const ResourcePack &pack);

        static bool isResourcePackEnabled(const ResourcePack &pack,
                                          const std::vector<std::string> &enabledResourcePack);

    public:
        int scan(const std::string &path, const std::vector<std::string> &enabledResourcePack,
                 const std::string &language);

        std::optional<std::string> getFontPath(const std::vector<std::string> &enabledResourcePack,
                                               const std::string &language);
    };
}

#endif //GLIMMERWORKS_RESOURCEPACKMANAGER_H
