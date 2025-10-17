//
// Created by Cold-Mint on 2025/10/17.
//

#ifndef GLIMMERWORKS_HOMESCENE_H
#define GLIMMERWORKS_HOMESCENE_H
#include "Scene.h"


namespace Glimmer {
    class HomeScene : public Scene {
    public:
        explicit HomeScene(AppContext *context)
            : Scene(context) {
        }
    };
}

#endif //GLIMMERWORKS_HOMESCENE_H
