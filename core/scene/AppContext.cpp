//
// Created by Cold-Mint on 2025/10/10.
//
#include "AppContext.h"
#include "core/console/command/AssetViewerCommand.h"
#include "core/console/command/Box2DCommand.h"
#include "core/console/command/EcsCommand.h"
#include "core/console/command/GiveCommand.h"
#include "core/console/command/HeightMapCommand.h"
#include "core/console/command/HelpCommand.h"
#include "core/console/command/LicenseCommand.h"
#include "core/console/command/SeedCommand.h"
#include "core/console/command/TpCommand.h"
#include "core/console/command/VFSCommand.h"
#include "core/console/suggestion/AbilityItemDynamicSuggestions.h"
#include "core/console/suggestion/BoolDynamicSuggestions.h"
#include "core/console/suggestion/ComposableItemDynamicSuggestions.h"
#include "core/console/suggestion/TileDynamicSuggestions.h"
#include "core/console/suggestion/VFSDynamicSuggestions.h"
#include "core/log/LogCat.h"
#include "core/utils/LanguageUtils.h"
#include "core/vfs/StdFileProvider.h"
#include "core/console/command/ClearCommand.h"
#include "core/console/command/ConfigCommand.h"
#include "core/console/command/LootCommand.h"
#include "core/console/command/PlaceCommand.h"
#include "core/console/suggestion/ConfigSuggestions.h"
#include "core/console/suggestion/LootSuggestions.h"
#include "core/console/suggestion/StructureDynamicSuggestions.h"
#include "core/saves/SavesManager.h"
#include "core/world/generator/FillBiomeDecorator.h"
#include "core/world/generator/SurfaceBiomeDecorator.h"
#include "core/world/generator/TreeBiomeDecorator.h"
#include "core/world/structure/StaticStructureGenerator.h"
#include "core/world/structure/TreeStructureGenerator.h"
#include "toml11/find.hpp"
#include "toml11/parser.hpp"

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
    langs_->lootTableNotFound = find<std::string>(value, "lootTableNotFound");
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

