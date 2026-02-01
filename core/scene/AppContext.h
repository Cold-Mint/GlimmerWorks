//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef APPCONTEXT_H
#define APPCONTEXT_H
#include <future>
#include <queue>
#include <string>

#include "SceneManager.h"
#include "../Langs.h"
#include "../console/CommandExecutor.h"
#include "../mod/dataPack/BiomesManager.h"
#include "../mod/dataPack/ItemManager.h"
#include "../mod/dataPack/TileManager.h"
#include "../mod/resourcePack/ResourcePackManager.h"
#include "../saves/SavesManager.h"
#include "../world/TilePlacerManager.h"
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
#include "core/inventory/DragAndDrop.h"
#include "core/log/LogCat.h"
#include "core/mod/ResourceLocator.h"
#include "core/mod/dataPack/DataPackManager.h"
#include "core/mod/dataPack/StringManager.h"
#include "core/utils/LanguageUtils.h"
#include "core/vfs/StdFileProvider.h"
#include "core/world/FillTilePlacer.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "cmake-build-debug/_deps/toml11-src/include/toml.hpp"
#include "core/Config.h"
#include "core/console/command/ClearCommand.h"
#include "core/console/command/ConfigCommand.h"
#include "core/console/command/LootCommand.h"
#include "core/console/suggestion/ConfigSuggestions.h"
#include "core/console/suggestion/LootSuggestions.h"
#include "core/lootTable/LootTableManager.h"


namespace glimmer {
    class ResourceLocator;
    class DynamicSuggestionsManager;
    class StringManager;
    class Config;
    class ResourcePackManager;
    /**
     * @class AppContext
     * @brief 应用上下文，管理共享资源和依赖。
     *
     * Application context that holds and provides shared resources
     * and dependencies to different modules.
     *
     */
    class AppContext {
        SDL_Window *window_ = nullptr;
        std::string language_;
        std::unique_ptr<DataPackManager> dataPackManager_;
        std::unique_ptr<Config> config_;
        std::unique_ptr<toml::value> configValue;
        std::unique_ptr<SceneManager> sceneManager_;
        std::unique_ptr<StringManager> stringManager_;
        std::unique_ptr<TileManager> tileManager_;
        std::unique_ptr<BiomesManager> biomesManager_;
        std::unique_ptr<CommandManager> commandManager_;
        std::unique_ptr<SavesManager> savesManager_;
        TTF_Font *ttfFont_ = nullptr;
        std::unique_ptr<CommandExecutor> commandExecutor_;
        std::unique_ptr<ResourcePackManager> resourcePackManager_;
        std::unique_ptr<LangsResources> langs_;
        std::unique_ptr<DynamicSuggestionsManager> dynamicSuggestionsManager_;
        std::unique_ptr<VirtualFileSystem> virtualFileSystem_;
        std::unique_ptr<TilePlacerManager> tilePlacerManager_;
        std::unique_ptr<LootTableManager> lootTableManager_;
        std::unique_ptr<ResourceLocator> resourceLocator_;
        std::unique_ptr<ItemManager> itemManager_;
        std::unique_ptr<DragAndDrop> dragAndDrop_;
        std::mutex mainThreadMutex_;
        std::queue<std::function<void()> > mainThreadTasks_;
        bool isRunning = true;
        std::thread::id mainThreadId_;
        toml::spec tomlVersion_ = toml::spec::v(1, 1, 0);

        void LoadLanguage(const std::string &data) const;

