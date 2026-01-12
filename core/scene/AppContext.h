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
#include "../console/CommandManager.h"
#include "../mod/dataPack/BiomesManager.h"
#include "../mod/dataPack/ItemManager.h"
#include "../mod/dataPack/TileManager.h"
#include "../mod/resourcePack/ResourcePackManager.h"
#include "../saves/SavesManager.h"
#include "../world/TilePlacerManager.h"
#include "SDL3_ttf/SDL_ttf.h"


namespace glimmer {
    class ResourceLocator;
    class DynamicSuggestionsManager;
    class StringManager;
    class Config;
    class DataPackManager;
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
        SDL_Window *window_;
        std::string *language_;
        DataPackManager *dataPackManager_;
        Config *config_;
        SceneManager *sceneManager_;
        StringManager *stringManager_;
        TileManager *tileManager_;
        BiomesManager *biomesManager_;
        CommandManager *commandManager_;
        SavesManager *savesManager_;
        TTF_Font *ttfFont_;
        CommandExecutor *commandExecutor_;
        ResourcePackManager *resourcePackManager_;
        LangsResources *langs_;
        DynamicSuggestionsManager *dynamicSuggestionsManager_;
        VirtualFileSystem *virtualFileSystem_;
        TilePlacerManager *tilePlacerManager_;
        ResourceLocator *resourceLocator_;
        ItemManager *itemManager_;
        std::mutex mainThreadMutex_;
        std::queue<std::function<void()> > mainThreadTasks_;
        bool isRunning = true;
        std::thread::id mainThreadId_;

    public:
        AppContext(SceneManager *sm, std::string *lang, DataPackManager *dpm, ResourcePackManager *rpm,
                   Config *cfg,
                   StringManager *stringManager,
                   CommandManager *commandManager, CommandExecutor *commandExecutor, LangsResources *langs,
                   DynamicSuggestionsManager *dynamicSuggestionsManager, VirtualFileSystem *virtualFileSystem,
                   TileManager *tileManager, BiomesManager *biomesManager, TilePlacerManager *tilePlacerManager,
                   ResourceLocator *resourceLocator, ItemManager *itemManager, SavesManager *savesManager)
            : window_(nullptr), language_(lang), dataPackManager_(dpm), config_(cfg), sceneManager_(sm),
              stringManager_(stringManager), tileManager_(tileManager), biomesManager_(biomesManager),
              commandManager_(commandManager), savesManager_(savesManager),
              ttfFont_(nullptr), commandExecutor_(commandExecutor), resourcePackManager_(rpm), langs_(langs),
              dynamicSuggestionsManager_(dynamicSuggestionsManager), virtualFileSystem_(virtualFileSystem),
              tilePlacerManager_(tilePlacerManager),
              resourceLocator_(resourceLocator), itemManager_(itemManager) {
            mainThreadId_ = std::this_thread::get_id();
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

        [[nodiscard]] std::string *GetLanguage() const;

        [[nodiscard]] LangsResources *GetLangsResources() const;

        [[nodiscard]] ResourcePackManager *GetResourcePackManager() const;

        [[nodiscard]] TileManager *GetTileManager() const;

        [[nodiscard]] TilePlacerManager *GetTilePlacerManager() const;

        [[nodiscard]] BiomesManager *GetBiomesManager() const;

        [[nodiscard]] ResourceLocator *GetResourceLocator() const;

        [[nodiscard]] VirtualFileSystem *GetVirtualFileSystem() const;

        [[nodiscard]] ItemManager *GetItemManager() const;

        [[nodiscard]] SceneManager *GetSceneManager() const;

        [[nodiscard]] SavesManager *GetSavesManager() const;

        [[nodiscard]] SDL_Window *GetWindow() const;

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
                    [func = std::forward<Func>(func), promise]() mutable { //skipcq: CXX-W1247
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
