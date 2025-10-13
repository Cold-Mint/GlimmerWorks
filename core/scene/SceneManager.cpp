//
// Created by coldmint on 2025/10/11.
//

#include "SceneManager.h"

void Glimmer::SceneManager::setConsoleScene(Scene *sc) {
    const Scene *oldScene = consoleScene;
    this->consoleScene = sc;
    delete oldScene;
}

void Glimmer::SceneManager::changeScene(Scene *sc) {
    const Scene *oldScene = scene;
    this->scene = sc;
    delete oldScene;
}

Glimmer::Scene * Glimmer::SceneManager::getConsoleScene() const {
    return consoleScene;
}


Glimmer::Scene *Glimmer::SceneManager::getScene() const {
    return scene;
}
