//
// Created by Cold-Mint on 2025/10/25.
//

#include "WorldScene.h"

#include "core/saves/SavesManager.h"

glimmer::WorldScene::WorldScene(AppContext *context, std::unique_ptr<WorldContext> worldContext)
    : Scene(context) {
    worldContext_ = std::move(worldContext);
    if (context != nullptr) {
        SDL_SetWindowTitle(context->GetWindow(), (PROJECT_NAME + " - " + worldContext_->GetMapManifest()->name).c_str());
    }
}

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
