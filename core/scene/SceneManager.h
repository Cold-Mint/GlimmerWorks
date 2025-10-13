//
// Created by coldmint on 2025/10/11.
//

#ifndef GLIMMERWORKS_SCENEMANAGER_H
#define GLIMMERWORKS_SCENEMANAGER_H
#include "Scene.h"

namespace Glimmer {
    class SceneManager {
        Scene *scene = nullptr;
        Scene *consoleScene = nullptr;

    public:
        void setConsoleScene(Scene *sc);

        void changeScene(Scene *sc);

        [[nodiscard]] Scene *getConsoleScene() const;

        [[nodiscard]] Scene *getScene() const;
    };
}


#endif //GLIMMERWORKS_SCENEMANAGER_H
