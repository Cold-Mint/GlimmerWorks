//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef STRINGMANAGER_H
#define STRINGMANAGER_H
#include <unordered_map>

#include "../Resource.h"


namespace glimmer {
    struct StringResource;

    class StringManager {
        std::unordered_map<std::string, std::unordered_map<std::string, StringResource> > stringMap_{};

    public:
        //Register the string resource to the manager
        //注册字符串资源到管理器
        void RegisterResource(const StringResource &stringResource);

        //Search for string resources based on the data packet id and string key.
        //根据数据包id和字符串key查找字符串资源。
        StringResource *Find(const std::string &packId, const std::string &key);

        /**
         * List all the string resources
         * 列出所有的字符串资源
         * @return
         */
        std::string ListStrings() const;
    };
}


#endif //STRINGMANAGER_H
