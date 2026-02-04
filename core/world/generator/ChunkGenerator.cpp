//
// Created by coldmint on 2026/2/4.
//

#include "ChunkGenerator.h"

#include "TerrainResultType.h"
#include "TilePlacer.h"
#include "core/log/LogCat.h"
#include "core/mod/Resource.h"
#include "core/mod/ResourceRef.h"


int glimmer::ChunkGenerator::GetHeight(int x) {
    const auto it = heightMap_.find(x);
    if (it != heightMap_.end()) {
        LogCat::d("HeightMap cache hit for chunkX=", x);
        return it->second;
    }

    LogCat::d("HeightMap cache miss, generating new chunk at chunkX=", x);
    const float sampleX = static_cast<float>(x);

    // 1. 获取三层归一化噪声 (0.0 到 1.0)

    // a) 极低频: 大陆/大型岛屿掩码。
    const float continentNoise = (continentHeightMapNoise->GetNoise(sampleX, 0.0F) + 1.0F) * 0.5F;

    // b) 低频: 宏观地形 (山脉/高原)。
    const float mountainNoise = (mountainHeightMapNoise->GetNoise(sampleX, 0.0F) + 1.0F) * 0.5F;

    // c) 中频: 细节地形 (丘陵/平原起伏)。
    const float hillsNoise = (hillsNoiseHeightMapNoise->GetNoise(sampleX, 0.0F) + 1.0F) * 0.5F;


    // 2. 核心：创建大陆掩码和陆地起伏

    // A) 大陆掩码 (控制海洋和大陆板块)：使用 3 次方夸大低值，创建清晰的海洋边界。
    // 这层噪声确保了大陆/岛屿（大型陆地板块）的孤立性。
    const float continentMask = std::pow(continentNoise, 3.0F);

    // B) 组合陆地起伏噪声 (主要由山脉和丘陵构成)
    // landmassNoise 范围 [0, 1]
    float landmassNoise = mountainNoise * MOUNTAIN_WEIGHT + hillsNoise * HILLS_WEIGHT;

    // C) 平滑山峰抬升 (生成高原和尖锐的山脉)
    float peakLift = 0.0F;
    if (landmassNoise > PEAK_LIFT_THRESHOLD) {
        // 只有在陆地噪声较高时，才计算额外的线性抬升，避免山峰顶部平坦。
        // liftFactor 范围 [0, 1]
        float liftFactor = (landmassNoise - PEAK_LIFT_THRESHOLD) / (1.0F - PEAK_LIFT_THRESHOLD);
        peakLift = liftFactor * MAX_PEAK_LIFT;
    }

    // D) 最终陆地噪声：基础起伏 + 额外山峰抬升
    float totalLandNoise = landmassNoise + peakLift;
    // 确保总噪声不会超出我们预设的最大抬升比例
    totalLandNoise = std::min(totalLandNoise, 1.0F + MAX_PEAK_LIFT);

    // E) 应用大陆掩码：只有大陆区域，陆地噪声才能抬升地形
    float combinedNoise = continentMask * totalLandNoise;


    // 3. 将总噪声映射到最终的世界高度
    // combinedNoise 的范围现在约为 [0, 1.3]，我们使用 TERRAIN_HEIGHT_RANGE 进行映射
    int height = WORLD_MIN_Y + BASE_HEIGHT_OFFSET +
                 static_cast<int>(combinedNoise * TERRAIN_HEIGHT_RANGE / (1.0F + MAX_PEAK_LIFT));


    // 5. 确保不超过世界最大高度
    height = std::min(height, MAX_LAND_HEIGHT);

    // 缓存并返回高度
    heightMap_[x] = height;
    LogCat::d("Generated and cached heights for chunkX=", x);
    return height;
}

