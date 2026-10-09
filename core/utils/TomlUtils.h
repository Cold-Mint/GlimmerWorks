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
#pragma once

#include "toml11/find.hpp"

#include "core/contributor/Contributor.h"
#include "core/ecs/Box2dFilter.h"
#include "core/lootTable/LootEntry.h"
#include "core/mod/PackManifest.h"
#include "core/mod/Resource.h"
#include "core/mod/dataPack/PackDependence.h"
#include "core/mod/resourcePack/ResourcePackConfig.h"

namespace toml {
    template<>
    struct from<glimmer::ResourceRef> {
        static glimmer::ResourceRef from_toml(const value &v) {
            glimmer::ResourceRef r;
            r.SetPackageId(toml::find<std::string>(v, "pack_id"));
            r.SetResourceType(static_cast<ResourceTypeMessage>(toml::find<int>(v, "resource_type")));
            r.SetResourceKey(toml::find<std::string>(v, "resource_key"));
            return r;
        }
    };


    template<>
    struct from<glimmer::AbilityConfig> {
        static glimmer::AbilityConfig from_toml(const value &v) {
            glimmer::AbilityConfig resource;
            resource.chainMiningRadius = toml::find_or<uint8_t>(v, "chain_mining_radius", 0);
            resource.enablePrecisionMining = toml::find_or<bool>(v, "enable_precision_mining", false);
            resource.mineAbleLayer = toml::find_or<uint8_t>(v, "mine_able_layer", 0);
            resource.miningEfficiency = toml::find_or<float>(v, "mining_efficiency", 0);
            resource.miningRange = toml::find_or<float>(v, "mining_range", 5);
            return resource;
        }
    };

    template<>
    struct from<glimmer::BiomeGrowthConditionResource> {
        static glimmer::BiomeGrowthConditionResource from_toml(const value &v) {
            glimmer::BiomeGrowthConditionResource resource;
            resource.processorId = toml::find_or<uint8_t>(v, "processor_id", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.targetBiomes = toml::find<std::vector<glimmer::ResourceRef> >(v, "target_biomes");
            return resource;
        }
    };

    template<>
    struct from<glimmer::BiomeStructurePlacementConditionsResource> {
        static glimmer::BiomeStructurePlacementConditionsResource from_toml(const value &v) {
            glimmer::BiomeStructurePlacementConditionsResource resource;
            resource.processorId = toml::find_or<uint8_t>(v, "processor_id", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.targetBiomes = toml::find<std::vector<glimmer::ResourceRef> >(v, "target_biomes");
            return resource;
        }
    };

    template<>
    struct from<glimmer::Box2dFilter> {
        static glimmer::Box2dFilter from_toml(const value &v) {
            glimmer::Box2dFilter resource;
            resource.categoryBits = toml::find_or<uint64_t>(v, "category_bits", 0);
            resource.maskBits = toml::find_or<uint64_t>(v, "mask_bits", 0);
            return resource;
        }
    };

    template<>
    struct from<glimmer::ColorResource> {
        static glimmer::ColorResource from_toml(const value &v) {
            glimmer::ColorResource resource;
            resource.a = toml::find_or<uint8_t>(v, "a", 255);
            resource.b = toml::find_or<uint8_t>(v, "b", 0);
            resource.g = toml::find_or<uint8_t>(v, "g", 0);
            resource.r = toml::find_or<uint8_t>(v, "r", 0);
            return resource;
        }
    };

    template<>
    struct from<glimmer::DataPackManifest> {
        static glimmer::DataPackManifest from_toml(const value &v) {
            glimmer::DataPackManifest resource;
            resource.author = toml::find<std::string>(v, "author");
            resource.description = toml::find<glimmer::ResourceRef>(v, "description");
            resource.id = toml::find<std::string>(v, "id");
            resource.minGameVersion = toml::find<uint32_t>(v, "min_game_version");
            resource.name = toml::find<glimmer::ResourceRef>(v, "name");
            resource.packDependencies = toml::find<std::vector<glimmer::PackDependence> >(v, "pack_dependencies");
            resource.resPack = toml::find<bool>(v, "res_pack");
            resource.templateSearchPath = toml::find_or<std::vector<std::string> >(
                v, "template_search_path", {TEMPLATE_CURRENT, TEMPLATE_ROOT});
            resource.versionName = toml::find<std::string>(v, "version_name");
            resource.versionNumber = toml::find<uint32_t>(v, "version_number");
            return resource;
        }
    };

    template<>
    struct from<glimmer::FixedColorResource> {
        static glimmer::FixedColorResource from_toml(const value &v) {
            glimmer::FixedColorResource resource;
            resource.a = toml::find_or<uint8_t>(v, "a", 255);
            resource.b = toml::find_or<uint8_t>(v, "b", 0);
            resource.g = toml::find_or<uint8_t>(v, "g", 0);
            resource.r = toml::find_or<uint8_t>(v, "r", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            return resource;
        }
    };

    template<>
    struct from<glimmer::GpuSamplerResource> {
        static glimmer::GpuSamplerResource from_toml(const value &v) {
            glimmer::GpuSamplerResource resource;
            resource.addressModeU = toml::find_or<uint8_t>(v, "address_mode_u", 0);
            resource.addressModeV = toml::find_or<uint8_t>(v, "address_mode_v", 0);
            resource.addressModeW = toml::find_or<uint8_t>(v, "address_mode_w", 2);
            resource.compareOp = toml::find_or<uint8_t>(v, "compare_op", 0);
            resource.enableAnisotropy = toml::find_or<bool>(v, "enable_anisotropy", false);
            resource.enableCompare = toml::find_or<bool>(v, "enable_compare", false);
            resource.magFilter = toml::find_or<uint8_t>(v, "mag_filter", 0);
            resource.maxAnisotropy = toml::find_or<float>(v, "max_anisotropy", 0);
            resource.maxLod = toml::find_or<float>(v, "max_lod", 0);
            resource.minFilter = toml::find_or<uint8_t>(v, "min_filter", 0);
            resource.minLod = toml::find_or<float>(v, "min_lod", 0);
            resource.mipLodBias = toml::find_or<float>(v, "mip_lod_bias", 0);
            resource.mipmapMode = toml::find_or<uint8_t>(v, "mipmap_mode", 0);
            return resource;
        }
    };

