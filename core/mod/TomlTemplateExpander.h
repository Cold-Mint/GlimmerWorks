//
// Created by coldmint on 2026/4/16.
//

#ifndef GLIMMERWORKS_TOMLTEMPLATEEXPANDER_H
#define GLIMMERWORKS_TOMLTEMPLATEEXPANDER_H
#include <memory>
#include <string>
#include <unordered_map>

#include "ITemplateCommand.h"


namespace glimmer {
    class TomlTemplateExpander {
        std::unordered_map<std::string_view, std::unique_ptr<ITemplateCommand> > commandMap_ = {};

    public:
        void Register(std::unique_ptr<ITemplateCommand> command);

        void Reset() const;

        /**
         * Expand
         * 展开模板
         * @param templateSearchPath templateSearchPath 模板搜索路径
         * @param inputText inputText 输入的文本（从文件读取的原始文本）
         * @param virtualFileSystem
         * @return
         */
        std::string Expand(const std::vector<std::string> &templateSearchPath, const std::string &inputText,
                           const VirtualFileSystem *virtualFileSystem) const;
    };
}


#endif //GLIMMERWORKS_TOMLTEMPLATEEXPANDER_H
