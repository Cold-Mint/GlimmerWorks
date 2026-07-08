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

#include "core/Box2dFilter.h"
#include "core/contributor/Contributor.h"
#include "core/lootTable/LootEntry.h"
#include "core/mod/PackManifest.h"
#include "core/mod/Resource.h"
#include "core/mod/dataPack/PackDependence.h"
#include "core/mod/resourcePack/ResourcePackConfig.h"

namespace toml
{
    template <>
    struct from<glimmer::ResourceRef>
    {
        static glimmer::ResourceRef from_toml(const value& v)
        {
            glimmer::ResourceRef r;
            r.SetPackageId(toml::find<std::string>(v, "pack_id"));
            r.SetResourceType(static_cast<ResourceTypeMessage>(toml::find<int>(v, "resource_type")));
            r.SetResourceKey(toml::find<std::string>(v, "resource_key"));
            return r;
        }
    };


    template <>
    struct from<glimmer::VariableDefinition>
    {
        static glimmer::VariableDefinition from_toml(const value& v)
        {
            glimmer::VariableDefinition r;
            r.key = toml::find<std::string>(v, "key");
            auto type = glimmer::VariableDefinition::ResolveVariableType(
                toml::find<std::string>(v, "type"));
            if (type == glimmer::VariableDefinitionType::INT)
            {
                r.value = std::to_string(toml::find<int>(v, "value"));
            }
            else if (type == glimmer::VariableDefinitionType::FLOAT)
            {
                r.value = std::to_string(toml::find<float>(v, "value"));
            }
            else if (type == glimmer::VariableDefinitionType::BOOL)
            {
                r.value = std::to_string(toml::find<bool>(v, "value"));
            }
            else if (type == glimmer::VariableDefinitionType::REF)
            {
                auto resourceRef = toml::find<glimmer::ResourceRef>(v, "value");
                ResourceRefMessage refMessage;
                resourceRef.WriteResourceRefMessage(refMessage);
                r.value = refMessage.SerializeAsString();
            }
            else
            {
                r.value = toml::find<std::string>(v, "value");
            }
            return r;
        }
    };


    template <>
    struct from<glimmer::AbilityConfig>
    {
        static glimmer::AbilityConfig from_toml(const value& v)
        {
            glimmer::AbilityConfig r;
            r.chainMiningRadius = toml::find_or<uint8_t>(v, "chain_mining_radius", 0);
            r.enablePrecisionMining = toml::find_or<bool>(v, "enable_precision_mining", false);
            r.mineAbleLayer = toml::find_or<uint8_t>(v, "mine_able_layer", 0);
            r.miningEfficiency = toml::find_or<float>(v, "mining_efficiency", 0);
            r.miningRange = toml::find_or<float>(v, "mining_range", 5);
            return r;
        }
    };

    template <>
    struct from<glimmer::Box2dFilter>
    {
        static glimmer::Box2dFilter from_toml(const value& v)
        {
            glimmer::Box2dFilter r;
            r.categoryBits = toml::find_or<uint64_t>(v, "category_bits", 0);
            r.maskBits = toml::find_or<uint64_t>(v, "mask_bits", 0);
            return r;
        }
    };

    template <>
    struct from<glimmer::ColorResource>
    {
        static glimmer::ColorResource from_toml(const value& v)
        {
            glimmer::ColorResource r;
            r.a = toml::find_or<uint8_t>(v, "a", 255);
            r.b = toml::find_or<uint8_t>(v, "b", 0);
            r.g = toml::find_or<uint8_t>(v, "g", 0);
            r.r = toml::find_or<uint8_t>(v, "r", 0);
            return r;
        }
    };

    template <>
    struct from<glimmer::DataPackManifest>
    {
        static glimmer::DataPackManifest from_toml(const value& v)
        {
            glimmer::DataPackManifest r;
            r.author = toml::find<std::string>(v, "author");
            r.description = toml::find<glimmer::ResourceRef>(v, "description");
            r.id = toml::find<std::string>(v, "id");
            r.minGameVersion = toml::find<uint32_t>(v, "min_game_version");
            r.name = toml::find<glimmer::ResourceRef>(v, "name");
            r.packDependencies = toml::find<std::vector<glimmer::PackDependence>>(v, "pack_dependencies");
            r.resPack = toml::find<bool>(v, "res_pack");
            r.templateSearchPath = toml::find_or<std::vector<std::string>>(
                v, "template_search_path", {TEMPLATE_CURRENT, TEMPLATE_ROOT});
            r.versionName = toml::find<std::string>(v, "version_name");
            r.versionNumber = toml::find<uint32_t>(v, "version_number");
            return r;
        }
    };

