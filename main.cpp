#include <fstream>

#include "core/App.h"
#include "core/Config.h"
#include "core/Langs.h"
#include "core/world/TilePlacerManager.h"
#include "core/console/CommandManager.h"
#include "core/console/command/AssetViewerCommand.h"
#include "core/console/command/Box2DCommand.h"
#include "core/console/command/ConfigCommand.h"
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
#include "core/console/suggestion/ConfigSuggestions.h"
#include "core/console/suggestion/DynamicSuggestionsManager.h"
#include "core/console/suggestion/TileDynamicSuggestions.h"
#include "core/console/suggestion/VFSDynamicSuggestions.h"
#include "core/log/LogCat.h"
#include "core/mod/ResourceLocator.h"
#include "core/mod/dataPack/BiomesManager.h"
#include "core/mod/dataPack/DataPackManager.h"
#include "core/mod/dataPack/ItemManager.h"
#include "core/mod/dataPack/StringManager.h"
#include "core/mod/resourcePack/ResourcePackManager.h"
#include "core/scene/AppContext.h"
#include "core/scene/SceneManager.h"
#include "core/utils/JsonUtils.h"
#include "core/utils/LanguageUtils.h"
#include "core/vfs/StdFileProvider.h"
#include "core/vfs/VirtualFileSystem.h"
#include "core/world/FillTilePlacer.h"
#include "fmt/args.h"

#ifdef __ANDROID__
#include <jni.h>
#include <android/asset_manager_jni.h>
#include <SDL3/SDL_system.h>
#include "core/vfs/AndroidAssetsFileProvider.h"
#endif

using namespace glimmer;
namespace fs = std::filesystem;

