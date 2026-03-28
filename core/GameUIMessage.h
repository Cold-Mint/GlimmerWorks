//
// Created by Cold-Mint on 2026/2/19.
//

#ifndef GLIMMERWORKS_MESSAGE_H
#define GLIMMERWORKS_MESSAGE_H
#include <cstdint>
#include <string>

#include "tweeny.h"

namespace glimmer {
    struct GameUIMessage {
        std::string text;

        uint64_t createTime;
        uint64_t expireTime;

        float alpha = 0.0F;

        tweeny::tween<float> tween;

        GameUIMessage(const std::string &t, uint64_t now);
    };
}


#endif //GLIMMERWORKS_MESSAGE_H