    template <>
    struct from<glimmer::FixedColorResource>
    {
        static glimmer::FixedColorResource from_toml(const value& v)
        {
            glimmer::FixedColorResource r;
            r.a = toml::find_or<uint8_t>(v, "a", 255);
            r.b = toml::find_or<uint8_t>(v, "b", 0);
            r.g = toml::find_or<uint8_t>(v, "g", 0);
            r.r = toml::find_or<uint8_t>(v, "r", 0);
            r.resourceId = toml::find<std::string>(v, "resource_id");
            return r;
        }
    };

    template <>
    struct from<glimmer::IBiomeDecoratorResource>
    {
        static glimmer::IBiomeDecoratorResource from_toml(const value& v)
        {
            glimmer::IBiomeDecoratorResource r;
            r.biomeDecoratorType = toml::find_or<uint8_t>(v, "biome_decorator_type", 0);
            r.layerType = toml::find_or<uint8_t>(v, "layer_type", 0);
            r.resourceId = toml::find<std::string>(v, "resource_id");
            return r;
        }
    };

    template <>
    struct from<glimmer::IShapeResource>
    {
        static glimmer::IShapeResource from_toml(const value& v)
        {
            glimmer::IShapeResource r;
            r.resourceId = toml::find<std::string>(v, "resource_id");
            r.shapeType = toml::find_or<uint8_t>(v, "shape_type", 0);
            return r;
        }
    };

    template <>
    struct from<glimmer::IStructureResource>
    {
        static glimmer::IStructureResource from_toml(const value& v)
        {
            glimmer::IStructureResource r;
            r.condition = toml::find_or<std::vector<glimmer::StructurePlacementConditions>>(v, "condition", {});
            r.data = toml::find_or<std::vector<glimmer::ResourceRef>>(v, "data", {});
            r.generatorId = toml::find_or<uint8_t>(v, "generator_id", 0);
            r.resourceId = toml::find<std::string>(v, "resource_id");
            return r;
        }
    };

    template <>
    struct from<glimmer::InitialInventoryResource>
    {
        static glimmer::InitialInventoryResource from_toml(const value& v)
        {
            glimmer::InitialInventoryResource r;
            r.addItems = toml::find<std::vector<glimmer::ItemMessageResource>>(v, "add_items");
            r.resourceId = toml::find<std::string>(v, "resource_id");
            return r;
        }
    };

    template <>
    struct from<glimmer::ItemTagResource>
    {
        static glimmer::ItemTagResource from_toml(const value& v)
        {
            glimmer::ItemTagResource r;
            r.name = toml::find<std::string>(v, "name");
            r.value = toml::find_or<uint8_t>(v, "value", 1);
            return r;
        }
    };

    template <>
    struct from<glimmer::LootResource>
    {
        static glimmer::LootResource from_toml(const value& v)
        {
            glimmer::LootResource r;
            r.empty_weight = toml::find_or<uint32_t>(v, "empty_weight", 0);
            r.mandatory = toml::find<std::vector<glimmer::LootEntry>>(v, "mandatory");
            r.pool = toml::find_or<std::vector<glimmer::LootEntry>>(v, "pool", {});
            r.resourceId = toml::find<std::string>(v, "resource_id");
            r.rolls = toml::find_or<uint32_t>(v, "rolls", 1);
            return r;
        }
    };

    template <>
    struct from<glimmer::NineSliceConfig>
    {
        static glimmer::NineSliceConfig from_toml(const value& v)
        {
            glimmer::NineSliceConfig r;
            r.bottomBorderPx = toml::find_or<float>(v, "bottom_border_px", 1.0F);
            r.enableTiled = toml::find_or<bool>(v, "enable_tiled", false);
            r.leftBorderPx = toml::find_or<float>(v, "left_border_px", 1.0F);
            r.rightBorderPx = toml::find_or<float>(v, "right_border_px", 1.0F);
            r.scale = toml::find_or<float>(v, "scale", 0.0F);
            r.tileScale = toml::find_or<float>(v, "tile_scale", 1.0F);
            r.topBorderPx = toml::find_or<float>(v, "top_border_px", 1.0F);
            return r;
        }
    };

