//
// Created by coldmint on 2026/4/1.
//

#ifndef GLIMMERWORKS_STRINGUTILS_H
#define GLIMMERWORKS_STRINGUTILS_H
#include <string>


namespace glimmer {
    class StringUtils {
    public:
        /**
         * Convert any UTF-8 string to a cross-platform secure archive directory name (in English + numbers)
         * 将任意 UTF-8 字符串 转换为 跨平台安全的存档目录名（英文+数字）
         * @param utf8Str
         * @return
         */
        static std::string ToSafeSaveName(const std::string &utf8Str);
    };
}


#endif //GLIMMERWORKS_STRINGUTILS_H
