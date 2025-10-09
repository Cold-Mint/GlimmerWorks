#include <fstream>

#include "core/App.h"
#include "core/Config.h"
#include "core/log/LogCat.h"

using namespace Glimmer;


int main() {
    LogCat::i("Starting GlimmerWorks...");
    Config &config = Config::getInstance();
    LogCat::i("Loading config.json...");
    if (!config.loadConfig("config.json")) {
        LogCat::e("Failed to load config.json");
        return 1;
    }
    LogCat::i("The config.json load was successful.");
    App app;
    if (!app.init(config)) {
        return 1;
    }
    app.run();
    return 0;
}
