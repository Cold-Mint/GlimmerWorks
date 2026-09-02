/*
 * Copyright (C) 2025-2026  Cold-Mint <cold_mint@qq.com>
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
 * 版权(C) 2025-2026  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "UIMessageOverlay.h"

#include <algorithm>

#include <SDL3/SDL.h>

#include "core/context/AppContext.h"
#include "core/log/LogCat.h"
#include "core/mod/ResourceRef.h"


glimmer::UIMessageOverlay::UIMessageOverlay(AppContext *context)
    : Scene(context) {
    Init();
}

void glimmer::UIMessageOverlay::LoadDocuments() {
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_RML_PATH);
    resourceRef.SetResourceKey("ui_message/ui_message");
    uiMessageDocument_ = LoadSingleDocument(&resourceRef);
    if (uiMessageDocument_ == nullptr) {
        LogCat::w(std::source_location::current(), "uiMessageDocument == nullptr");
        return;
    }
    uiMessageDocument_->Show();
}

void glimmer::UIMessageOverlay::OnCreateDataModels() {
    Rml::DataModelConstructor *constructor = CreateDataModel("ui_message");
    if (constructor == nullptr) {
        return;
    }
    if (auto messageStruct = constructor->RegisterStruct<UIMessage>()) {
        messageStruct.RegisterMember("message", &UIMessage::message);
        constructor->RegisterArray<std::vector<UIMessage> >();
    }
    constructor->Bind("ui_messages", &GetAppContext()->GetUIMessages());
    uiMessageModelHandle_ = constructor->GetModelHandle();
}

void glimmer::UIMessageOverlay::Update(const float delta) {
    AppContext *appContext = GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    std::vector<UIMessage> &messages = appContext->GetUIMessages();
    const uint64_t now = SDL_GetTicks();
    const size_t erased = std::erase_if(messages, [now](const UIMessage &message) {
        return now >= message.expireTime;
    });
    // Dirty every frame: the list is small, and this also picks up messages
    // added externally via AppContext::AddUIMessage.
    // 每帧 dirty：列表很小，且这样也能捕获到通过 AppContext::AddUIMessage 在
    // 外部新增的消息。
    if ((erased != 0 || !messages.empty()) && uiMessageModelHandle_) {
        uiMessageModelHandle_.DirtyVariable("ui_messages");
    }
    (void) delta;
}
