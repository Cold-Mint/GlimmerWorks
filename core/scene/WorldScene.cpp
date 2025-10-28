//
// Created by Cold-Mint on 2025/10/25.
//

#include "WorldScene.h"

void glimmer::WorldScene::OnFrameStart() {
    worldContext->OnFrameStart();
}

bool glimmer::WorldScene::HandleEvent(const SDL_Event &event) {
    return worldContext->HandleEvent(event);
}

void glimmer::WorldScene::Update(float delta) {
    worldContext->Update(delta);
}

void glimmer::WorldScene::Render(SDL_Renderer *renderer) {
    worldContext->Render(renderer);
}
