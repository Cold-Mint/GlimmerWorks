//
// Created by Cold-Mint on 2025/10/10.
//
#include "AppContext.h"

#include "../log/LogCat.h"

glimmer::Config *glimmer::AppContext::GetConfig() const {
    return config_.get();
}

TTF_Font *glimmer::AppContext::GetFont() const {
    return ttfFont_;
}

glimmer::CommandManager *glimmer::AppContext::GetCommandManager() const {
    return commandManager_.get();
}

glimmer::StringManager *glimmer::AppContext::GetStringManager() const {
    return stringManager_.get();
}

glimmer::DynamicSuggestionsManager *glimmer::AppContext::GetDynamicSuggestionsManager() const {
    return dynamicSuggestionsManager_.get();
}

const std::string &glimmer::AppContext::GetLanguage() {
    return language_;
}

glimmer::LangsResources *glimmer::AppContext::GetLangsResources() const {
    return langs_.get();
}

glimmer::ResourcePackManager *glimmer::AppContext::GetResourcePackManager() const {
    return resourcePackManager_.get();
}

glimmer::TileManager *glimmer::AppContext::GetTileManager() const {
    return tileManager_.get();
}

glimmer::TilePlacerManager *glimmer::AppContext::GetTilePlacerManager() const {
    return tilePlacerManager_.get();
}

glimmer::BiomesManager *glimmer::AppContext::GetBiomesManager() const {
    return biomesManager_.get();
}

glimmer::ResourceLocator *glimmer::AppContext::GetResourceLocator() const {
    return resourceLocator_.get();
}

glimmer::VirtualFileSystem *glimmer::AppContext::GetVirtualFileSystem() const {
    return virtualFileSystem_.get();
}

glimmer::ItemManager *glimmer::AppContext::GetItemManager() const {
    return itemManager_.get();
}

void glimmer::AppContext::LoadLanguage(const nlohmann::json &json) const {
    langs_->startGame = json["startGame"].get<std::string>();
    langs_->settings = json["settings"].get<std::string>();
    langs_->mods = json["mods"].get<std::string>();
    langs_->exitGame = json["exitGame"].get<std::string>();
    langs_->console = json["console"].get<std::string>();
    langs_->commandNotFound = json["commandNotFound"].get<std::string>();
    langs_->executionFailed = json["executionFailed"].get<std::string>();
    langs_->executedSuccess = json["executedSuccess"].get<std::string>();
    langs_->commandIsEmpty = json["commandIsEmpty"].get<std::string>();
    langs_->createWorld = json["createWorld"].get<std::string>();
    langs_->savedGames = json["savedGames"].get<std::string>();
    langs_->cancel = json["cancel"].get<std::string>();
    langs_->worldName = json["worldName"].get<std::string>();
    langs_->seed = json["seed"].get<std::string>();
    langs_->random = json["random"].get<std::string>();
    langs_->commandInfo = json["commandInfo"].get<std::string>();
    langs_->awakeBodyCount = json["awakeBodyCount"].get<std::string>();
    langs_->getActualPathError = json["getActualPathError"].get<std::string>();
    langs_->unknownAssetType = json["unknownAssetType"].get<std::string>();
    langs_->unknownCommandParameters = json["unknownCommandParameters"].get<std::string>();
    langs_->worldContextIsNull = json["worldContextIsNull"].get<std::string>();
    langs_->insufficientParameterLength = json["insufficientParameterLength"].get<std::string>();
    langs_->entryCannotFoundInConfigurationFile = json["entryCannotFoundInConfigurationFile"].get<
        std::string>();
    langs_->configurationUpdate = json["configurationUpdate"].get<std::string>();
    langs_->itemIdNotFound = json["itemIdNotFound"].get<std::string>();
    langs_->tileResourceNotFound = json["tileResourceNotFound"].get<std::string>();
    langs_->itemContainerIsNull = json["itemContainerIsNull"].get<std::string>();
    langs_->composableItemIsNull = json["composableItemIsNull"].get<std::string>();
    langs_->abilityItemIsNull = json["abilityItemIsNull"].get<std::string>();
    langs_->itemResourceNotFound = json["itemResourceNotFound"].get<std::string>();
    langs_->itemResourceIsNull = json["itemResourceIsNull"].get<std::string>();
    langs_->tileResourceIsNull = json["tileResourceIsNull"].get<std::string>();
    langs_->minXIsGreaterThanMaxX = json["minXIsGreaterThanMaxX"].get<std::string>();
    langs_->folderCreationFailed = json["folderCreationFailed"].get<std::string>();
    langs_->fileWritingFailed = json["fileWritingFailed"].get<std::string>();
    langs_->failedToLoadLicense = json["failedToLoadLicense"].get<std::string>();
    langs_->cantFindObject = json["cantFindObject"].get<std::string>();
    langs_->teleportEntity = json["teleportEntity"].get<std::string>();
    langs_->loadGame = json["loadGame"].get<std::string>();
    langs_->deleteGame = json["deleteGame"].get<std::string>();
    langs_->confirm = json["confirm"].get<std::string>();
    langs_->wantDeleteThisSave = json["wantDeleteThisSave"].get<std::string>();
    langs_->savesList = json["savesList"].get<std::string>();
    langs_->pause = json["pause"].get<std::string>();
    langs_->restore = json["restore"].get<std::string>();
    langs_->saveAndExit = json["saveAndExit"].get<std::string>();
    langs_->worldNamePrefix = json["worldNamePrefix"].get<std::vector<std::string> >();
    langs_->worldNameSuffix = json["worldNameSuffix"].get<std::vector<std::string> >();
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
    return sceneManager_.get();
}

glimmer::SavesManager *glimmer::AppContext::GetSavesManager() const {
    return savesManager_.get();
}

SDL_Window *glimmer::AppContext::GetWindow() const {
    return window_;
}

glimmer::DragAndDrop *glimmer::AppContext::GetDragAndDrop() const {
    return dragAndDrop_.get();
}

bool glimmer::AppContext::IsMainThread() const {
    return std::this_thread::get_id() == mainThreadId_;
}

void glimmer::AppContext::ProcessMainThreadTasks() {
    std::queue<std::function<void()> > tasks;
    {
        std::lock_guard lock(mainThreadMutex_);
        std::swap(tasks, mainThreadTasks_);
    }

    while (!tasks.empty()) {
        tasks.front()();
        tasks.pop();
    }
}

void glimmer::AppContext::RestoreColorRenderer(SDL_Renderer *sdlRenderer) {
    //Set the default renderer color to black.
    //设置默认渲染器颜色为黑色。
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
}

void glimmer::AppContext::AddMainThreadTask(std::function<void()> task) {
    if (IsMainThread()) {
        task();
        return;
    }
    std::lock_guard lock(mainThreadMutex_);
    mainThreadTasks_.push(std::move(task));
}
