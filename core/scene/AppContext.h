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
#include "../mod/dataPack/TileManager.h"
#include "../mod/resourcePack/ResourcePackManager.h"
#include "../world/TilePlacerManager.h"
#include "SDL3_ttf/SDL_ttf.h"


namespace glimmer {
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

    public:
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
        Langs *langs_;
        DynamicSuggestionsManager *dynamicSuggestionsManager_;
        VirtualFileSystem *virtualFileSystem_;
        TilePlacerManager *tilePlacerManager_;
        bool isRunning;

        AppContext(const bool run, SceneManager *sm, std::string *lang, DataPackManager *dpm, ResourcePackManager *rpm,
                   Config *cfg,
                   StringManager *stringManager,
                   CommandManager *commandManager, CommandExecutor *commandExecutor, Langs *langs,
                   DynamicSuggestionsManager *dynamicSuggestionsManager, VirtualFileSystem *virtualFileSystem,
                   TileManager *tileManager, BiomesManager *biomesManager, TilePlacerManager *tilePlacerManager)
            : language_(lang), dataPackManager_(dpm), config_(cfg), sceneManager_(sm), stringManager_(stringManager),
              commandManager_(commandManager), ttfFont_(nullptr), commandExecutor_(commandExecutor),
              resourcePackManager_(rpm), tilePlacerManager_(tilePlacerManager),
              window_(nullptr), langs_(langs), isRunning(run), dynamicSuggestionsManager_(dynamicSuggestionsManager),
              virtualFileSystem_(virtualFileSystem), tileManager_(tileManager), biomesManager_(biomesManager) {
        }

        [[nodiscard]] std::string *GetLanguage() const;

        void SetWindow(SDL_Window *window);

        [[nodiscard]] SDL_Window *GetWindow() const;
    };
}


#endif //APPCONTEXT_H
