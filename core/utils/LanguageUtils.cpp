//
// Created by Cold-Mint on 2025/10/10.
//
#include "LanguageUtils.h"
#include <iomanip>
#ifdef __ANDROID__
#include <jni.h>
#include <android/asset_manager_jni.h>
#include <SDL3/SDL_system.h>
#endif
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <locale>
#endif

std::string glimmer::LanguageUtils::getLanguage() {
#ifdef __ANDROID__
    JNIEnv *env = (JNIEnv *) SDL_GetAndroidJNIEnv();
    jobject activity = (jobject) SDL_GetAndroidActivity();
    jclass localeClass = env->FindClass("java/util/Locale");
    jmethodID getDefaultMethod = env->GetStaticMethodID(localeClass, "getDefault", "()Ljava/util/Locale;");
    jobject defaultLocale = env->CallStaticObjectMethod(localeClass, getDefaultMethod);

    jmethodID getLanguageMethod = env->GetMethodID(localeClass, "getLanguage", "()Ljava/lang/String;");
    jmethodID getCountryMethod = env->GetMethodID(localeClass, "getCountry", "()Ljava/lang/String;");

    jstring languageJ = (jstring) env->CallObjectMethod(defaultLocale, getLanguageMethod);
    jstring countryJ = (jstring) env->CallObjectMethod(defaultLocale, getCountryMethod);

    const char *languageC = env->GetStringUTFChars(languageJ, nullptr);
    const char *countryC = env->GetStringUTFChars(countryJ, nullptr);

    std::string result = std::string(languageC) + "_" + std::string(countryC);

    env->ReleaseStringUTFChars(languageJ, languageC);
    env->ReleaseStringUTFChars(countryJ, countryC);
    return result;
#elif defined(_WIN32) || defined(_WIN64)
    LCID localeId = GetUserDefaultUILanguage();
    if (localeId == 0) {
        localeId = GetSystemDefaultLCID();
    }

    WCHAR langCode[3] = {0};
    WCHAR countryCode[3] = {0};
    
    if (!GetLocaleInfoW(localeId, LOCALE_SISO639LANGNAME, langCode, _countof(langCode))) {
        wcscpy_s(langCode, L"en");
    }
    if (!GetLocaleInfoW(localeId, LOCALE_SISO3166CTRYNAME, countryCode, _countof(countryCode))) {
        wcscpy_s(countryCode, L"US");
    }

    int langLen = WideCharToMultiByte(CP_UTF8, 0, langCode, -1, nullptr, 0, nullptr, nullptr);
    int countryLen = WideCharToMultiByte(CP_UTF8, 0, countryCode, -1, nullptr, 0, nullptr, nullptr);
    
    std::string langStr(langLen, 0);
    std::string countryStr(countryLen, 0);
    
    WideCharToMultiByte(CP_UTF8, 0, langCode, -1, &langStr[0], langLen, nullptr, nullptr);
    WideCharToMultiByte(CP_UTF8, 0, countryCode, -1, &countryStr[0], countryLen, nullptr, nullptr);

    langStr = langStr.substr(0, langStr.find_first_of('\0'));
    countryStr = countryStr.substr(0, countryStr.find_first_of('\0'));

    return langStr + "_" + countryStr;
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
