//
// Created by Cold-Mint on 2026/2/26.
//

#include "PreloadColors.h"

#include "core/mod/ResourceRef.h"
#include "core/mod/Resource.h"
#include "core/mod/ResourceLocator.h"
#include "core/utils/ColorUtils.h"

void glimmer::PreloadColors::LoadAllColors(const ResourceLocator *resourceLocator) {
    ColorResource text;
    text.r = 250;
    text.g = 250;
    text.b = 252;
    text.a = 255;
    textColor = LoadColor(resourceLocator, "text", &text)->ToSDLColor();
    ColorResource textDisabled;
    textDisabled.r = 100;
    textDisabled.g = 110;
    textDisabled.b = 120;
    textDisabled.a = 255;
    textDisabledColor = LoadColor(resourceLocator, "text_disabled", &textDisabled)->ToSDLColor();
    ColorResource textLink;
    textLink.r = 60;
    textLink.g = 190;
    textLink.b = 220;
    textLink.a = 255;
    textLinkColor = LoadColor(resourceLocator, "text_link", &textLink)->ToSDLColor();
    ColorResource textSelectedBg;
    textSelectedBg.r = 50;
    textSelectedBg.g = 80;
    textSelectedBg.b = 120;
    textSelectedBg.a = 255;
    textSelectedBgColor = LoadColor(resourceLocator, "text_selected_bg", &textSelectedBg)->ToSDLColor();
    ColorResource background;
    background.r = 25;
    background.g = 30;
    background.b = 40;
    background.a = 255;
    backgroundColor = LoadColor(resourceLocator, "background", &background)->ToSDLColor();
    ColorResource border;
    border.r = 45;
    border.g = 50;
    border.b = 65;
    border.a = 255;
    borderColor = LoadColor(resourceLocator, "border", &border)->ToSDLColor();
    ColorResource borderShadow;
    borderShadow.r = 10;
    borderShadow.g = 15;
    borderShadow.b = 20;
    borderShadow.a = 200;
    borderShadowColor = LoadColor(resourceLocator, "border_shadow", &borderShadow)->ToSDLColor();
    ColorResource separator;
    separator.r = 40;
    separator.g = 45;
    separator.b = 55;
    separator.a = 255;
    separatorColor = LoadColor(resourceLocator, "separator", &separator)->ToSDLColor();
    ColorResource separatorHovered;
    separatorHovered.r = 60;
    separatorHovered.g = 70;
    separatorHovered.b = 85;
    separatorHovered.a = 255;
    separatorHoveredColor = LoadColor(resourceLocator, "separator_hovered", &separatorHovered)->ToSDLColor();
    ColorResource separatorActive;
    separatorActive.r = 80;
    separatorActive.g = 90;
    separatorActive.b = 110;
    separatorActive.a = 255;
    separatorActiveColor = LoadColor(resourceLocator, "separator_active", &separatorActive)->ToSDLColor();
    ColorResource frameBg;
    frameBg.r = 35;
    frameBg.g = 40;
    frameBg.b = 55;
    frameBg.a = 255;
    frameBgColor = LoadColor(resourceLocator, "frame_bg", &frameBg)->ToSDLColor();
    ColorResource frameBgHovered;
    frameBgHovered.r = 45;
    frameBgHovered.g = 50;
    frameBgHovered.b = 65;
    frameBgHovered.a = 255;
    frameBgHoveredColor = LoadColor(resourceLocator, "frame_bg_hovered", &frameBgHovered)->ToSDLColor();
    ColorResource frameBgActive;
    frameBgActive.r = 55;
    frameBgActive.g = 60;
    frameBgActive.b = 75;
    frameBgActive.a = 255;
    frameBgActiveColor = LoadColor(resourceLocator, "frame_bg_active", &frameBgActive)->ToSDLColor();
    ColorResource button;
    button.r = 40;
    button.g = 45;
    button.b = 60;
    button.a = 255;
    buttonColor = LoadColor(resourceLocator, "button", &button)->ToSDLColor();
    ColorResource buttonHovered;
    buttonHovered.r = 55;
    buttonHovered.g = 60;
    buttonHovered.b = 75;
    buttonHovered.a = 255;
    buttonHoveredColor = LoadColor(resourceLocator, "button_hovered", &buttonHovered)->ToSDLColor();
    ColorResource buttonActive;
    buttonHovered.r = 30;
    buttonHovered.g = 35;
    buttonHovered.b = 50;
    buttonActive.a = 255;
    buttonActiveColor = LoadColor(resourceLocator, "button_active", &buttonActive)->ToSDLColor();
    ColorResource scrollbarBg;
    scrollbarBg.r = 30;
    scrollbarBg.g = 35;
    scrollbarBg.b = 50;
    scrollbarBg.a = 255;
    scrollbarBgColor = LoadColor(resourceLocator, "scrollbar_bg", &scrollbarBg)->ToSDLColor();
    ColorResource scrollbarGrab;
    scrollbarGrab.r = 50;
    scrollbarGrab.g = 55;
    scrollbarGrab.b = 70;
    scrollbarGrab.a = 255;
    scrollbarGrabColor = LoadColor(resourceLocator, "scrollbar_grab", &scrollbarGrab)->ToSDLColor();
    ColorResource scrollbarGrabHovered;
    scrollbarGrabHovered.r = 65;
    scrollbarGrabHovered.g = 70;
    scrollbarGrabHovered.b = 85;
    scrollbarGrabHovered.a = 255;
    scrollbarGrabHoveredColor = LoadColor(resourceLocator, "scrollbar_grab_hovered", &scrollbarGrabHovered)->
            ToSDLColor();
    ColorResource scrollbarGrabActive;
    scrollbarGrabActive.r = 40;
    scrollbarGrabActive.g = 45;
    scrollbarGrabActive.b = 60;
    scrollbarGrabActive.a = 255;
    scrollbarGrabActiveColor = LoadColor(resourceLocator, "scrollbar_grab_active", &scrollbarGrabActive)->ToSDLColor();
    ColorResource inputTextCursor;
    inputTextCursor.r = 70;
    inputTextCursor.g = 200;
    inputTextCursor.b = 230;
    inputTextCursor.a = 255;
    inputTextCursorColor = LoadColor(resourceLocator, "input_text_cursor", &inputTextCursor)->ToSDLColor();
    ColorResource navCursor;
    navCursor.r = 90;
    navCursor.g = 210;
    navCursor.b = 240;
    navCursor.a = 255;
    navCursorColor = LoadColor(resourceLocator, "nav_cursor", &navCursor)->ToSDLColor();
    ColorResource header;
    header.r = 30;
    header.g = 35;
    header.b = 50;
    header.a = 255;
    headerColor = LoadColor(resourceLocator, "header", &header)->ToSDLColor();
    ColorResource headerHovered;
    headerHovered.r = 45;
    headerHovered.g = 50;
    headerHovered.b = 65;
    headerHovered.a = 255;
    headerHoveredColor = LoadColor(resourceLocator, "header_hovered", &headerHovered)->ToSDLColor();
    ColorResource headerActive;
    headerActive.r = 55;
    headerActive.g = 60;
    headerActive.b = 75;
    headerActive.a = 255;
    headerActiveColor = LoadColor(resourceLocator, "header_active", &headerActive)->ToSDLColor();

    ColorResource areaMarkerBorder;
    areaMarkerBorder.r = 45;
    areaMarkerBorder.g = 65;
    areaMarkerBorder.b = 120;
    areaMarkerBorder.a = 255;
    areaMarkerBorderColor = LoadColor(resourceLocator, "area_marker_border", &areaMarkerBorder)->ToSDLColor();

    ColorResource areaMarker;
    areaMarker.r = 45;
    areaMarker.g = 65;
    areaMarker.b = 120;
    areaMarker.a = 180;
    areaMarkerColor = LoadColor(resourceLocator, "area_marker", &areaMarker)->ToSDLColor();

    ColorResource consoleBackgroundColor;
    consoleBackgroundColor.r = 20;
    consoleBackgroundColor.g = 25;
    consoleBackgroundColor.b = 35;
    consoleBackgroundColor.a = 255;
    console.backgroundColor = LoadColor(resourceLocator, "console/background", &consoleBackgroundColor)->ToSDLColor();
    ColorResource consoleTitleColor;
    consoleTitleColor.r = 80;
    consoleTitleColor.g = 200;
    consoleTitleColor.b = 230;
    consoleTitleColor.a = 255;
    console.titleColor = LoadColor(resourceLocator, "console/title", &consoleTitleColor)->ToSDLColor();
    ColorResource consoleKeywordColor;
    consoleKeywordColor.r = 100;
    consoleKeywordColor.g = 210;
    consoleKeywordColor.b = 240;
    consoleKeywordColor.a = 255;
    console.keywordColor = LoadColor(resourceLocator, "console/keyword", &consoleKeywordColor)->ToSDLColor();
    ColorResource consoleTextColor;
    consoleTextColor.r = 245;
    consoleTextColor.g = 245;
    consoleTextColor.b = 248;
    consoleTextColor.a = 255;
    console.textColor = LoadColor(resourceLocator, "console/text", &consoleTextColor)->ToSDLColor();
    ColorResource errorAccentColor;
    errorAccentColor.r = 160; // 低饱和紫，替代原亮紫
    errorAccentColor.g = 80;
    errorAccentColor.b = 220;
    errorAccentColor.a = 255;
    error.accentColor = LoadColor(resourceLocator, "errorTexture/accent", &errorAccentColor)->ToSDLColor();
    ColorResource errorBaseColor;
    errorBaseColor.r = 20;
    errorBaseColor.g = 20;
    errorBaseColor.b = 30;
    errorBaseColor.a = 255;
    error.baseColor = LoadColor(resourceLocator, "errorTexture/base", &errorBaseColor)->ToSDLColor();
    ColorResource focusTileBorderColor;
    focusTileBorderColor.r = 250;
    focusTileBorderColor.g = 250;
    focusTileBorderColor.b = 252;
    focusTileBorderColor.a = 255;
    game.focusTileBorderColor = LoadColor(resourceLocator, "game/focus_tile_border", &focusTileBorderColor)->
            ToSDLColor();
    ColorResource positiveAttrColor;
    positiveAttrColor.r = 46;
    positiveAttrColor.g = 184;
    positiveAttrColor.b = 91;
    positiveAttrColor.a = 255;
    game.positiveAttributeColor = LoadColor(resourceLocator, "game/positive_attribute", &positiveAttrColor)->
            ToSDLColor();
    ColorResource negativeAttrColor;
    negativeAttrColor.r = 190;
    negativeAttrColor.g = 45;
    negativeAttrColor.b = 45;
    negativeAttrColor.a = 255;
    game.negativeAttributeColor = LoadColor(resourceLocator, "game/negative_attribute", &negativeAttrColor)->
            ToSDLColor();
    ColorResource box2dBorderColorRes;
    box2dBorderColorRes.r = 0;
    box2dBorderColorRes.g = 0;
    box2dBorderColorRes.b = 255;
    box2dBorderColorRes.a = 255;
    debugColor.box2dBorderColor = LoadColor(resourceLocator, "debug/box2d_border_color", &box2dBorderColorRes)->
            ToSDLColor();
    ColorResource box2dFullColorRes;
    box2dFullColorRes.r = 100;
    box2dFullColorRes.g = 149;
    box2dFullColorRes.b = 237;
    box2dFullColorRes.a = 128;
    debugColor.box2dFullColor = LoadColor(resourceLocator, "debug/box2d_full_color", &box2dFullColorRes)->ToSDLColor();
    ColorResource draggableColorRes;
    draggableColorRes.r = 0;
    draggableColorRes.g = 0;
    draggableColorRes.b = 255;
    draggableColorRes.a = 255;
    debugColor.draggableColor = LoadColor(resourceLocator, "debug/draggable_color", &draggableColorRes)->ToSDLColor();
    ColorResource debugPanelTextColorRes;
    debugPanelTextColorRes.r = 255;
    debugPanelTextColorRes.g = 0;
    debugPanelTextColorRes.b = 0;
    debugPanelTextColorRes.a = 255;
    debugColor.debugPanelTextColor = LoadColor(resourceLocator, "debug/debug_panel_text_color", &debugPanelTextColorRes)
            ->
            ToSDLColor();
    ColorResource debugPanelTextBGColorRes;
    debugPanelTextBGColorRes.r = 30;
    debugPanelTextBGColorRes.g = 30;
    debugPanelTextBGColorRes.b = 30;
    debugPanelTextBGColorRes.a = 180;
    debugColor.debugPanelTextBGColor = LoadColor(resourceLocator, "debug/debug_panel_text_bg_color",
                                                 &debugPanelTextBGColorRes)->
            ToSDLColor();

    uint8_t debugMapAlpha = 191;
    ColorResource elevationMapFromRes;
    elevationMapFromRes.r = 0;
    elevationMapFromRes.g = 103;
    elevationMapFromRes.b = 153;
    elevationMapFromRes.a = debugMapAlpha;
    debugColor.elevationMapFrom = LoadColor(resourceLocator, "debug/elevation_map_from", &elevationMapFromRes)->
            ToSDLColor();

    ColorResource elevationMapToRes;
    elevationMapToRes.r = 255;
    elevationMapToRes.g = 255;
    elevationMapToRes.b = 255;
    elevationMapToRes.a = debugMapAlpha;
    debugColor.elevationMapTo = LoadColor(resourceLocator, "debug/elevation_map_to", &elevationMapToRes)->ToSDLColor();

    ColorResource tempMapFromRes;
    tempMapFromRes.r = 100;
    tempMapFromRes.g = 200;
    tempMapFromRes.b = 255;
    tempMapFromRes.a = debugMapAlpha;
    debugColor.tempMapFrom = LoadColor(resourceLocator, "debug/temp_map_from", &tempMapFromRes)->ToSDLColor();

    ColorResource tempMapToRes;
    tempMapToRes.r = 255;
    tempMapToRes.g = 50;
    tempMapToRes.b = 0;
    tempMapToRes.a = debugMapAlpha;
    debugColor.tempMapTo = LoadColor(resourceLocator, "debug/temp_map_to", &tempMapToRes)->ToSDLColor();
    ColorResource humidityMapFromRes;
    humidityMapFromRes.r = 250;
    humidityMapFromRes.g = 210;
    humidityMapFromRes.b = 120;
    humidityMapFromRes.a = debugMapAlpha;
    debugColor.humidityMapFrom = LoadColor(resourceLocator, "debug/humidity_map_from", &humidityMapFromRes)->
            ToSDLColor();

    ColorResource humidityMapToRes;
    humidityMapToRes.r = 0;
    humidityMapToRes.g = 150;
    humidityMapToRes.b = 255;
    humidityMapToRes.a = debugMapAlpha;
    debugColor.humidityMapTo = LoadColor(resourceLocator, "debug/humidity_map_to", &humidityMapToRes)->ToSDLColor();

    ColorResource erosionMapFromRes;
    erosionMapFromRes.r = 200;
    erosionMapFromRes.g = 150;
    erosionMapFromRes.b = 100;
    erosionMapFromRes.a = debugMapAlpha;
    debugColor.erosionMapFrom = LoadColor(resourceLocator, "debug/erosion_map_from", &erosionMapFromRes)->ToSDLColor();

    ColorResource erosionMapToRes;
    erosionMapToRes.r = 50;
    erosionMapToRes.g = 30;
    erosionMapToRes.b = 10;
    erosionMapToRes.a = debugMapAlpha;
    debugColor.erosionMapTo = LoadColor(resourceLocator, "debug/erosion_map_to", &erosionMapToRes)->ToSDLColor();

    ColorResource weirdnessMapFromRes;
    weirdnessMapFromRes.r = 180;
    weirdnessMapFromRes.g = 120;
    weirdnessMapFromRes.b = 255;
    weirdnessMapFromRes.a = debugMapAlpha;
    debugColor.weirdnessMapFrom = LoadColor(resourceLocator, "debug/weirdness_map_from", &weirdnessMapFromRes)->
            ToSDLColor();

    ColorResource weirdnessMapToRes;
    weirdnessMapToRes.r = 255;
    weirdnessMapToRes.g = 0;
    weirdnessMapToRes.b = 255;
    weirdnessMapToRes.a = debugMapAlpha;
    debugColor.weirdnessMapTo = LoadColor(resourceLocator, "debug/weirdness_map_to", &weirdnessMapToRes)->ToSDLColor();
    ColorResource defaultEmissionRes;
    defaultEmissionRes.r = 0;
    defaultEmissionRes.g = 0;
    defaultEmissionRes.b = 0;
    defaultEmissionRes.a = 0;
    light.defaultEmissionColor = LoadColor(resourceLocator, "light/default_emission_color", &defaultEmissionRes)->
            ToSDLColor();
    ColorResource defaultLightTransmissionRes;
    defaultLightTransmissionRes.r = 0;
    defaultLightTransmissionRes.g = 0;
    defaultLightTransmissionRes.b = 0;
    defaultLightTransmissionRes.a = 0;
    light.defaultLightTransmissionColor = LoadColor(resourceLocator, "light/default_light_transmission_color",
                                                    &defaultLightTransmissionRes)->
            ToSDLColor();
}

glimmer::ColorResource *glimmer::PreloadColors::LoadColor(const ResourceLocator *resourceLocator,
                                                          const std::string &key, ColorResource *defaultColor) {
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_TYPE_COLOR);
    resourceRef.SetResourceKey(key);
    ColorResource *result = resourceLocator->FindColorResource(resourceRef);
    if (result == nullptr) {
        result = defaultColor;
    }
    return result;
}