    template<>
    struct from<glimmer::HeightGrowthConditionResource> {
        static glimmer::HeightGrowthConditionResource from_toml(const value &v) {
            glimmer::HeightGrowthConditionResource resource;
            resource.maxHeightPercent = toml::find_or<float>(v, "max_height_percent", 1.0F);
            resource.minHeightPercent = toml::find_or<float>(v, "min_height_percent", 0.0F);
            resource.processorId = toml::find_or<uint8_t>(v, "processor_id", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            return resource;
        }
    };

    template<>
    struct from<glimmer::HeightStructureConditionsResource> {
        static glimmer::HeightStructureConditionsResource from_toml(const value &v) {
            glimmer::HeightStructureConditionsResource resource;
            resource.maxHeightPercent = toml::find_or<float>(v, "max_height_percent", 1.0F);
            resource.minHeightPercent = toml::find_or<float>(v, "min_height_percent", 0.0F);
            resource.processorId = toml::find_or<uint8_t>(v, "processor_id", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            return resource;
        }
    };

    template<>
    struct from<glimmer::IBiomeDecoratorResource> {
        static glimmer::IBiomeDecoratorResource from_toml(const value &v) {
            glimmer::IBiomeDecoratorResource resource;
            resource.biomeDecoratorType = toml::find_or<uint8_t>(v, "biome_decorator_type", 0);
            resource.layerType = toml::find_or<uint8_t>(v, "layer_type", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            return resource;
        }
    };

    template<>
    struct from<glimmer::IGrowthConditionResource> {
        static glimmer::IGrowthConditionResource from_toml(const value &v) {
            glimmer::IGrowthConditionResource resource;
            resource.processorId = toml::find_or<uint8_t>(v, "processor_id", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            return resource;
        }
    };

    template<>
    struct from<glimmer::IShapeResource> {
        static glimmer::IShapeResource from_toml(const value &v) {
            glimmer::IShapeResource resource;
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.shapeType = toml::find_or<uint8_t>(v, "shape_type", 0);
            return resource;
        }
    };

    template<>
    struct from<glimmer::IStructurePlacementConditionsResource> {
        static glimmer::IStructurePlacementConditionsResource from_toml(const value &v) {
            glimmer::IStructurePlacementConditionsResource resource;
            resource.processorId = toml::find_or<uint8_t>(v, "processor_id", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            return resource;
        }
    };

    template<>
    struct from<glimmer::IStructureResource> {
        static glimmer::IStructureResource from_toml(const value &v) {
            glimmer::IStructureResource resource;
            resource.condition = toml::find_or<std::vector<glimmer::ResourceRef> >(v, "condition", {});
            resource.data = toml::find_or<std::vector<glimmer::ResourceRef> >(v, "data", {});
            resource.generatorId = toml::find_or<uint8_t>(v, "generator_id", 0);
            resource.priority = toml::find_or<uint8_t>(v, "priority", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            return resource;
        }
    };

    template<>
    struct from<glimmer::InitialInventoryResource> {
        static glimmer::InitialInventoryResource from_toml(const value &v) {
            glimmer::InitialInventoryResource resource;
            resource.addItems = toml::find<std::vector<glimmer::ItemMessageResource> >(v, "add_items");
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            return resource;
        }
    };

    template<>
    struct from<glimmer::ItemTagResource> {
        static glimmer::ItemTagResource from_toml(const value &v) {
            glimmer::ItemTagResource resource;
            resource.name = toml::find<std::string>(v, "name");
            resource.value = toml::find_or<uint8_t>(v, "value", 1);
            return resource;
        }
    };

    template<>
    struct from<glimmer::LightGrowthConditionResource> {
        static glimmer::LightGrowthConditionResource from_toml(const value &v) {
            glimmer::LightGrowthConditionResource resource;
            resource.lightSourceMask = toml::find_or<uint8_t>(v, "light_source_mask", 7);
            resource.maxLight = toml::find_or<uint8_t>(v, "max_light", 255);
            resource.minLight = toml::find_or<uint8_t>(v, "min_light", 0);
            resource.processorId = toml::find_or<uint8_t>(v, "processor_id", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            return resource;
        }
    };

    template<>
    struct from<glimmer::LootResource> {
        static glimmer::LootResource from_toml(const value &v) {
            glimmer::LootResource resource;
            resource.emptyWeight = toml::find_or<uint32_t>(v, "empty_weight", 0);
            resource.mandatory = toml::find_or<std::vector<glimmer::LootEntry> >(v, "mandatory", {});
            resource.pool = toml::find_or<std::vector<glimmer::LootEntry> >(v, "pool", {});
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.rolls = toml::find_or<uint32_t>(v, "rolls", 1);
            return resource;
        }
    };

    template<>
    struct from<glimmer::NineSliceConfig> {
        static glimmer::NineSliceConfig from_toml(const value &v) {
            glimmer::NineSliceConfig resource;
            resource.bottomBorderPx = toml::find_or<float>(v, "bottom_border_px", 1.0F);
            resource.enableTiled = toml::find_or<bool>(v, "enable_tiled", false);
            resource.leftBorderPx = toml::find_or<float>(v, "left_border_px", 1.0F);
            resource.rightBorderPx = toml::find_or<float>(v, "right_border_px", 1.0F);
            resource.scale = toml::find_or<float>(v, "scale", 0.0F);
            resource.tileScale = toml::find_or<float>(v, "tile_scale", 1.0F);
            resource.topBorderPx = toml::find_or<float>(v, "top_border_px", 1.0F);
            return resource;
        }
    };

