//
// Created by coldmint on 2025/12/3.
//

#ifndef GLIMMERWORKS_TILEMANAGER_H
#define GLIMMERWORKS_TILEMANAGER_H
#include <string>
#include <unordered_map>

#include "../Resource.h"

namespace glimmer {
    class TileManager {
    protected:
        std::unordered_map<std::string, std::unordered_map<std::string, TileResource> > tileMap_{};

    public:
        void RegisterResource(TileResource &tileResource);

        TileResource *Find(const std::string &packId, const std::string &key);

        std::string ListTiles() const;
    };
}


#endif //GLIMMERWORKS_TILEMANAGER_H