    template <>
    struct from<glimmer::PackDependence>
    {
        static glimmer::PackDependence from_toml(const value& v)
        {
            glimmer::PackDependence r;
            r.minVersion = toml::find<uint32_t>(v, "min_version");
            r.packId = toml::find<std::string>(v, "pack_id");
            return r;
        }
    };

    template <>
    struct from<glimmer::RectangleShapeResource>
    {
        static glimmer::RectangleShapeResource from_toml(const value& v)
        {
            glimmer::RectangleShapeResource r;
            r.height = toml::find_or<float>(v, "height", 1.0F);
            r.resourceId = toml::find<std::string>(v, "resource_id");
            r.shapeType = toml::find_or<uint8_t>(v, "shape_type", 0);
            r.width = toml::find_or<float>(v, "width", 1.0F);
            return r;
        }
    };

    template <>
    struct from<glimmer::RequiredTag>
    {
        static glimmer::RequiredTag from_toml(const value& v)
        {
            glimmer::RequiredTag r;
            r.exactMatch = toml::find_or<bool>(v, "exact_match", true);
            r.requiredTag = toml::find<std::string>(v, "required_tag");
            r.requiredWeight = toml::find_or<uint16_t>(v, "required_weight", 1);
            return r;
        }
    };

    template <>
    struct from<glimmer::Resource>
    {
        static glimmer::Resource from_toml(const value& v)
        {
            glimmer::Resource r;
            r.resourceId = toml::find<std::string>(v, "resource_id");
            return r;
        }
    };

    template <>
    struct from<glimmer::ResourcePackManifest>
    {
        static glimmer::ResourcePackManifest from_toml(const value& v)
        {
            glimmer::ResourcePackManifest r;
            r.author = toml::find<std::string>(v, "author");
            r.description = toml::find<glimmer::ResourceRef>(v, "description");
            r.id = toml::find<std::string>(v, "id");
            r.minGameVersion = toml::find<uint32_t>(v, "min_game_version");
            r.name = toml::find<glimmer::ResourceRef>(v, "name");
            r.resPack = toml::find<bool>(v, "res_pack");
            r.templateSearchPath = toml::find_or<std::vector<std::string>>(
                v, "template_search_path", {TEMPLATE_CURRENT, TEMPLATE_ROOT});
            r.versionName = toml::find<std::string>(v, "version_name");
            r.versionNumber = toml::find<uint32_t>(v, "version_number");
            return r;
        }
    };

    template <>
    struct from<glimmer::RoundedRectangleShapeResource>
    {
        static glimmer::RoundedRectangleShapeResource from_toml(const value& v)
        {
            glimmer::RoundedRectangleShapeResource r;
            r.height = toml::find_or<float>(v, "height", 1.0F);
            r.radius = toml::find_or<float>(v, "radius", 0.0F);
            r.resourceId = toml::find<std::string>(v, "resource_id");
            r.shapeType = toml::find_or<uint8_t>(v, "shape_type", 0);
            r.width = toml::find_or<float>(v, "width", 1.0F);
            return r;
        }
    };

    template <>
    struct from<glimmer::StaticStructureResource>
    {
        static glimmer::StaticStructureResource from_toml(const value& v)
        {
            glimmer::StaticStructureResource r;
            r.condition = toml::find_or<std::vector<glimmer::StructurePlacementConditions>>(v, "condition", {});
            r.data = toml::find_or<std::vector<glimmer::ResourceRef>>(v, "data", {});
            r.generatorId = toml::find_or<uint8_t>(v, "generator_id", 0);
            r.resourceId = toml::find<std::string>(v, "resource_id");
            r.tileInfo = toml::find_or<std::vector<glimmer::TileInfo>>(v, "tile_info", {});
            return r;
        }
    };

    template <>
    struct from<glimmer::StringResource>
    {
        static glimmer::StringResource from_toml(const value& v)
        {
            glimmer::StringResource r;
            r.resourceId = toml::find<std::string>(v, "resource_id");
            r.value = toml::find<std::string>(v, "value");
            return r;
        }
    };

    template <>
    struct from<glimmer::TilePlacementForbiddenZone>
    {
        static glimmer::TilePlacementForbiddenZone from_toml(const value& v)
        {
            glimmer::TilePlacementForbiddenZone r;
            r.height = toml::find_or<int>(v, "height", 1);
            r.offsetX = toml::find_or<int>(v, "offset_x", 0);
            r.offsetY = toml::find_or<int>(v, "offset_y", 0);
            r.width = toml::find_or<int>(v, "width", 1);
            return r;
        }
    };