    template<>
    struct from<glimmer::NoiseConfig> {
        static glimmer::NoiseConfig from_toml(const value &v) {
            glimmer::NoiseConfig resource;
            resource.cellularDistanceFunction = toml::find_or<uint8_t>(v, "cellular_distance_function", 1);
            resource.cellularJitter = toml::find_or<float>(v, "cellular_jitter", 1.0F);
            resource.cellularReturnType = toml::find_or<uint8_t>(v, "cellular_return_type", 1);
            resource.fractalType = toml::find_or<uint8_t>(v, "fractal_type", 0);
            resource.frequency = toml::find_or<float>(v, "frequency", 0.01F);
            resource.gain = toml::find_or<float>(v, "gain", 0.5F);
            resource.lacunarity = toml::find_or<float>(v, "lacunarity", 2.0F);
            resource.noiseType = toml::find_or<uint8_t>(v, "noise_type", 3);
            resource.octaves = toml::find_or<int>(v, "octaves", 3);
            resource.pingPongStrength = toml::find_or<float>(v, "ping_pong_strength", 2.0F);
            resource.seedOffset = toml::find_or<int>(v, "seed_offset", 0);
            resource.weightedStrength = toml::find_or<float>(v, "weighted_strength", 0.0F);
            return resource;
        }
    };

    template<>
    struct from<glimmer::NoneGrowthConditionResource> {
        static glimmer::NoneGrowthConditionResource from_toml(const value &v) {
            glimmer::NoneGrowthConditionResource resource;
            resource.processorId = toml::find_or<uint8_t>(v, "processor_id", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            return resource;
        }
    };

    template<>
    struct from<glimmer::NoneStructurePlacementConditionsResource> {
        static glimmer::NoneStructurePlacementConditionsResource from_toml(const value &v) {
            glimmer::NoneStructurePlacementConditionsResource resource;
            resource.processorId = toml::find_or<uint8_t>(v, "processor_id", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            return resource;
        }
    };

    template<>
    struct from<glimmer::PackDependence> {
        static glimmer::PackDependence from_toml(const value &v) {
            glimmer::PackDependence resource;
            resource.minVersion = toml::find<uint32_t>(v, "min_version");
            resource.packId = toml::find<std::string>(v, "pack_id");
            return resource;
        }
    };

    template<>
    struct from<glimmer::RectangleShapeResource> {
        static glimmer::RectangleShapeResource from_toml(const value &v) {
            glimmer::RectangleShapeResource resource;
            resource.height = toml::find_or<float>(v, "height", 1.0F);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.shapeType = toml::find_or<uint8_t>(v, "shape_type", 0);
            resource.width = toml::find_or<float>(v, "width", 1.0F);
            return resource;
        }
    };

    template<>
    struct from<glimmer::RequiredTag> {
        static glimmer::RequiredTag from_toml(const value &v) {
            glimmer::RequiredTag resource;
            resource.exactMatch = toml::find_or<bool>(v, "exact_match", true);
            resource.requiredTag = toml::find<std::string>(v, "required_tag");
            resource.requiredWeight = toml::find_or<uint16_t>(v, "required_weight", 1);
            return resource;
        }
    };

    template<>
    struct from<glimmer::Resource> {
        static glimmer::Resource from_toml(const value &v) {
            glimmer::Resource resource;
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            return resource;
        }
    };

    template<>
    struct from<glimmer::ResourcePackManifest> {
        static glimmer::ResourcePackManifest from_toml(const value &v) {
            glimmer::ResourcePackManifest resource;
            resource.author = toml::find<std::string>(v, "author");
            resource.description = toml::find<glimmer::ResourceRef>(v, "description");
            resource.id = toml::find<std::string>(v, "id");
            resource.minGameVersion = toml::find<uint32_t>(v, "min_game_version");
            resource.name = toml::find<glimmer::ResourceRef>(v, "name");
            resource.resPack = toml::find<bool>(v, "res_pack");
            resource.templateSearchPath = toml::find_or<std::vector<std::string> >(
                v, "template_search_path", {TEMPLATE_CURRENT, TEMPLATE_ROOT});
            resource.versionName = toml::find<std::string>(v, "version_name");
            resource.versionNumber = toml::find<uint32_t>(v, "version_number");
            return resource;
        }
    };

    template<>
    struct from<glimmer::RoundedRectangleShapeResource> {
        static glimmer::RoundedRectangleShapeResource from_toml(const value &v) {
            glimmer::RoundedRectangleShapeResource resource;
            resource.height = toml::find_or<float>(v, "height", 1.0F);
            resource.radius = toml::find_or<float>(v, "radius", 0.0F);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.shapeType = toml::find_or<uint8_t>(v, "shape_type", 0);
            resource.width = toml::find_or<float>(v, "width", 1.0F);
            return resource;
        }
    };

    template<>
    struct from<glimmer::SpacingStructureConditionsResource> {
        static glimmer::SpacingStructureConditionsResource from_toml(const value &v) {
            glimmer::SpacingStructureConditionsResource resource;
            resource.isVertical = toml::find_or<bool>(v, "is_vertical", false);
            resource.minDistance = toml::find_or<int>(v, "min_distance", 0);
            resource.processorId = toml::find_or<uint8_t>(v, "processor_id", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            return resource;
        }
    };

