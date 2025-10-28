//
// Created by Cold-Mint on 2025/10/11.
//

#include "SceneManager.h"

#include <algorithm>


void glimmer::SceneManager::AddOverlayScene(Scene *overlay) {
    if (overlay == nullptr) {
        return;
    }
    if (std::ranges::find(overlayScenes, overlay) == overlayScenes.end()) {
        overlayScenes.push_back(overlay);
    }
}

void glimmer::SceneManager::RemoveOverlayScene(Scene *overlay) {
    if (overlay == nullptr) {
        return;
    }
    overlayScenes.erase(
        std::ranges::remove(overlayScenes, overlay).begin(),
        overlayScenes.end()
    );
}

const std::vector<glimmer::Scene *> &glimmer::SceneManager::GetOverlayScenes() const {
    return overlayScenes;
}

void glimmer::SceneManager::ChangeScene(Scene *sc) {
    pendingScene = sc;
    hasPending = true;
}

void glimmer::SceneManager::ApplyPendingScene() {
    if (hasPending) {
        delete scene;
        scene = pendingScene;
        pendingScene = nullptr;
        hasPending = false;
    }
}


glimmer::Scene *glimmer::SceneManager::getScene() const {
    return scene;
}
