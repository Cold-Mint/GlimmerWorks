#include <fstream>

#include "core/App.h"
#include "core/Config.h"
#include "core/log/LogCat.h"

using namespace Glimmer;


int main() {
    std::set_terminate([]() {
        LogCat::e("Fatal error: unhandled exception!");
        std::abort();
    });
    try {
        LogCat::i("Starting GlimmerWorks...");
        Config &config = Config::getInstance();
        LogCat::i("Loading config.json...");
        if (!config.loadConfig("config.json")) {
            return EXIT_FAILURE;
        }
        LogCat::i("The config.json load was successful.");
        LogCat::d("windowHeight = ", config.window.height);
        LogCat::d("windowWidth = ", config.window.width);
        LogCat::d("dataPackPath = ", config.mods.dataPackPath);
        LogCat::d("resourcePackPath = ", config.mods.resourcePackPath);

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