    template<>
    struct from<glimmer::StaticStructureResource> {
        static glimmer::StaticStructureResource from_toml(const value &v) {
            glimmer::StaticStructureResource resource;
            resource.condition = toml::find_or<std::vector<glimmer::ResourceRef> >(v, "condition", {});
            resource.data = toml::find_or<std::vector<glimmer::ResourceRef> >(v, "data", {});
            resource.generatorId = toml::find_or<uint8_t>(v, "generator_id", 0);
            resource.priority = toml::find_or<uint8_t>(v, "priority", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.tileInfo = toml::find_or<std::vector<glimmer::TileInfo> >(v, "tile_info", {});
            return resource;
        }
    };

    template<>
    struct from<glimmer::StringResource> {
        static glimmer::StringResource from_toml(const value &v) {
            glimmer::StringResource resource;
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.value = toml::find<std::string>(v, "value");
            return resource;
        }
    };

    template<>
    struct from<glimmer::SurfaceStructurePlacementConditionsResource> {
        static glimmer::SurfaceStructurePlacementConditionsResource from_toml(const value &v) {
            glimmer::SurfaceStructurePlacementConditionsResource resource;
            resource.processorId = toml::find_or<uint8_t>(v, "processor_id", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            return resource;
        }
    };

    template<>
    struct from<glimmer::TilePlacementForbiddenZone> {
        static glimmer::TilePlacementForbiddenZone from_toml(const value &v) {
            glimmer::TilePlacementForbiddenZone resource;
            resource.height = toml::find_or<int>(v, "height", 1);
            resource.offsetX = toml::find_or<int>(v, "offset_x", 0);
            resource.offsetY = toml::find_or<int>(v, "offset_y", 0);
            resource.width = toml::find_or<int>(v, "width", 1);
            return resource;
        }
    };

    template<>
    struct from<glimmer::TimeGrowthConditionResource> {
        static glimmer::TimeGrowthConditionResource from_toml(const value &v) {
            glimmer::TimeGrowthConditionResource resource;
            resource.maxTime = toml::find_or<float>(v, "max_time", 1.0F);
            resource.minTime = toml::find_or<float>(v, "min_time", 0.0F);
            resource.processorId = toml::find_or<uint8_t>(v, "processor_id", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            return resource;
        }
    };

    template<>
    struct from<glimmer::TreeStructureResource> {
        static glimmer::TreeStructureResource from_toml(const value &v) {
            glimmer::TreeStructureResource resource;
            resource.condition = toml::find_or<std::vector<glimmer::ResourceRef> >(v, "condition", {});
            resource.data = toml::find_or<std::vector<glimmer::ResourceRef> >(v, "data", {});
            resource.generatorId = toml::find_or<uint8_t>(v, "generator_id", 0);
            resource.hasLeaves = toml::find_or<bool>(v, "has_leaves", false);
            resource.leafClusterCount = toml::find_or<uint8_t>(v, "leaf_cluster_count", 1);
            resource.leafDataIndex = toml::find_or<uint8_t>(v, "leaf_data_index", 0);
            resource.leafRadius = toml::find_or<uint8_t>(v, "leaf_radius", 2);
            resource.leafTileLayer = toml::find_or<uint8_t>(v, "leaf_tile_layer", 0);
            resource.leafVerticalSpacing = toml::find_or<uint8_t>(v, "leaf_vertical_spacing", 0);
            resource.priority = toml::find_or<uint8_t>(v, "priority", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.trunkDataIndex = toml::find_or<uint8_t>(v, "trunk_data_index", 0);
            resource.trunkHeightMax = toml::find_or<uint8_t>(v, "trunk_height_max", 9);
            resource.trunkHeightMin = toml::find_or<uint8_t>(v, "trunk_height_min", 5);
            resource.trunkTileLayer = toml::find_or<uint8_t>(v, "trunk_tile_layer", 0);
            resource.trunkWidth = toml::find_or<uint8_t>(v, "trunk_width", 1);
            return resource;
        }
    };

    template<>
    struct from<glimmer::UniformBlockResource> {
        static glimmer::UniformBlockResource from_toml(const value &v) {
            glimmer::UniformBlockResource resource;
            resource.members = toml::find_or<std::vector<glimmer::UniformMemberResource> >(v, "members", {});
            resource.name = toml::find<std::string>(v, "name");
            return resource;
        }
    };

    template<>
    struct from<glimmer::UniformMemberResource> {
        static glimmer::UniformMemberResource from_toml(const value &v) {
            glimmer::UniformMemberResource resource;
            resource.name = toml::find<std::string>(v, "name");
            resource.source = toml::find<std::string>(v, "source");
            resource.type = toml::find<std::string>(v, "type");
            resource.value = toml::find_or<std::vector<float> >(v, "value", {});
            return resource;
        }
    };

    template<>
    struct from<glimmer::Vector2DIResource> {
        static glimmer::Vector2DIResource from_toml(const value &v) {
            glimmer::Vector2DIResource resource;
            resource.x = toml::find_or<int>(v, "x", 0);
            resource.y = toml::find_or<int>(v, "y", 0);
            return resource;
        }
    };

    template<>
    struct from<glimmer::Vector2DResource> {
        static glimmer::Vector2DResource from_toml(const value &v) {
            glimmer::Vector2DResource resource;
            resource.x = toml::find_or<float>(v, "x", 0.0F);
            resource.y = toml::find_or<float>(v, "y", 0.0F);
            return resource;
        }
    };

