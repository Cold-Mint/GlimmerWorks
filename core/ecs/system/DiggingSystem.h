//
// Created by coldmint on 2025/12/29.
//

#ifndef GLIMMERWORKS_DIGGINGSYSTEM_H
#define GLIMMERWORKS_DIGGINGSYSTEM_H
#include <memory>
#include <vector>

#include "../GameSystem.h"
#include "../component/DiggingComponent.h"

namespace glimmer {
    class DiggingSystem : public GameSystem {
        bool cacheTexture = false;
        std::vector<std::shared_ptr<SDL_Texture> > textureList = {};

    public:
        explicit DiggingSystem(WorldContext *worldContext);

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        uint8_t GetRenderOrder() override;

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_DIGGINGSYSTEM_H
