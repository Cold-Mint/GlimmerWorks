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
        std::pmr::unordered_map<std::string, std::unordered_map<std::string, ComposableItemResource> >
        composableItemMap_{};

        std::pmr::unordered_map<std::string, std::unordered_map<std::string, AbilityItemResource> >
        abilityItemMap_{};

    public:
        void RegisterComposableResource(ComposableItemResource &itemResource);

        void RegisterAbilityItemResource(AbilityItemResource &itemResource);

        [[nodiscard]] ComposableItemResource *FindComposableItemResource(
            const std::string &packId, const std::string &key);

        [[nodiscard]] AbilityItemResource *FindAbilityItemResource(
            const std::string &packId, const std::string &key);


        [[nodiscard]] std::vector<std::string> GetComposableItemIDList();

        [[nodiscard]] std::vector<std::string> GetAbilityItemIDList();

        std::string ListComposableItems() const;

        std::string ListAbilityItems() const;
    };
}

#endif //GLIMMERWORKS_ITEMMANAGER_H
