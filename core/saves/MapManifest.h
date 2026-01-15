//
// Created by Cold-Mint on 2025/10/19.
//

#ifndef GLIMMERWORKS_MAPMANIFEST_H
#define GLIMMERWORKS_MAPMANIFEST_H
#include <string>

#include "core/ecs/GameEntity.h"
#include "src/saves/map_manifest.pb.h"

namespace glimmer {
    struct MapManifest {
        std::string name;
        std::string gameVersionName;
        uint32_t gameVersionNumber;
        int seed;
        long createTime;
        long lastPlayedTime;
        long totalPlayTime;
        GameEntity::ID entityIDIndex;

        void FromMessage(const MapManifestMessage &manifestMessage) {
            name = manifestMessage.name();
            gameVersionName = manifestMessage.gameversionname();
            gameVersionNumber = manifestMessage.gameversionnumber();
            seed = manifestMessage.seed();
            createTime = manifestMessage.createtime();
            lastPlayedTime = manifestMessage.lastplayedtime();
            totalPlayTime = manifestMessage.totalplaytime();
            entityIDIndex = manifestMessage.entityidindex();
        }

        void ToMessage(MapManifestMessage &manifestMessage) {
            manifestMessage.set_name(name);
            manifestMessage.set_gameversionname(gameVersionName);
            manifestMessage.set_gameversionnumber(gameVersionNumber);
            manifestMessage.set_seed(seed);
            manifestMessage.set_createtime(createTime);
            manifestMessage.set_lastplayedtime(lastPlayedTime);
            manifestMessage.set_totalplaytime(totalPlayTime);
            manifestMessage.set_entityidindex(entityIDIndex);
        }
    };
}

#endif //GLIMMERWORKS_MAPMANIFEST_H