    template <>
    struct from<glimmer::TreeStructureResource>
    {
        static glimmer::TreeStructureResource from_toml(const value& v)
        {
            glimmer::TreeStructureResource r;
            r.condition = toml::find_or<std::vector<glimmer::StructurePlacementConditions>>(v, "condition", {});
            r.data = toml::find_or<std::vector<glimmer::ResourceRef>>(v, "data", {});
            r.generatorId = toml::find_or<uint8_t>(v, "generator_id", 0);
            r.hasLeaves = toml::find_or<bool>(v, "has_leaves", false);
            r.leafClusterCount = toml::find_or<uint8_t>(v, "leaf_cluster_count", 1);
            r.leafDataIndex = toml::find_or<uint8_t>(v, "leaf_data_index", 0);
            r.leafRadius = toml::find_or<uint8_t>(v, "leaf_radius", 2);
            r.leafTileLayer = toml::find_or<uint8_t>(v, "leaf_tile_layer", 0);
            r.leafVerticalSpacing = toml::find_or<uint8_t>(v, "leaf_vertical_spacing", 0);
            r.resourceId = toml::find<std::string>(v, "resource_id");
            r.trunkDataIndex = toml::find_or<uint8_t>(v, "trunk_data_index", 0);
            r.trunkHeightMax = toml::find_or<uint8_t>(v, "trunk_height_max", 9);
            r.trunkHeightMin = toml::find_or<uint8_t>(v, "trunk_height_min", 5);
            r.trunkTileLayer = toml::find_or<uint8_t>(v, "trunk_tile_layer", 0);
            r.trunkWidth = toml::find_or<uint8_t>(v, "trunk_width", 1);
            return r;
        }
    };

    template <>
    struct from<glimmer::VariableConfig>
    {
        static glimmer::VariableConfig from_toml(const value& v)
        {
            glimmer::VariableConfig r;
            r.definition = toml::find<std::vector<glimmer::VariableDefinition>>(v, "definition");
            return r;
        }
    };

    template <>
    struct from<glimmer::Vector2DIResource>
    {
        static glimmer::Vector2DIResource from_toml(const value& v)
        {
            glimmer::Vector2DIResource r;
            r.x = toml::find_or<int>(v, "x", 0);
            r.y = toml::find_or<int>(v, "y", 0);
            return r;
        }
    };

    template <>
    struct from<glimmer::Vector2DResource>
    {
        static glimmer::Vector2DResource from_toml(const value& v)
        {
            glimmer::Vector2DResource r;
            r.x = toml::find_or<float>(v, "x", 0.0F);
            r.y = toml::find_or<float>(v, "y", 0.0F);
            return r;
        }
    };

    template <>
    struct from<glimmer::ResourcePackConfig>
    {
        static glimmer::ResourcePackConfig from_toml(const value& v)
        {
            glimmer::ResourcePackConfig r;
            r.buttonNineSlice = toml::find_or<glimmer::NineSliceConfig>(v, "button_nine_slice", {});
            r.craftPreviewSlotNineSlice = toml::find_or<glimmer::NineSliceConfig>(
                v, "craft_preview_slot_nine_slice", {});
            r.itemSlotNineSlice = toml::find_or<glimmer::NineSliceConfig>(v, "item_slot_nine_slice", {});
            r.itemSlotQuantityNineSlice = toml::find_or<glimmer::NineSliceConfig>(
                v, "item_slot_quantity_nine_slice", {});
            r.itemToolTipNineSlice = toml::find_or<glimmer::NineSliceConfig>(v, "item_tool_tip_nine_slice", {});
            return r;
        }
    };

    template <>
    struct from<glimmer::AbilityItemResource>
    {
        static glimmer::AbilityItemResource from_toml(const value& v)
        {
            glimmer::AbilityItemResource r;
            r.ability = toml::find<std::string>(v, "ability");
            r.abilityConfig = toml::find_or<glimmer::AbilityConfig>(v, "ability_config", {});
            r.canUseAlone = toml::find_or<bool>(v, "can_use_alone", false);
            r.description = toml::find_or<glimmer::ResourceRef>(v, "description", {});
            r.maxDurability = toml::find_or<uint32_t>(v, "max_durability", 16);
            r.name = toml::find<glimmer::ResourceRef>(v, "name");
            r.resourceId = toml::find<std::string>(v, "resource_id");
            r.tags = toml::find_or<std::vector<glimmer::ItemTagResource>>(v, "tags", {});
            r.texture = toml::find<glimmer::ResourceRef>(v, "texture");
            r.unbreakable = toml::find_or<bool>(v, "unbreakable", false);
            return r;
        }
    };

