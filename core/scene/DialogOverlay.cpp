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
#include "DialogOverlay.h"

#include <utility>

#include "core/context/AppContext.h"
#include "core/log/LogCat.h"
#include "core/mod/ResourceRef.h"

glimmer::DialogOverlay::DialogOverlay(AppContext *context) : Scene(context) {
    Init();
}

void glimmer::DialogOverlay::LoadDocuments() {
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_RML_PATH);
    resourceRef.SetResourceKey("dialog/dialog");
    dialogDocument_ = LoadSingleDocument(&resourceRef);
    if (dialogDocument_ == nullptr) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "dialog_document_is_null",
                  "dialogDocument == nullptr");
        return;
    }
    dialogDocument_->Hide();
}

void glimmer::DialogOverlay::OnCreateDataModels() {
    Rml::DataModelConstructor *constructor = CreateDataModel("dialog");
    if (constructor == nullptr) {
        return;
    }
    if (auto buttonStruct = constructor->RegisterStruct<DialogButton>()) {
        buttonStruct.RegisterMember("label", &DialogButton::label);
        buttonStruct.RegisterMember("result", &DialogButton::result);
        buttonStruct.RegisterMember("destructive", &DialogButton::destructive);
        buttonStruct.RegisterMember("suggested", &DialogButton::suggested);
        constructor->RegisterArray<std::vector<DialogButton> >();
    }
    constructor->Bind("title", &title_);
    constructor->Bind("message", &message_);
    constructor->Bind("buttons", &buttons_);
    constructor->BindEventCallback(
        "on_dialog_button_click",
        &DialogOverlay::OnButtonClick,
        this
    );
    modelHandle_ = constructor->GetModelHandle();
}

void glimmer::DialogOverlay::ShowDialog(DialogRequest request) {
    if (dialogDocument_ == nullptr) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "dialog_document_is_null",
                  "dialogDocument == nullptr");
        return;
    }
    title_ = std::move(request.title);
    message_ = std::move(request.message);
    buttons_ = std::move(request.buttons);
    onResult_ = std::move(request.onResult);
    cancelResult_ = request.cancelResult;
    ApplyToModel();
    dialogDocument_->Show();
}

bool glimmer::DialogOverlay::IsVisible() const {
    return dialogDocument_ != nullptr && dialogDocument_->IsVisible();
}

void glimmer::DialogOverlay::OnButtonClick(Rml::DataModelHandle handle, Rml::Event &event,
                                           const Rml::VariantList &args) {
    if (args.empty()) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "dialog_button_args_empty",
                  "args.empty()");
        return;
    }
    InvokeResult(args[0].Get<int>());
}

void glimmer::DialogOverlay::HideAndClear() {
    if (dialogDocument_ != nullptr) {
        dialogDocument_->Hide();
    }
    title_.clear();
    message_.clear();
    buttons_.clear();
}

void glimmer::DialogOverlay::InvokeResult(const int result) {
    auto callback = std::move(onResult_);
    onResult_ = nullptr;
    HideAndClear();
    if (callback) {
        callback(result);
    }
}

void glimmer::DialogOverlay::ApplyToModel() {
    modelHandle_.DirtyVariable("title");
    modelHandle_.DirtyVariable("message");
    modelHandle_.DirtyVariable("buttons");
}

bool glimmer::DialogOverlay::HandleEvent(const SDL_Event &event) {
    if (dialogDocument_ == nullptr || !dialogDocument_->IsVisible()) {
        return Scene::HandleEvent(event);
    }
    // While visible the dialog is modal: consume events so the underlying scene
    // does not react. Mouse events are still delivered to RmlUi by the event
    // loop before this dispatch.
    // 对话框可见时为模态：消费事件以免底层场景响应。鼠标事件仍会在本分发前由事件
    // 循环交给 RmlUi。
    if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) {
        if (event.key.scancode == SDL_SCANCODE_RETURN) {
            if (!buttons_.empty()) {
                InvokeResult(buttons_.back().result);
            }
            return true;
        }
    }
    return true;
}

bool glimmer::DialogOverlay::OnBackPressed() {
    if (dialogDocument_ == nullptr || !dialogDocument_->IsVisible()) {
        return false;
    }
    if (cancelResult_ >= 0) {
        InvokeResult(cancelResult_);
    } else {
        HideAndClear();
    }
    return true;
}
