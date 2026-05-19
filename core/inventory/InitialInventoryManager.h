//
// Created by Cold-Mint on 2026/2/21.
//

#pragma once
#include <memory>
#include <string>
#include "core/mod/Resource.h"

namespace glimmer {
    class InitialInventoryManager {
        std::vector<std::unique_ptr<InitialInventoryResource> > initialInventory_;
        std::vector<InitialInventoryResource *> initialInventoryPtr_;

    public:
        void AddResource(std::unique_ptr<InitialInventoryResource> resource);

        [[nodiscard]] const std::vector<InitialInventoryResource *> &GetAllInitialInventory() const;

        [[nodiscard]] std::string ListInitialInventory() const;
    };
}
