//
// Created by coldmint on 2025/12/23.
//

#ifndef GLIMMERWORKS_ITEMMANAGER_H
#define GLIMMERWORKS_ITEMMANAGER_H
#include <string>
#include <unordered_map>

#include "../Resource.h"

namespace glimmer {
    class ItemManager {
        std::pmr::unordered_map<std::string, std::unordered_map<std::string, ItemResource> > itemMap_{};

    public:
        void RegisterResource(ItemResource &itemResource);

        [[nodiscard]] ItemResource *Find(const std::string &packId, const std::string &key);


        [[nodiscard]] std::vector<std::string> GetItemIDList();

        std::string ListItems() const;
    };
}

#endif //GLIMMERWORKS_ITEMMANAGER_H
