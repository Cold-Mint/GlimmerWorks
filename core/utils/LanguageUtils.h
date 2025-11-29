//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef LANGUAGEUTILS_H
#define LANGUAGEUTILS_H
#include <string>
#include <iomanip>
#ifdef __ANDROID__
#include <jni.h>
#include <android/asset_manager_jni.h>
#include <SDL3/SDL_system.h>
#endif

namespace glimmer {
    class LanguageUtils {
    public:
        static std::string getLanguage() {
            #ifdef __ANDROID__
              JNIEnv* env = (JNIEnv*)SDL_GetAndroidJNIEnv();
        jobject activity = (jobject) SDL_GetAndroidActivity();
        jclass localeClass = env->FindClass("java/util/Locale");
        jmethodID getDefaultMethod = env->GetStaticMethodID(localeClass, "getDefault", "()Ljava/util/Locale;");
        jobject defaultLocale = env->CallStaticObjectMethod(localeClass, getDefaultMethod);

        jmethodID getLanguageMethod = env->GetMethodID(localeClass, "getLanguage", "()Ljava/lang/String;");
        jmethodID getCountryMethod  = env->GetMethodID(localeClass, "getCountry",  "()Ljava/lang/String;");

        jstring languageJ = (jstring)env->CallObjectMethod(defaultLocale, getLanguageMethod);
        jstring countryJ  = (jstring)env->CallObjectMethod(defaultLocale, getCountryMethod);

        const char* languageC = env->GetStringUTFChars(languageJ, nullptr);
        const char* countryC  = env->GetStringUTFChars(countryJ, nullptr);

        std::string result = std::string(languageC) + "_" + std::string(countryC);

        env->ReleaseStringUTFChars(languageJ, languageC);
        env->ReleaseStringUTFChars(countryJ, countryC);
        return result;
            #else
            //linux
            if (const char *langEnv = getenv("LANG"); langEnv != nullptr) {
                auto language = std::string(langEnv);
                if (language.size() >= 5) {
                    language = language.substr(0, 5);
                }
                return language;
            }
            #endif
            return "en_US";
        }
    };
}


#endif //LANGUAGEUTILS_H
