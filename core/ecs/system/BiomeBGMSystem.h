//
// Created by coldmint on 2026/3/26.
//

#ifndef GLIMMERWORKS_BIOMEBGMSYSTEM_H
#define GLIMMERWORKS_BIOMEBGMSYSTEM_H
#include "core/ecs/GameSystem.h"
#include "core/mod/Resource.h"
#include "SDL3_mixer/SDL_mixer.h"

namespace glimmer {
    class BiomeBGMSystem final : public GameSystem {
        BiomeResource *biomeResource_ = nullptr;
        std::shared_ptr<MIX_Audio> audio_ = nullptr;

    public:
        explicit BiomeBGMSystem(WorldContext *worldContext);


        void Update(float delta) override;

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_BIOMEBGMSYSTEM_H
