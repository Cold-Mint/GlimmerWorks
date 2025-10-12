//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef APPCONTEXT_H
#define APPCONTEXT_H
#include <string>

#include "SceneManager.h"
#include "../mod/dataPack/DataPackManager.h"


namespace Glimmer {
    /**
     * @class AppContext
     * @brief 应用上下文，管理共享资源和依赖。
     *
     * Application context that holds and provides shared resources
     * and dependencies to different modules.
     *
     */
    class AppContext {
    public:
        std::string *language;
        DataPackManager *dataPackManager;
        Config *config;
        SceneManager *sceneManager;
        StringManager *stringManager;

        AppContext(SceneManager *sm, std::string *lang, DataPackManager *dpm, Config *cfg, StringManager *stringManager)
            : language(lang), dataPackManager(dpm), config(cfg), sceneManager(sm), stringManager(stringManager) {
        }
    };
}


#endif //APPCONTEXT_H
