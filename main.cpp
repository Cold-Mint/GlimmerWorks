#include <fstream>

#include "core/App.h"
#include "core/Config.h"
#include "core/log/LogCat.h"
#include "core/mod/dataPack/DataPackManager.h"
#include "core/utils/LanguageUtils.h"

using namespace Glimmer;


int main() {
    std::set_terminate([]() {
        LogCat::e("Fatal error: unhandled exception!");
        std::abort();
    });
    try {
        const LanguageUtils &languageUtils = LanguageUtils::getInstance();
        LogCat::i("System language: ", languageUtils.getLanguage());
        LogCat::i("GAME_VERSION_NUMBER = ", GAME_VERSION_NUMBER);
        LogCat::i("GAME_VERSION_STRING = ", GAME_VERSION_STRING);
        LogCat::i("Starting GlimmerWorks...");
        Config &config = Config::getInstance();
        LogCat::i("Loading ",CONFIG_FILE_NAME, "...");
        if (!config.loadConfig(CONFIG_FILE_NAME)) {
            return EXIT_FAILURE;
        }
        LogCat::i("The ",CONFIG_FILE_NAME, " load was successful.");

        LogCat::i("windowHeight = ", config.window.height);
        LogCat::i("windowWidth = ", config.window.width);
        LogCat::i("dataPackPath = ", config.mods.dataPackPath);
        LogCat::i("resourcePackPath = ", config.mods.resourcePackPath);

        if (DataPackManager::scan(config.mods.dataPackPath) == 0) {
            return EXIT_FAILURE;
        }
        LogCat::i("Starting the app...");
        App app;
        if (!app.init(config)) {
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
