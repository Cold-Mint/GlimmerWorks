//
// Created by Cold-Mint on 2025/10/19.
//

#ifndef GLIMMERWORKS_MAPMANIFEST_H
#define GLIMMERWORKS_MAPMANIFEST_H
#include <string>

#include "saves/map_manifest.pb.h"

namespace glimmer {
    struct MapManifest {
        std::string name;
        std::string gameVersionName;
        uint32_t gameVersionNumber;
        int seed;

        void FromMessage(const MapManifestMessage &manifestMessage) {
            name = manifestMessage.name();
            gameVersionName = manifestMessage.gameversionname();
            gameVersionNumber = manifestMessage.gameversionnumber();
            seed = manifestMessage.seed();
        }

        void ToMessage(MapManifestMessage &manifestMessage) {
            manifestMessage.set_name(name);
            manifestMessage.set_gameversionname(gameVersionName);
            manifestMessage.set_gameversionnumber(gameVersionNumber);
            manifestMessage.set_seed(seed);
        }
    };
}

#endif //GLIMMERWORKS_MAPMANIFEST_H
