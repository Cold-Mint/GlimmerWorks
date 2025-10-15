#include <fstream>

#include "core/App.h"
#include "core/Config.h"
#include "core/console/CommandManager.h"
#include "core/console/command/HelpCommand.h"
#include "core/log/LogCat.h"
#include "core/mod/dataPack/DataPackManager.h"
#include "core/mod/dataPack/StringManager.h"
#include "core/mod/resourcePack/ResourcePackManager.h"
#include "core/scene/AppContext.h"
#include "core/scene/SceneManager.h"
#include "core/utils/LanguageUtils.h"

using namespace Glimmer;


int main() {
    std::set_terminate([]() {
        LogCat::e("Fatal error: unhandled exception!");
        std::abort();
    });
    try {
        std::string language = LanguageUtils::getLanguage();
        DataPackManager dataPackManager;
        ResourcePackManager resourcePackManager;
        SceneManager sceneManager;
        StringManager stringManager;
        CommandManager commandManager;
        CommandExecutor commandExecutor;
        Config config;
        LogCat::i("Loading ",CONFIG_FILE_NAME, "...");
        if (!config.loadConfig(CONFIG_FILE_NAME)) {
            return EXIT_FAILURE;
        }
        LogCat::i("windowHeight = ", config.window.height);
        LogCat::i("windowWidth = ", config.window.width);
        LogCat::i("dataPackPath = ", config.mods.dataPackPath);
        LogCat::i("resourcePackPath = ", config.mods.resourcePackPath);
        LogCat::i("The ",CONFIG_FILE_NAME, " load was successful.");
        commandManager.registerCommand(std::make_unique<HelpCommand>());
        AppContext appContext(&sceneManager, &language, &dataPackManager, &resourcePackManager, &config, &stringManager,
                              &commandManager,
                              &commandExecutor);
        LogCat::i("GAME_VERSION_NUMBER = ", GAME_VERSION_NUMBER);
        LogCat::i("GAME_VERSION_STRING = ", GAME_VERSION_STRING);
        LogCat::i("Starting GlimmerWorks...");
        if (dataPackManager.scan(config.mods.dataPackPath, config.mods.enabledDataPack, language, stringManager) == 0) {
            return EXIT_FAILURE;
        }
        if (resourcePackManager.scan(config.mods.resourcePackPath, config.mods.enabledResourcePack) == 0) {
            return EXIT_FAILURE;
        }
        LogCat::i("Starting the app...");
        App app(&appContext);
        if (!app.init()) {
            return EXIT_FAILURE;
        }
        app.run();
        return EXIT_SUCCESS;
    } catch (const std::exception &e) {
        LogCat::e("Unhandled exception: ", e.what());
    } catch (...) {
        LogCat::e("Unhandled unknown exception");
    }
    return EXIT_FAILURE;
}
