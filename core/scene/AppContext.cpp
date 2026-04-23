//
// Created by Cold-Mint on 2025/10/10.
//
#include "AppContext.h"

#include <random>

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
#include "core/console/command/EchoCommand.h"
#include "core/console/command/FlyCommand.h"
#include "core/console/command/LocateCommand.h"
#include "core/console/command/LootCommand.h"
#include "core/console/command/PlaceCommand.h"
#include "core/console/command/ScreenshotCommand.h"
#include "core/console/command/SummonCommand.h"
#include "core/console/suggestion/BiomeSuggestions.h"
#include "core/console/suggestion/BooleanToggleDynamicSuggestions.h"
#include "core/console/suggestion/ConfigSuggestions.h"
#include "core/console/suggestion/CoordinateDynamicSuggestions.h"
#include "core/console/suggestion/LootSuggestions.h"
#include "core/console/suggestion/MobDynamicSuggestions.h"
#include "core/console/suggestion/StructureDynamicSuggestions.h"
#include "core/mod/templateCommand/InsertTemplateCommand.h"
#include "core/mod/templateCommand/SetTemplateCommand.h"
#include "core/mod/templateCommand/UnSetTemplateCommand.h"
#include "core/saves/SavesManager.h"
#include "core/world/generator/FillBiomeDecorator.h"
#include "core/world/generator/MineralBiomeDecorator.h"
#include "core/world/generator/SurfaceBiomeDecorator.h"
#include "core/world/structure/BiomeStructureConditionProcessor.h"
#include "core/world/structure/HeightStructureConditionProcessor.h"
#include "core/world/structure/HorizontalSpacingStructureConditionProcessor.h"
#include "core/world/structure/StaticStructureGenerator.h"
#include "core/world/structure/SurfaceStructureConditionProcessor.h"
#include "core/world/structure/TreeStructureGenerator.h"
#include "fmt/xchar.h"
#include "SDL3_image/SDL_image.h"
#include "toml11/find.hpp"
#include "toml11/parser.hpp"
#ifdef __ANDROID__
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "core/vfs/AndroidAssetsFileProvider.h"
#endif
void glimmer::AppContext::LoadLanguage(const std::string &data) const {
    toml::value value = toml::parse_str(data, tomlVersion_);
    langs_->startGame = find<std::string>(value, "start_game");
    langs_->settings = find<std::string>(value, "settings");
    langs_->mods = find<std::string>(value, "mods");
    langs_->exitGame = find<std::string>(value, "exit_game");
    langs_->console = find<std::string>(value, "console");
    langs_->commandNotFound = find<std::string>(value, "command_not_found");
    langs_->executionFailed = find<std::string>(value, "execution_failed");
    langs_->executedSuccess = find<std::string>(value, "executed_success");
    langs_->commandIsEmpty = find<std::string>(value, "command_is_empty");
    langs_->createWorld = find<std::string>(value, "create_world");
    langs_->savedGames = find<std::string>(value, "saved_games");
    langs_->cancel = find<std::string>(value, "cancel");
    langs_->worldName = find<std::string>(value, "world_name");
    langs_->seed = find<std::string>(value, "seed");
    langs_->random = find<std::string>(value, "random");
    langs_->commandInfo = find<std::string>(value, "command_info");
    langs_->awakeBodyCount = find<std::string>(value, "awake_body_count");
    langs_->getActualPathError = find<std::string>(value, "get_actual_path_error");
    langs_->unknownAssetType = find<std::string>(value, "unknown_asset_type");
    langs_->unknownCommandParameters = find<std::string>(value, "unknown_command_parameters");
    langs_->worldContextIsNull = find<std::string>(value, "world_context_is_null");
    langs_->insufficientParameterLength = find<std::string>(value, "insufficient_parameter_length");
    langs_->entryCannotFoundInConfigurationFile = find<std::string>(value, "entry_cannot_found_in_configuration_file");
    langs_->configurationUpdate = find<std::string>(value, "configuration_update");
    langs_->itemIdNotFound = find<std::string>(value, "item_id_not_found");
    langs_->lootTableNotFound = find<std::string>(value, "loot_table_not_found");
    langs_->itemContainerIsNull = find<std::string>(value, "item_container_is_null");
    langs_->composableItemIsNull = find<std::string>(value, "composable_item_is_null");
    langs_->abilityItemIsNull = find<std::string>(value, "ability_item_is_null");
    langs_->itemResourceIsNull = find<std::string>(value, "item_resource_is_null");
    langs_->tileResourceIsNull = find<std::string>(value, "tile_resource_is_null");
    langs_->minXIsGreaterThanMaxX = find<std::string>(value, "min_x_is_greater_than_max_x");
    langs_->folderCreationFailed = find<std::string>(value, "folder_creation_failed");
    langs_->fileWritingFailed = find<std::string>(value, "file_writing_failed");
    langs_->failedToLoadLicense = find<std::string>(value, "failed_to_load_license");
    langs_->cantFindObject = find<std::string>(value, "cant_find_object");
    langs_->teleportEntity = find<std::string>(value, "teleport_entity");
    langs_->loadGame = find<std::string>(value, "load_game");
    langs_->deleteGame = find<std::string>(value, "delete_game");
    langs_->confirm = find<std::string>(value, "confirm");
    langs_->wantDeleteThisSave = find<std::string>(value, "want_delete_this_save");
    langs_->savesList = find<std::string>(value, "saves_list");
    langs_->pause = find<std::string>(value, "pause");
    langs_->restore = find<std::string>(value, "restore");
    langs_->saveAndExit = find<std::string>(value, "save_and_exit");
    langs_->screenshotSavedSuccess = find<std::string>(value, "screenshot_saved_success");
    langs_->screenshotSavedFailed = find<std::string>(value, "screenshot_saved_failed");
    langs_->areaMarkerTip = find<std::string>(value, "area_marker_tip");
    langs_->efficiencyTip = find<std::string>(value, "efficiency_tip");
    langs_->precisionMiningTip = find<std::string>(value, "precision_mining_tip");
    langs_->canMineBlockTip = find<std::string>(value, "can_mine_block_tip");
    langs_->canMineWallTip = find<std::string>(value, "can_mine_wall_tip");
    langs_->fumbleTip = find<std::string>(value, "fumble_tip");
    langs_->chainMiningTip = find<std::string>(value, "chain_mining_tip");
    langs_->flyEnable = find<std::string>(value, "fly_enable");
    langs_->flyDisable = find<std::string>(value, "fly_disable");
    langs_->tileDebugInfo = find<std::string>(value, "tile_debug_info");
    langs_->tileResDebugInfo = find<std::string>(value, "tile_res_debug_info");
    langs_->mousePosition = find<std::string>(value, "mouse_position");
    langs_->totalLight = find<std::string>(value, "total_light");
    langs_->noBiomeWasFound = find<std::string>(value, "no_biome_was_found");
    langs_->biomeHasFound = find<std::string>(value, "biome_has_found");
    langs_->configurationCommitSuccess = find<std::string>(value, "configuration_commit_success");
    langs_->configurationCommitFail = find<std::string>(value, "configuration_commit_fail");
    langs_->worldNamePrefix = find<std::vector<std::string> >(value, "world_name_prefix");
    langs_->worldNameSuffix = find<std::vector<std::string> >(value, "world_name_suffix");
    langs_->slogans = find<std::vector<std::string> >(value, "slogans");
}

