//
// Created by Cold-Mint on 2025/10/10.
//
#include "AppContext.h"

void glimmer::AppContext::LoadLanguage(const std::string &data) const {
    toml::value value = toml::parse_str(data, tomlVersion_);
    langs_->startGame = find<std::string>(value, "startGame");
    langs_->settings = find<std::string>(value, "settings");
    langs_->mods = find<std::string>(value, "mods");
    langs_->exitGame = find<std::string>(value, "exitGame");
    langs_->console = find<std::string>(value, "console");
    langs_->commandNotFound = find<std::string>(value, "commandNotFound");
    langs_->executionFailed = find<std::string>(value, "executionFailed");
    langs_->executedSuccess = find<std::string>(value, "executedSuccess");
    langs_->commandIsEmpty = find<std::string>(value, "commandIsEmpty");
    langs_->createWorld = find<std::string>(value, "createWorld");
    langs_->savedGames = find<std::string>(value, "savedGames");
    langs_->cancel = find<std::string>(value, "cancel");
    langs_->worldName = find<std::string>(value, "worldName");
    langs_->seed = find<std::string>(value, "seed");
    langs_->random = find<std::string>(value, "random");
    langs_->commandInfo = find<std::string>(value, "commandInfo");
    langs_->awakeBodyCount = find<std::string>(value, "awakeBodyCount");
    langs_->getActualPathError = find<std::string>(value, "getActualPathError");
    langs_->unknownAssetType = find<std::string>(value, "unknownAssetType");
    langs_->unknownCommandParameters = find<std::string>(value, "unknownCommandParameters");
    langs_->worldContextIsNull = find<std::string>(value, "worldContextIsNull");
    langs_->insufficientParameterLength = find<std::string>(value, "insufficientParameterLength");
    langs_->entryCannotFoundInConfigurationFile = find<std::string>(value, "entryCannotFoundInConfigurationFile");
    langs_->configurationUpdate = find<std::string>(value, "configurationUpdate");
    langs_->itemIdNotFound = find<std::string>(value, "itemIdNotFound");
    langs_->tileResourceNotFound = find<std::string>(value, "tileResourceNotFound");
    langs_->itemContainerIsNull = find<std::string>(value, "itemContainerIsNull");
    langs_->composableItemIsNull = find<std::string>(value, "composableItemIsNull");
    langs_->abilityItemIsNull = find<std::string>(value, "abilityItemIsNull");
    langs_->itemResourceNotFound = find<std::string>(value, "itemResourceNotFound");
    langs_->itemResourceIsNull = find<std::string>(value, "itemResourceIsNull");
    langs_->tileResourceIsNull = find<std::string>(value, "tileResourceIsNull");
    langs_->minXIsGreaterThanMaxX = find<std::string>(value, "minXIsGreaterThanMaxX");
    langs_->folderCreationFailed = find<std::string>(value, "folderCreationFailed");
    langs_->fileWritingFailed = find<std::string>(value, "fileWritingFailed");
    langs_->failedToLoadLicense = find<std::string>(value, "failedToLoadLicense");
    langs_->cantFindObject = find<std::string>(value, "cantFindObject");
    langs_->teleportEntity = find<std::string>(value, "teleportEntity");
    langs_->loadGame = find<std::string>(value, "loadGame");
    langs_->deleteGame = find<std::string>(value, "deleteGame");
    langs_->confirm = find<std::string>(value, "confirm");
    langs_->wantDeleteThisSave = find<std::string>(value, "wantDeleteThisSave");
    langs_->savesList = find<std::string>(value, "savesList");
    langs_->pause = find<std::string>(value, "pause");
    langs_->restore = find<std::string>(value, "restore");
    langs_->saveAndExit = find<std::string>(value, "saveAndExit");
    langs_->worldNamePrefix = find<std::vector<std::string> >(value, "worldNamePrefix");
    langs_->worldNameSuffix = find<std::vector<std::string> >(value, "worldNameSuffix");
}

const toml::spec &glimmer::AppContext::GetTomlVersion() const {
    return tomlVersion_;
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
