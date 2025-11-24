#include <fstream>

#include "core/App.h"
#include "core/Config.h"
#include "core/Langs.h"
#include "core/console/CommandManager.h"
#include "core/console/command/Box2DCommand.h"
#include "core/console/command/HelpCommand.h"
#include "core/console/command/TpCommand.h"
#include "core/log/LogCat.h"
#include "core/mod/dataPack/DataPackManager.h"
#include "core/mod/dataPack/StringManager.h"
#include "core/mod/resourcePack/ResourcePackManager.h"
#include "core/scene/AppContext.h"
#include "core/scene/SceneManager.h"
#include "core/utils/JsonUtils.h"
#include "core/utils/LanguageUtils.h"
#include "fmt/args.h"


using namespace glimmer;
namespace fs = std::filesystem;

int main()
{
    std::set_terminate([]()
    {
        LogCat::e("Fatal error: unhandled exception!");
        std::abort();
    });
    try
    {
        std::string language = LanguageUtils::getLanguage();
        LogCat::i("Load the built-in language file.");
        fs::path langFile = fs::path("langs") / (language + ".json");
        LogCat::i("Try to load language file:", langFile.c_str());
        if (!fs::exists(langFile))
        {
            LogCat::w("Not found, fall back to default.json");
            langFile = fs::path("langs/default.json");
        }
        auto jsonOpt = JsonUtils::LoadJsonFromFile(langFile.string());
        if (!jsonOpt)
        {
            LogCat::e("Failed to load any language file!");
            return EXIT_FAILURE;
        }
        auto& jsonObject = *jsonOpt;
        Langs langs;
        langs.startGame = jsonObject["startGame"].get<std::string>();
        langs.settings = jsonObject["settings"].get<std::string>();
        langs.mods = jsonObject["mods"].get<std::string>();
        langs.exitGame = jsonObject["exitGame"].get<std::string>();
        langs.console = jsonObject["console"].get<std::string>();
        langs.commandNotFound = jsonObject["commandNotFound"].get<std::string>();
        langs.executionFailed = jsonObject["executionFailed"].get<std::string>();
        langs.executedSuccess = jsonObject["executedSuccess"].get<std::string>();
        langs.commandIsEmpty = jsonObject["commandIsEmpty"].get<std::string>();
        langs.createWorld = jsonObject["createWorld"].get<std::string>();
        langs.cancel = jsonObject["cancel"].get<std::string>();
        langs.worldName = jsonObject["worldName"].get<std::string>();
        langs.seed = jsonObject["seed"].get<std::string>();
        langs.random = jsonObject["random"].get<std::string>();
        langs.commandInfo = jsonObject["commandInfo"].get<std::string>();
        langs.awakeBodyCount = jsonObject["awakeBodyCount"].get<std::string>();
        DataPackManager dataPackManager;
        ResourcePackManager resourcePackManager;
        SceneManager sceneManager;
        StringManager stringManager;
        CommandManager commandManager;
        CommandExecutor commandExecutor;
        Config config;
        LogCat::i("Loading ",CONFIG_FILE_NAME, "...");
        if (!config.loadConfig(CONFIG_FILE_NAME))
        {
            return EXIT_FAILURE;
        }
        LogCat::i("windowHeight = ", config.window.height);
        LogCat::i("windowWidth = ", config.window.width);
        LogCat::i("dataPackPath = ", config.mods.dataPackPath);
        LogCat::i("resourcePackPath = ", config.mods.resourcePackPath);
        LogCat::i("framerate = ", config.window.framerate);
        LogCat::i("The ",CONFIG_FILE_NAME, " load was successful.");
        AppContext appContext(true, &sceneManager, &language, &dataPackManager, &resourcePackManager, &config,
                              &stringManager,
                              &commandManager,
                              &commandExecutor, &langs);
        commandManager.RegisterCommand(std::make_unique<HelpCommand>(&appContext));
        commandManager.RegisterCommand(std::make_unique<TpCommand>(&appContext));
        commandManager.RegisterCommand(std::make_unique<Box2DCommand>(&appContext));
        LogCat::i("GAME_VERSION_NUMBER = ", GAME_VERSION_NUMBER);
        LogCat::i("GAME_VERSION_STRING = ", GAME_VERSION_STRING);
        LogCat::i("Starting GlimmerWorks...");
        if (dataPackManager.Scan(config.mods.dataPackPath, config.mods.enabledDataPack, language, stringManager) == 0)
        {
            return EXIT_FAILURE;
        }
        if (resourcePackManager.Scan(config.mods.resourcePackPath, config.mods.enabledResourcePack) == 0)
        {
            return EXIT_FAILURE;
        }
        LogCat::i("Starting the app...");
        App app(&appContext);
        if (!app.init())
        {
            return EXIT_FAILURE;
        }
        app.run();
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        LogCat::e("Unhandled exception: ", e.what());
    }
    catch (...)
    {
        LogCat::e("Unhandled unknown exception");
    }
    return EXIT_FAILURE;
}


#ifdef __ANDROID__
extern "C" int SDL_main(int argc, char* argv[])
{
    LogCat::i("SDL_main() called — entering main()");
    int result = main();
    LogCat::i("SDL_main() finished, result = ", result);
    return result;
}
#endif
