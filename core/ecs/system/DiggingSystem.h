//
// Created by Cold-Mint on 2025/12/29.
//

#pragma once
#include <memory>
#include <vector>
#include "core/ecs/GameSystem.h"
#include "core/ecs/component/TileLayerComponent.h"
#include "core/mod/ResourceRef.h"


namespace glimmer {
    class DiggingComponent;

    class DiggingSystem : public GameSystem {
        bool cacheTexture = false;
        std::vector<std::shared_ptr<SDL_Texture> > textureList = {};
        DiggingComponent *diggingComponent_ = nullptr;

    public:
        /**
     * BreakTile
     * 破坏方块
     * @param worldContext worldContext 上下文环境
     * @param tileLayerComponent tileLayerComponent 图层
     * @param topLeftVector TopLeftVector 左上角
     * @param precisionMining Is precise collection carried out? 是否精准采集
     * @param isPlaceMode 是否为放置方块模式，例如将某个方块放置在空气上。
     * @param tileWidth
     * @param tileHeight
     * @param newTileRef newTileRef 新瓦片
     * @return
     */
        static uint16_t BreakTile(WorldContext *worldContext, const TileLayerComponent *tileLayerComponent,
                                  const TileVector2D &topLeftVector, bool precisionMining, bool isPlaceMode,
                                  uint8_t tileWidth, uint8_t tileHeight,
                                  const ResourceRef &newTileRef);

        explicit DiggingSystem(WorldContext *worldContext);

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        uint8_t GetRenderOrder() override;

        std::string GetName() override;
    };
}
