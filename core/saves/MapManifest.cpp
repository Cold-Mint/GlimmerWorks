//
// Created by Cold-Mint on 2025/10/19.
//

#include "MapManifest.h"

void glimmer::MapManifest::FromMessage(const MapManifestMessage &manifestMessage) {
    name = manifestMessage.name();
    gameVersionName = manifestMessage.gameversionname();
    gameVersionNumber = manifestMessage.gameversionnumber();
    seed = manifestMessage.seed();
    createTime = manifestMessage.createtime();
    lastPlayedTime = manifestMessage.lastplayedtime();
    totalPlayTime = manifestMessage.totalplaytime();
    entityIDIndex = manifestMessage.entityidindex();
}

void glimmer::MapManifest::ToMessage(MapManifestMessage &manifestMessage) {
    manifestMessage.set_name(name);
    manifestMessage.set_gameversionname(gameVersionName);
    manifestMessage.set_gameversionnumber(gameVersionNumber);
    manifestMessage.set_seed(seed);
    manifestMessage.set_createtime(createTime);
    manifestMessage.set_lastplayedtime(lastPlayedTime);
    manifestMessage.set_totalplaytime(totalPlayTime);
    manifestMessage.set_entityidindex(entityIDIndex);
}
