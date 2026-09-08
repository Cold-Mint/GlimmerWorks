/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "SceneManager.h"

#include <algorithm>
#include "core/log/LogCat.h"

void glimmer::SceneManager::ClearScenes() {
    LogCat::i("clearing_all_scenes", "Clearing all scenes: overlayScenes count={}, sceneStack count={}",
              overlayScenes_.size(), sceneStack_.size());
    overlayScenes_.clear();
    while (!sceneStack_.empty()) {
        sceneStack_.pop();
    }
}

void glimmer::SceneManager::AddOverlayScene(std::unique_ptr<Scene> overlay) {
    if (overlay == nullptr) {
        LogCat::w(std::source_location::current(), "add_overlay_scene_called_with_null", "AddOverlayScene called with nullptr");
        return;
    }
    if (std::ranges::find(overlayScenes_, overlay) == overlayScenes_.end()) {
        overlayScenes_.push_back(std::move(overlay));
        overlayScenesPtr_.push_back(overlayScenes_.back().get());
        LogCat::i("overlay_scene_added", "Overlay scene added, total overlay scenes: {}", overlayScenes_.size());
    } else {
        LogCat::w(std::source_location::current(), "overlay_scene_already_exists", "Overlay scene already exists, skipping");
    }
}

void glimmer::SceneManager::RemoveOverlayScene(const Scene *overlay) {
    if (overlay == nullptr) {
        LogCat::w(std::source_location::current(), "remove_overlay_scene_called_with_null", "RemoveOverlayScene called with nullptr");
        return;
    }
    for (int i = 0; i < overlayScenes_.size(); i++) {
        if (overlayScenes_[i].get() == overlay) {
            overlayScenes_.erase(overlayScenes_.begin() + i);
            overlayScenesPtr_.erase(overlayScenesPtr_.begin() + i);
            LogCat::i("overlay_scene_removed", "Overlay scene removed, remaining overlay scenes: {}", overlayScenes_.size());
            return;
        }
    }
    LogCat::w(std::source_location::current(), "remove_overlay_scene_not_found", "RemoveOverlayScene: overlay scene not found");
}

std::vector<glimmer::Scene *> glimmer::SceneManager::GetOverlayScenes() const {
    return overlayScenesPtr_;
}


void glimmer::SceneManager::PushScene(std::unique_ptr<Scene> scene) {
    if (!sceneStack_.empty()) {
        LogCat::i("pausing_current_scene", "Pausing current scene, scene count: {}", sceneStack_.size());
        sceneStack_.top()->OnPauseScene();
    }
    LogCat::i("pushing_new_scene", "Pushing new scene, scene count: {}", sceneStack_.size() + 1);
    scene->OnResumeScene();
    sceneStack_.push(std::move(scene));
}

void glimmer::SceneManager::ReplaceScene(std::unique_ptr<Scene> scene) {
    if (sceneStack_.empty()) {
        LogCat::i("replacing_scene_stack_empty", "Replacing scene: stack is empty, pushing new scene");
        scene->OnResumeScene();
        sceneStack_.push(std::move(scene));
        return;
    }
    LogCat::i("replacing_scene_pausing_removing", "Replacing scene: pausing and removing current scene");
    sceneStack_.top()->OnPauseScene();
    sceneStack_.pop();
    LogCat::i("replacing_scene_pushing_new", "Replacing scene: pushing new scene");
    scene->OnResumeScene();
    sceneStack_.push(std::move(scene));
}

void glimmer::SceneManager::PopScene() {
    if (sceneStack_.empty()) {
        LogCat::w(std::source_location::current(), "pop_scene_stack_empty", "PopScene called but scene stack is empty");
        return;
    }
    LogCat::i("popping_scene", "Popping scene, scene count: {}", sceneStack_.size());
    sceneStack_.top()->OnPauseScene();
    sceneStack_.pop();
    if (!sceneStack_.empty()) {
        LogCat::i("resuming_previous_scene", "Resuming previous scene, scene count: {}", sceneStack_.size());
        sceneStack_.top()->OnResumeScene();
    } else {
        LogCat::i("scene_stack_empty_after_pop", "Scene stack is now empty after pop");
    }
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
