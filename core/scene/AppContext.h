//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef APPCONTEXT_H
#define APPCONTEXT_H
#include <string>

#include "SceneManager.h"
#include "../Langs.h"
#include "../console/CommandExecutor.h"
#include "../console/CommandManager.h"
#include "../mod/dataPack/BiomesManager.h"
#include "../mod/dataPack/ItemManager.h"
#include "../mod/dataPack/TileManager.h"
#include "../mod/resourcePack/ResourcePackManager.h"
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
        TTF_Font *ttfFont_;
        CommandExecutor *commandExecutor_;
        ResourcePackManager *resourcePackManager_;
        LangsResources *langs_;
        DynamicSuggestionsManager *dynamicSuggestionsManager_;
        VirtualFileSystem *virtualFileSystem_;
        TilePlacerManager *tilePlacerManager_;
        ResourceLocator *resourceLocator_;
        ItemManager *itemManager_;
        bool isRunning;

    public:
        AppContext(const bool run, SceneManager *sm, std::string *lang, DataPackManager *dpm, ResourcePackManager *rpm,
                   Config *cfg,
                   StringManager *stringManager,
                   CommandManager *commandManager, CommandExecutor *commandExecutor, LangsResources *langs,
                   DynamicSuggestionsManager *dynamicSuggestionsManager, VirtualFileSystem *virtualFileSystem,
                   TileManager *tileManager, BiomesManager *biomesManager, TilePlacerManager *tilePlacerManager,
                   ResourceLocator *resourceLocator, ItemManager *itemManager)
            : language_(lang), dataPackManager_(dpm), config_(cfg), sceneManager_(sm), stringManager_(stringManager),
              commandManager_(commandManager), ttfFont_(nullptr), commandExecutor_(commandExecutor),
              resourcePackManager_(rpm), tilePlacerManager_(tilePlacerManager),
              window_(nullptr), langs_(langs), isRunning(run), dynamicSuggestionsManager_(dynamicSuggestionsManager),
              virtualFileSystem_(virtualFileSystem), tileManager_(tileManager), biomesManager_(biomesManager),
              resourceLocator_(resourceLocator), itemManager_(itemManager) {
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

        [[nodiscard]] SDL_Window *GetWindow() const;
    };
}


#endif //APPCONTEXT_H
