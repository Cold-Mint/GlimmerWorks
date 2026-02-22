//
// Created by Cold-Mint on 2026/02/01.
//

#include "TreeBiomeDecorator.h"


void glimmer::TreeBiomeDecorator::SetWorldSeed(const int seed) {
    if (treeNoise == nullptr) {
        treeNoise = std::make_unique<FastNoiseLite>();
    }
    treeNoise->SetSeed(seed);
}

void glimmer::TreeBiomeDecorator::Decoration(WorldContext *worldContext, TerrainResult *terrainResult,
                                             BiomeDecoratorResource *biomeDecoratorResource,
                                             BiomeResource *biomeResource,
                                             std::array<ResourceRef, CHUNK_AREA> &tilesRef) {
    if (treeNoise == nullptr) {
        return;
    }
    //The minimum distance between trees.(Spacing)
    //树木间的最小距离。（间隔）
    auto *minDistanceDefinition = biomeDecoratorResource->config.FindVariable("minDistance");
    int minDistance = 6;
    if (minDistanceDefinition != nullptr) {
        minDistance = minDistanceDefinition->AsInt();
    }
    auto *probabilityDefinition = biomeDecoratorResource->config.FindVariable("probability");
    float probability = 0.3F;
    if (probabilityDefinition != nullptr) {
        probability = probabilityDefinition->AsFloat();
    }


    const ResourceRef &resourceRef = biomeDecoratorResource->data[0];
    for (int localX = 0; localX < CHUNK_SIZE; localX++) {
        for (int localY = 0; localY < CHUNK_SIZE; localY++) {
            const int idx = localY * CHUNK_SIZE + localX;
            TerrainTileResult self = terrainResult->QueryTerrain(localX, localY);
            if (self.terrainType != SOLID) {
                //Not solid tiles.
                //不是固体瓦片。
                continue;
            }
            if (self.biomeResource != biomeResource) {
                //Tiles do not belong to the current biome.
                //瓦片不属于当前生物群系。
                continue;
            }
            TerrainTileResult up = terrainResult->QueryTerrain(localX, localY + 1);
            if (up.terrainType != AIR) {
                // The tiles above are not air.
                //上方的瓦片不是空气。
                continue;
            }
            TileVector2D position = terrainResult->GetPosition() + TileVector2D(localX, localY);
            //Calculate the current section that X belongs to
            //计算当前 X 所属区段
            int gx = std::floor(static_cast<float>(position.x) / minDistance);

            //Determine whether this section constitutes a tree.
            //判断这个区段是否生成树
            float segmentNoise =
                    (treeNoise->GetNoise<float>(gx, 0) + 1.0f) * 0.5f;

            if (segmentNoise > probability) {
                continue;
            }

            // Generate a stable random offset for this X segment.
            // Use a large multiplier to decorrelate from other noise queries.
            // Remap noise range from [-1,1] to [0,1].
            // 为当前 X 区段生成一个稳定的随机偏移。
            // 使用大常数打散坐标，避免与其他噪声计算相关。
            // 将噪声范围从 [-1,1] 映射到 [0,1]。
            float offsetNoise =
                    (treeNoise->GetNoise<float>(gx * 928371 + 7, 0) + 1.0f) * 0.5f;
            int offsetX = static_cast<int>(offsetNoise * minDistance);
            int targetX = gx * minDistance + offsetX;
            //Generated only when matching X
            //只在匹配 X 生成
            if (position.x != targetX) {
                continue;
            }

            tilesRef[idx] = resourceRef;
        }
    }
}


std::string glimmer::TreeBiomeDecorator::GetId() {
    return TREE_BIOME_DECORATOR;
}
