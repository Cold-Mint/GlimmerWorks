//
// Created by Cold-Mint on 2025/11/2.
//

#include "Tile.h"

#include <optional>

#include "../log/LogCat.h"
#include "../scene/AppContext.h"
#include "../mod/ResourceLocator.h"
#include "../Config.h"


glimmer::Tile glimmer::Tile::FromResourceRef(const AppContext *appContext, const TileResource *tileResource) {
    Tile tile;
    tile.id = tileResource->packId + ":" + tileResource->key;
    const std::optional<StringResource *> nameStringResource = appContext->GetResourceLocator()->FindString(
        tileResource->name);
    if (nameStringResource.has_value()) {
        tile.name = nameStringResource.value()->value;
    } else {
        tile.name = tile.id;
    }
    const std::optional<StringResource *> stringResource = appContext->GetResourceLocator()->FindString(
        tileResource->name);
    if (stringResource.has_value()) {
        tile.name = stringResource.value()->value;
    } else {
        tile.name = tile.id;
    }
    tile.layerType = static_cast<TileLayerType>(tileResource->layerType);
    tile.physicsType = static_cast<TilePhysicsType>(tileResource->physicsType);
    auto texture = appContext->GetResourcePackManager()->LoadTextureFromFile(
        appContext->GetConfig()->mods.enabledResourcePack, tileResource->texture);
    tile.texture = texture;
    return tile;
}