    template<>
    struct from<glimmer::AbilityItemResource> {
        static glimmer::AbilityItemResource from_toml(const value &v) {
            glimmer::AbilityItemResource resource;
            resource.ability = toml::find<uint8_t>(v, "ability");
            resource.abilityConfig = toml::find_or<glimmer::AbilityConfig>(v, "ability_config", {});
            resource.canUseAlone = toml::find_or<bool>(v, "can_use_alone", false);
            resource.description = toml::find_or<glimmer::ResourceRef>(v, "description", {});
            resource.lightSource = toml::find_or<glimmer::ResourceRef>(v, "light_source", {});
            resource.maxDurability = toml::find_or<uint32_t>(v, "max_durability", 16);
            resource.name = toml::find<glimmer::ResourceRef>(v, "name");
            resource.pipeline = toml::find_or<glimmer::ResourceRef>(v, "pipeline", {});
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.sampler = toml::find_or<glimmer::ResourceRef>(v, "sampler", {});
            resource.tags = toml::find_or<std::vector<glimmer::ItemTagResource> >(v, "tags", {});
            resource.texture = toml::find<glimmer::ResourceRef>(v, "texture");
            resource.unbreakable = toml::find_or<bool>(v, "unbreakable", false);
            return resource;
        }
    };

    template<>
    struct from<glimmer::BiomeResource> {
        static glimmer::BiomeResource from_toml(const value &v) {
            glimmer::BiomeResource resource;
            resource.bgm = toml::find<glimmer::ResourceRef>(v, "bgm");
            resource.decors = toml::find<std::vector<glimmer::ResourceRef> >(v, "decors");
            resource.dimensions = toml::find<std::vector<glimmer::ResourceRef> >(v, "dimensions");
            resource.elevation = toml::find_or<float>(v, "elevation", 0.5F);
            resource.erosion = toml::find_or<float>(v, "erosion", 0.5F);
            resource.humidity = toml::find_or<float>(v, "humidity", 0.5F);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.strictnessElevation = toml::find_or<float>(v, "strictness_elevation", 1.0F);
            resource.strictnessErosion = toml::find_or<float>(v, "strictness_erosion", 1.0F);
            resource.strictnessHumidity = toml::find_or<float>(v, "strictness_humidity", 1.0F);
            resource.strictnessSurfaceProximity = toml::find_or<float>(v, "strictness_surface_proximity", 1.0F);
            resource.strictnessTemperature = toml::find_or<float>(v, "strictness_temperature", 1.0F);
            resource.strictnessWeirdness = toml::find_or<float>(v, "strictness_weirdness", 1.0F);
            resource.surfaceProximity = toml::find_or<float>(v, "surface_proximity", 0.5F);
            resource.temperature = toml::find_or<float>(v, "temperature", 0.5F);
            resource.weirdness = toml::find_or<float>(v, "weirdness", 0.5F);
            return resource;
        }
    };

    template<>
    struct from<glimmer::ComposableItemResource> {
        static glimmer::ComposableItemResource from_toml(const value &v) {
            glimmer::ComposableItemResource resource;
            resource.defaultAbilityList = toml::find_or<std::vector<glimmer::ItemMessageResource> >(
                v, "default_ability_list", {});
            resource.description = toml::find_or<glimmer::ResourceRef>(v, "description", {});
            resource.lightSource = toml::find_or<glimmer::ResourceRef>(v, "light_source", {});
            resource.maxDurability = toml::find_or<uint32_t>(v, "max_durability", 16);
            resource.name = toml::find<glimmer::ResourceRef>(v, "name");
            resource.pipeline = toml::find_or<glimmer::ResourceRef>(v, "pipeline", {});
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.sampler = toml::find_or<glimmer::ResourceRef>(v, "sampler", {});
            resource.slotSize = toml::find<size_t>(v, "slot_size");
            resource.tags = toml::find_or<std::vector<glimmer::ItemTagResource> >(v, "tags", {});
            resource.texture = toml::find<glimmer::ResourceRef>(v, "texture");
            resource.unbreakable = toml::find_or<bool>(v, "unbreakable", false);
            return resource;
        }
    };

    template<>
    struct from<glimmer::Contributor> {
        static glimmer::Contributor from_toml(const value &v) {
            glimmer::Contributor resource;
            resource.country = toml::find<std::string>(v, "country");
            resource.displayName = toml::find<glimmer::ResourceRef>(v, "display_name");
            resource.name = toml::find<std::string>(v, "name");
            resource.uuid = toml::find<std::string>(v, "uuid");
            return resource;
        }
    };

    template<>
    struct from<glimmer::DimensionResource> {
        static glimmer::DimensionResource from_toml(const value &v) {
            glimmer::DimensionResource resource;
            resource.allowAsStarting = toml::find_or<bool>(v, "allow_as_starting", false);
            resource.backLightKeyframes = toml::find<std::vector<glimmer::LightKeyframe> >(v, "back_light_keyframes");
            resource.continentMaxY = toml::find_or<int>(v, "continent_max_y", 176);
            resource.continentMinY = toml::find_or<int>(v, "continent_min_y", 64);
            resource.continentNoise = toml::find<glimmer::NoiseConfig>(v, "continent_noise");
            resource.erosionNoise = toml::find<glimmer::NoiseConfig>(v, "erosion_noise");
            resource.humidityNoise = toml::find<glimmer::NoiseConfig>(v, "humidity_noise");
            resource.initialTime = toml::find_or<float>(v, "initial_time", 0.0F);
            resource.maxX = toml::find_or<int>(v, "max_x", 30000);
            resource.maxY = toml::find_or<int>(v, "max_y", 320);
            resource.minX = toml::find_or<int>(v, "min_x", -30000);
            resource.minY = toml::find_or<int>(v, "min_y", 0);
            resource.name = toml::find<glimmer::ResourceRef>(v, "name");
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.seaLevelY = toml::find_or<int>(v, "sea_level_y", 160);
            resource.skyHeight = toml::find_or<int>(v, "sky_height", 16);
            resource.skyLightKeyframes = toml::find<std::vector<glimmer::LightKeyframe> >(v, "sky_light_keyframes");
            resource.temperatureNoise = toml::find<glimmer::NoiseConfig>(v, "temperature_noise");
            resource.timeFlowSpeed = toml::find_or<float>(v, "time_flow_speed", 1.0F);
            resource.weirdnessNoise = toml::find<glimmer::NoiseConfig>(v, "weirdness_noise");
            return resource;
        }
    };