    public:
        AppContext() {
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
            virtualFileSystem->Mount(
                std::make_unique<StdFileProvider>(dataDirPath + "/assets"));
            virtualFileSystem->Mount(std::unique_ptr<FileProvider>(std::move(assetsProvider)));
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
            commandManager_->RegisterCommand(std::make_unique<HeightMapCommand>(this, vfs));
            commandManager_->RegisterCommand(std::make_unique<Box2DCommand>(this));
            commandManager_->RegisterCommand(std::make_unique<AssetViewerCommand>(this));
            commandManager_->RegisterCommand(std::make_unique<ClearCommand>(this));
            commandManager_->RegisterCommand(std::make_unique<LootCommand>(this));
            commandManager_->RegisterCommand(std::make_unique<VFSCommand>(this, vfs));
            commandManager_->RegisterCommand(std::make_unique<LicenseCommand>(this, vfs));
            commandManager_->RegisterCommand(std::make_unique<SeedCommand>(this));

            commandExecutor_ = std::make_unique<CommandExecutor>();
            tilePlacerManager_ = std::make_unique<TilePlacerManager>();
            itemManager_ = std::make_unique<ItemManager>();
            ItemManager *im = itemManager_.get();
            dynamicSuggestionsManager_->RegisterDynamicSuggestions(
                std::make_unique<ComposableItemDynamicSuggestions>(im));
            dynamicSuggestionsManager_->RegisterDynamicSuggestions(
                std::make_unique<AbilityItemDynamicSuggestions>(im));
            tilePlacerManager_->RegisterTilePlacer(std::make_unique<FillTilePlacer>());
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
            dynamicSuggestionsManager_->RegisterDynamicSuggestions(
                std::make_unique<LootSuggestions>(lootTableManager_.get()));
            LogCat::i("GAME_VERSION_NUMBER = ", GAME_VERSION_NUMBER);
            LogCat::i("GAME_VERSION_STRING = ", GAME_VERSION_STRING);
            LogCat::i("Starting GlimmerWorks...");
            if (dataPackManager_->Scan(config_->mods.dataPackPath, config_->mods.enabledDataPack, language_,
                                       stringManager_.get(), tileManager_.get(), biomesManager_.get(),
                                       itemManager_.get(), lootTableManager_.get(), tomlVersion_) == 0) {
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

        [[nodiscard]] const toml::spec &GetTomlVersion() const;

        ~AppContext() {
            sceneManager_->ClearScenes();
        }

        void SetWindow(SDL_Window *window);

        void SetFont(TTF_Font *font);

        [[nodiscard]] bool Running() const;

        void ExitApp();

        [[nodiscard]] Config *GetConfig() const;

        [[nodiscard]] TTF_Font *GetFont() const;

        [[nodiscard]] CommandManager *GetCommandManager() const;

        [[nodiscard]] StringManager *GetStringManager() const;

        [[nodiscard]] DynamicSuggestionsManager *GetDynamicSuggestionsManager() const;

        [[nodiscard]] const std::string &GetLanguage();

        [[nodiscard]] LangsResources *GetLangsResources() const;

        [[nodiscard]] ResourcePackManager *GetResourcePackManager() const;

        [[nodiscard]] TileManager *GetTileManager() const;

        [[nodiscard]] LootTableManager *GetLootTableManager() const;

        [[nodiscard]] TilePlacerManager *GetTilePlacerManager() const;

        [[nodiscard]] BiomesManager *GetBiomesManager() const;

        [[nodiscard]] ResourceLocator *GetResourceLocator() const;

        [[nodiscard]] VirtualFileSystem *GetVirtualFileSystem() const;

        [[nodiscard]] ItemManager *GetItemManager() const;

        [[nodiscard]] SceneManager *GetSceneManager() const;

        [[nodiscard]] SavesManager *GetSavesManager() const;

        [[nodiscard]] SDL_Window *GetWindow() const;

        [[nodiscard]] DragAndDrop *GetDragAndDrop() const;

        [[nodiscard]] bool IsMainThread() const;

        void ProcessMainThreadTasks();

        /**
         * Execute a function on the main thread. For example: Loading textures must be done in the main thread.
         * 在主线程上执行一个函数。例如：加载纹理，必须在主线程。
         * @tparam Func
         * @param func
         * @return
         */
        template<typename Func>
        auto AddMainThreadTaskAwait(Func &&func)
            -> std::future<std::invoke_result_t<Func> > {
            using Result = std::invoke_result_t<Func>;
            if (IsMainThread()) {
                std::promise<Result> p;
                try {
                    if constexpr (std::is_void_v<Result>) {
                        func();
                        p.set_value();
                    } else {
                        p.set_value(func());
                    }
                } catch (...) {
                    p.set_exception(std::current_exception());
                }
                return p.get_future();
            }
            auto promise = std::make_shared<std::promise<Result> >();
            auto future = promise->get_future();
            {
                std::lock_guard lock(mainThreadMutex_);
                mainThreadTasks_.push(
                    [func = std::forward<Func>(func), promise]() mutable {
                        //skipcq: CXX-W1247
                        try {
                            if constexpr (std::is_void_v<Result>) {
                                func();
                                promise->set_value();
                            } else {
                                promise->set_value(func());
                            }
                        } catch (...) {
                            promise->set_exception(std::current_exception());
                        }
                    }
                );
            }

            return future;
        }

        /**
         * Restore the color of the renderer.
         * 恢复渲染器的颜色。
         */
        static void RestoreColorRenderer(SDL_Renderer *sdlRenderer);


        void AddMainThreadTask(std::function<void()> task);
    };
}


#endif //APPCONTEXT_H
