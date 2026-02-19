//
// Created by coldmint on 2026/2/19.
//

#ifndef GLIMMERWORKS_MESSAGE_H
#define GLIMMERWORKS_MESSAGE_H
#include <cstdint>
#include <string>

#include "../cmake-build-debug/_deps/tweeny-src/include/tweeny.h"

namespace glimmer {
    struct GameUIMessage {
        std::string text;

        uint64_t createTime;
        uint64_t expireTime;

        float alpha = 0.0f;

        tweeny::tween<float> tween;

        GameUIMessage(const std::string &t, const uint64_t now)
            : text(t),
              createTime(now),
              expireTime(now + 2500),
              tween(tweeny::from(0.0f)
                  .to(1.0f).during(200)
                  .to(1.0f).during(2000)
                  .to(0.0f).during(300)) {
        }
    };
}


#endif //GLIMMERWORKS_MESSAGE_H