    template <>
    struct from<glimmer::BiomeResource>
    {
        static glimmer::BiomeResource from_toml(const value& v)
        {
            glimmer::BiomeResource r;
            r.bgm = toml::find<glimmer::ResourceRef>(v, "bgm");
            r.decors = toml::find<std::vector<glimmer::ResourceRef>>(v, "decors");
            r.elevation = toml::find_or<float>(v, "elevation", 0.5F);
            r.erosion = toml::find_or<float>(v, "erosion", 0.5F);
            r.humidity = toml::find_or<float>(v, "humidity", 0.5F);
            r.parallaxBackground = toml::find<glimmer::ResourceRef>(v, "parallax_background");
            r.resourceId = toml::find<std::string>(v, "resource_id");
            r.strictnessElevation = toml::find_or<float>(v, "strictness_elevation", 1.0F);
            r.strictnessErosion = toml::find_or<float>(v, "strictness_erosion", 1.0F);
            r.strictnessHumidity = toml::find_or<float>(v, "strictness_humidity", 1.0F);
            r.strictnessSurfaceProximity = toml::find_or<float>(v, "strictness_surface_proximity", 1.0F);
            r.strictnessTemperature = toml::find_or<float>(v, "strictness_temperature", 1.0F);
            r.strictnessWeirdness = toml::find_or<float>(v, "strictness_weirdness", 1.0F);
            r.surfaceProximity = toml::find_or<float>(v, "surface_proximity", 0.5F);
            r.temperature = toml::find_or<float>(v, "temperature", 0.5F);
            r.weirdness = toml::find_or<float>(v, "weirdness", 0.5F);
            return r;
        }
    };

    template <>
    struct from<glimmer::ComposableItemResource>
    {
        static glimmer::ComposableItemResource from_toml(const value& v)
        {
            glimmer::ComposableItemResource r;
            r.defaultAbilityList = toml::find_or<std::vector<glimmer::ItemMessageResource>>(
                v, "default_ability_list", {});
            r.description = toml::find_or<glimmer::ResourceRef>(v, "description", {});
            r.maxDurability = toml::find_or<uint32_t>(v, "max_durability", 16);
            r.name = toml::find<glimmer::ResourceRef>(v, "name");
            r.resourceId = toml::find<std::string>(v, "resource_id");
            r.slotSize = toml::find<size_t>(v, "slot_size");
            r.tags = toml::find_or<std::vector<glimmer::ItemTagResource>>(v, "tags", {});
            r.texture = toml::find<glimmer::ResourceRef>(v, "texture");
            r.unbreakable = toml::find_or<bool>(v, "unbreakable", false);
            return r;
        }
    };

    template <>
    struct from<glimmer::Contributor>
    {
        static glimmer::Contributor from_toml(const value& v)
        {
            glimmer::Contributor r;
            r.country = toml::find<std::string>(v, "country");
            r.displayName = toml::find<glimmer::ResourceRef>(v, "display_name");
            r.name = toml::find<std::string>(v, "name");
            r.uuid = toml::find<std::string>(v, "uuid");
            return r;
        }
    };

    template <>
    struct from<glimmer::FillBiomeDecoratorResource>
    {
        static glimmer::FillBiomeDecoratorResource from_toml(const value& v)
        {
            glimmer::FillBiomeDecoratorResource r;
            r.biomeDecoratorType = toml::find_or<uint8_t>(v, "biome_decorator_type", 0);
            r.layerType = toml::find_or<uint8_t>(v, "layer_type", 0);
            r.resourceId = toml::find<std::string>(v, "resource_id");
            r.tile = toml::find<glimmer::ResourceRef>(v, "tile");
            return r;
        }
    };

    template <>
    struct from<glimmer::ItemMessageResource>
    {
        static glimmer::ItemMessageResource from_toml(const value& v)
        {
            glimmer::ItemMessageResource r;
            r.abilityItemRef = toml::find_or<std::vector<glimmer::ItemMessageResource>>(v, "ability_item_ref", {});
            r.amount = toml::find_or<uint64_t>(v, "amount", 1);
            r.item = toml::find<glimmer::ResourceRef>(v, "item");
            r.locked = toml::find_or<bool>(v, "locked", false);
            return r;
        }
    };

