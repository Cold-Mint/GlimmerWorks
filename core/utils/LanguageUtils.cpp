/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
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
