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
#include <Windows.h>
#include <locale>
#endif

std::string glimmer::LanguageUtils::getLanguage()
{
#ifdef __ANDROID__
    JNIEnv* env = (JNIEnv*)SDL_GetAndroidJNIEnv();
    jobject activity = (jobject)SDL_GetAndroidActivity();
    jclass localeClass = env->FindClass("java/util/Locale");
    jmethodID getDefaultMethod = env->GetStaticMethodID(localeClass, "getDefault", "()Ljava/util/Locale;");
    jobject defaultLocale = env->CallStaticObjectMethod(localeClass, getDefaultMethod);

    jmethodID getLanguageMethod = env->GetMethodID(localeClass, "getLanguage", "()Ljava/lang/String;");
    jmethodID getCountryMethod = env->GetMethodID(localeClass, "getCountry", "()Ljava/lang/String;");

    jstring languageJ = (jstring)env->CallObjectMethod(defaultLocale, getLanguageMethod);
    jstring countryJ = (jstring)env->CallObjectMethod(defaultLocale, getCountryMethod);

    const char* languageC = env->GetStringUTFChars(languageJ, nullptr);
    const char* countryC = env->GetStringUTFChars(countryJ, nullptr);

    std::string result = std::string(languageC) + "_" + std::string(countryC);

    env->ReleaseStringUTFChars(languageJ, languageC);
    env->ReleaseStringUTFChars(countryJ, countryC);
    return result;
#elif defined(_WIN32) || defined(_WIN64)
    // 获取用户首选 UI 语言 ID (LANGID)，若失败则用系统区域 ID (LCID)
    LCID localeId = GetUserDefaultUILanguage(); // 注意：返回值是 LANGID，但可隐式转为 LCID
    if (localeId == 0)
    {
        localeId = GetSystemDefaultLCID();
    }

    // 使用 std::wstring 存储宽字符，先分配已知最大长度（ISO 代码为 2 字符 + 终止符）
    // 但为保证安全，先请求所需缓冲区大小（含终止符）
    int langLen = GetLocaleInfoW(localeId, LOCALE_SISO639LANGNAME, nullptr, 0);
    int countryLen = GetLocaleInfoW(localeId, LOCALE_SISO3166CTRYNAME, nullptr, 0);

    std::wstring langCode;
    std::wstring countryCode;

    if (langLen > 0)
    {
        langCode.resize(langLen); // 长度包含终止符
        if (GetLocaleInfoW(localeId, LOCALE_SISO639LANGNAME, &langCode[0], langLen) == 0)
        {
            langCode = L"en";
        }
    }
    else
    {
        langCode = L"en";
    }

    if (countryLen > 0)
    {
        countryCode.resize(countryLen);
        if (GetLocaleInfoW(localeId, LOCALE_SISO3166CTRYNAME, &countryCode[0], countryLen) == 0)
        {
            countryCode = L"US";
        }
    }
    else
    {
        countryCode = L"US";
    }

    // 辅助 lambda：将 std::wstring 转换为 UTF-8 std::string
    auto WideToUtf8 = [](const std::wstring& wstr) -> std::string
    {
        if (wstr.empty()) return {};
        int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (len <= 0) return {};
        std::string utf8(len, '\0');
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8[0], len, nullptr, nullptr);
        // 移除末尾的终止符（如果有）
        utf8.pop_back(); // len 包含终止符，所以转换后末尾是 '\0'，移除之
        return utf8;
    };

    std::string langStr = WideToUtf8(langCode);
    std::string countryStr = WideToUtf8(countryCode);

    return langStr + "_" + countryStr;
#else
    //linux
    if (const char* langEnv = getenv("LANG"); langEnv != nullptr)
    {
        auto language = std::string(langEnv);
        if (language.size() >= 5)
        {
            language = language.substr(0, 5);
        }
        return language;
    }
    return "en_US";
#endif
}