    template <>
    struct from<glimmer::LightMaskResource>
    {
        static glimmer::LightMaskResource from_toml(const value& v)
        {
            glimmer::LightMaskResource r;
            r.lightMaskColor = toml::find<glimmer::ResourceRef>(v, "light_mask_color");
            r.resourceId = toml::find<std::string>(v, "resource_id");
            r.tintFactor = toml::find_or<float>(v, "tint_factor", 0.0F);
            return r;
        }
    };

    template <>
    struct from<glimmer::LightSourceResource>
    {
        static glimmer::LightSourceResource from_toml(const value& v)
        {
            glimmer::LightSourceResource r;
            r.lightBrightestAtCenter = toml::find_or<bool>(v, "light_brightest_at_center", true);
            r.lightColor = toml::find_or<glimmer::ResourceRef>(v, "light_color", {});
            r.lightRadius = toml::find_or<uint8_t>(v, "light_radius", 0);
            r.resourceId = toml::find<std::string>(v, "resource_id");
            return r;
        }
    };

    template <>
    struct from<glimmer::LootEntry>
    {
        static glimmer::LootEntry from_toml(const value& v)
        {
            glimmer::LootEntry r;
            r.item = toml::find<glimmer::ResourceRef>(v, "item");
            r.mandatory = toml::find_or<bool>(v, "mandatory", false);
            r.max = toml::find_or<uint32_t>(v, "max", 1);
            r.min = toml::find_or<uint32_t>(v, "min", 1);
            r.weight = toml::find_or<uint32_t>(v, "weight", 30);
            return r;
        }
    };

    template <>
    struct from<glimmer::MaterialItemResource>
    {
        static glimmer::MaterialItemResource from_toml(const value& v)
        {
            glimmer::MaterialItemResource r;
            r.description = toml::find_or<glimmer::ResourceRef>(v, "description", {});
            r.name = toml::find<glimmer::ResourceRef>(v, "name");
            r.resourceId = toml::find<std::string>(v, "resource_id");
            r.tags = toml::find_or<std::vector<glimmer::ItemTagResource>>(v, "tags", {});
            r.texture = toml::find<glimmer::ResourceRef>(v, "texture");
            return r;
        }
    };

    template <>
    struct from<glimmer::MineralBiomeDecoratorResource>
    {
        static glimmer::MineralBiomeDecoratorResource from_toml(const value& v)
        {
            glimmer::MineralBiomeDecoratorResource r;
            r.biomeDecoratorType = toml::find_or<uint8_t>(v, "biome_decorator_type", 0);
            r.frequency = toml::find_or<float>(v, "frequency", 0.01F);
            r.invertOreSpawnByDepth = toml::find_or<bool>(v, "invert_ore_spawn_by_depth", true);
            r.layerType = toml::find_or<uint8_t>(v, "layer_type", 0);
            r.maxSpawnElevation = toml::find_or<float>(v, "max_spawn_elevation", 0.5F);
            r.minSpawnElevation = toml::find_or<float>(v, "min_spawn_elevation", 0.0F);
            r.noiseType = toml::find<uint8_t>(v, "noise_type");
            r.ore = toml::find<glimmer::ResourceRef>(v, "ore");
            r.oreSpawnMaxNoiseThreshold = toml::find_or<float>(v, "ore_spawn_max_noise_threshold", 0.8F);
            r.oreSpawnMinNoiseThreshold = toml::find_or<float>(v, "ore_spawn_min_noise_threshold", 0.5F);
            r.resourceId = toml::find<std::string>(v, "resource_id");
            r.seedOffset = toml::find_or<int>(v, "seed_offset", 1024);
            return r;
        }
    };

    template <>
    struct from<glimmer::PackManifest>
    {
        static glimmer::PackManifest from_toml(const value& v)
        {
            glimmer::PackManifest r;
            r.author = toml::find<std::string>(v, "author");
            r.description = toml::find<glimmer::ResourceRef>(v, "description");
            r.id = toml::find<std::string>(v, "id");
            r.minGameVersion = toml::find<uint32_t>(v, "min_game_version");
            r.name = toml::find<glimmer::ResourceRef>(v, "name");
            r.resPack = toml::find<bool>(v, "res_pack");
            r.templateSearchPath = toml::find_or<std::vector<std::string>>(
                v, "template_search_path", {TEMPLATE_CURRENT, TEMPLATE_ROOT});
            r.versionName = toml::find<std::string>(v, "version_name");
            r.versionNumber = toml::find<uint32_t>(v, "version_number");
            return r;
        }
    };

