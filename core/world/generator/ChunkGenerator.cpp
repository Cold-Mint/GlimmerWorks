//
// Created by coldmint on 2026/2/4.
//

#include "ChunkGenerator.h"

#include "TerrainResultType.h"
#include "BiomeDecorator.h"
#include "core/log/LogCat.h"
#include "core/mod/Resource.h"
#include "core/mod/ResourceRef.h"
#include "core/world/WorldContext.h"


glimmer::ChunkGenerator::ChunkGenerator(WorldContext *worldContext, const int worldSeed) : worldContext_(worldContext) {
    // 1. 大型陆地板块/大陆噪声 (极低频) - 控制大岛屿和大陆的生成
    continentHeightMapNoise_ = std::make_unique<FastNoiseLite>();
    continentHeightMapNoise_->SetSeed(worldSeed);
    continentHeightMapNoise_->SetFrequency(0.005F); // 极低频，用于大型板块
    continentHeightMapNoise_->SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    // 2. 高原/山脉噪声 (低频) - 控制地形的宏观起伏
    mountainHeightMapNoise_ = std::make_unique<FastNoiseLite>();
    mountainHeightMapNoise_->SetSeed(worldSeed + 1); // 不同的种子
    mountainHeightMapNoise_->SetFrequency(0.01F); // 低频，用于主要地形
    mountainHeightMapNoise_->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    // 3. 丘陵/细节噪声 (中频) - 控制平原和丘陵的细节
    hillsNoiseHeightMapNoise_ = std::make_unique<FastNoiseLite>();
    hillsNoiseHeightMapNoise_->SetSeed(worldSeed + 2); // 不同的种子
    hillsNoiseHeightMapNoise_->SetFrequency(0.02F); // 中频，用于细节
    hillsNoiseHeightMapNoise_->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    humidityMapNoise_ = std::make_unique<FastNoiseLite>();
    humidityMapNoise_->SetSeed(worldSeed + 100);
    humidityMapNoise_->SetFrequency(0.005F);
    humidityMapNoise_->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    temperatureMapNoise_ = std::make_unique<FastNoiseLite>();
    temperatureMapNoise_->SetSeed(worldSeed + 200);
    temperatureMapNoise_->SetFrequency(0.01F);
    temperatureMapNoise_->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    weirdnessMapNoise_ = std::make_unique<FastNoiseLite>();
    weirdnessMapNoise_->SetSeed(worldSeed + 300);
    weirdnessMapNoise_->SetFrequency(0.02F);
    weirdnessMapNoise_->SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    erosionMapNoise_ = std::make_unique<FastNoiseLite>();
    erosionMapNoise_->SetSeed(worldSeed + 400);
    erosionMapNoise_->SetFrequency(0.003F);
    erosionMapNoise_->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    airTileRef_ = ResourceRef();
    airTileRef_.SetResourceType(RESOURCE_TYPE_TILE);
    airTileRef_.SetPackageId(RESOURCE_REF_CORE);
    airTileRef_.SetSelfPackageId(RESOURCE_REF_CORE);
    airTileRef_.SetResourceKey(TILE_ID_AIR);
    waterTileRef_ = ResourceRef();
    waterTileRef_.SetResourceType(RESOURCE_TYPE_TILE);
    waterTileRef_.SetPackageId(RESOURCE_REF_CORE);
    waterTileRef_.SetSelfPackageId(RESOURCE_REF_CORE);
    waterTileRef_.SetResourceKey(TILE_ID_WATER);
    bedrockTileRef_ = ResourceRef();
    bedrockTileRef_.SetResourceType(RESOURCE_TYPE_TILE);
    bedrockTileRef_.SetPackageId(RESOURCE_REF_CORE);
    bedrockTileRef_.SetSelfPackageId(RESOURCE_REF_CORE);
    bedrockTileRef_.SetResourceKey(TILE_ID_BEDROCK);
}

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
    const float continentNoise = (continentHeightMapNoise_->GetNoise(sampleX, 0.0F) + 1.0F) * 0.5F;

    // b) 低频: 宏观地形 (山脉/高原)。
    const float mountainNoise = (mountainHeightMapNoise_->GetNoise(sampleX, 0.0F) + 1.0F) * 0.5F;

    // c) 中频: 细节地形 (丘陵/平原起伏)。
    const float hillsNoise = (hillsNoiseHeightMapNoise_->GetNoise(sampleX, 0.0F) + 1.0F) * 0.5F;


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
    terrainResult->SetPosition(position);
    for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
        const int height = GetHeight(position.x + localX);
        for (int localY = 0; localY < CHUNK_SIZE; ++localY) {
            terrainResult->SetTerrainTileResult(localX, localY,
                                                GetTerrainTileResult(position + TileVector2D(localX, localY), height));
        }
    }
    const int leftWorldX = position.x - 1;

    for (int localY = 0; localY < CHUNK_SIZE; ++localY) {
        const int worldY = position.y + localY;
        const int height = GetHeight(leftWorldX);

        terrainResult->SetLeftTerrainTileResult(
            localY,
            GetTerrainTileResult({leftWorldX, worldY}, height)
        );
    }


    const int rightWorldX = position.x + CHUNK_SIZE;

    for (int localY = 0; localY < CHUNK_SIZE; ++localY) {
        const int worldY = position.y + localY;
        const int height = GetHeight(rightWorldX);

        terrainResult->SetRightTerrainTileResult(
            localY,
            GetTerrainTileResult({rightWorldX, worldY}, height)
        );
    }


    const int downWorldY = position.y - 1;

    for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
        const int worldX = position.x + localX;
        const int height = GetHeight(worldX);

        terrainResult->SetDownTerrainTileResult(
            localX,
            GetTerrainTileResult({worldX, downWorldY}, height)
        );
    }

    const int upWorldY = position.y + CHUNK_SIZE;

    for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
        const int worldX = position.x + localX;
        const int height = GetHeight(worldX);

        terrainResult->SetUpTerrainTileResult(
            localX,
            GetTerrainTileResult({worldX, upWorldY}, height)
        );
    }

    return terrainResult;
}

