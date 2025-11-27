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
#include "../mod/resourcePack/ResourcePackManager.h"
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
        bool debugMode_ = false;

    public:
        DataPackManager *dataPackManager;
        Config *config;
        SceneManager *sceneManager;
        StringManager *stringManager;
        CommandManager *commandManager;
        TTF_Font *ttfFont;
        CommandExecutor *commandExecutor;
        ResourcePackManager *resourcePackManager;
        Langs *langs;
        DynamicSuggestionsManager *dynamicSuggestionsManager;
        bool isRunning;
        /**
         * Whether to draw screen coordinates
         * 是否绘制屏幕坐标
         */
        bool debugScreenCoords = true;

        AppContext(bool run, SceneManager *sm, std::string *lang, DataPackManager *dpm, ResourcePackManager *rpm,
                   Config *cfg,
                   StringManager *stringManager,
                   CommandManager *commandManager, CommandExecutor *commandExecutor, Langs *langs,
                   DynamicSuggestionsManager *dynamicSuggestionsManager)
            : language_(lang), dataPackManager(dpm), config(cfg), sceneManager(sm), stringManager(stringManager),
              commandManager(commandManager), ttfFont(nullptr), commandExecutor(commandExecutor),
              resourcePackManager(rpm),
              window_(nullptr), langs(langs), isRunning(run), dynamicSuggestionsManager(dynamicSuggestionsManager) {
        }

        void SetDebugMode(bool isDebug);

        [[nodiscard]] bool isDebugMode() const;

        [[nodiscard]] std::string *GetLanguage() const;

        void SetWindow(SDL_Window *window);

        [[nodiscard]] SDL_Window *GetWindow() const;
    };
}


#endif //APPCONTEXT_H
