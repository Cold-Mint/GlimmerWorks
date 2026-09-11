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
#if  !defined(NDEBUG)
#include "DebugPanelSystem.h"
#include <cmath>
#include <utility>
#include "core/math/CoordinateTransformer.h"

#include "core/config/Constants.h"
#include "core/log/LogCat.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/world/WorldContext.h"
#include "core/world/ChunkManager.h"
#include "core/ecs/component/CameraComponent.h"
#include "core/mod/ResourceRef.h"
#include "core/world/Tile.h"
#include "core/world/generator/Chunk.h"
#include "core/world/generator/TerrainMath.h"
#include "fmt/xchar.h"

bool glimmer::DebugPanelSystem::CanActive() const {
    return displayDebugPanel_;
}

void glimmer::DebugPanelSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) {
    const EntityShortCut *entityShortCut = GetEntityShortCut();
    EntityManager *entityManager = GetEntityManager();
    if (gameComponentType == COMPONENT_CAMERA && cameraComponent_ == nullptr) {
        cameraComponent_ = entityShortCut->GetCameraComponent();
    }
    if (gameComponentType == COMPONENT_TRANSFORM_2D && cameraTransform2DComponent_ == nullptr) {
        cameraTransform2DComponent_ = entityShortCut->GetCameraTransform2DComponent();
    }
    if (gameComponentType == COMPONENT_TILE_LAYER) {
        tileLayerComponents_.clear();
        auto tileLayerEntities = entityManager->GetEntityIDWithComponents({COMPONENT_TILE_LAYER});
        std::sort(tileLayerEntities.begin(), tileLayerEntities.end());
        for (GameEntityID tileLayerEntity: tileLayerEntities) {
            auto tileLayerComponent = entityManager->GetComponent<TileLayerComponent>(tileLayerEntity);
            if (tileLayerComponent == nullptr) {
                continue;
            }
            tileLayerComponents_.emplace_back(tileLayerComponent);
        }
    }
}


glimmer::DebugPanelSystem::DebugPanelSystem(WorldContext *worldContext) : GuiGameSystem(worldContext) {
    WatchComponent(COMPONENT_CAMERA);
    WatchComponent(COMPONENT_TRANSFORM_2D);
    WatchComponent(COMPONENT_TILE_LAYER);
    WorldContext *worldContextPtr = GetWorldContext();
    appContext_ = worldContextPtr->GetAppContext();
    Init();
}

void glimmer::DebugPanelSystem::OnConfigChanged(const Config *config) {
    displayDebugPanel_ = config->debug.displayDebugPanel;
    LogCat::i("debug_panel_display_changed", "DebugPanel display changed: {}", displayDebugPanel_);
}

void glimmer::DebugPanelSystem::OnActivationChanged(bool activeStatus) {
    Rml::ElementDocument *elementDocument = GetElementDocument();
    if (elementDocument == nullptr) {
        return;
    }
    if (activeStatus) {
        elementDocument->Show();
    } else {
        elementDocument->Hide();
    }
}

void glimmer::DebugPanelSystem::LoadDocuments(IDocumentRegistry *documentRegistry) {
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_RML_PATH);
    resourceRef.SetResourceKey("debug_panel/debug_panel");
    Rml::ElementDocument *elementDocument = documentRegistry->LoadSingleDocument(&resourceRef);
    SetElementDocument(elementDocument);
    if (elementDocument != nullptr && !displayDebugPanel_) {
        elementDocument->Hide();
    }
}



void glimmer::DebugPanelSystem::OnCreateDataModels(IDocumentRegistry *documentRegistry) {
    Rml::DataModelConstructor *constructor = documentRegistry->CreateDataModel("debug_panel");
    if (constructor == nullptr) {
        return;
    }
    if (auto lineStruct = constructor->RegisterStruct<DebugLine>()) {
        lineStruct.RegisterMember("text", &DebugLine::text);
        constructor->RegisterArray<std::vector<DebugLine> >();
    }
    constructor->Bind("debug_lines", &debugLines_);
    constructor->Bind("chunk_text", &chunkText_);
    constructor->Bind("crosshair_x", &crosshairX_);
    constructor->Bind("crosshair_y", &crosshairY_);
    debugModelHandle_ = constructor->GetModelHandle();
}

