//
// Created by coldmint on 2025/10/13.
//

#include "ConsoleScene.h"

#include "../log/LogCat.h"

void Glimmer::ConsoleScene::HandleEvent(const SDL_Event &event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.scancode == SDL_SCANCODE_GRAVE) {
            show = !show;
        }
    }
}

void Glimmer::ConsoleScene::Update(float delta) {
}

void Glimmer::ConsoleScene::Render(SDL_Renderer *renderer) {
    if (!show) return;
    LogCat::i("Rendering console scene...");
}

Glimmer::ConsoleScene::~ConsoleScene() = default;
