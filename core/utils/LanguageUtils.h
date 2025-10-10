//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef LANGUAGEUTILS_H
#define LANGUAGEUTILS_H
#include <string>


namespace Glimmer {
    class LanguageUtils {
    public:
        static std::string getLanguage() {
            if (const char *langEnv = std::getenv("LANG"); langEnv != nullptr) {
                auto language = std::string(langEnv);
                if (language.size() >= 5) {
                    language = language.substr(0, 5);
                }
                return language;
            }
            return "en_US";
        }
    };
}


#endif //LANGUAGEUTILS_H
