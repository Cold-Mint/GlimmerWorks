#include <fstream>

#include "core/App.h"
#include "core/Config.h"
#include "core/console/command/ConfigCommand.h"
#include "core/log/LogCat.h"
#include "core/mod/resourcePack/ResourcePackManager.h"
#include "core/scene/AppContext.h"
#include "core/scene/SceneManager.h"
#include "fmt/args.h"

#ifdef __ANDROID__
#include <jni.h>
#include <android/asset_manager_jni.h>
#include <SDL3/SDL_system.h>
#include "core/vfs/AndroidAssetsFileProvider.h"
#endif

using namespace glimmer;
namespace fs = std::filesystem;

int main() {
    std::set_terminate([] {
        try {
            if (std::current_exception()) {
                std::rethrow_exception(std::current_exception());
            }
        } catch (const std::exception &e) {
            const std::string msg =
                    std::string("Oops! An unexpected error occurred in GlimmerWorks.\n\n") +
                    "Error details:\n" + e.what() + "\n\n" +
                    "The application needs to close.";

            SDL_ShowSimpleMessageBox(
                SDL_MESSAGEBOX_ERROR,
                "GlimmerWorks - Fatal Error",
                msg.c_str(),
                nullptr
            );
        } catch (...) {
            std::cerr << "Unhandled non-standard exception" << std::endl;
        }
        std::abort();
    });

    AppContext appContext;
    App app(&appContext);
    if (!app.Init()) {
        LogCat::e("Failed to init app");
        return EXIT_FAILURE;
    }
    app.Run();
    return EXIT_SUCCESS;
}


#ifdef __ANDROID__
extern "C" {
int SDL_main(int argc, char *argv[]) {
    LogCat::i("SDL_main() called — entering main()");
    int result = main();
    LogCat::i("SDL_main() finished, result = ", result);
    return result;
}

//Set whether to allow the Activity to be recreated
//设置是否允许Activity被重新创建
JNIEXPORT jboolean

JNICALL
Java_org_libsdl_app_SDLActivity_nativeAllowRecreateActivity(JNIEnv *, jclass) {
    return JNI_TRUE;
}
}


#endif
