//
// Created by Cold-Mint on 2026/3/7.
//

#ifndef GLIMMERWORKS_SHAPEMANAGER_H
#define GLIMMERWORKS_SHAPEMANAGER_H
#include "core/mod/Resource.h"


namespace glimmer {
    class ShapeManager {
        std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<IShapeResource> > > shapeMap_
                {};
    public:
        ShapeManager();

        IShapeResource *Register(std::unique_ptr<IShapeResource> shapeResource);

        IShapeResource* FindShape(const std::string &packId, const std::string &resourceId);

        std::string ListShapes() const;
    };
}

#endif //GLIMMERWORKS_SHAPEMANAGER_H
