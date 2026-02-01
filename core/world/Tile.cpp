//
// Created by Cold-Mint on 2025/11/2.
//

#include "Tile.h"

#include <optional>

#include "../scene/AppContext.h"
#include "../mod/ResourceLocator.h"


std::unique_ptr<glimmer::Tile> glimmer::Tile::FromResourceRef(AppContext *appContext,
                                                              const TileResource *tileResource) {
    auto tile = std::make_unique<Tile>();
    tile->id = Resource::GenerateId(*tileResource);
    const std::optional<StringResource *> nameStringResource = appContext->GetResourceLocator()->FindString(
        tileResource->name);
    if (nameStringResource.has_value()) {
        tile->name = nameStringResource.value()->value;
    } else {
        tile->name = tile->id;
    }
    const std::optional<StringResource *> descriptionStringResource = appContext->GetResourceLocator()->FindString(
        tileResource->description);
    if (descriptionStringResource.has_value()) {
        tile->description = descriptionStringResource.value()->value;
    } else {
        tile->description = tile->id;
    }
    tile->customLootTable = tileResource->customLootTable;
    tile->lootTable = tileResource->lootTable;
    tile->layerType = static_cast<TileLayerType>(tileResource->layerType);
    tile->physicsType = static_cast<TilePhysicsType>(tileResource->physicsType);
    tile->hardness = tileResource->hardness;
    tile->breakable = tileResource->breakable;
    tile->texture = appContext->GetResourcePackManager()->LoadTextureFromFile(
        appContext, tileResource->texture);
    return tile;
}
