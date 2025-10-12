//
// Created by coldmint on 2025/10/11.
//

#include "SceneManager.h"

void Glimmer::SceneManager::changeScene(Scene *sc) {
    const Scene *oldScene = scene;
    this->scene = sc;
    delete oldScene;
}


const Glimmer::Scene *Glimmer::SceneManager::getScene() const {
    return scene;
}
