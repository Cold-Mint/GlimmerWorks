//
// Created by Cold-Mint on 2025/10/10.
//
#include "AppContext.h"

#include "../log/LogCat.h"

glimmer::Config *glimmer::AppContext::GetConfig() const {
    return config_;
}

TTF_Font *glimmer::AppContext::GetFont() const {
    return ttfFont_;
}

glimmer::CommandManager *glimmer::AppContext::GetCommandManager() const {
    return commandManager_;
}

glimmer::StringManager *glimmer::AppContext::GetStringManager() const {
    return stringManager_;
}

glimmer::DynamicSuggestionsManager *glimmer::AppContext::GetDynamicSuggestionsManager() const {
    return dynamicSuggestionsManager_;
}

std::string *glimmer::AppContext::GetLanguage() const {
    return language_;
}

glimmer::LangsResources *glimmer::AppContext::GetLangsResources() const {
    return langs_;
}

glimmer::ResourcePackManager *glimmer::AppContext::GetResourcePackManager() const {
    return resourcePackManager_;
}

glimmer::TileManager *glimmer::AppContext::GetTileManager() const {
    return tileManager_;
}

glimmer::TilePlacerManager *glimmer::AppContext::GetTilePlacerManager() const {
    return tilePlacerManager_;
}

glimmer::BiomesManager *glimmer::AppContext::GetBiomesManager() const {
    return biomesManager_;
}

glimmer::ResourceLocator *glimmer::AppContext::GetResourceLocator() const {
    return resourceLocator_;
}

glimmer::VirtualFileSystem *glimmer::AppContext::GetVirtualFileSystem() const {
    return virtualFileSystem_;
}

glimmer::ItemManager *glimmer::AppContext::GetItemManager() const {
    return itemManager_;
}

void glimmer::AppContext::SetWindow(SDL_Window *window) {
    this->window_ = window;
}

void glimmer::AppContext::SetFont(TTF_Font *font) {
    this->ttfFont_ = font;
}

bool glimmer::AppContext::Running() const {
    return isRunning;
}

void glimmer::AppContext::ExitApp() {
    isRunning = false;
}

glimmer::SceneManager *glimmer::AppContext::GetSceneManager() const {
    return sceneManager_;
}

SDL_Window *glimmer::AppContext::GetWindow() const {
    return window_;
}
