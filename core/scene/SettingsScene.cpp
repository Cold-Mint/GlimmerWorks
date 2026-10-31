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
#include "SettingsScene.h"

#include <unordered_set>

#include "core/context/AppContext.h"
#include "core/config/Constants.h"
#include "core/log/LogCat.h"
#include "core/mod/ResourceRef.h"
#include "core/scene/MainThreadDispatcher.h"
#include "core/scene/SceneManager.h"
#include "core/vfs/VirtualFileSystem.h"
#include "fmt/format.h"
#include "RmlUi/Core/Element.h"
#include "RmlUi/Core/Variant.h"
#include "toml.hpp"


namespace {
    std::string VariantValueToString(const Rml::Variant &variant) {
        switch (variant.GetType()) {
            case Rml::Variant::FLOAT:
                return fmt::format("{}", variant.Get<float>());
            case Rml::Variant::DOUBLE:
                return fmt::format("{}", variant.Get<double>());
            case Rml::Variant::INT:
                return std::to_string(variant.Get<int>());
            case Rml::Variant::INT64:
                return std::to_string(variant.Get<int64_t>());
            case Rml::Variant::STRING:
                return variant.Get<std::string>();
            default:
                return "";
        }
    }
} // namespace


glimmer::SettingsScene::SettingsScene(AppContext *context) : Scene(context) {
    config_ = context->GetConfig();
    sceneManager_ = context->GetSceneManager();
    mainThreadDispatcher_ = context->GetMainThreadDispatcher();
    virtualFileSystem_ = context->GetVirtualFileSystem();
    if (config_ != nullptr) {
        draftTags_ = config_->Tags();
        draftSettings_ = config_->Settings();
        for (const auto &tag: draftTags_) {
            if (tag.tagId == "advanced") {
                showAdvanced_ = tag.enable;
                break;
            }
        }
        RebuildSections();
    }
    Init();
}

void glimmer::SettingsScene::LoadDocuments() {
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_RML_PATH);
    resourceRef.SetResourceKey("settings/settings");
    LoadSingleDocument(&resourceRef);
}

void glimmer::SettingsScene::OnCreateDataModels() {
    Rml::DataModelConstructor *constructor = CreateDataModel("settings_scene");
    if (constructor == nullptr) {
        return;
    }
    if (auto itemStruct = constructor->RegisterStruct<SettingItemModel>()) {
        itemStruct.RegisterMember("key", &SettingItemModel::key);
        itemStruct.RegisterMember("label", &SettingItemModel::label);
        itemStruct.RegisterMember("is_bool", &SettingItemModel::isBool);
        itemStruct.RegisterMember("is_enum", &SettingItemModel::isEnum);
        itemStruct.RegisterMember("is_range", &SettingItemModel::isRange);
        itemStruct.RegisterMember("is_text", &SettingItemModel::isText);
        itemStruct.RegisterMember("value", &SettingItemModel::value);
        itemStruct.RegisterMember("value_bool", &SettingItemModel::valueBool);
        itemStruct.RegisterMember("min", &SettingItemModel::min);
        itemStruct.RegisterMember("max", &SettingItemModel::max);
        itemStruct.RegisterMember("step", &SettingItemModel::step);
        itemStruct.RegisterMember("options", &SettingItemModel::options);
        constructor->RegisterArray<std::vector<SettingItemModel> >();
    }
    if (auto sectionStruct = constructor->RegisterStruct<SettingSectionModel>()) {
        sectionStruct.RegisterMember("title", &SettingSectionModel::title);
        sectionStruct.RegisterMember("items", &SettingSectionModel::items);
        constructor->RegisterArray<std::vector<SettingSectionModel> >();
    }
    constructor->Bind("sections", &sections_);
    constructor->Bind("selected_index", &selectedIndex_);
    constructor->Bind("selected_items", &selectedItems_);
    constructor->Bind("selected_title", &selectedTitle_);
    constructor->Bind("show_advanced", &showAdvanced_);
    constructor->BindEventCallback(
        "on_value_changed",
        &SettingsScene::OnValueChanged,
        this
    );
    constructor->BindEventCallback(
        "on_bool_changed",
        &SettingsScene::OnBoolChanged,
        this
    );
    constructor->BindEventCallback(
        "on_section_select",
        &SettingsScene::OnSectionSelect,
        this
    );
    constructor->BindEventCallback(
        "on_show_advanced_changed",
        &SettingsScene::OnShowAdvancedChanged,
        this
    );
    constructor->BindEventCallback(
        "on_save_click",
        &SettingsScene::OnSaveClick,
        this
    );
    constructor->BindEventCallback(
        "on_back_click",
        &SettingsScene::OnBackClick,
        this
    );
    modelHandle_ = constructor->GetModelHandle();
}