void glimmer::DebugPanelSystem::Update(const float delta) {
    if (cameraComponent_ == nullptr || cameraTransform2DComponent_ == nullptr || appContext_ == nullptr ||
        tileLayerComponents_.empty()) {
        return;
    }
    WorldContext *worldContext = GetWorldContext();
    if (worldContext == nullptr) {
        return;
    }
    const LangsResources *langsResources = appContext_->GetLangsResources();
    if (langsResources == nullptr) {
        return;
    }
    debugLines_.clear();
    chunkText_.clear();
    crosshairX_ = -1;
    crosshairY_ = -1;

    if (cameraComponent_->IsPointInViewport(cameraTransform2DComponent_->GetPosition(), mousePosition_)) {
        ChunkGenerator *chunkGenerator = worldContext->GetChunkGenerator();
        ScreenVector2D screenPos = CoordinateTransformer::WorldToScreen(
            cameraTransform2DComponent_->GetPosition(), mousePosition_,
            cameraComponent_->GetSize(), cameraComponent_->GetZoom());

        debugLines_.push_back(DebugLine{fmt::format(
            fmt::runtime(langsResources->mousePosition),
            mousePosition_.x, mousePosition_.y, screenPos.x, screenPos.y
        )});

        bool firstLayer = true;
        TileVector2D tileCoord = CoordinateTransformer::WorldToTile(mousePosition_);
        for (auto tileLayerComponent: tileLayerComponents_) {
            TileVector2D chunkRelative = Chunk::TileCoordinatesToChunkRelativeCoordinates(tileCoord);
            if (firstLayer) {
                float elevation = TerrainMath::GetElevation(tileCoord.y);
                debugLines_.push_back(DebugLine{fmt::format(
                    fmt::runtime(langsResources->tileDebugInfo),
                    tileCoord.x, tileCoord.y,
                    chunkRelative.x, chunkRelative.y,
                    chunkGenerator->GetHumidity(tileCoord),
                    chunkGenerator->GetTemperature(tileCoord, elevation),
                    chunkGenerator->GetErosion(tileCoord),
                    elevation,
                    chunkGenerator->GetWeirdness(tileCoord)
                )});
                firstLayer = false;
            }

            auto tile = tileLayerComponent->GetSelfLayerTile(tileCoord);
            if (tile == nullptr) {
                continue;
            }
            const TileMiningData *miningData = tile->GetMiningData();
            if (miningData == nullptr) {
                continue;
            }
            debugLines_.push_back(DebugLine{fmt::format(
                fmt::runtime(langsResources->tileResDebugInfo),
                std::to_underlying(tile->GetLayerType()), tile->GetId(), miningData->GetHardness(), tile->GetName()
            )});
        }

        if (const Color *finalLightColor = worldContext->GetLightingBuffer()->GetFinalLightColor(tileCoord);
            finalLightColor == nullptr) {
            debugLines_.push_back(DebugLine{fmt::format(
                fmt::runtime(langsResources->totalLight),
                -1, -1, -1, -1
            )});
        } else {
            debugLines_.push_back(DebugLine{fmt::format(
                fmt::runtime(langsResources->totalLight),
                finalLightColor->a, finalLightColor->r, finalLightColor->g, finalLightColor->b
            )});
        }

        // Chunk info text
        // 区块信息文本
        const auto *chunksPtr = worldContext->GetChunkManager()->GetAllChunks();
        int playerTileX = static_cast<int>(std::floor(mousePosition_.x / TILE_SIZE));
        int playerTileY = static_cast<int>(std::floor(mousePosition_.y / TILE_SIZE));

        auto getChunkIndex = [](const int tileCoord) {
            return static_cast<int>(std::floor(static_cast<float>(tileCoord) / CHUNK_SIZE));
        };

        int playerChunkX = getChunkIndex(playerTileX);
        int playerChunkY = getChunkIndex(playerTileY);

        SDL_FRect viewport = CoordinateTransformer::GetViewportRect(cameraTransform2DComponent_->GetPosition(),
                                                                    cameraComponent_->GetSize(),
                                                                    cameraComponent_->GetZoom());
        int startChunkX = static_cast<int>(std::floor(viewport.x / TILE_SIZE / CHUNK_SIZE));
        int startChunkY = static_cast<int>(std::floor(viewport.y / TILE_SIZE / CHUNK_SIZE));
        int endChunkX = static_cast<int>(std::floor((viewport.x + viewport.w) / TILE_SIZE / CHUNK_SIZE));
        int endChunkY = static_cast<int>(std::floor((viewport.y + viewport.h) / TILE_SIZE / CHUNK_SIZE));
        int visibleChunkCount = (endChunkX - startChunkX + 1) * (endChunkY - startChunkY + 1);

        chunkText_ = fmt::format(fmt::runtime(langsResources->debugChunkInfo), playerChunkX,
                                 playerChunkY, visibleChunkCount, chunksPtr->size());

        crosshairX_ = static_cast<int>(screenPos.x);
        crosshairY_ = static_cast<int>(screenPos.y);
    }

    if (debugModelHandle_) {
        debugModelHandle_.DirtyVariable("debug_lines");
        debugModelHandle_.DirtyVariable("chunk_text");
        debugModelHandle_.DirtyVariable("crosshair_x");
        debugModelHandle_.DirtyVariable("crosshair_y");
    }
}

bool glimmer::DebugPanelSystem::HandleEvent(const SDL_Event &event) {
    if (cameraComponent_ == nullptr) {
        return false;
    }
    if (cameraTransform2DComponent_ == nullptr) {
        return false;
    }
    if (event.type == SDL_EVENT_MOUSE_MOTION) {
        mousePosition_ = CoordinateTransformer::ScreenToWorld(
            cameraTransform2DComponent_->GetPosition(),
            ScreenVector2D{
                event.motion.x, event.motion.y
            },
            cameraComponent_->GetSize(),
            cameraComponent_->GetZoom());
    }
    return false;
}

uint8_t glimmer::DebugPanelSystem::GetExecutionOrder() {
    return EXECUTION_ORDER_DEBUG_PANEL;
}

glimmer::GameSystemType glimmer::DebugPanelSystem::GetGameSystemType() const {
    return GameSystemType::DebugPanelSystem;
}

#endif
