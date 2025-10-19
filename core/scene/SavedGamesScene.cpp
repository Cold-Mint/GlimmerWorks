//
// Created by Cold-Mint on 2025/10/19.
//

#include "SavedGamesScene.h"

#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"

bool Glimmer::SavedGamesScene::HandleEvent(const SDL_Event &event) {
    return false;
}

void Glimmer::SavedGamesScene::Update(float delta) {
    ImGui_ImplSDL3_NewFrame();
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui::NewFrame();
}

void Glimmer::SavedGamesScene::Render(SDL_Renderer *renderer) {
}
