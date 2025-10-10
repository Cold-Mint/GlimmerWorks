//
// Created by Cold-Mint on 2025/10/10.
//
#include "LanguageUtils.h"

std::string Glimmer::LanguageUtils::getLanguage() const {
    return currentLanguage;
}

Glimmer::LanguageUtils::LanguageUtils() {
#ifdef __linux__
    if (const char *langEnv = std::getenv("LANG"); langEnv != nullptr) {
        currentLanguage = std::string(langEnv);
        if (currentLanguage.size() >= 5) {
            currentLanguage = currentLanguage.substr(0, 5);
        }
        return;
    }
#endif
    currentLanguage = "en_US";
}
