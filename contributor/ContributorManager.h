//
// Created by coldmint on 2026/3/1.
//

#ifndef GLIMMERWORKS_CONTRIBUTORMANAGER_H
#define GLIMMERWORKS_CONTRIBUTORMANAGER_H
#include <memory>
#include <string>
#include <unordered_map>

#include "Contributor.h"

namespace glimmer {
    class ContributorManager {
        std::unordered_map<std::string, std::unique_ptr<Contributor> > contributors;

    public:
        ContributorManager();

        bool Register(std::unique_ptr<Contributor> contributor);

        /**
         * Find
         * 查找贡献者信息
         * @param uuid
         * @return
         */
        Contributor* Find(const std::string& uuid) const;
    };
}


#endif //GLIMMERWORKS_CONTRIBUTORMANAGER_H
