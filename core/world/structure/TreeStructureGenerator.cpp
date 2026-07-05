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
#include "TreeStructureGenerator.h"

#include "core/world/WorldContext.h"

std::optional<glimmer::StructureInfo> glimmer::TreeStructureGenerator::Generate(WorldContext* worldContext,
    const TileVector2D& startPosition, IStructureResource* structureResource)
{
    if (structureResource == nullptr || worldContext == nullptr)
    {
        return std::nullopt;
    }
    ChunkGenerator* chunkGenerator = worldContext->GetChunkGenerator();
    if (chunkGenerator == nullptr)
    {
        return std::nullopt;
    }
    auto treeStructureResource = dynamic_cast<TreeStructureResource*>(structureResource);
    ResourceRef& trunkRef = treeStructureResource->data.at(treeStructureResource->trunkDataIndex);
    ResourceRef& leafRef = treeStructureResource->data.at(treeStructureResource->leafDataIndex);
    auto structureInfo = StructureInfo();
    int trunkHeight = treeStructureResource->trunkHeightMin + static_cast<uint8_t>(chunkGenerator->
        GetHumidity(startPosition) *
        static_cast<float>(treeStructureResource->trunkHeightMax - treeStructureResource->trunkHeightMin));
    auto trunkTileLayer = static_cast<TileLayerType>(treeStructureResource->trunkTileLayer);
    for (int y = 0; y < trunkHeight; ++y)
    {
        for (int x = 0; x < treeStructureResource->trunkWidth; ++x)
        {
            structureInfo.SetTile(trunkTileLayer, TileVector2D(x, y), trunkRef);
        }
    }
    if (treeStructureResource->hasLeaves)
    {
        auto leafTileLayer = static_cast<TileLayerType>(treeStructureResource->leafTileLayer);
        uint8_t leafRadius = treeStructureResource->leafRadius;
        for (int i = 0; i < treeStructureResource->leafClusterCount; ++i)
        {
            int clusterY = trunkHeight - i * treeStructureResource->leafVerticalSpacing;
            for (int x = -leafRadius; x <= leafRadius; ++x)
            {
                for (int y = -leafRadius; y <= leafRadius; ++y)
                {
                    if (x * x + y * y <= leafRadius * leafRadius && y >= 0)
                    {
                        structureInfo.SetTile(leafTileLayer,
                                              TileVector2D{
                                                  x + treeStructureResource->trunkWidth / 2,
                                                  clusterY + y
                                              },
                                              leafRef
                        );
                    }
                }
            }
        }
    }
    return structureInfo;
}

glimmer::StructureGeneratorType glimmer::TreeStructureGenerator::GetStructureGeneratorType() const
{
    return StructureGeneratorType::Tree;
}