std::string glimmer::AppContext::GetTimeFileName(const std::string &prefix, const std::string &ext) {
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
#if defined(_WIN32)
    localtime_s(&tm, &t); // Windows 安全函数
#else
    localtime_r(&t, &tm); // Linux/macOS
#endif
    std::ostringstream oss;
    oss << prefix << "_"
            << std::put_time(&tm, "%Y%m%d_%H%M%S")
            << ext;

    return oss.str();
}

glimmer::AppContext::AppContext() {
    mainThreadId_ = std::this_thread::get_id();
    virtualFileSystem_ = std::make_unique<VirtualFileSystem>();
#ifdef __ANDROID__
    auto *env = static_cast<JNIEnv *>(SDL_GetAndroidJNIEnv());
    if (env == nullptr) {
        LogCat::e("Failed to get JNIEnv!");
        initSuccess_ = false;
        return;
    }
    auto activity = static_cast<jobject>(SDL_GetAndroidActivity());
    if (activity == nullptr) {
        LogCat::e("Failed to get Android activity!");
        initSuccess_ = false;
        return;
    }
    jclass activityClass = env->GetObjectClass(activity);
    if (activityClass == nullptr) {
        LogCat::e("Android: Failed to get activity class!");
        return;
    }
    jmethodID getAssetsMethod = env->GetMethodID(activityClass, "getAssets", "()Landroid/content/res/AssetManager;");
    if (getAssetsMethod == nullptr) {
        LogCat::e("Android: Failed to find 'getAssets' method!");
        return;
    }
    jobject assetManagerJava = env->CallObjectMethod(activity, getAssetsMethod);
    if (assetManagerJava == nullptr) {
        LogCat::e("Android: Failed to get AssetManager instance!");
        return;
    }
    AAssetManager *assetManager = AAssetManager_fromJava(env, assetManagerJava);
    if (assetManager == nullptr) {
        LogCat::e("Android: Failed to convert to native AAssetManager!");
        return;
    }
    auto assetsProvider = std::make_unique<AndroidAssetsFileProvider>(assetManager);
    std::optional<std::string> indexTomlOptional = assetsProvider->ReadFile("index.toml");
    if (!indexTomlOptional.has_value()) {
        LogCat::e("Android: 'index.toml' not found or could not be read!");
        return;
    }
    const toml::value tomlValue = toml::parse_str(indexTomlOptional.value(), tomlVersion_);
    auto assetsEntry = toml::get<std::vector<AndroidAssetEntry> >(tomlValue);
    assetsProvider->SetAssetEntryData(assetsEntry);
    jmethodID getDataDirMethod = env->GetMethodID(activityClass, "getFilesDir", "()Ljava/io/File;");
    if (getDataDirMethod == nullptr) {
        LogCat::e("Android: Failed to find 'getFilesDir' method!");
        return;
    }
    jobject dataDirFile = env->CallObjectMethod(activity, getDataDirMethod);
    if (dataDirFile == nullptr) {
        LogCat::e("Android: Failed to get data directory File object!");
        return;
    }
    jclass fileClass = env->GetObjectClass(dataDirFile);
    if (fileClass == nullptr) {
        LogCat::e("Android: Failed to get File class!");
        return;
    }
    jmethodID getAbsolutePathMethod = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
    if (getAbsolutePathMethod == nullptr) {
        LogCat::e("Android: Failed to find 'getAbsolutePath' method!");
        return;
    }
    auto absolutePathJStr = static_cast<jstring>(env->CallObjectMethod(dataDirFile, getAbsolutePathMethod));
    if (!absolutePathJStr) {
        LogCat::e("Android: Failed to get absolute path string!");
        return;
    }
    const char *absolutePathCStr = env->GetStringUTFChars(absolutePathJStr, nullptr);
    if (!absolutePathCStr) {
        LogCat::e("Android: Failed to convert JString to UTF-8!");
        return;
    }
    std::string dataDirPath(absolutePathCStr);
    env->ReleaseStringUTFChars(absolutePathJStr, absolutePathCStr);
    virtualFileSystem_->Mount(std::make_unique<StdFileProvider>(dataDirPath + "/assets"));
    virtualFileSystem_->Mount(std::unique_ptr<IFileProvider>(std::move(assetsProvider)));
#else
    virtualFileSystem_->Mount(
        std::make_unique<StdFileProvider>(std::filesystem::current_path().string()));
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
    contributorManager_ = std::make_unique<ContributorManager>();
    mobManager_ = std::make_unique<MobManager>();
    shapeManager_ = std::make_unique<ShapeManager>();
    audioManager_ = std::make_unique<AudioManager>();
    lightSourceManager_ = std::make_unique<LightSourceManager>();
    lightMaskManager_ = std::make_unique<LightMaskManager>();
    fixedColorManager_ = std::make_unique<FixedColorManager>();
    tomlTemplateExpander_ = std::make_unique<TomlTemplateExpander>();
    tomlTemplateExpander_->Register(std::make_unique<InsertTemplateCommand>());
    tomlTemplateExpander_->Register(std::make_unique<SetTemplateCommand>());
    tomlTemplateExpander_->Register(std::make_unique<UnSetTemplateCommand>());
    LoadLanguage(langData.value());
    dynamicSuggestionsManager_ = std::make_unique<DynamicSuggestionsManager>();
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<BoolDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<CoordinateDynamicSuggestions>(Y_DYNAMIC_SUGGESTIONS_NAME));
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<CoordinateDynamicSuggestions>(X_DYNAMIC_SUGGESTIONS_NAME));
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<MobDynamicSuggestions>(mobManager_.get()));
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<BoolDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<BooleanToggleDynamicSuggestions>());
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<VFSDynamicSuggestions>(vfs));
    dataPackManager_ = std::make_unique<DataPackManager>(vfs, tomlTemplateExpander_.get());
    resourcePackManager_ = std::make_unique<ResourcePackManager>(vfs);
    resourceLocator_ = std::make_unique<ResourceLocator>(this);
    sceneManager_ = std::make_unique<SceneManager>();
    stringManager_ = std::make_unique<StringManager>();
    biomesManager_ = std::make_unique<BiomesManager>();
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(std::make_unique<BiomeSuggestions>(biomesManager_.get()));
    tileManager_ = std::make_unique<TileManager>();
    structurePlacementConditionsManager_ = std::make_unique<StructurePlacementConditionsManager>();
    structurePlacementConditionsManager_->AddConditionProcessor(std::make_unique<SurfaceStructureConditionProcessor>());
    structurePlacementConditionsManager_->AddConditionProcessor(std::make_unique<HeightStructureConditionProcessor>());
    structurePlacementConditionsManager_->AddConditionProcessor(
        std::make_unique<HorizontalSpacingStructureConditionProcessor>());
    structurePlacementConditionsManager_->AddConditionProcessor(std::make_unique<BiomeStructureConditionProcessor>());
    initialInventoryManager_ = std::make_unique<InitialInventoryManager>();
    tileManager_->InitBuiltinTiles();
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<TileDynamicSuggestions>(tileManager_.get()));
    commandManager_ = std::make_unique<CommandManager>();
    commandManager_->RegisterCommand(std::make_unique<GiveCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<EcsCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<HelpCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<TpCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<SummonCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<PlaceCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<HeightMapCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<Box2DCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<AssetViewerCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<ClearCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<LootCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<VFSCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<LicenseCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<SeedCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<FlyCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<EchoCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<ScreenshotCommand>(this));
    commandManager_->RegisterCommand(std::make_unique<LocateCommand>(this));

    commandExecutor_ = std::make_unique<CommandExecutor>();
    biomeDecoratorManager_ = std::make_unique<BiomeDecoratorManager>();
    biomeDecoratorResourcesManager_ = std::make_unique<BiomeDecoratorResourcesManager>();
    itemManager_ = std::make_unique<ItemManager>();
    ItemManager *im = itemManager_.get();
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<ComposableItemDynamicSuggestions>(im));
    dynamicSuggestionsManager_->RegisterDynamicSuggestions(
        std::make_unique<AbilityItemDynamicSuggestions>(im));
    biomeDecoratorManager_->RegisterBiomeDecorator(std::make_unique<FillBiomeDecorator>());
    biomeDecoratorManager_->RegisterBiomeDecorator(std::make_unique<SurfaceBiomeDecorator>());
    biomeDecoratorManager_->RegisterBiomeDecorator(std::make_unique<MineralBiomeDecorator>());
    config_ = std::make_unique<Config>();
    LogCat::i("Loading ",CONFIG_FILE_NAME, "...");
    const std::optional<std::string> configData = vfs->ReadFile(CONFIG_FILE_NAME);
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
    savesManager_->LoadAllSaves(config_->runtimePath);
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
    preloadColors_ = std::make_unique<PreloadColors>();
    preloadColors_->LoadAllColors(resourceLocator_.get());
    const size_t number = mobManager_->GetPlayerResourceList().size();
    if (number == 0) {
        LogCat::e("At least one of the mob files must have the \"isPlayer = true\" setting.");
        return;
    }
    commandHistoryManager_ = std::make_unique<CommandHistoryManager>(config_->runtimePath, virtualFileSystem_.get());
    if (config_->console.maxHistoryEntries > 0) {
        commandHistoryManager_->Read();
    }
    LogCat::i("Starting the app...");
    initSuccess_ = true;
}

