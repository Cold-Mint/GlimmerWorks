//
// Created by coldmint on 2026/3/7.
//

#ifndef GLIMMERWORKS_SHAPEMANAGER_H
#define GLIMMERWORKS_SHAPEMANAGER_H
#include "core/mod/Resource.h"


namespace glimmer {
    class ShapeManager {
        std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<ShapeResource> > > shapeMap_
                {};
    public:
        ShapeManager();

        ShapeResource *Register(std::unique_ptr<ShapeResource> shapeResource);

        ShapeResource* FindShape(const std::string &packId, const std::string &key);

        std::string ListShapes() const;
    };
}

#endif //GLIMMERWORKS_SHAPEMANAGER_H