    template<>
    struct from<glimmer::FillBiomeDecoratorResource> {
        static glimmer::FillBiomeDecoratorResource from_toml(const value &v) {
            glimmer::FillBiomeDecoratorResource resource;
            resource.biomeDecoratorType = toml::find_or<uint8_t>(v, "biome_decorator_type", 0);
            resource.layerType = toml::find_or<uint8_t>(v, "layer_type", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.tile = toml::find<glimmer::ResourceRef>(v, "tile");
            return resource;
        }
    };

    template<>
    struct from<glimmer::GPUPipelineResource> {
        static glimmer::GPUPipelineResource from_toml(const value &v) {
            glimmer::GPUPipelineResource resource;
            resource.blendMode = toml::find_or<uint8_t>(v, "blend_mode", 0);
            resource.fragmentShader = toml::find<glimmer::ResourceRef>(v, "fragment_shader");
            resource.fragmentUniformBlock = toml::find_or<std::vector<glimmer::ResourceRef> >(
                v, "fragment_uniform_block", {});
            resource.primitiveType = toml::find_or<uint8_t>(v, "primitive_type", 0);
            resource.vertexShader = toml::find<glimmer::ResourceRef>(v, "vertex_shader");
            resource.vertexUniformBlock = toml::find_or<std::vector<glimmer::ResourceRef> >(
                v, "vertex_uniform_block", {});
            return resource;
        }
    };

    template<>
    struct from<glimmer::ItemMessageResource> {
        static glimmer::ItemMessageResource from_toml(const value &v) {
            glimmer::ItemMessageResource resource;
            resource.abilityItemRef = toml::find_or<std::vector<glimmer::ItemMessageResource> >(
                v, "ability_item_ref", {});
            resource.amount = toml::find_or<uint64_t>(v, "amount", 1);
            resource.durabilityStrategyType = toml::find_or<int8_t>(v, "durability_strategy_type", -1);
            resource.item = toml::find<glimmer::ResourceRef>(v, "item");
            resource.locked = toml::find_or<bool>(v, "locked", false);
            return resource;
        }
    };

    template<>
    struct from<glimmer::LightKeyframe> {
        static glimmer::LightKeyframe from_toml(const value &v) {
            glimmer::LightKeyframe resource;
            resource.color = toml::find<glimmer::ResourceRef>(v, "color");
            resource.t = toml::find_or<float>(v, "t", 0.0F);
            return resource;
        }
    };

    template<>
    struct from<glimmer::LightMaskResource> {
        static glimmer::LightMaskResource from_toml(const value &v) {
            glimmer::LightMaskResource resource;
            resource.lightMaskColor = toml::find<glimmer::ResourceRef>(v, "light_mask_color");
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.tintFactor = toml::find_or<float>(v, "tint_factor", 0.0F);
            return resource;
        }
    };

    template<>
    struct from<glimmer::LightSourceResource> {
        static glimmer::LightSourceResource from_toml(const value &v) {
            glimmer::LightSourceResource resource;
            resource.lightBrightestAtCenter = toml::find_or<bool>(v, "light_brightest_at_center", true);
            resource.lightColor = toml::find_or<glimmer::ResourceRef>(v, "light_color", {});
            resource.lightRadius = toml::find_or<uint8_t>(v, "light_radius", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            return resource;
        }
    };

    template<>
    struct from<glimmer::LootEntry> {
        static glimmer::LootEntry from_toml(const value &v) {
            glimmer::LootEntry resource;
            resource.item = toml::find<glimmer::ResourceRef>(v, "item");
            resource.mandatory = toml::find_or<bool>(v, "mandatory", false);
            resource.max = toml::find_or<uint32_t>(v, "max", 1);
            resource.min = toml::find_or<uint32_t>(v, "min", 1);
            resource.weight = toml::find_or<uint32_t>(v, "weight", 30);
            return resource;
        }
    };

    template<>
    struct from<glimmer::MaterialItemResource> {
        static glimmer::MaterialItemResource from_toml(const value &v) {
            glimmer::MaterialItemResource resource;
            resource.description = toml::find_or<glimmer::ResourceRef>(v, "description", {});
            resource.lightSource = toml::find_or<glimmer::ResourceRef>(v, "light_source", {});
            resource.name = toml::find<glimmer::ResourceRef>(v, "name");
            resource.pipeline = toml::find_or<glimmer::ResourceRef>(v, "pipeline", {});
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.sampler = toml::find_or<glimmer::ResourceRef>(v, "sampler", {});
            resource.tags = toml::find_or<std::vector<glimmer::ItemTagResource> >(v, "tags", {});
            resource.texture = toml::find<glimmer::ResourceRef>(v, "texture");
            return resource;
        }
    };

