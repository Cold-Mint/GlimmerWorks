/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "ClimateSampler.h"

#include <cmath>
#include <utility>

#include "core/config/Constants.h"
#include "core/log/LogCat.h"
#include "core/mod/Resource.h"


FastNoiseLite *glimmer::ClimateSampler::GetNoise(const NoiseField field) const {
    return noises_[std::to_underlying(field)].get();
}

const glimmer::NoiseConfig &glimmer::ClimateSampler::GetNoiseConfig(const DimensionResource *dimensionResource,
                                                                    const NoiseField field) {
    switch (field) {
        case NoiseField::ContinentHeight:
            return dimensionResource->continentNoise;
        case NoiseField::Humidity:
            return dimensionResource->humidityNoise;
        case NoiseField::Temperature:
            return dimensionResource->temperatureNoise;
        case NoiseField::Weirdness:
            return dimensionResource->weirdnessNoise;
        case NoiseField::Erosion:
            return dimensionResource->erosionNoise;
        case NoiseField::Count:
            break;
    }
    return dimensionResource->continentNoise;
}

void glimmer::ClimateSampler::ApplyNoiseConfig(FastNoiseLite *noise, const NoiseConfig &config, const int baseSeed) {
    if (noise == nullptr) {
        return;
    }
    noise->SetSeed(baseSeed + config.seedOffset);
    noise->SetFrequency(config.frequency);
    noise->SetNoiseType(static_cast<FastNoiseLite::NoiseType>(config.noiseType));
    noise->SetFractalType(static_cast<FastNoiseLite::FractalType>(config.fractalType));
    noise->SetFractalOctaves(config.octaves);
    noise->SetFractalLacunarity(config.lacunarity);
    noise->SetFractalGain(config.gain);
    noise->SetFractalWeightedStrength(config.weightedStrength);
    noise->SetFractalPingPongStrength(config.pingPongStrength);
    noise->SetCellularDistanceFunction(
        static_cast<FastNoiseLite::CellularDistanceFunction>(config.cellularDistanceFunction));
    noise->SetCellularReturnType(static_cast<FastNoiseLite::CellularReturnType>(config.cellularReturnType));
    noise->SetCellularJitter(config.cellularJitter);
}

void glimmer::ClimateSampler::EnsureNoiseBound(const int worldSeed,
                                               const DimensionResource *dimensionResource) {
    if (dimensionResource == nullptr) {
        return;
    }
    if (boundWorldSeed_ == worldSeed && boundDimensionResource_ == dimensionResource) {
        return;
    }
    for (uint8_t i = 0; i < fieldCount_; ++i) {
        noises_[i] = std::make_unique<FastNoiseLite>();
        ApplyNoiseConfig(noises_[i].get(), GetNoiseConfig(dimensionResource, static_cast<NoiseField>(i)), worldSeed);
    }
    boundWorldSeed_ = worldSeed;
    boundDimensionResource_ = dimensionResource;
}

int glimmer::ClimateSampler::GetFirstTileTerrainY(const int worldSeed, const DimensionResource *dimensionResource,
                                                  const int x) {
    if (dimensionResource == nullptr) {
        return std::numeric_limits<int>::min();
    }
    EnsureNoiseBound(worldSeed, dimensionResource);
    auto &heightMap = heightMap_[dimensionResource];
    const auto it = heightMap.find(x);
    if (it != heightMap.end()) {
        return it->second;
    }
    const auto sampleX = static_cast<float>(x);
    const float continentNoise = (GetNoise(NoiseField::ContinentHeight)->GetNoise(sampleX, 0.0F) + 1.0F) * 0.5F;
    const int continentMaxHeight = dimensionResource->continentMaxY - dimensionResource->
                                   continentMinY;
    const int height = dimensionResource->continentMinY + static_cast<int>(
                           static_cast<float>(continentMaxHeight) * continentNoise);
    heightMap[x] = height;
    return height;
}

float glimmer::ClimateSampler::GetHumidity(const int worldSeed, const DimensionResource *dimensionResource,
                                           const TileVector2D &pos) {
    if (dimensionResource == nullptr) {
        return 0.5F;
    }
    EnsureNoiseBound(worldSeed, dimensionResource);
    auto &humidityMap = humidityMap_[dimensionResource];
    const auto it = humidityMap.find(pos);
    if (it != humidityMap.end()) {
        return it->second;
    }
    humidityMap[pos] = (GetNoise(NoiseField::Humidity)->GetNoise(static_cast<float>(pos.x),
                                                                 static_cast<float>(pos.y)) + 1) * 0.5F;
    return humidityMap[pos];
}

float glimmer::ClimateSampler::GetTemperature(const int worldSeed, const DimensionResource *dimensionResource,
                                              const TileVector2D &pos, const float elevation) {
    if (dimensionResource == nullptr) {
        return 0.5F;
    }
    EnsureNoiseBound(worldSeed, dimensionResource);
    auto &temperatureMap = temperatureMap_[dimensionResource];
    const auto it = temperatureMap.find(pos);
    if (it != temperatureMap.end()) {
        return it->second;
    }
    const float noiseTemp = (GetNoise(NoiseField::Temperature)->GetNoise(
                                 static_cast<float>(pos.x),
                                 static_cast<float>(pos.y)
                             ) + 1.0F) * 0.5F;
    const float altitudePenalty = std::pow(1.0F - elevation, 1.5F);
    const float temperature = noiseTemp * altitudePenalty;
    temperatureMap[pos] = temperature;
    return temperatureMap[pos];
}

float glimmer::ClimateSampler::GetWeirdness(const int worldSeed, const DimensionResource *dimensionResource,
                                            const TileVector2D &pos) {
    if (dimensionResource == nullptr) {
        return 0.5F;
    }
    EnsureNoiseBound(worldSeed, dimensionResource);
    auto &weirdnessMap = weirdnessMap_[dimensionResource];
    const auto it = weirdnessMap.find(pos);
    if (it != weirdnessMap.end()) {
        return it->second;
    }
    weirdnessMap[pos] = (GetNoise(NoiseField::Weirdness)->GetNoise(static_cast<float>(pos.x * 0.000285714),
                                                                   static_cast<float>(pos.y * 0.000285714)) + 1) *
                        0.5F;
    return weirdnessMap[pos];
}

float glimmer::ClimateSampler::GetErosion(const int worldSeed, const DimensionResource *dimensionResource,
                                          const TileVector2D &pos) {
    if (dimensionResource == nullptr) {
        return 0.5F;
    }
    EnsureNoiseBound(worldSeed, dimensionResource);
    auto &erosionMap = erosionMap_[dimensionResource];
    const auto it = erosionMap.find(pos);
    if (it != erosionMap.end()) {
        return it->second;
    }
    erosionMap[pos] = (GetNoise(NoiseField::Erosion)->GetNoise(static_cast<float>(pos.x),
                                                               static_cast<float>(pos.y)) + 1) * 0.5F;
    return erosionMap[pos];
}
