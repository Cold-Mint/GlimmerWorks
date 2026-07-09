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
#include "BiomeStructureConditionProcessor.h"
#include <set>

#include "core/log/LogCat.h"

std::string glimmer::BiomeStructureConditionProcessor::GetName()
{
    return STRUCTURE_PLACEMENT_CONDITIONS_BIOME;
}

std::bitset<CHUNK_AREA> glimmer::BiomeStructureConditionProcessor::Match(TerrainResult* terrainResult,
                                                                         const VariableConfig& variableConfig)
{
    const size_t size = variableConfig.definition.size();
    std::set<std::string, std::less<>> biomeSet;
    for (int i = 0; i < size; i++)
    {
        std::stringstream biomeStream;
        biomeStream << "biome_";
        biomeStream << std::to_string(i);
        std::string varKey = biomeStream.str();
        const VariableDefinition* definition = variableConfig.FindVariable(varKey);
        if (definition == nullptr || definition->type != std::to_underlying(VariableDefinitionType::REF))
        {
            break;
        }
        ResourceRef resourceRef;
        definition->AsResourceRef(resourceRef);
        std::string biomeId = Resource::GenerateId(resourceRef.GetPackageId(), resourceRef.GetResourceKey());
        biomeSet.insert(biomeId);
    }
    std::bitset<CHUNK_AREA> result;
    if (biomeSet.empty())
    {
        return result;
    }
    int matchedTileCount = 0;
    for (int localX = 0; localX < CHUNK_SIZE; localX++)
    {
        for (int localY = 0; localY < CHUNK_SIZE; localY++)
        {
            const TerrainTileResult& self = terrainResult->QueryTerrain(localX, localY);
            if (self.terrainType != TerrainResultType::SOLID || self.biomeResource == nullptr)
            {
                continue;
            }
            std::string tileBiomeId = Resource::GenerateId(*self.biomeResource);
            if (biomeSet.contains(tileBiomeId))
            {
                int tileIndex = localY * CHUNK_SIZE + localX;
                result[tileIndex] = true;
                matchedTileCount++;
            }
        }
    }
    return result;
}