TerrainTileResult glimmer::ChunkGenerator::GetTerrainTileResult(const TileVector2D world, const int height) {
    TerrainTileResult terrainTileResult;
    if (world.y <= WORLD_MIN_Y) {
        terrainTileResult.terrainType = BEDROCK;
        return terrainTileResult;
    }
    if (world.y > height) {
        if (world.y < SEA_LEVEL_HEIGHT) {
            //water
            //水
            terrainTileResult.terrainType = WATER;
            return terrainTileResult;
        }
        //sky
        //天空
        terrainTileResult.terrainType = AIR;
        return terrainTileResult;
    }
    const float elevation = GetElevation(world.y);
    const auto humidity = GetHumidity(world);
    const auto temperature = GetTemperature(world, elevation);
    const auto weirdness = GetWeirdness(world);
    const auto erosion = GetErosion(world);
    terrainTileResult.world = world;
    terrainTileResult.terrainType = SOLID;
    terrainTileResult.biomeResource = worldContext_->GetAppContext()->GetBiomesManager()->FindBestBiome(
        humidity, temperature, weirdness, erosion,
        elevation);
    return terrainTileResult;
}

float glimmer::ChunkGenerator::GetElevation(const int height) {
    return static_cast<float>(height) / (WORLD_MAX_Y - WORLD_MIN_Y + WORLD_MIN_Y);
}

float glimmer::ChunkGenerator::GetHumidity(TileVector2D tileVector2d) {
    const auto it = humidityMap_.find(tileVector2d);
    if (it != humidityMap_.end()) {
        return it->second;
    }
    humidityMap_[tileVector2d] = (humidityMapNoise_->GetNoise(static_cast<float>(tileVector2d.x),
                                                              static_cast<float>(tileVector2d.y)) + 1) * 0.5F;
    return humidityMap_[tileVector2d];
}

float glimmer::ChunkGenerator::GetTemperature(TileVector2D tileVector2d, float elevation) {
    const auto it = temperatureMap_.find(tileVector2d);
    if (it != temperatureMap_.end()) {
        return it->second;
    }
    const float noiseTemp = (temperatureMapNoise_->GetNoise(
                                 static_cast<float>(tileVector2d.x),
                                 static_cast<float>(tileVector2d.y)
                             ) + 1.0F) * 0.5F;
    const float altitudePenalty = std::pow(1.0F - elevation, 1.5F);
    const float temperature = noiseTemp * altitudePenalty;

    temperatureMap_[tileVector2d] = temperature;
    return temperatureMap_[tileVector2d];
}

