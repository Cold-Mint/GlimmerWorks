//
// Created by coldmint on 2025/11/28.
//

#include "VFSDynamicSuggestions.h"

#include "../../Constants.h"
#include "../../log/LogCat.h"

std::string glimmer::VFSDynamicSuggestions::GetId() const {
    return VFS_DYNAMIC_SUGGESTIONS_NAME;
}

std::vector<std::string> glimmer::VFSDynamicSuggestions::GetSuggestions(std::string param) {
    LogCat::d("VFSDynamicSuggestions key=" + param);
    std::string directory;
    std::string keyword;
    // 判断是否包含 '/'
    auto pos = param.find_last_of('/');
    if (pos != std::string::npos) {
        // 目录 = 最后一个 '/' 之前的部分
        directory = param.substr(0, pos);
        if (directory.empty()) directory = "/"; // 根目录情况

        // 关键字 = '/' 之后的部分
        keyword = param.substr(pos + 1);
    } else {
        // 不包含 '/'
        directory = "";
        keyword = param;
    }

    // 目录不存在 → 返回空
    if (!virtualFileSystem_->Exists(directory)) {
        return {};
    }

    // 列出目录下所有文件
    auto files = virtualFileSystem_->ListFile(directory);
    std::vector<std::string> result;

    // 根据关键字过滤（关键字为空则全部返回）
    for (auto &f: files) {
        if (keyword.empty() || f.find(keyword) != std::string::npos) {
            result.push_back(f);
        }
    }

    return result;
}