int main() {
    std::set_terminate([] {
        try {
            if (std::current_exception()) {
                std::rethrow_exception(std::current_exception());
            }
        } catch (const std::exception &e) {
            const std::string msg =
                    std::string("Oops! An unexpected error occurred in GlimmerWorks.\n\n") +
                    "Error details:\n" + e.what() + "\n\n" +
                    "The application needs to close.";

            SDL_ShowSimpleMessageBox(
                SDL_MESSAGEBOX_ERROR,
                "GlimmerWorks - Fatal Error",
                msg.c_str(),
                nullptr
            );
        } catch (...) {
            std::cerr << "Unhandled non-standard exception" << std::endl;
        }
        std::abort();
    });
    VirtualFileSystem virtualFileSystem;
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
    virtualFileSystem.Mount(
        std::make_unique<StdFileProvider>(dataDirPath + "/assets"));
    virtualFileSystem.Mount(std::unique_ptr<FileProvider>(std::move(assetsProvider)));
#else
    virtualFileSystem.Mount(
        std::make_unique<StdFileProvider>("."));
#endif
    std::string language = LanguageUtils::getLanguage();
    LogCat::i("Load the built-in language file.");
    std::string langFile = "langs/" + language + ".json";
    LogCat::i("Try to load language file:", langFile);
    if (!virtualFileSystem.Exists(langFile)) {
        LogCat::w("Not found, fall back to default.json");
        langFile = "langs/default.json";
    }
    auto jsonOpt = JsonUtils::LoadJsonFromFile(&virtualFileSystem, langFile);
    if (!jsonOpt.has_value()) {
        LogCat::e("Failed to load language file!");
        return EXIT_FAILURE;
    }
    auto &jsonObject = *jsonOpt;
    LangsResources langsResources;
    langsResources.startGame = jsonObject["startGame"].get<std::string>();
    langsResources.settings = jsonObject["settings"].get<std::string>();
    langsResources.mods = jsonObject["mods"].get<std::string>();
    langsResources.exitGame = jsonObject["exitGame"].get<std::string>();
    langsResources.console = jsonObject["console"].get<std::string>();
    langsResources.commandNotFound = jsonObject["commandNotFound"].get<std::string>();
    langsResources.executionFailed = jsonObject["executionFailed"].get<std::string>();
    langsResources.executedSuccess = jsonObject["executedSuccess"].get<std::string>();
    langsResources.commandIsEmpty = jsonObject["commandIsEmpty"].get<std::string>();
    langsResources.createWorld = jsonObject["createWorld"].get<std::string>();
    langsResources.savedGames = jsonObject["savedGames"].get<std::string>();
    langsResources.cancel = jsonObject["cancel"].get<std::string>();
    langsResources.worldName = jsonObject["worldName"].get<std::string>();
    langsResources.seed = jsonObject["seed"].get<std::string>();
    langsResources.random = jsonObject["random"].get<std::string>();
    langsResources.commandInfo = jsonObject["commandInfo"].get<std::string>();
    langsResources.awakeBodyCount = jsonObject["awakeBodyCount"].get<std::string>();
    langsResources.getActualPathError = jsonObject["getActualPathError"].get<std::string>();
    langsResources.unknownAssetType = jsonObject["unknownAssetType"].get<std::string>();
    langsResources.unknownCommandParameters = jsonObject["unknownCommandParameters"].get<std::string>();
    langsResources.worldContextIsNull = jsonObject["worldContextIsNull"].get<std::string>();
    langsResources.insufficientParameterLength = jsonObject["insufficientParameterLength"].get<std::string>();
    langsResources.entryCannotFoundInConfigurationFile = jsonObject["entryCannotFoundInConfigurationFile"].get<
        std::string>();
    langsResources.configurationUpdate = jsonObject["configurationUpdate"].get<std::string>();
    langsResources.itemIdNotFound = jsonObject["itemIdNotFound"].get<std::string>();
    langsResources.tileResourceNotFound = jsonObject["tileResourceNotFound"].get<std::string>();
    langsResources.itemContainerIsNull = jsonObject["itemContainerIsNull"].get<std::string>();
    langsResources.composableItemIsNull = jsonObject["composableItemIsNull"].get<std::string>();
    langsResources.abilityItemIsNull = jsonObject["abilityItemIsNull"].get<std::string>();
    langsResources.itemResourceNotFound = jsonObject["itemResourceNotFound"].get<std::string>();
    langsResources.itemResourceIsNull = jsonObject["itemResourceIsNull"].get<std::string>();
    langsResources.tileResourceIsNull = jsonObject["tileResourceIsNull"].get<std::string>();
    langsResources.minXIsGreaterThanMaxX = jsonObject["minXIsGreaterThanMaxX"].get<std::string>();
    langsResources.folderCreationFailed = jsonObject["folderCreationFailed"].get<std::string>();
    langsResources.fileWritingFailed = jsonObject["fileWritingFailed"].get<std::string>();
    langsResources.failedToLoadLicense = jsonObject["failedToLoadLicense"].get<std::string>();
    langsResources.cantFindObject = jsonObject["cantFindObject"].get<std::string>();
    langsResources.teleportEntity = jsonObject["teleportEntity"].get<std::string>();
    langsResources.loadGame = jsonObject["loadGame"].get<std::string>();
    langsResources.deleteGame = jsonObject["deleteGame"].get<std::string>();
    langsResources.confirm = jsonObject["confirm"].get<std::string>();
    langsResources.wantDeleteThisSave = jsonObject["wantDeleteThisSave"].get<std::string>();
    langsResources.savesList = jsonObject["savesList"].get<std::string>();
    langsResources.worldNamePrefix = jsonObject["worldNamePrefix"].get<std::vector<std::string>>();
    langsResources.worldNameSuffix = jsonObject["worldNameSuffix"].get<std::vector<std::string>>();
    DynamicSuggestionsManager dynamicSuggestionsManager;
    dynamicSuggestionsManager.RegisterDynamicSuggestions(std::make_unique<BoolDynamicSuggestions>());
    dynamicSuggestionsManager.RegisterDynamicSuggestions(
        std::make_unique<VFSDynamicSuggestions>(&virtualFileSystem));
    DataPackManager dataPackManager(&virtualFileSystem);
    ResourcePackManager resourcePackManager(&virtualFileSystem);
    ResourceLocator resourceLocator;
    SceneManager sceneManager;
    StringManager stringManager;
    BiomesManager biomesManager;
    TileManager tileManager;
    tileManager.InitBuiltinTiles();
    dynamicSuggestionsManager.RegisterDynamicSuggestions(
        std::make_unique<TileDynamicSuggestions>(&tileManager));
    CommandManager commandManager;
    CommandExecutor commandExecutor;
    TilePlacerManager tilePlacerManager;
    ItemManager itemManager;
    dynamicSuggestionsManager.RegisterDynamicSuggestions(
        std::make_unique<ComposableItemDynamicSuggestions>(&itemManager));
    dynamicSuggestionsManager.RegisterDynamicSuggestions(
        std::make_unique<AbilityItemDynamicSuggestions>(&itemManager));
    tilePlacerManager.RegisterTilePlacer(std::make_unique<FillTilePlacer>());
    Config config;
    LogCat::i("Loading ",CONFIG_FILE_NAME, "...");
    auto configJsonOpt = JsonUtils::LoadJsonFromFile(&virtualFileSystem, CONFIG_FILE_NAME);
    if (!configJsonOpt.has_value()) {
        return EXIT_FAILURE;
    }

    dynamicSuggestionsManager.
            RegisterDynamicSuggestions(std::make_unique<ConfigSuggestions>(configJsonOpt.value()));
    if (!config.LoadConfig(configJsonOpt.value())) {
        LogCat::e("Failed to load ",CONFIG_FILE_NAME, " file!");
        return EXIT_FAILURE;
    }
    LogCat::i("windowHeight = ", config.window.height);
    LogCat::i("windowWidth = ", config.window.width);
    LogCat::i("dataPackPath = ", config.mods.dataPackPath);
    LogCat::i("resourcePackPath = ", config.mods.resourcePackPath);
    LogCat::i("framerate = ", config.window.framerate);
    LogCat::i("The ",CONFIG_FILE_NAME, " load was successful.");
    SavesManager savesManager(&virtualFileSystem);
    savesManager.LoadAllSaves();
    AppContext appContext(true, &sceneManager, &language, &dataPackManager, &resourcePackManager, &config,
                          &stringManager,
                          &commandManager,
                          &commandExecutor, &langsResources, &dynamicSuggestionsManager, &virtualFileSystem,
                          &tileManager,
                          &biomesManager, &tilePlacerManager, &resourceLocator, &itemManager, &savesManager);
    resourceLocator.SetAppContext(&appContext);
    commandManager.RegisterCommand(std::make_unique<GiveCommand>(&appContext));
    commandManager.RegisterCommand(std::make_unique<HelpCommand>(&appContext));
    commandManager.RegisterCommand(std::make_unique<TpCommand>(&appContext));
    commandManager.RegisterCommand(std::make_unique<HeightMapCommand>(&appContext, &virtualFileSystem));
    commandManager.RegisterCommand(std::make_unique<Box2DCommand>(&appContext));
    commandManager.RegisterCommand(std::make_unique<ConfigCommand>(&appContext, configJsonOpt.value()));
    commandManager.RegisterCommand(std::make_unique<AssetViewerCommand>(&appContext));
    commandManager.RegisterCommand(std::make_unique<VFSCommand>(&appContext, &virtualFileSystem));
    commandManager.RegisterCommand(std::make_unique<LicenseCommand>(&appContext, &virtualFileSystem));
    commandManager.RegisterCommand(std::make_unique<SeedCommand>(&appContext));
    LogCat::i("GAME_VERSION_NUMBER = ", GAME_VERSION_NUMBER);
    LogCat::i("GAME_VERSION_STRING = ", GAME_VERSION_STRING);
    LogCat::i("Starting GlimmerWorks...");
    if (dataPackManager.Scan(config.mods.dataPackPath, config.mods.enabledDataPack, language,
                             stringManager, tileManager, biomesManager, itemManager) == 0) {
        LogCat::e("Failed to load dataPack");
        return EXIT_FAILURE;
    }
    if (resourcePackManager.Scan(config.mods.resourcePackPath, config.mods.enabledResourcePack) == 0) {
        LogCat::e("Failed to load resourcePack");
        return EXIT_FAILURE;
    }
    LogCat::i("Starting the app...");
    App app(&appContext);
    if (!app.init()) {
        LogCat::e("Failed to init app");
        return EXIT_FAILURE;
    }
    app.run();
    return EXIT_SUCCESS;
}


#ifdef __ANDROID__
extern "C" {
int SDL_main(int argc, char *argv[]) {
    LogCat::i("SDL_main() called — entering main()");
    int result = main();
    LogCat::i("SDL_main() finished, result = ", result);
    return result;
}

//Set whether to allow the Activity to be recreated
//设置是否允许Activity被重新创建
JNIEXPORT jboolean

JNICALL
Java_org_libsdl_app_SDLActivity_nativeAllowRecreateActivity(JNIEnv *, jclass) {
    return JNI_TRUE;
}
}


#endif