float glimmer::ChunkGenerator::GetWeirdness(TileVector2D tileVector2d) {
    const auto it = weirdnessMap_.find(tileVector2d);
    if (it != weirdnessMap_.end()) {
        return it->second;
    }
    weirdnessMap_[tileVector2d] = (weirdnessMapNoise_->GetNoise(static_cast<float>(tileVector2d.x),
                                                                static_cast<float>(tileVector2d.y)) + 1) * 0.5F;
    return weirdnessMap_[tileVector2d];
}

float glimmer::ChunkGenerator::GetErosion(TileVector2D tileVector2d) {
    const auto it = erosionMap_.find(tileVector2d);
    if (it != erosionMap_.end()) {
        return it->second;
    }
    erosionMap_[tileVector2d] = (erosionMapNoise_->GetNoise(static_cast<float>(tileVector2d.x),
                                                            static_cast<float>(tileVector2d.y)) + 1) * 0.5F;
    return erosionMap_[tileVector2d];
}

std::unique_ptr<glimmer::Chunk> glimmer::ChunkGenerator::GenerateChunkAt(TileVector2D position) {
    AppContext *appContext = worldContext_->GetAppContext();
    auto chunk = std::make_unique<Chunk>(position);
    std::unique_ptr<TerrainResult> terrainResult = GenerateTerrain(position);
    std::array<std::array<ResourceRef, CHUNK_SIZE>, CHUNK_SIZE> tilesRef;
    std::unordered_set<BiomeResource *> biomeResourcesSet;
    for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
        for (int localY = 0; localY < CHUNK_SIZE; ++localY) {
            auto terrainTileResult = terrainResult->QueryTerrain(localX, localY);
            auto terrainType = terrainTileResult.terrainType;
            LogCat::d("Chunk x=", position.x + localX, ",y=", position.y + localY, ",terrainType=", terrainType);
            if (terrainType == AIR) {
                tilesRef[localX][localY] = airTileRef_;
                continue;
            }
            if (terrainType == WATER) {
                tilesRef[localX][localY] = waterTileRef_;
                continue;
            }
            if (terrainType == BEDROCK) {
                tilesRef[localX][localY] = bedrockTileRef_;
                continue;
            }
            if (terrainType == SOLID) {
                tilesRef[localX][localY] = airTileRef_;
                if (terrainTileResult.biomeResource != nullptr && !biomeResourcesSet.contains(
                        terrainTileResult.biomeResource)) {
                    biomeResourcesSet.insert(terrainTileResult.biomeResource);
                }
            }
        }
    }
    for (auto *biomeResources: biomeResourcesSet) {
        if (auto &decorator = biomeResources->decorator; decorator.empty()) {
            continue;
        }
        for (auto &dec: biomeResources->decorator) {
            BiomeDecorator *biomeDecorator = appContext->GetBiomeDecoratorManager()->
                    GetBiomeDecorator(dec.id);
            if (biomeDecorator != nullptr) {
                biomeDecorator->Decoration(worldContext_, terrainResult.get(), &dec, biomeResources, tilesRef);
            }
        }
    }

    for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
        for (int localY = 0; localY < CHUNK_SIZE; ++localY) {
            TileVector2D localTile(localX, localY);
            ResourceRef &resourceRef = tilesRef[localX][localY];
            const std::optional tileResource = appContext->GetResourceLocator()->FindTile(
                resourceRef);
            TileResource *tileResourceValue = nullptr;
            if (tileResource.has_value()) {
                tileResourceValue = tileResource.value();
            } else {
                LogCat::w("Tile packageId=", resourceRef.GetPackageId(), ", key=", resourceRef.GetResourceKey(),
                          " does not exist.");
                tileResourceValue = appContext->GetTileManager()->GetAir();
            }
            chunk->SetTile(localTile, Tile::FromResourceRef(appContext, tileResourceValue));
        }
    }
    return chunk;
}
