//
// Created by Cold-Mint on 2025/10/25.
//

#include "WorldScene.h"

void glimmer::WorldScene::OnFrameStart() {
    worldContext_->OnFrameStart();
}

bool glimmer::WorldScene::HandleEvent(const SDL_Event &event) {
    return worldContext_->HandleEvent(event);
}

bool glimmer::WorldScene::OnBackPressed() {
    return worldContext_->OnBackPressed();
}

void glimmer::WorldScene::Update(float delta) {
    worldContext_->Update(delta);
}

void glimmer::WorldScene::Render(SDL_Renderer *renderer) {
    worldContext_->Render(renderer);
}
