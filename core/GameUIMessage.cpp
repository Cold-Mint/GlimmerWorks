//
// Created by Cold-Mint on 2026/2/19.
//

#include "GameUIMessage.h"

glimmer::GameUIMessage::GameUIMessage(const std::string &t, uint64_t now) : text(t),
                                                                            createTime(now),
                                                                            expireTime(now + 2500),
                                                                            tween(tweeny::from(0.0f)
                                                                                .to(1.0f).during(200)
                                                                                .to(1.0f).during(2000)
                                                                                .to(0.0f).during(300)) {
}
