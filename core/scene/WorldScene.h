//
// Created by Cold-Mint on 2025/10/25.
//

#ifndef GLIMMERWORKS_WORLDSCENE_H
#define GLIMMERWORKS_WORLDSCENE_H
#include "Scene.h"
#include "../log/LogCat.h"
#include "../world/WorldGenerator.h"
#include "../world/WorldContext.h"

namespace glimmer {
    class WorldScene : public Scene {
        WorldContext *worldContext;
        WorldGenerator *worldGenerator;

    public:
        explicit WorldScene(AppContext *context, WorldContext *wc, WorldGenerator *wg)
            : Scene(context) {
            worldContext = wc;
            worldGenerator = wg;
            LogCat::i("WorldScene created");
            LogCat::i("chunks size: ", worldContext->chunks.size());
            for (size_t i = 0; i < worldContext->chunks.size(); ++i) {
                const auto &chunkPtr = worldContext->chunks[i];
                LogCat::i("chunks[", i, "] use_count: ", chunkPtr.use_count());
                const std::vector<int> heights = worldContext->getHeightMap(chunkPtr->origin.x);
                std::ostringstream oss;
                oss << "heights: [";
                for (size_t j = 0; j < heights.size(); ++j) {
                    oss << heights[j];
                    if (j + 1 < heights.size()) oss << ", ";
                }
                oss << "]";
                LogCat::i(oss.str());
            }
        }

        bool HandleEvent(const SDL_Event &event) override;

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        ~WorldScene() override {
            delete worldContext;
            delete worldGenerator;
        };
    };
}

#endif //GLIMMERWORKS_WORLDSCENE_H
