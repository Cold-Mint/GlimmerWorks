//
// Created by Cold-Mint on 2025/10/11.
//

#include "SceneManager.h"

#include <algorithm>


void glimmer::SceneManager::AddOverlayScene(std::unique_ptr<Scene> overlay) {
    if (overlay == nullptr) {
        return;
    }
    if (std::ranges::find(overlayScenes_, overlay) == overlayScenes_.end()) {
        overlayScenes_.push_back(std::move(overlay));
        overlayScenesPtr_.push_back(overlayScenes_.back().get());
    }
}

void glimmer::SceneManager::RemoveOverlayScene(const Scene *overlay) {
    if (overlay == nullptr) {
        return;
    }
    for (int i = 0; i < overlayScenes_.size(); i++) {
        if (overlayScenes_[i].get() == overlay) {
            overlayScenes_.erase(overlayScenes_.begin() + i);
            overlayScenesPtr_.erase(overlayScenesPtr_.begin() + i);
            return;
        }
    }
}

const std::vector<glimmer::Scene *> &glimmer::SceneManager::GetOverlayScenes() const {
    return overlayScenesPtr_;
}

void glimmer::SceneManager::PushScene(std::unique_ptr<Scene> scene) {
    sceneStack_.push(std::move(scene));
}

void glimmer::SceneManager::ReplaceScene(std::unique_ptr<Scene> scene) {
    if (sceneStack_.empty()) {
        return;
    }
    sceneStack_.pop();
    sceneStack_.push(std::move(scene));
}

void glimmer::SceneManager::PopScene() {
    if (sceneStack_.empty()) {
        return;
    }
    sceneStack_.pop();
}

glimmer::Scene *glimmer::SceneManager::GetTopScene() const {
    if (sceneStack_.empty()) {
        return nullptr;
    }
    return sceneStack_.top().get();
}

size_t glimmer::SceneManager::GetSceneCount() const {
    return sceneStack_.size();
}