    template<>
    struct from<glimmer::MineralBiomeDecoratorResource> {
        static glimmer::MineralBiomeDecoratorResource from_toml(const value &v) {
            glimmer::MineralBiomeDecoratorResource resource;
            resource.biomeDecoratorType = toml::find_or<uint8_t>(v, "biome_decorator_type", 0);
            resource.frequency = toml::find_or<float>(v, "frequency", 0.01F);
            resource.invertOreSpawnByDepth = toml::find_or<bool>(v, "invert_ore_spawn_by_depth", true);
            resource.layerType = toml::find_or<uint8_t>(v, "layer_type", 0);
            resource.maxSpawnElevation = toml::find_or<float>(v, "max_spawn_elevation", 0.5F);
            resource.minSpawnElevation = toml::find_or<float>(v, "min_spawn_elevation", 0.0F);
            resource.noiseType = toml::find<uint8_t>(v, "noise_type");
            resource.ore = toml::find<glimmer::ResourceRef>(v, "ore");
            resource.oreSpawnMaxNoiseThreshold = toml::find_or<float>(v, "ore_spawn_max_noise_threshold", 0.8F);
            resource.oreSpawnMinNoiseThreshold = toml::find_or<float>(v, "ore_spawn_min_noise_threshold", 0.5F);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.seedOffset = toml::find_or<int>(v, "seed_offset", 1024);
            return resource;
        }
    };

    template<>
    struct from<glimmer::PackManifest> {
        static glimmer::PackManifest from_toml(const value &v) {
            glimmer::PackManifest resource;
            resource.author = toml::find<std::string>(v, "author");
            resource.description = toml::find<glimmer::ResourceRef>(v, "description");
            resource.id = toml::find<std::string>(v, "id");
            resource.minGameVersion = toml::find<uint32_t>(v, "min_game_version");
            resource.name = toml::find<glimmer::ResourceRef>(v, "name");
            resource.resPack = toml::find<bool>(v, "res_pack");
            resource.templateSearchPath = toml::find_or<std::vector<std::string> >(
                v, "template_search_path", {TEMPLATE_CURRENT, TEMPLATE_ROOT});
            resource.versionName = toml::find<std::string>(v, "version_name");
            resource.versionNumber = toml::find<uint32_t>(v, "version_number");
            return resource;
        }
    };

    template<>
    struct from<glimmer::SkyColorKeyframe> {
        static glimmer::SkyColorKeyframe from_toml(const value &v) {
            glimmer::SkyColorKeyframe resource;
            resource.horizon = toml::find<glimmer::ResourceRef>(v, "horizon");
            resource.t = toml::find_or<float>(v, "t", 0.0F);
            resource.top = toml::find<glimmer::ResourceRef>(v, "top");
            return resource;
        }
    };

    template<>
    struct from<glimmer::SurfaceBiomeDecoratorResource> {
        static glimmer::SurfaceBiomeDecoratorResource from_toml(const value &v) {
            glimmer::SurfaceBiomeDecoratorResource resource;
            resource.biomeDecoratorType = toml::find_or<uint8_t>(v, "biome_decorator_type", 0);
            resource.layerType = toml::find_or<uint8_t>(v, "layer_type", 0);
            resource.openAirTile = toml::find_or<glimmer::ResourceRef>(v, "open_air_tile", {});
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.underwaterTile = toml::find_or<glimmer::ResourceRef>(v, "underwater_tile", {});
            return resource;
        }
    };

    template<>
    struct from<glimmer::AdjacentTileGrowthConditionResource> {
        static glimmer::AdjacentTileGrowthConditionResource from_toml(const value &v) {
            glimmer::AdjacentTileGrowthConditionResource resource;
            resource.offset = toml::find_or<glimmer::Vector2DIResource>(v, "offset", {0, -1});
            resource.processorId = toml::find_or<uint8_t>(v, "processor_id", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.targetTile = toml::find_or<glimmer::ResourceRef>(v, "target_tile", {});
            return resource;
        }
    };

    template<>
    struct from<glimmer::TileInfo> {
        static glimmer::TileInfo from_toml(const value &v) {
            glimmer::TileInfo resource;
            resource.layerType = toml::find_or<uint8_t>(v, "layer_type", 0);
            resource.position = toml::find<glimmer::Vector2DIResource>(v, "position");
            resource.tile = toml::find<glimmer::ResourceRef>(v, "tile");
            return resource;
        }
    };

