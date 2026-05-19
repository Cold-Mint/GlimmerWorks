//
// Created by Cold-Mint on 2025/12/9.
//

#pragma once
#include <memory>
#include <unordered_map>
#include "core/mod/dataPack/BiomeDecoratorType.h"

namespace glimmer {
    class IBiomeDecorator;

    class BiomeDecoratorManager {
        std::unordered_map<BiomeDecoratorType, std::unique_ptr<IBiomeDecorator> > biomeDecoratorMap_;

    public:
        void RegisterBiomeDecorator(std::unique_ptr<IBiomeDecorator> biomeDecorator);

        IBiomeDecorator *GetBiomeDecorator(BiomeDecoratorType biomeDecoratorType);

        void SetWorldSeed(int worldSeed) const;
    };
}
