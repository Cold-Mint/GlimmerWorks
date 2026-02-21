//
// Created by coldmint on 2026/2/21.
//

#ifndef GLIMMERWORKS_INITIALINVENTORYMANAGER_H
#define GLIMMERWORKS_INITIALINVENTORYMANAGER_H
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

        std::string ListInitialInventory() const;
    };
}

#endif //GLIMMERWORKS_INITIALINVENTORYMANAGER_H
