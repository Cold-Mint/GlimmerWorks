//
// Created by coldmint on 2026/4/6.
//

#ifndef GLIMMERWORKS_BIOMEDECORATORRESOURCESMANAGER_H
#define GLIMMERWORKS_BIOMEDECORATORRESOURCESMANAGER_H
#include <memory>
#include <string>
#include <unordered_map>

#include "core/mod/Resource.h"


namespace glimmer {
    class BiomeDecoratorResourcesManager {
        std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<IBiomeDecoratorResource> > > biomeDecoratorMap_
             {};

    public:

        IBiomeDecoratorResource *Register(std::unique_ptr<IBiomeDecoratorResource> biomeDecoratorResource);

        IBiomeDecoratorResource* FindBiomeDecorator(const std::string &packId, const std::string &resourceId);

        std::string ListBiomeDecorators() const;
    };
}



#endif //GLIMMERWORKS_BIOMEDECORATORRESOURCESMANAGER_H