void glimmer::AppContext::LoadMainMenuBGM() {
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_TYPE_AUDIO);
    resourceRef.SetResourceKey("bgm/main_menu");
    mainMenuBGM_ = resourceLocator_->FindAudio(resourceRef);
}

void glimmer::AppContext::PlayMainMenuBGM() const {
    if (audioManager_ == nullptr || mainMenuBGM_ == nullptr) {
        return;
    }
    audioManager_->ForcePlayReplace(BGM, mainMenuBGM_.get(), -1);
}

const toml::spec &glimmer::AppContext::GetTomlVersion() const {
    return tomlVersion_;
}

glimmer::AppContext::~AppContext() {
    sceneManager_->ClearScenes();
}

bool glimmer::AppContext::InitSuccess() const {
    return initSuccess_;
}

void glimmer::AppContext::SetWindow(SDL_Window *window) {
    this->window_ = window;
}

void glimmer::AppContext::SetRenderer(SDL_Renderer *renderer) {
    this->renderer_ = renderer;
}

void glimmer::AppContext::CreateScreenshot(const std::function<void(const std::string &text)> &onMessage) const {
    if (!renderer_) {
        onMessage(fmt::format(
            fmt::runtime(GetLangsResources()->screenshotSavedFailed),
            "renderer is null failed"));
        return;
    }
    if (!virtualFileSystem_->Exists(config_->runtimePath + "/screenshots")) {
        if (!virtualFileSystem_->CreateFolder(config_->runtimePath + "/screenshots")) {
            onMessage(fmt::format(
                fmt::runtime(GetLangsResources()->screenshotSavedFailed),
                "CreateFolder failed"));
            return;
        }
    }
    const auto actualPath = virtualFileSystem_->GetActualPath(
        config_->runtimePath + "/screenshots/" + GetTimeFileName());
    if (!actualPath.has_value()) {
        onMessage(fmt::format(
            fmt::runtime(GetLangsResources()->screenshotSavedFailed),
            "GetActualPath failed"));
        return;
    }
    int width, height;
    if (!SDL_GetRenderOutputSize(renderer_, &width, &height)) {
        onMessage(fmt::format(
            fmt::runtime(GetLangsResources()->screenshotSavedFailed),
            "SDL_GetRenderOutputSize failed"));
        return;
    }
    auto sdlRect = SDL_Rect();
    sdlRect.x = 0;
    sdlRect.y = 0;
    sdlRect.w = width;
    sdlRect.h = height;
    SDL_Surface *surface = SDL_RenderReadPixels(
        renderer_, &sdlRect);
    if (surface == nullptr) {
        SDL_DestroySurface(surface);
        onMessage(fmt::format(
            fmt::runtime(GetLangsResources()->screenshotSavedFailed),
            "SDL_RenderReadPixels failed"));
        return;
    }
    const bool result = IMG_SavePNG(surface, actualPath.value().c_str());
    SDL_DestroySurface(surface);
    if (result) {
        onMessage(fmt::format(
            fmt::runtime(GetLangsResources()->screenshotSavedSuccess),
            actualPath.value()));
    } else {
        onMessage(fmt::format(
            fmt::runtime(GetLangsResources()->screenshotSavedFailed),
            "IMG_SavePNG Failed"));
    }
}

