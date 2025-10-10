//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef LANGUAGEUTILS_H
#define LANGUAGEUTILS_H
#include <string>


namespace Glimmer {
    class LanguageUtils {
    public:
        static LanguageUtils &getInstance() {
            static LanguageUtils instance;
            return instance;
        }


        [[nodiscard]] std::string getLanguage() const;

        LanguageUtils(const LanguageUtils &) = delete;

        LanguageUtils &operator=(const LanguageUtils &) = delete;

    private:
        LanguageUtils();

        ~LanguageUtils() = default;

        std::string currentLanguage;
    };
}


#endif //LANGUAGEUTILS_H
