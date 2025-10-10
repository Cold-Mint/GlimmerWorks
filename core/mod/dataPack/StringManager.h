//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef STRINGMANAGER_H
#define STRINGMANAGER_H
#include <unordered_map>

#include "../Resource.h"


namespace Glimmer {
    class StringManager {
        StringManager() = default;

        ~StringManager() = default;

        std::unordered_map<std::string, std::unordered_map<std::string, StringResource> > stringMap{};

    public:
        //Register the string resource to the manager
        //注册字符串资源到管理器
        void registerResource(const StringResource &stringResource);

        //Search for string resources based on the data packet id and string key.
        //根据数据包id和字符串key查找字符串资源。
        StringResource *find(const std::string &packId, const std::string &key);

        static StringManager &getInstance() {
            static StringManager instance;
            return instance;
        }

        StringManager(const StringManager &) = delete;

        StringManager &operator=(const StringManager &) = delete;
    };
}


#endif //STRINGMANAGER_H