void glimmer::AppContext::SetFont(TTF_Font *font) {
    this->ttfFont_ = font;
}

bool glimmer::AppContext::Running() const {
    return isRunning;
}

void glimmer::AppContext::AddUIMessage(const std::string &string) {
    gameUIMessages_.emplace_back(string, SDL_GetTicks());
}

std::vector<glimmer::GameUIMessage> &glimmer::AppContext::GetGameUIMessages() {
    return gameUIMessages_;
}

void glimmer::AppContext::ExitApp() {
    if (config_->console.maxHistoryEntries > 0) {
        commandHistoryManager_->Save();
    }
    isRunning = false;
}

glimmer::PreloadColors *glimmer::AppContext::GetPreloadColors() const {
    return preloadColors_.get();
}

glimmer::Config *glimmer::AppContext::GetConfig() const {
    return config_.get();
}

TTF_Font *glimmer::AppContext::GetFont() const {
    return ttfFont_;
}

glimmer::InitialInventoryManager *glimmer::AppContext::GetInitialInventoryManager() const {
    return initialInventoryManager_.get();
}

glimmer::CommandManager *glimmer::AppContext::GetCommandManager() const {
    return commandManager_.get();
}

glimmer::AudioManager *glimmer::AppContext::GetAudioManager() const {
    return audioManager_.get();
}

