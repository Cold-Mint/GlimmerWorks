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
#include "LoadWorldGuiSystem.h"

#include <fmt/format.h>

#include "core/context/AppContext.h"
#include "core/event/ChunkTaskExecEvent.h"
#include "core/event/ChunkTaskFinshEvent.h"
#include "core/event/EventBus.h"
#include "core/scene/MainThreadDispatcher.h"
#include "core/world/WorldContext.h"

glimmer::LoadWorldGuiSystem::LoadWorldGuiSystem(WorldContext *worldContext) : GuiGameSystem(
    worldContext) {
    Init();
    AppContext *appContext = worldContext->GetAppContext();
    if (appContext == nullptr) return;
    EventBus *eventBus = appContext->GetEventBus();
    if (eventBus == nullptr) return;
    MainThreadDispatcher *dispatcher = appContext->GetMainThreadDispatcher();
    chunkTaskFinishSubscription_ = eventBus->Subscribe<ChunkTaskFinshEvent>(
        [this, dispatcher, worldContext](const ChunkTaskFinshEvent &) {
            worldContext->SetLoadFinish(true);
            if (dispatcher == nullptr) return;
            dispatcher->PostToNextMainFrame([this] {
                if (Rml::ElementDocument *document = GetElementDocument(); document != nullptr) {
                    document->Hide();
                }
            });
        });

    chunkTaskExecSubscription_ = eventBus->Subscribe<ChunkTaskExecEvent>(
        [this, dispatcher, appContext](const ChunkTaskExecEvent &event) {
            if (dispatcher == nullptr || appContext == nullptr) {
                return;
            }
            const ChunkTask *task = event.GetChunkTask();
            const LangsResources *langsResources = appContext->GetLangsResources();
            if (task == nullptr || langsResources == nullptr) {
                return;
            }
            const ChunkVertexVector2D pos = task->GetPosition();
            std::string text;
            switch (task->GetTaskType()) {
                case ChunkTaskType::LOAD:
                    text = fmt::format(fmt::runtime(langsResources->loadChunk), pos.x, pos.y);
                    break;
                case ChunkTaskType::UNLOAD:
                    text = fmt::format(fmt::runtime(langsResources->unloadChunk), pos.x, pos.y);
                    break;
                default:
                    return;
            }
            dispatcher->PostToNextMainFrame([this, text] {
                statusText_ = text;
                dataModelHandle_.DirtyVariable("status_text");
            });
        });
}

glimmer::GameSystemType glimmer::LoadWorldGuiSystem::GetGameSystemType() const {
    return GameSystemType::LoadWorldGuiStackSystem;
}

void glimmer::LoadWorldGuiSystem::LoadDocuments(IDocumentRegistry *documentRegistry) {
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_RML_PATH);
    resourceRef.SetResourceKey("load_world/load_world");
    SetElementDocument(documentRegistry->LoadSingleDocument(&resourceRef));
}

void glimmer::LoadWorldGuiSystem::OnCreateDataModels(IDocumentRegistry *documentRegistry) {
    Rml::DataModelConstructor *constructor = documentRegistry->CreateDataModel("load_world");
    if (constructor == nullptr) {
        return;
    }
    constructor->Bind("status_text", &statusText_);
    dataModelHandle_ = constructor->GetModelHandle();
}