glimmer::AppContext::AppContext() {
    mainThreadId_ = std::this_thread::get_id();
    virtualFileSystem_ = std::make_unique<VirtualFileSystem>();
#ifdef __ANDROID__
    JNIEnv *env = (JNIEnv *) SDL_GetAndroidJNIEnv();
    jobject activity = (jobject) SDL_GetAndroidActivity();
    jclass activityClass = env->GetObjectClass(activity);
    jmethodID getAssetsMethod = env->
            GetMethodID(activityClass, "getAssets", "()Landroid/content/res/AssetManager;");
    jobject assetManagerJava = env->CallObjectMethod(activity, getAssetsMethod);
    AAssetManager *assetManager = AAssetManager_fromJava(env, assetManagerJava);
    std::unique_ptr<AndroidAssetsFileProvider> assetsProvider = std::make_unique<AndroidAssetsFileProvider>(
        assetManager);
    auto indexJsonOpt = JsonUtils::LoadJsonFromFile(assetsProvider.get(), "index.json");
    if (!indexJsonOpt.has_value()) {
        LogCat::e("indexJsonOpt file!");
        return EXIT_FAILURE;
    }
    bool setIndex = assetsProvider->SetIndex(indexJsonOpt.value());
    if (!setIndex) {
        LogCat::e("setIndex error");
        return EXIT_FAILURE;
    }
    // 获取 getFilesDir 方法
    jmethodID getDataDirMethod = env->GetMethodID(activityClass, "getFilesDir", "()Ljava/io/File;");
    jobject dataDirFile = env->CallObjectMethod(activity, getDataDirMethod);

    // 获取 getAbsolutePath 方法
    jclass fileClass = env->GetObjectClass(dataDirFile);
    jmethodID getAbsolutePathMethod = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
    jstring absolutePathJStr = (jstring) env->CallObjectMethod(dataDirFile, getAbsolutePathMethod);

    // 转换为 std::string
    const char *absolutePathCStr = env->GetStringUTFChars(absolutePathJStr, nullptr);
    std::string dataDirPath(absolutePathCStr);
    env->ReleaseStringUTFChars(absolutePathJStr, absolutePathCStr);
    virtualFileSystem_->Mount(
        std::make_unique<StdFileProvider>(dataDirPath + "/assets"));
    virtualFileSystem_->Mount(std::unique_ptr<FileProvider>(std::move(assetsProvider)));
#else
    virtualFileSystem_->Mount(
        std::make_unique<StdFileProvider>("."));
#endif
    language_ = LanguageUtils::getLanguage();
    LogCat::i("Load the built-in language file.");
    std::string langFile = "langs/" + language_ + ".toml";
    LogCat::i("Try to load language file:", langFile);
    if (!virtualFileSystem_->Exists(langFile)) {
        LogCat::w("Not found, fall back to default.toml");
        langFile = "langs/default.toml";
    }
    VirtualFileSystem *vfs = virtualFileSystem_.get();
    const auto langData = virtualFileSystem_->ReadFile(langFile);
    if (!langData.has_value()) {
        LogCat::e("Failed to load language file!");
        return;
    }
    langs_ = std::make_unique<LangsResources>();
    LoadLanguage(langData.value());
    dynamicSuggestionsManager_ = std::make_unique<DynamicSuggestionsManager>();
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<BoolDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<VFSDynamicSuggestions>(vfs));
    dataPackManager_ = std::make_unique<DataPackManager>(vfs);
    resourcePackManager_ = std::make_unique<ResourcePackManager>(vfs);
    resourceLocator_ = std::make_unique<ResourceLocator>(this);
    sceneManager_ = std::make_unique<SceneManager>();
    stringManager_ = std::make_unique<StringManager>();
    biomesManager_ = std::make_unique<BiomesManager>();
    tileManager_ = std::make_unique<TileManager>();
    tileManager_->InitBuiltinTiles();
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<TileDynamicSuggestions>(tileManager_.get()));
    commandManager_ = std::make_unique<CommandManager>();
    commandManager_->RegisterCommand(std::make_unique<GiveCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<EcsCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<HelpCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<TpCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<PlaceCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<HeightMapCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<Box2DCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<AssetViewerCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<ClearCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<LootCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<VFSCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<LicenseCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<SeedCommand>(this));

    commandExecutor_ = std::make_unique<CommandExecutor>();
    biomeDecoratorManager_ = std::make_unique<BiomeDecoratorManager>();
    itemManager_ = std::make_unique<ItemManager>();
    ItemManager *im = itemManager_.get();
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<ComposableItemDynamicSuggestions>(im));
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<AbilityItemDynamicSuggestions>(im));
    biomeDecoratorManager_->RegisterBiomeDecorator(std::make_unique<TreeBiomeDecorator>());
    biomeDecoratorManager_->RegisterBiomeDecorator(std::make_unique<FillBiomeDecorator>());
    biomeDecoratorManager_->RegisterBiomeDecorator(std::make_unique<SurfaceBiomeDecorator>());
    config_ = std::make_unique<Config>();
    LogCat::i("Loading ",CONFIG_FILE_NAME, "...");
    std::optional<std::string> configData = vfs->ReadFile(CONFIG_FILE_NAME);
    if (!configData.has_value()) {
        LogCat::e("Failed to read ",CONFIG_FILE_NAME, " file!");
    }
    configValue = std::make_unique<toml::value>(toml::parse_str(configData.value(), tomlVersion_));
    toml::value *configValuePtr = configValue.get();
    config_->LoadConfig(*configValuePtr);
    commandManager_->RegisterCommand(std::make_unique<ConfigCommand>(this, configValuePtr));
    dynamicSuggestionsManager_->
            RegisterDynamicSuggestions(std::make_unique<ConfigSuggestions>(configValuePtr));
    LogCat::i("windowHeight = ", config_->window.height);
    LogCat::i("windowWidth = ", config_->window.width);
    LogCat::i("dataPackPath = ", config_->mods.dataPackPath);
    LogCat::i("resourcePackPath = ", config_->mods.resourcePackPath);
    LogCat::i("framerate = ", config_->window.framerate);
    LogCat::i("The ",CONFIG_FILE_NAME, " load was successful.");
    savesManager_ = std::make_unique<SavesManager>(vfs);
    savesManager_->LoadAllSaves();
    dragAndDrop_ = std::make_unique<DragAndDrop>();
    lootTableManager_ = std::make_unique<LootTableManager>();
    structureGeneratorManager_ = std::make_unique<StructureGeneratorManager>();
    structureGeneratorManager_->RegisterStructureGenerator(std::make_unique<StaticStructureGenerator>());
    structureGeneratorManager_->RegisterStructureGenerator(std::make_unique<TreeStructureGenerator>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<LootSuggestions>(lootTableManager_.get()));
    structureManager_ = std::make_unique<StructureManager>();
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<StructureDynamicSuggestions>(structureManager_.get()));
    LogCat::i("GAME_VERSION_NUMBER = ", GAME_VERSION_NUMBER);
    LogCat::i("GAME_VERSION_STRING = ", GAME_VERSION_STRING);
    LogCat::i("Starting GlimmerWorks...");
    if (dataPackManager_->Scan(this, tomlVersion_) == 0) {
        LogCat::e("Failed to load dataPack");
        return;
    }
    if (resourcePackManager_->Scan(config_->mods.resourcePackPath, config_->mods.enabledResourcePack,
                                   tomlVersion_) == 0) {
        LogCat::e("Failed to load resourcePack");
        return;
    }
    LogCat::i("Starting the app...");
}

const toml::spec &glimmer::AppContext::GetTomlVersion() const {
    return tomlVersion_;
}

glimmer::AppContext::~AppContext() {
    sceneManager_->ClearScenes();
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

glimmer::LootTableManager *glimmer::AppContext::GetLootTableManager() const {
    return lootTableManager_.get();
}

glimmer::StructureManager *glimmer::AppContext::GetStructureManager() const {
    return structureManager_.get();
}

glimmer::StructureGeneratorManager *glimmer::AppContext::GetStructureGeneratorManager() const {
    return structureGeneratorManager_.get();
}

glimmer::BiomeDecoratorManager *glimmer::AppContext::GetBiomeDecoratorManager() const {
    return biomeDecoratorManager_.get();
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