glimmer::LightMaskManager *glimmer::AppContext::GetLightMaskManager() const {
    return lightMaskManager_.get();
}

glimmer::LightSourceManager *glimmer::AppContext::GetLightSourceManager() const {
    return lightSourceManager_.get();
}

glimmer::FixedColorManager *glimmer::AppContext::GetFixedColorManager() const {
    return fixedColorManager_.get();
}

glimmer::StringManager *glimmer::AppContext::GetStringManager() const {
    return stringManager_.get();
}

glimmer::BiomeDecoratorResourcesManager *glimmer::AppContext::GetBiomeDecoratorResourcesManager() const {
    return biomeDecoratorResourcesManager_.get();
}

glimmer::TomlTemplateExpander *glimmer::AppContext::GetTomlTemplateExpander() const {
    return tomlTemplateExpander_.get();
}

glimmer::DynamicSuggestionsManager *glimmer::AppContext::GetDynamicSuggestionsManager() const {
    return dynamicSuggestionsManager_.get();
}

const std::string &glimmer::AppContext::GetLanguage() {
    return language_;
}

glimmer::CommandHistoryManager *glimmer::AppContext::GetCommandHistoryManager() const {
    return commandHistoryManager_.get();
}

glimmer::LangsResources *glimmer::AppContext::GetLangsResources() const {
    return langs_.get();
}

