//
// Created by Cold-Mint on 2025/10/29.
//

#ifndef GLIMMERWORKS_GRIDSYSTEM_H
#define GLIMMERWORKS_GRIDSYSTEM_H
#include "../GameSystem.h"
#include "../component/GridComponent.h"

namespace glimmer {
    class GridSystem final : public GameSystem {
    public:
        explicit GridSystem(WorldContext *worldContext) 
            : GameSystem(worldContext) {
            RequireComponent<GridComponent>();
        }
        
        void Update(float delta) override;
        
        void Render(SDL_Renderer *renderer) override;
        
        std::string GetName() override;
    
    private:
        [[nodiscard]] bool ShouldActivate() override;
    };
} 

#endif //GLIMMERWORKS_GRIDSYSTEM_H