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
        std::pmr::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<ComposableItemResource> > >
        composableItemMap_{};

        std::pmr::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<AbilityItemResource> > >
        abilityItemMap_{};

        static std::unique_ptr<ComposableItemResource> CreatePlaceholderComposableItemResource(
            const std::string &packId,
            const std::string &key);


        static std::unique_ptr<AbilityItemResource> CreateAbilityItemResource(
            const std::string &packId,
            const std::string &key);

    public:
        ComposableItemResource *AddComposableResource(std::unique_ptr<ComposableItemResource> itemResource);


        AbilityItemResource *AddAbilityItemResource(std::unique_ptr<AbilityItemResource> itemResource);

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