    template <>
    struct from<glimmer::SurfaceBiomeDecoratorResource>
    {
        static glimmer::SurfaceBiomeDecoratorResource from_toml(const value& v)
        {
            glimmer::SurfaceBiomeDecoratorResource r;
            r.allowAir = toml::find_or<bool>(v, "allow_air", true);
            r.allowWater = toml::find_or<bool>(v, "allow_water", false);
            r.biomeDecoratorType = toml::find_or<uint8_t>(v, "biome_decorator_type", 0);
            r.layerType = toml::find_or<uint8_t>(v, "layer_type", 0);
            r.resourceId = toml::find<std::string>(v, "resource_id");
            r.tile = toml::find<glimmer::ResourceRef>(v, "tile");
            return r;
        }
    };

    template <>
    struct from<glimmer::StructurePlacementConditions>
    {
        static glimmer::StructurePlacementConditions from_toml(const value& v)
        {
            glimmer::StructurePlacementConditions r;
            r.config = toml::find_or<glimmer::VariableConfig>(v, "config", {});
            r.processorId = toml::find<std::string>(v, "processor_id");
            return r;
        }
    };

    template <>
    struct from<glimmer::TileInfo>
    {
        static glimmer::TileInfo from_toml(const value& v)
        {
            glimmer::TileInfo r;
            r.layerType = toml::find_or<uint8_t>(v, "layer_type", 0);
            r.position = toml::find<glimmer::Vector2DIResource>(v, "position");
            r.tile = toml::find<glimmer::ResourceRef>(v, "tile");
            return r;
        }
    };

    template <>
    struct from<glimmer::TileResource>
    {
        static glimmer::TileResource from_toml(const value& v)
        {
            glimmer::TileResource r;
            r.allowChainMining = toml::find_or<bool>(v, "allow_chain_mining", false);
            r.allowDirAdjustAnchor = toml::find_or<bool>(v, "allow_dir_adjust_anchor", true);
            r.autoDigCostScale = toml::find_or<bool>(v, "auto_dig_cost_scale", true);
            r.autoHardnessScale = toml::find_or<bool>(v, "auto_hardness_scale", true);
            r.backLightMask = toml::find<glimmer::ResourceRef>(v, "back_light_mask");
            r.blueprintTexture = toml::find_or<glimmer::ResourceRef>(v, "blueprint_texture", {});
            r.breakSfx = toml::find<glimmer::ResourceRef>(v, "break_sfx");
            r.canDropLoot = toml::find_or<bool>(v, "can_drop_loot", true);
            r.customLootTable = toml::find_or<bool>(v, "custom_loot_table", false);
            r.customTileAnchor = toml::find_or<glimmer::Vector2DIResource>(v, "custom_tile_anchor", {1, 1});
            r.description = toml::find_or<glimmer::ResourceRef>(v, "description", {});
            r.drawValidBlueprintColor = toml::find_or<bool>(v, "draw_valid_blueprint_color", false);
            r.enableBlueprint = toml::find_or<bool>(v, "enable_blueprint", true);
            r.enableBlueprintMask = toml::find_or<bool>(v, "enable_blueprint_mask", true);
            r.isOverwritable = toml::find_or<bool>(v, "is_overwritable", false);
            r.layerType = toml::find_or<uint8_t>(v, "layer_type", 0);
            r.lightSource = toml::find<glimmer::ResourceRef>(v, "light_source");
            r.lootScaleBySize = toml::find_or<bool>(v, "loot_scale_by_size", false);
            r.lootTable = toml::find_or<glimmer::ResourceRef>(v, "loot_table", {});
            r.minMiningEfficiency = toml::find_or<float>(v, "min_mining_efficiency", 0);
            r.name = toml::find<glimmer::ResourceRef>(v, "name");
            r.physicsType = toml::find_or<uint8_t>(v, "physics_type", 0);
            r.placeSfx = toml::find<glimmer::ResourceRef>(v, "place_sfx");
            r.recipeGroup = toml::find_or<uint8_t>(v, "recipe_group", 0);
            r.resourceId = toml::find<std::string>(v, "resource_id");
            r.sideLightMask = toml::find<glimmer::ResourceRef>(v, "side_light_mask");
            r.tags = toml::find_or<std::vector<glimmer::ItemTagResource>>(v, "tags", {});
            r.technologyLevel = toml::find_or<uint8_t>(v, "technology_level", 0);
            r.texture = toml::find<glimmer::ResourceRef>(v, "texture");
            r.tileAnchorType = toml::find_or<uint8_t>(v, "tile_anchor_type", 6);
            r.tileHeight = toml::find_or<uint8_t>(v, "tile_height", 1);
            r.tileWidth = toml::find_or<uint8_t>(v, "tile_width", 1);
            r.unitHardness = toml::find_or<float>(v, "unit_hardness", 1.0F);
            return r;
        }
    };