glimmer::ResourcePackManager *glimmer::AppContext::GetResourcePackManager() const {
    return resourcePackManager_.get();
}

glimmer::StructurePlacementConditionsManager *glimmer::AppContext::GetStructurePlacementConditionsManager() const {
    return structurePlacementConditionsManager_.get();
}

glimmer::TileManager *glimmer::AppContext::GetTileManager() const {
    return tileManager_.get();
}

glimmer::DataPackManager *glimmer::AppContext::GetDataPackManager() const {
    return dataPackManager_.get();
}

glimmer::LootTableManager *glimmer::AppContext::GetLootTableManager() const {
    return lootTableManager_.get();
}

glimmer::ContributorManager *glimmer::AppContext::GetContributorManager() const {
    return contributorManager_.get();
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

glimmer::ShapeManager *glimmer::AppContext::GetShapeManager() const {
    return shapeManager_.get();
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

void glimmer::AppContext::SetRandomSlogan() const {
    if (window_ == nullptr || langs_ == nullptr) {
        SDL_SetWindowTitle(window_, PROJECT_NAME.c_str());
        return;
    }
    const std::vector<std::string> &slogans = langs_->slogans;
    if (slogans.empty()) {
        SDL_SetWindowTitle(window_, PROJECT_NAME.c_str());
    } else {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution dist(0, static_cast<int>(slogans.size()) - 1);
        int idx = dist(gen);
        const std::string &random_str = slogans[idx];
        SDL_SetWindowTitle(window_, random_str.c_str());
    }
}

glimmer::SavesManager *glimmer::AppContext::GetSavesManager() const {
    return savesManager_.get();
}

SDL_Window *glimmer::AppContext::GetWindow() const {
    return window_;
}

glimmer::MobManager *glimmer::AppContext::GetMobManager() const {
    return mobManager_.get();
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
