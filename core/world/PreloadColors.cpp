//
// Created by coldmint on 2026/2/26.
//

#include "PreloadColors.h"

#include "core/mod/ResourceRef.h"
#include "core/mod/Resource.h"
#include "core/mod/ResourceLocator.h"

void glimmer::PreloadColors::LoadAllColors(const ResourceLocator *resourceLocator) {
    defaultTextColor = LoadColor(resourceLocator, "default_text")->ToSDLColor();
    console.keywordColor = LoadColor(resourceLocator, "console/keyword")->ToSDLColor();
    console.textColor = LoadColor(resourceLocator, "console/text")->ToSDLColor();
    console.backgroundColor = LoadColor(resourceLocator, "console/background")->ToSDLColor();
    console.titleColor = LoadColor(resourceLocator, "console/title")->ToSDLColor();
}

glimmer::ColorResource *glimmer::PreloadColors::LoadColor(const ResourceLocator *resourceLocator,
                                                          const std::string &key) {
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_TYPE_COLOR);
    resourceRef.SetResourceKey(key);
    return resourceLocator->FindColorResource(resourceRef);
}