    template <>
    struct from<glimmer::CircularShapeResource>
    {
        static glimmer::CircularShapeResource from_toml(const value& v)
        {
            glimmer::CircularShapeResource r;
            r.center = toml::find<glimmer::Vector2DResource>(v, "center");
            r.radius = toml::find_or<float>(v, "radius", 1.0F);
            r.resourceId = toml::find<std::string>(v, "resource_id");
            r.shapeType = toml::find_or<uint8_t>(v, "shape_type", 0);
            return r;
        }
    };

    template <>
    struct from<glimmer::RayCastResource>
    {
        static glimmer::RayCastResource from_toml(const value& v)
        {
            glimmer::RayCastResource r;
            r.filter = toml::find<glimmer::Box2dFilter>(v, "filter");
            r.origin = toml::find<glimmer::Vector2DResource>(v, "origin");
            r.translation = toml::find<glimmer::Vector2DResource>(v, "translation");
            return r;
        }
    };

    template <>
    struct from<glimmer::MobResource>
    {
        static glimmer::MobResource from_toml(const value& v)
        {
            glimmer::MobResource r;
            r.airControlFactor = toml::find_or<float>(v, "air_control_factor", 1.0F);
            r.allowBodySleep = toml::find<bool>(v, "allow_body_sleep");
            r.bodyType = toml::find<uint8_t>(v, "body_type");
            r.box2dFilter = toml::find<glimmer::Box2dFilter>(v, "box2d_filter");
            r.density = toml::find_or<float>(v, "density", 0.001F);
            r.emptyHandAutoUseItem = toml::find<glimmer::ItemMessageResource>(v, "empty_hand_auto_use_item");
            r.fixedRotation = toml::find<bool>(v, "fixed_rotation");
            r.friction = toml::find_or<float>(v, "friction", 0.0F);
            r.groundCheckRayCast = toml::find_or<std::vector<glimmer::RayCastResource>>(v, "ground_check_ray_cast", {});
            r.isPlayer = toml::find_or<bool>(v, "is_player", false);
            r.jumpForce = toml::find_or<float>(v, "jump_force", 7.5F);
            r.maxSpeed = toml::find_or<float>(v, "max_speed", 18.0F);
            r.movementAcceleration = toml::find_or<float>(v, "movement_acceleration", 6.0F);
            r.resourceId = toml::find<std::string>(v, "resource_id");
            r.shape = toml::find<glimmer::ResourceRef>(v, "shape");
            r.texture = toml::find_or<glimmer::ResourceRef>(v, "texture", {});
            r.textureOffset = toml::find<glimmer::Vector2DResource>(v, "texture_offset");
            r.tilePlacementForbiddenZone = toml::find_or<glimmer::TilePlacementForbiddenZone>(
                v, "tile_placement_forbidden_zone", {});
            return r;
        }
    };

    template <>
    struct from<glimmer::RecipeResource>
    {
        static glimmer::RecipeResource from_toml(const value& v)
        {
            glimmer::RecipeResource r;
            r.duration = toml::find_or<float>(v, "duration", 0.0F);
            r.input = toml::find<std::vector<glimmer::RequiredTag>>(v, "input");
            r.minTechnologyLevel = toml::find_or<uint8_t>(v, "min_technology_level", 0);
            r.output = toml::find<glimmer::ItemMessageResource>(v, "output");
            r.recipeGroup = toml::find_or<uint8_t>(v, "recipe_group", 0);
            r.resourceId = toml::find<std::string>(v, "resource_id");
            return r;
        }
    };
}
