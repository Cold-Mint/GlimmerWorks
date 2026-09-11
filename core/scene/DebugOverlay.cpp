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
#include "DebugOverlay.h"

#include "core/config/Config.h"
#include "core/config/Constants.h"
#include "core/context/AppContext.h"
#include "core/log/LogCat.h"
#include "core/mod/ResourceRef.h"
#include "fmt/xchar.h"


glimmer::DebugOverlay::DebugOverlay(AppContext *context)
    : Scene(context), langsResources_(context->GetLangsResources()) {
    LogCat::i("creating_debug_overlay", "Creating DebugOverlay");
    Init();
}

void glimmer::DebugOverlay::Update(const float delta) {
    if (!displayDebugPanel_) {
        return;
    }
    if (delta <= 0.0F) {
        return;
    }
    fpsAccumTime_ += delta;
    fpsFrameCount_ += 1;
    constexpr float kFpsUpdateInterval = 1.0F;
    if (fpsAccumTime_ >= kFpsUpdateInterval) {
        fps_ = static_cast<float>(fpsFrameCount_) / fpsAccumTime_;
        frameTimeMs_ = fpsAccumTime_ / static_cast<float>(fpsFrameCount_) * 1000.0F;
        // Average time consumption per frame (ms) 平均每帧耗时(ms)
        fpsFrameCount_ = 0;
        fpsAccumTime_ = 0.0F;
        if (langsResources_ == nullptr) {
            fpsText_ = fmt::format("FPS:{:.2f} | Frame Time:{:.2f} ms", fps_, frameTimeMs_);
        } else {
            fpsText_ = fmt::format(fmt::runtime(langsResources_->fpsInfo), fps_, frameTimeMs_);
        }
        if (debugModelHandle_) {
            debugModelHandle_.DirtyVariable("fps_text");
        }
    }
}

void glimmer::DebugOverlay::LoadDocuments() {
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_RML_PATH);
    resourceRef.SetResourceKey("debug_overlay/debug_overlay");
    debugDocument_ = LoadSingleDocument(&resourceRef);
    if (debugDocument_ == nullptr) {
        LogCat::w(std::source_location::current(), "debug_document_is_null", "debugDocument_ == nullptr");
        return;
    }
    UpdateDocumentVisibility();
}

void glimmer::DebugOverlay::OnCreateDataModels() {
    Rml::DataModelConstructor *constructor = CreateDataModel("debug_overlay_scene");
    if (constructor == nullptr) {
        return;
    }
    constructor->Bind("fps_text", &fpsText_);
    if (auto labelStruct = constructor->RegisterStruct<CoordinateLabel>()) {
        labelStruct.RegisterMember("coordinate", &CoordinateLabel::coordinate);
        constructor->RegisterArray<std::vector<CoordinateLabel> >();
    }
    constructor->Bind("x_coordinates", &xCoordinateLabels_);
    constructor->Bind("y_coordinates", &yCoordinateLabels_);
    debugModelHandle_ = constructor->GetModelHandle();
}

void glimmer::DebugOverlay::UpdateDocumentVisibility() {
    if (debugDocument_ == nullptr) {
        return;
    }
    if (displayDebugPanel_) {
        debugDocument_->Show();
    } else {
        debugDocument_->Hide();
    }
}

void glimmer::DebugOverlay::RebuildCoordinateLabels() {
    const auto labelSpacing = static_cast<int>(50 * uiScale_);
    if (labelSpacing <= 0) {
        return;
    }
    xCoordinateLabels_.clear();
    for (int x = 0; x <= windowWidth_; x += labelSpacing) {
        xCoordinateLabels_.push_back(CoordinateLabel{x});
    }
    yCoordinateLabels_.clear();
    for (int y = 0; y <= windowHeight_; y += labelSpacing) {
        yCoordinateLabels_.push_back(CoordinateLabel{y});
    }
    if (debugModelHandle_) {
        debugModelHandle_.DirtyVariable("x_coordinates");
        debugModelHandle_.DirtyVariable("y_coordinates");
    }
}

void glimmer::DebugOverlay::OnConfigChanged(const Config *config) {
    displayDebugPanel_ = config->debug.displayDebugPanel;
    uiScale_ = config->window.uiScale;
    UpdateDocumentVisibility();
    RebuildCoordinateLabels();
}

void glimmer::DebugOverlay::OnWindowSizeChanged(const int &width, const int &height) {
    windowWidth_ = width;
    windowHeight_ = height;
    RebuildCoordinateLabels();
}
#endif
