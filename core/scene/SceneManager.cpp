//
// Created by Cold-Mint on 2025/10/11.
//

#include "SceneManager.h"

#include <algorithm>


void Glimmer::SceneManager::addOverlayScene(Scene *overlay) {
    if (overlay == nullptr) {
        return;
    }
    if (std::ranges::find(overlayScenes, overlay) == overlayScenes.end()) {
        overlayScenes.push_back(overlay);
    }
}

void Glimmer::SceneManager::removeOverlayScene(Scene *overlay) {
    if (overlay == nullptr) {
        return;
    }
    overlayScenes.erase(
        std::ranges::remove(overlayScenes, overlay).begin(),
        overlayScenes.end()
    );
}

const std::vector<Glimmer::Scene *> &Glimmer::SceneManager::getOverlayScenes() const {
    return overlayScenes;
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


Glimmer::Scene *Glimmer::SceneManager::getScene() const {
    return scene;
}
