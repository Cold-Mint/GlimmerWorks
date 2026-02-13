//
// Created by Cold-Mint on 2025/12/9.
//

#ifndef GLIMMERWORKS_TILEPLACERMANAGER_H
#define GLIMMERWORKS_TILEPLACERMANAGER_H
#include <memory>
#include <string>
#include <unordered_map>

namespace glimmer {
    class BiomeDecorator;

    class BiomeDecoratorManager {
        std::unordered_map<std::string, std::unique_ptr<BiomeDecorator> > biomeDecoratorMap_;

    public:
        void RegisterBiomeDecorator(std::unique_ptr<BiomeDecorator> biomeDecorator);

        BiomeDecorator *GetBiomeDecorator(const std::string &id);

        void SetWorldSeed(int worldSeed) const;
    };
}

#endif //GLIMMERWORKS_TILEPLACERMANAGER_H
