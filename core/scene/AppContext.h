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
#include "core/inventory/DragAndDrop.h"
#include "core/mod/ResourceLocator.h"
#include "core/mod/dataPack/DataPackManager.h"
#include "core/mod/dataPack/StringManager.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "core/Config.h"
#include "core/GameUIMessage.h"
#include "core/lootTable/LootTableManager.h"
#include "core/mod/dataPack/StructureManager.h"
#include "core/world/generator/BiomeDecoratorManager.h"
#include "core/world/structure/StructureGeneratorManager.h"


namespace glimmer {
    class SavesManager;
    class ResourceLocator;
    class DynamicSuggestionsManager;
    class StringManager;
    class Config;
    class ResourcePackManager;
    class WorldContext;
    class DragAndDrop;
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
        SDL_Renderer *renderer_ = nullptr;
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
        std::unique_ptr<BiomeDecoratorManager> biomeDecoratorManager_;
        std::unique_ptr<LootTableManager> lootTableManager_;
        std::unique_ptr<StructureGeneratorManager> structureGeneratorManager_;
        std::unique_ptr<StructureManager> structureManager_;
        std::unique_ptr<ResourceLocator> resourceLocator_;
        std::unique_ptr<ItemManager> itemManager_;
        std::unique_ptr<DragAndDrop> dragAndDrop_;
        std::mutex mainThreadMutex_;
        std::queue<std::function<void()> > mainThreadTasks_;
        bool isRunning = true;
        std::thread::id mainThreadId_;
        toml::spec tomlVersion_ = toml::spec::v(1, 1, 0);
        std::vector<GameUIMessage> gameUIMessages_;

        void LoadLanguage(const std::string &data) const;

        static std::string GetTimeFileName(const std::string &prefix = "screenshot", const std::string &ext = ".png");

    public:
        AppContext();

        [[nodiscard]] const toml::spec &GetTomlVersion() const;

        ~AppContext();

        void SetWindow(SDL_Window *window);

        void SetRenderer(SDL_Renderer *renderer);

        void CreateScreenshot();

        void SetFont(TTF_Font *font);

        [[nodiscard]] bool Running() const;


        void AddUIMessage(const std::string &string);

        std::vector<GameUIMessage> &GetGameUIMessages();

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

        [[nodiscard]] DataPackManager *GetDataPackManager() const;

        [[nodiscard]] LootTableManager *GetLootTableManager() const;

        [[nodiscard]] StructureManager *GetStructureManager() const;

        [[nodiscard]] StructureGeneratorManager *GetStructureGeneratorManager() const;

        [[nodiscard]] BiomeDecoratorManager *GetBiomeDecoratorManager() const;

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
