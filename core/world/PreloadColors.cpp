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
#include "PreloadColors.h"

#include "core/log/LogCat.h"
#include "core/mod/ResourceRef.h"
#include "core/mod/Resource.h"
#include "core/mod/ResourceLocator.h"

void glimmer::PreloadColors::LoadAllColors(const ResourceLocator *resourceLocator) {
    LogCat::i("preload_colors_start", "Preloading colors");
    Color areaMarkerBorder;
    areaMarkerBorder.r = 45;
    areaMarkerBorder.g = 65;
    areaMarkerBorder.b = 120;
    areaMarkerBorder.a = 255;
    areaMarkerBorderColor = LoadColor(resourceLocator, "area_marker_border", areaMarkerBorder);

    Color areaMarker;
    areaMarker.r = 45;
    areaMarker.g = 65;
    areaMarker.b = 120;
    areaMarker.a = 180;
    areaMarkerColor = LoadColor(resourceLocator, "area_marker", areaMarker);
    Color errorAccentColor;
    errorAccentColor.r = 160;
    errorAccentColor.g = 80;
    errorAccentColor.b = 220;
    errorAccentColor.a = 255;
    error.accentColor = LoadColor(resourceLocator, "errorTexture/accent", errorAccentColor);
    Color baseColor;
    baseColor.r = 20;
    baseColor.g = 20;
    baseColor.b = 30;
    baseColor.a = 255;
    error.baseColor = LoadColor(resourceLocator, "errorTexture/base", baseColor);
    Color accessDeniedAccentColor;
    accessDeniedAccentColor.r = 190;
    accessDeniedAccentColor.g = 30;
    accessDeniedAccentColor.b = 30;
    accessDeniedAccentColor.a = 255;
    accessDenied.accentColor = LoadColor(resourceLocator, "accessDeniedTexture/accent", accessDeniedAccentColor);
    accessDenied.baseColor = LoadColor(resourceLocator, "accessDeniedTexture/base", baseColor);
    Color focusTileBorderColor;
    focusTileBorderColor.r = 250;
    focusTileBorderColor.g = 250;
    focusTileBorderColor.b = 252;
    focusTileBorderColor.a = 255;
    game.focusTileBorderColor = LoadColor(resourceLocator, "game/focus_tile_border", focusTileBorderColor);
    Color positiveAttrColor;
    positiveAttrColor.r = 46;
    positiveAttrColor.g = 184;
    positiveAttrColor.b = 91;
    positiveAttrColor.a = 255;
    game.positiveAttributeColor = LoadColor(resourceLocator, "game/positive_attribute", positiveAttrColor);
    Color negativeAttrColor;
    negativeAttrColor.r = 190;
    negativeAttrColor.g = 45;
    negativeAttrColor.b = 45;
    negativeAttrColor.a = 255;
    game.negativeAttributeColor = LoadColor(resourceLocator, "game/negative_attribute", negativeAttrColor);
    Color itemSlotTextColor;
    itemSlotTextColor.r = 255;
    itemSlotTextColor.g = 255;
    itemSlotTextColor.b = 255;
    itemSlotTextColor.a = 255;
    game.itemSlotTextColor = LoadColor(resourceLocator, "game/item_slot_text_color", itemSlotTextColor);
    Color box2dBorderColor;
    box2dBorderColor.r = 0;
    box2dBorderColor.g = 0;
    box2dBorderColor.b = 255;
    box2dBorderColor.a = 255;
    debugColor.box2dBorderColor = LoadColor(resourceLocator, "debug/box2d_border_color", box2dBorderColor);
    Color box2dFullColor;
    box2dFullColor.r = 100;
    box2dFullColor.g = 149;
    box2dFullColor.b = 237;
    box2dFullColor.a = 128;
    debugColor.box2dFullColor = LoadColor(resourceLocator, "debug/box2d_full_color", box2dFullColor);
    Color draggableColor;
    draggableColor.r = 0;
    draggableColor.g = 0;
    draggableColor.b = 255;
    draggableColor.a = 255;
    debugColor.draggableColor = LoadColor(resourceLocator, "debug/draggable_color", draggableColor);
    Color debugPanelTextColor;
    debugPanelTextColor.r = 255;
    debugPanelTextColor.g = 0;
    debugPanelTextColor.b = 0;
    debugPanelTextColor.a = 255;
    debugColor.debugPanelTextColor = LoadColor(resourceLocator, "debug/debug_panel_text_color", debugPanelTextColor);
    Color debugPanelTextBGColor;
    debugPanelTextBGColor.r = 30;
    debugPanelTextBGColor.g = 30;
    debugPanelTextBGColor.b = 30;
    debugPanelTextBGColor.a = 180;
    debugColor.debugPanelTextBGColor = LoadColor(resourceLocator, "debug/debug_panel_text_bg_color",
                                                 debugPanelTextBGColor);

    uint8_t debugMapAlpha = 191;
    Color elevationMapFrom;
    elevationMapFrom.r = 0;
    elevationMapFrom.g = 103;
    elevationMapFrom.b = 153;
    elevationMapFrom.a = debugMapAlpha;
    debugColor.elevationMapFrom = LoadColor(resourceLocator, "debug/elevation_map_from", elevationMapFrom);

    Color elevationMapTo;
    elevationMapTo.r = 255;
    elevationMapTo.g = 255;
    elevationMapTo.b = 255;
    elevationMapTo.a = debugMapAlpha;
    debugColor.elevationMapTo = LoadColor(resourceLocator, "debug/elevation_map_to", elevationMapTo);

    Color tempMapFrom;
    tempMapFrom.r = 100;
    tempMapFrom.g = 200;
    tempMapFrom.b = 255;
    tempMapFrom.a = debugMapAlpha;
    debugColor.tempMapFrom = LoadColor(resourceLocator, "debug/temp_map_from", tempMapFrom);

    Color tempMapTo;
    tempMapTo.r = 255;
    tempMapTo.g = 50;
    tempMapTo.b = 0;
    tempMapTo.a = debugMapAlpha;
    debugColor.tempMapTo = LoadColor(resourceLocator, "debug/temp_map_to", tempMapTo);
    Color humidityMapFrom;
    humidityMapFrom.r = 250;
    humidityMapFrom.g = 210;
    humidityMapFrom.b = 120;
    humidityMapFrom.a = debugMapAlpha;
    debugColor.humidityMapFrom = LoadColor(resourceLocator, "debug/humidity_map_from", humidityMapFrom);

    Color humidityMapTo;
    humidityMapTo.r = 0;
    humidityMapTo.g = 150;
    humidityMapTo.b = 255;
    humidityMapTo.a = debugMapAlpha;
    debugColor.humidityMapTo = LoadColor(resourceLocator, "debug/humidity_map_to", humidityMapTo);

    Color erosionMapFrom;
    erosionMapFrom.r = 200;
    erosionMapFrom.g = 150;
    erosionMapFrom.b = 100;
    erosionMapFrom.a = debugMapAlpha;
    debugColor.erosionMapFrom = LoadColor(resourceLocator, "debug/erosion_map_from", erosionMapFrom);

    Color erosionMapTo;
    erosionMapTo.r = 50;
    erosionMapTo.g = 30;
    erosionMapTo.b = 10;
    erosionMapTo.a = debugMapAlpha;
    debugColor.erosionMapTo = LoadColor(resourceLocator, "debug/erosion_map_to", erosionMapTo);

    Color weirdnessMapFrom;
    weirdnessMapFrom.r = 180;
    weirdnessMapFrom.g = 120;
    weirdnessMapFrom.b = 255;
    weirdnessMapFrom.a = debugMapAlpha;
    debugColor.weirdnessMapFrom = LoadColor(resourceLocator, "debug/weirdness_map_from", weirdnessMapFrom);

    Color weirdnessMapTo;
    weirdnessMapTo.r = 255;
    weirdnessMapTo.g = 0;
    weirdnessMapTo.b = 255;
    weirdnessMapTo.a = debugMapAlpha;
    debugColor.weirdnessMapTo = LoadColor(resourceLocator, "debug/weirdness_map_to", weirdnessMapTo);
    Color defaultEmission;
    defaultEmission.r = 0;
    defaultEmission.g = 0;
    defaultEmission.b = 0;
    defaultEmission.a = 0;
    light.defaultEmissionColor = LoadColor(resourceLocator, "light/default_emission_color", defaultEmission);
    Color defaultLightTransmission;
    defaultLightTransmission.r = 0;
    defaultLightTransmission.g = 0;
    defaultLightTransmission.b = 0;
    defaultLightTransmission.a = 0;
    light.defaultLightTransmissionColor = LoadColor(resourceLocator, "light/default_light_transmission_color",
                                                    defaultLightTransmission);
    Color defaultBlueprintValid;
    defaultBlueprintValid.r = 65;
    defaultBlueprintValid.g = 175;
    defaultBlueprintValid.b = 255;
    defaultBlueprintValid.a = 160;
    blueprint.validColor = LoadColor(resourceLocator, "blueprint/valid_place_color",
                                     defaultBlueprintValid);
    Color defaultBlueprintInvalid;
    defaultBlueprintInvalid.r = 230;
    defaultBlueprintInvalid.g = 60;
    defaultBlueprintInvalid.b = 60;
    defaultBlueprintInvalid.a = 160;
    blueprint.invalidColor = LoadColor(resourceLocator, "blueprint/invalid_place_color",
                                       defaultBlueprintInvalid);

    Color defaultDurabilityGood;
    defaultDurabilityGood.r = 60;
    defaultDurabilityGood.g = 230;
    defaultDurabilityGood.b = 60;
    defaultDurabilityGood.a = 96;
    durability.durabilityGood = LoadColor(resourceLocator, "durability/good", defaultDurabilityGood);

    Color defaultDurabilityNotice;
    defaultDurabilityNotice.r = 230;
    defaultDurabilityNotice.g = 200;
    defaultDurabilityNotice.b = 60;
    defaultDurabilityNotice.a = 96;
    durability.durabilityNotice = LoadColor(resourceLocator, "durability/notice", defaultDurabilityNotice);

    Color defaultDurabilityWarning;
    defaultDurabilityWarning.r = 230;
    defaultDurabilityWarning.g = 140;
    defaultDurabilityWarning.b = 60;
    defaultDurabilityWarning.a = 96;
    durability.durabilityWarning = LoadColor(resourceLocator, "durability/warning", defaultDurabilityWarning);

    Color defaultDurabilityDanger;
    defaultDurabilityDanger.r = 230;
    defaultDurabilityDanger.g = 60;
    defaultDurabilityDanger.b = 60;
    defaultDurabilityDanger.a = 96;
    durability.durabilityDanger = LoadColor(resourceLocator, "durability/danger", defaultDurabilityDanger);
    LogCat::i("preload_colors_completed", "Preloading colors completed");
}

glimmer::Color glimmer::PreloadColors::LoadColor(const ResourceLocator *resourceLocator, const std::string &key,
                                                 const Color &defaultColor) {
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_COLOR);
    resourceRef.SetResourceKey(key);
    const std::unique_ptr<Color> targetColor = resourceLocator->FindColor(&resourceRef);
    if (targetColor == nullptr) {
        LogCat::w(std::source_location::current(), "preload_color_not_found",
                  "Color not found, using default: key={}", key);
        return defaultColor;
    }
    return Color{targetColor->r, targetColor->g, targetColor->b, targetColor->a};
}