std::unique_ptr<glimmer::TerrainResult> glimmer::ChunkGenerator::GenerateTerrain(TileVector2D position) {
    auto terrainResult = std::make_unique<TerrainResult>();
    for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
        const int height = GetHeight(position.x + localX);
        for (int localY = 0; localY < CHUNK_SIZE; ++localY) {
            TileVector2D localTile(localX, localY);
            TileVector2D worldTilePos = position + localTile;
            if (worldTilePos.y == WORLD_MIN_Y) {
                TerrainTileResult terrainTileResult;
                terrainTileResult.terrainType = BEDROCK;
                terrainResult->AddTile(localTile, terrainTileResult);
                continue;
            }
            if (worldTilePos.y > height) {
                if (worldTilePos.y < SEA_LEVEL_HEIGHT) {
                    //water
                    //水
                    TerrainTileResult terrainTileResult;
                    terrainTileResult.terrainType = WATER;
                    terrainResult->AddTile(localTile, terrainTileResult);
                } else {
                    //sky
                    //天空
                    TerrainTileResult terrainTileResult;
                    terrainTileResult.terrainType = AIR;
                    terrainResult->AddTile(localTile, terrainTileResult);
                }
                continue;
            }
            const float elevation = GetElevation(worldTilePos.y);
            const auto humidity = GetHumidity(worldTilePos);
            const auto temperature = GetTemperature(worldTilePos, elevation);
            const auto weirdness = GetWeirdness(worldTilePos);
            const auto erosion = GetErosion(worldTilePos);
            TerrainTileResult terrainTileResult;
            terrainTileResult.terrainType = SOLID;
            terrainTileResult.biomeResource = appContext_->GetBiomesManager()->FindBestBiome(
                humidity, temperature, weirdness, erosion,
                elevation);
            terrainResult->AddTile(localTile, terrainTileResult);
        }
    }
    return terrainResult;
}

float glimmer::ChunkGenerator::GetElevation(const int height) {
    return static_cast<float>(height) / (WORLD_MAX_Y - WORLD_MIN_Y + WORLD_MIN_Y);
}

float glimmer::ChunkGenerator::GetHumidity(TileVector2D tileVector2d) {
    const auto it = humidityMap.find(tileVector2d);
    if (it != humidityMap.end()) {
        return it->second;
    }
    humidityMap[tileVector2d] = (humidityMapNoise->GetNoise(static_cast<float>(tileVector2d.x),
                                                            static_cast<float>(tileVector2d.y)) + 1) * 0.5F;
    return humidityMap[tileVector2d];
}

float glimmer::ChunkGenerator::GetTemperature(TileVector2D tileVector2d, float elevation) {
    const auto it = temperatureMap.find(tileVector2d);
    if (it != temperatureMap.end()) {
        return it->second;
    }
    const float noiseTemp = (temperatureMapNoise->GetNoise(
                                 static_cast<float>(tileVector2d.x),
                                 static_cast<float>(tileVector2d.y)
                             ) + 1.0F) * 0.5F;
    const float altitudePenalty = std::pow(1.0F - elevation, 1.5F);
    const float temperature = noiseTemp * altitudePenalty;

    temperatureMap[tileVector2d] = temperature;
    return temperatureMap[tileVector2d];
}

float glimmer::ChunkGenerator::GetWeirdness(TileVector2D tileVector2d) {
    const auto it = weirdnessMap.find(tileVector2d);
    if (it != weirdnessMap.end()) {
        return it->second;
    }
    weirdnessMap[tileVector2d] = (weirdnessMapNoise->GetNoise(static_cast<float>(tileVector2d.x),
                                                              static_cast<float>(tileVector2d.y)) + 1) * 0.5F;
    return weirdnessMap[tileVector2d];
}

float glimmer::ChunkGenerator::GetErosion(TileVector2D tileVector2d) {
    const auto it = erosionMap.find(tileVector2d);
    if (it != erosionMap.end()) {
        return it->second;
    }
    erosionMap[tileVector2d] = (erosionMapNoise->GetNoise(static_cast<float>(tileVector2d.x),
                                                          static_cast<float>(tileVector2d.y)) + 1) * 0.5F;
    return erosionMap[tileVector2d];
}

