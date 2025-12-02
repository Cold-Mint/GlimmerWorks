//
// Created by Cold-Mint on 2025/11/27.
//

#ifndef GLIMMERWORKS_BOOLDYNAMICSUGGESTIONS_H
#define GLIMMERWORKS_BOOLDYNAMICSUGGESTIONS_H
#include "DynamicSuggestions.h"

namespace glimmer {
    class BoolDynamicSuggestions final : public DynamicSuggestions {
    public:
        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;

        /**
         * Determine whether the input content matches this suggestion, and when obtaining the suggestion list, get the subtree
         * 判断输入的内容是否匹配此建议，用于获取建议列表时，获取子树
         * @param keyword
         * @param param
         * @return
         */
        bool Match(std::string keyword, std::string param) override;
    };
}

#endif //GLIMMERWORKS_BOOLDYNAMICSUGGESTIONS_H
