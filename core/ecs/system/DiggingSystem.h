//
// Created by Cold-Mint on 2025/12/29.
//

#ifndef GLIMMERWORKS_DIGGINGSYSTEM_H
#define GLIMMERWORKS_DIGGINGSYSTEM_H
#include <memory>
#include <vector>

#include "../GameSystem.h"
#include "core/ecs/component/TileLayerComponent.h"


namespace glimmer {
    class DiggingComponent;

    class DiggingSystem : public GameSystem {
        bool cacheTexture = false;
        std::vector<std::shared_ptr<SDL_Texture> > textureList = {};

    public:
        /**
     * BreakTile
     * 破坏方块
     * @param worldContext worldContext 上下文环境
     * @param tileLayerComponent tileLayerComponent 图层
     * @param tilePosition tilePosition 瓦片位置
     * @param precisionMining Is precise collection carried out? 是否精准采集
     * @param overwrite 是否为覆盖方块模式，例如将某个方块放置在空气上。
     * @param newTile newTile 新瓦片
     * @return
     */
        static bool BreakTile(WorldContext *worldContext,
                              const TileLayerComponent *tileLayerComponent,
                              TileVector2D tilePosition, bool precisionMining, bool overwrite,
                              const std::shared_ptr<Tile>& newTile);

        explicit DiggingSystem(WorldContext *worldContext);

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        uint8_t GetRenderOrder() override;

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_DIGGINGSYSTEM_H