glimmer::ChunkGenerator::ChunkGenerator(AppContext *appContext, const int seed) : appContext_(appContext) {
    // 1. 大型陆地板块/大陆噪声 (极低频) - 控制大岛屿和大陆的生成
    continentHeightMapNoise = std::make_unique<FastNoiseLite>();
    continentHeightMapNoise->SetSeed(seed);
    continentHeightMapNoise->SetFrequency(0.005F); // 极低频，用于大型板块
    continentHeightMapNoise->SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    // 2. 高原/山脉噪声 (低频) - 控制地形的宏观起伏
    mountainHeightMapNoise = std::make_unique<FastNoiseLite>();
    mountainHeightMapNoise->SetSeed(seed + 1); // 不同的种子
    mountainHeightMapNoise->SetFrequency(0.01F); // 低频，用于主要地形
    mountainHeightMapNoise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    // 3. 丘陵/细节噪声 (中频) - 控制平原和丘陵的细节
    hillsNoiseHeightMapNoise = std::make_unique<FastNoiseLite>();
    hillsNoiseHeightMapNoise->SetSeed(seed + 2); // 不同的种子
    hillsNoiseHeightMapNoise->SetFrequency(0.02F); // 中频，用于细节
    hillsNoiseHeightMapNoise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    humidityMapNoise = std::make_unique<FastNoiseLite>();
    humidityMapNoise->SetSeed(seed + 100);
    humidityMapNoise->SetFrequency(0.005F);
    humidityMapNoise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    temperatureMapNoise = std::make_unique<FastNoiseLite>();
    temperatureMapNoise->SetSeed(seed + 200);
    temperatureMapNoise->SetFrequency(0.01F);
    temperatureMapNoise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    weirdnessMapNoise = std::make_unique<FastNoiseLite>();
    weirdnessMapNoise->SetSeed(seed + 300);
    weirdnessMapNoise->SetFrequency(0.02F);
    weirdnessMapNoise->SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    erosionMapNoise = std::make_unique<FastNoiseLite>();
    erosionMapNoise->SetSeed(seed + 400);
    erosionMapNoise->SetFrequency(0.003F);
    erosionMapNoise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
}

std::unique_ptr<glimmer::Chunk> glimmer::ChunkGenerator::GenerateChunkAt(TileVector2D position) {
    auto chunk = std::make_unique<Chunk>(position);
    std::unique_ptr<TerrainResult> terrainResult = GenerateTerrain(position);
    ResourceRef airTileRef;
    airTileRef.SetResourceType(RESOURCE_TYPE_TILE);
    airTileRef.SetPackageId(RESOURCE_REF_CORE);
    airTileRef.SetSelfPackageId(RESOURCE_REF_CORE);
    airTileRef.SetResourceKey(TILE_ID_AIR);
    ResourceRef waterTileRef;
    waterTileRef.SetResourceType(RESOURCE_TYPE_TILE);
    waterTileRef.SetPackageId(RESOURCE_REF_CORE);
    waterTileRef.SetSelfPackageId(RESOURCE_REF_CORE);
    waterTileRef.SetResourceKey(TILE_ID_WATER);
    ResourceRef bedrockTileRef;
    bedrockTileRef.SetResourceType(RESOURCE_TYPE_TILE);
    bedrockTileRef.SetPackageId(RESOURCE_REF_CORE);
    bedrockTileRef.SetSelfPackageId(RESOURCE_REF_CORE);
    bedrockTileRef.SetResourceKey(TILE_ID_BEDROCK);
    std::array<std::array<ResourceRef, CHUNK_SIZE>, CHUNK_SIZE> tilesRef;
    for (auto result: terrainResult->GetResult()) {
        auto terrainType = result.second.terrainType;
        if (terrainType == AIR) {
            tilesRef[result.first.x][result.first.y] = airTileRef;
            continue;
        }
        if (terrainType == WATER) {
            tilesRef[result.first.x][result.first.y] = waterTileRef;
            continue;
        }
        if (terrainType == BEDROCK) {
            tilesRef[result.first.x][result.first.y] = bedrockTileRef;
            continue;
        }
        if (terrainType == SOLID) {
            if (result.second.biomeResource == nullptr) {
                tilesRef[result.first.x][result.first.y] = airTileRef;
            } else {
                tilesRef[result.first.x][result.first.y] = result.second.biomeResource->baseTileRef;
            }
        }
    }
    for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
        for (int localY = 0; localY < CHUNK_SIZE; ++localY) {
            TileVector2D localTile(localX, localY);
            ResourceRef resourceRef = tilesRef[localX][localY];
            const std::optional<TileResource *> tileResource = appContext_->GetResourceLocator()->FindTile(
                resourceRef);
            TileResource *tileResourceValue = nullptr;
            if (tileResource.has_value()) {
                tileResourceValue = tileResource.value();
            } else {
                LogCat::w("Tile packageId=", resourceRef.GetPackageId(), ", key=", resourceRef.GetResourceKey(),
                          " does not exist.");
                tileResourceValue = appContext_->GetTileManager()->GetAir();
            }
            chunk->SetTile(localTile, Tile::FromResourceRef(appContext_, tileResourceValue));
        }
    }
    return chunk;
}
