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
#include "core/mod/Resource.h"

glimmer::ClimateSampler::ClimateSampler(const int worldSeed, const DimensionResource *dimensionResource) {
    const DimensionResource defaultDimension;
    if (dimensionResource == nullptr) {
        dimensionResource = &defaultDimension;
    }
    for (uint8_t i = 0; i < kNoiseFieldCount; ++i) {
        const auto field = static_cast<NoiseField>(i);
        noises_[i] = std::make_unique<FastNoiseLite>();
        ApplyNoiseConfig(noises_[i].get(), GetNoiseConfig(dimensionResource, field), worldSeed);
    }
}

FastNoiseLite *glimmer::ClimateSampler::GetNoise(const NoiseField field) {
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

int glimmer::ClimateSampler::GetFirstTileTerrainY(const int x) {
    const auto it = heightMap_.find(x);
    if (it != heightMap_.end()) {
        return it->second;
    }
    const auto sampleX = static_cast<float>(x);
    const float continentNoise = (GetNoise(NoiseField::ContinentHeight)->GetNoise(sampleX, 0.0F) + 1.0F) * 0.5F;
    const int height = GROUND_START_HEIGHT + CONTINENT_MAX_HEIGHT * continentNoise;
    heightMap_[x] = height;
    return height;
}

float glimmer::ClimateSampler::GetHumidity(const TileVector2D &pos) {
    const auto it = humidityMap_.find(pos);
    if (it != humidityMap_.end()) {
        return it->second;
    }
    humidityMap_[pos] = (GetNoise(NoiseField::Humidity)->GetNoise(static_cast<float>(pos.x),
                                                                  static_cast<float>(pos.y)) + 1) * 0.5F;
    return humidityMap_[pos];
}

float glimmer::ClimateSampler::GetTemperature(const TileVector2D &pos, const float elevation) {
    const auto it = temperatureMap_.find(pos);
    if (it != temperatureMap_.end()) {
        return it->second;
    }
    const float noiseTemp = (GetNoise(NoiseField::Temperature)->GetNoise(
                                 static_cast<float>(pos.x),
                                 static_cast<float>(pos.y)
                             ) + 1.0F) * 0.5F;
    const float altitudePenalty = std::pow(1.0F - elevation, 1.5F);
    const float temperature = noiseTemp * altitudePenalty;

    temperatureMap_[pos] = temperature;
    return temperatureMap_[pos];
}

float glimmer::ClimateSampler::GetWeirdness(const TileVector2D &pos) {
    const auto it = weirdnessMap_.find(pos);
    if (it != weirdnessMap_.end()) {
        return it->second;
    }
    weirdnessMap_[pos] = (GetNoise(NoiseField::Weirdness)->GetNoise(static_cast<float>(pos.x * 0.000285714),
                                                                    static_cast<float>(pos.y * 0.000285714)) + 1) *
                         0.5F;
    return weirdnessMap_[pos];
}

float glimmer::ClimateSampler::GetErosion(const TileVector2D &pos) {
    const auto it = erosionMap_.find(pos);
    if (it != erosionMap_.end()) {
        return it->second;
    }
    erosionMap_[pos] = (GetNoise(NoiseField::Erosion)->GetNoise(static_cast<float>(pos.x),
                                                                static_cast<float>(pos.y)) + 1) * 0.5F;
    return erosionMap_[pos];
}
