//
// Created by Cold-Mint on 2025/10/11.
//

#include "SceneManager.h"

void Glimmer::SceneManager::setConsoleScene(Scene *sc) {
    const Scene *oldScene = consoleScene;
    this->consoleScene = sc;
    delete oldScene;
}

void Glimmer::SceneManager::changeScene(Scene *sc) {
    pendingScene = sc;
    hasPending = true;
}

void Glimmer::SceneManager::applyPendingScene() {
    if (hasPending) {
        delete scene;
        scene = pendingScene;
        pendingScene = nullptr;
        hasPending = false;
    }
}

Glimmer::Scene *Glimmer::SceneManager::getConsoleScene() const {
    return consoleScene;
}


Glimmer::Scene *Glimmer::SceneManager::getScene() const {
    return scene;
}