    template<>
    struct from<glimmer::TileResource> {
        static glimmer::TileResource from_toml(const value &v) {
            glimmer::TileResource resource;
            resource.allowChainMining = toml::find_or<bool>(v, "allow_chain_mining", false);
            resource.allowDirAdjustAnchor = toml::find_or<bool>(v, "allow_dir_adjust_anchor", true);
            resource.allowOfflineGrowth = toml::find_or<bool>(v, "allow_offline_growth", true);
            resource.autoDigCostScale = toml::find_or<bool>(v, "auto_dig_cost_scale", true);
            resource.autoHardnessScale = toml::find_or<bool>(v, "auto_hardness_scale", true);
            resource.backLightMask = toml::find<glimmer::ResourceRef>(v, "back_light_mask");
            resource.blueprintTexture = toml::find_or<glimmer::ResourceRef>(v, "blueprint_texture", {});
            resource.breakSfx = toml::find<glimmer::ResourceRef>(v, "break_sfx");
            resource.canDropLoot = toml::find_or<bool>(v, "can_drop_loot", true);
            resource.customLootTable = toml::find_or<bool>(v, "custom_loot_table", false);
            resource.customTileAnchor = toml::find_or<glimmer::Vector2DIResource>(v, "custom_tile_anchor", {1, 1});
            resource.description = toml::find_or<glimmer::ResourceRef>(v, "description", {});
            resource.destroySelfOnGrowth = toml::find_or<bool>(v, "destroy_self_on_growth", false);
            resource.drawValidBlueprintColor = toml::find_or<bool>(v, "draw_valid_blueprint_color", true);
            resource.enableBlueprint = toml::find_or<bool>(v, "enable_blueprint", true);
            resource.enableBlueprintMask = toml::find_or<bool>(v, "enable_blueprint_mask", true);
            resource.growthConditions = toml::find_or<std::vector<glimmer::ResourceRef> >(v, "growth_conditions", {});
            resource.growthMaxTicks = toml::find_or<uint64_t>(v, "growth_max_ticks", 0);
            resource.growthMinTicks = toml::find_or<uint64_t>(v, "growth_min_ticks", 0);
            resource.growthTarget = toml::find_or<glimmer::ResourceRef>(v, "growth_target", {});
            resource.isOverwritable = toml::find_or<bool>(v, "is_overwritable", false);
            resource.layerType = toml::find_or<uint8_t>(v, "layer_type", 0);
            resource.lightSource = toml::find<glimmer::ResourceRef>(v, "light_source");
            resource.lootScaleBySize = toml::find_or<bool>(v, "loot_scale_by_size", false);
            resource.lootTable = toml::find_or<glimmer::ResourceRef>(v, "loot_table", {});
            resource.minMiningEfficiency = toml::find_or<float>(v, "min_mining_efficiency", 0);
            resource.name = toml::find<glimmer::ResourceRef>(v, "name");
            resource.physicsType = toml::find_or<uint8_t>(v, "physics_type", 0);
            resource.pipeline = toml::find_or<glimmer::ResourceRef>(v, "pipeline", {});
            resource.placeSfx = toml::find<glimmer::ResourceRef>(v, "place_sfx");
            resource.recipeGroup = toml::find_or<uint8_t>(v, "recipe_group", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.sampler = toml::find_or<glimmer::ResourceRef>(v, "sampler", {});
            resource.sideLightMask = toml::find<glimmer::ResourceRef>(v, "side_light_mask");
            resource.tags = toml::find_or<std::vector<glimmer::ItemTagResource> >(v, "tags", {});
            resource.technologyLevel = toml::find_or<uint8_t>(v, "technology_level", 0);
            resource.texture = toml::find<glimmer::ResourceRef>(v, "texture");
            resource.tileAnchorType = toml::find_or<uint8_t>(v, "tile_anchor_type", 6);
            resource.tileHeight = toml::find_or<uint8_t>(v, "tile_height", 1);
            resource.tileWidth = toml::find_or<uint8_t>(v, "tile_width", 1);
            resource.unitHardness = toml::find_or<float>(v, "unit_hardness", 1.0F);
            return resource;
        }
    };

    template<>
    struct from<glimmer::CircularShapeResource> {
        static glimmer::CircularShapeResource from_toml(const value &v) {
            glimmer::CircularShapeResource resource;
            resource.center = toml::find<glimmer::Vector2DResource>(v, "center");
            resource.radius = toml::find_or<float>(v, "radius", 1.0F);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.shapeType = toml::find_or<uint8_t>(v, "shape_type", 0);
            return resource;
        }
    };

    template<>
    struct from<glimmer::RayCastResource> {
        static glimmer::RayCastResource from_toml(const value &v) {
            glimmer::RayCastResource resource;
            resource.filter = toml::find<glimmer::Box2dFilter>(v, "filter");
            resource.origin = toml::find<glimmer::Vector2DResource>(v, "origin");
            resource.translation = toml::find<glimmer::Vector2DResource>(v, "translation");
            return resource;
        }
    };

    template<>
    struct from<glimmer::MobResource> {
        static glimmer::MobResource from_toml(const value &v) {
            glimmer::MobResource resource;
            resource.airControlFactor = toml::find_or<float>(v, "air_control_factor", 1.0F);
            resource.allowBodySleep = toml::find<bool>(v, "allow_body_sleep");
            resource.bodyType = toml::find<uint8_t>(v, "body_type");
            resource.box2dFilter = toml::find<glimmer::Box2dFilter>(v, "box2d_filter");
            resource.density = toml::find_or<float>(v, "density", 0.001F);
            resource.emptyHandAutoUseItem = toml::find<glimmer::ItemMessageResource>(v, "empty_hand_auto_use_item");
            resource.fixedRotation = toml::find<bool>(v, "fixed_rotation");
            resource.friction = toml::find_or<float>(v, "friction", 0.0F);
            resource.groundCheckRayCast = toml::find_or<std::vector<glimmer::RayCastResource> >(
                v, "ground_check_ray_cast", {});
            resource.isPlayer = toml::find_or<bool>(v, "is_player", false);
            resource.jumpForce = toml::find_or<float>(v, "jump_force", 7.5F);
            resource.maxSpeed = toml::find_or<float>(v, "max_speed", 18.0F);
            resource.movementAcceleration = toml::find_or<float>(v, "movement_acceleration", 6.0F);
            resource.pipeline = toml::find_or<glimmer::ResourceRef>(v, "pipeline", {});
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            resource.sampler = toml::find_or<glimmer::ResourceRef>(v, "sampler", {});
            resource.shape = toml::find<glimmer::ResourceRef>(v, "shape");
            resource.texture = toml::find_or<glimmer::ResourceRef>(v, "texture", {});
            resource.textureOffset = toml::find<glimmer::Vector2DResource>(v, "texture_offset");
            resource.tilePlacementForbiddenZone = toml::find_or<glimmer::TilePlacementForbiddenZone>(
                v, "tile_placement_forbidden_zone", {});
            return resource;
        }
    };

    template<>
    struct from<glimmer::RecipeResource> {
        static glimmer::RecipeResource from_toml(const value &v) {
            glimmer::RecipeResource resource;
            resource.duration = toml::find_or<float>(v, "duration", 0.0F);
            resource.input = toml::find<std::vector<glimmer::RequiredTag> >(v, "input");
            resource.minTechnologyLevel = toml::find_or<uint8_t>(v, "min_technology_level", 0);
            resource.output = toml::find<glimmer::ItemMessageResource>(v, "output");
            resource.recipeGroup = toml::find_or<uint8_t>(v, "recipe_group", 0);
            resource.resourceId = toml::find<std::string>(v, "resource_id");
            return resource;
        }
    };
}