glimmer::SettingDef *glimmer::SettingsScene::FindSetting(const std::string &key) {
    for (auto &setting: draftSettings_) {
        if (setting.key == key) {
            return &setting;
        }
    }
    return nullptr;
}

glimmer::SettingItemModel glimmer::SettingsScene::ToItemModel(const SettingDef &setting) {
    const std::optional<std::string> valueOptional = setting.GetAsString();
    std::string value;
    if (valueOptional.has_value()) {
        value = valueOptional.value();
    }
    SettingItemModel model;
    model.key = setting.key;
    model.label = setting.labelKey;
    switch (setting.type) {
        case SettingType::Bool:
            model.isBool = true;
            model.valueBool = std::get<bool>(setting.value);
            break;
        case SettingType::Enum:
            model.isEnum = true;
            model.value = value;
            model.options = setting.options;
            break;
        case SettingType::IntRange:
        case SettingType::FloatRange:
            model.isRange = true;
            model.value = value;
            model.min = setting.minValue.value_or(0.0F);
            model.max = setting.maxValue.value_or(0.0F);
            model.step = setting.stepValue.value_or(1.0F);
            break;
        case SettingType::String:
        case SettingType::Int:
        case SettingType::Float:
        case SettingType::List:
        default:
            model.isText = true;
            model.value = value;
            break;
    }
    return model;
}

void glimmer::SettingsScene::RebuildSections() {
    // 将"显示高级"开关同步到 advanced 标签的 enable。
    for (auto &tag: draftTags_) {
        if (tag.tagId == "advanced") {
            tag.enable = showAdvanced_;
        }
    }

    std::unordered_set<std::string> enabledTags;
    for (const auto &tag: draftTags_) {
        if (tag.enable) {
            enabledTags.insert(tag.tagId);
        }
    }

    sections_.clear();
    for (const auto &tag: draftTags_) {
        if (!tag.enable || !tag.visible) {
            continue;
        }
        SettingSectionModel section;
        section.title = tag.labelKey;
        for (const auto &setting: draftSettings_) {
            if (setting.groupTag != tag.tagId) {
                continue;
            }
            bool visible = true;
            for (const auto &required: setting.requiredTags) {
                if (!enabledTags.contains(required)) {
                    visible = false;
                    break;
                }
            }
            if (!visible) {
                continue;
            }
            section.items.emplace_back(ToItemModel(setting));
        }
        if (!section.items.empty()) {
            sections_.push_back(std::move(section));
        }
    }
    UpdateSelectedSection();
}

void glimmer::SettingsScene::UpdateSelectedSection() {
    if (sections_.empty()) {
        selectedIndex_ = -1;
        selectedTitle_.clear();
        selectedItems_.clear();
        return;
    }
    if (selectedIndex_ < 0 || selectedIndex_ >= static_cast<int>(sections_.size())) {
        selectedIndex_ = 0;
    }
    selectedTitle_ = sections_[selectedIndex_].title;
    selectedItems_ = sections_[selectedIndex_].items;
}

void glimmer::SettingsScene::OnValueChanged(Rml::DataModelHandle handle, Rml::Event &event,
                                            const Rml::VariantList &args) {
    if (args.empty()) {
        return;
    }
    const auto key = args[0].Get<std::string>();
    const auto &parameters = event.GetParameters();
    const auto valueIt = parameters.find("value");
    if (valueIt == parameters.end()) {
        return;
    }
    const std::string value = VariantValueToString(valueIt->second);
    if (value.empty()) {
        return;
    }
    if (SettingDef *setting = FindSetting(key); setting != nullptr) {
        setting->SetFromString(value);
    }
}

void glimmer::SettingsScene::OnBoolChanged(Rml::DataModelHandle handle, Rml::Event &event,
                                           const Rml::VariantList &args) {
    if (args.empty()) {
        return;
    }
    const auto key = args[0].Get<std::string>();
    const auto &parameters = event.GetParameters();
    const auto checkedIt = parameters.find("checked");
    if (checkedIt == parameters.end() || checkedIt->second.GetType() != Rml::Variant::BOOL) {
        return;
    }
    const bool checked = checkedIt->second.Get<bool>();
    if (SettingDef *setting = FindSetting(key); setting != nullptr) {
        setting->SetFromString(checked ? "true" : "false");
    }
}

void glimmer::SettingsScene::OnShowAdvancedChanged(Rml::DataModelHandle handle, Rml::Event &event,
                                                   const Rml::VariantList &args) {
    // showAdvanced_ 已由 data-checked 双向绑定更新。
    RebuildSections();
    modelHandle_.DirtyVariable("sections");
    modelHandle_.DirtyVariable("selected_items");
    modelHandle_.DirtyVariable("selected_title");
    modelHandle_.DirtyVariable("selected_index");
}

void glimmer::SettingsScene::OnSectionSelect(Rml::DataModelHandle handle, Rml::Event &event,
                                             const Rml::VariantList &args) {
    if (args.empty()) {
        return;
    }
    const int index = args[0].Get<int>();
    if (index < 0 || index >= static_cast<int>(sections_.size())) {
        return;
    }
    selectedIndex_ = index;
    // 重新由草稿重建，保证切换后右侧显示的是最新编辑值。
    RebuildSections();
    modelHandle_.DirtyVariable("sections");
    modelHandle_.DirtyVariable("selected_items");
    modelHandle_.DirtyVariable("selected_title");
    modelHandle_.DirtyVariable("selected_index");
}

void glimmer::SettingsScene::OnSaveClick(Rml::DataModelHandle handle, Rml::Event &event,
                                         const Rml::VariantList &args) {
    if (config_ == nullptr) {
        return;
    }
    for (const auto &draft: draftSettings_) {
        config_->Set(StringUtils::StringToUint64(draft.key), draft.value);
    }
    for (const auto &draftTag: draftTags_) {
        config_->SetTagEnabled(StringUtils::StringToUint64(draftTag.tagId), draftTag.enable);
    }
    if (virtualFileSystem_ != nullptr) {
        if (!virtualFileSystem_->WriteFile(CONFIG_FILE_NAME, toml::format(config_->Save()))) {
            LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "settings_save_failed",
                      "Failed to write {}.", CONFIG_FILE_NAME);
        }
    }
    PopSceneAndApplyConfig();
}

void glimmer::SettingsScene::OnBackClick(Rml::DataModelHandle handle, Rml::Event &event,
                                         const Rml::VariantList &args) {
    PopScene();
}

void glimmer::SettingsScene::PopScene() {
    if (mainThreadDispatcher_ == nullptr || sceneManager_ == nullptr) {
        return;
    }
    mainThreadDispatcher_->PostToNextMainFrame([this] {
        sceneManager_->PopScene();
    });
}

void glimmer::SettingsScene::PopSceneAndApplyConfig() {
    if (mainThreadDispatcher_ == nullptr || sceneManager_ == nullptr) {
        return;
    }
    mainThreadDispatcher_->PostToNextMainFrame([this] {
        sceneManager_->PopScene();
        // 设置页在保存后弹出，此时底层场景才重新成为栈顶。由于 CheckConfigChange
        // 已在设置页仍为栈顶时消费了指纹变化，这里手动通知新栈顶场景应用配置。
        if (Scene *top = sceneManager_->GetTopScene(); top != nullptr) {
            top->OnConfigChanged(config_);
        }
    });
}
