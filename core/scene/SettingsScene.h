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
#pragma once
#include <string>
#include <vector>

#include "Scene.h"
#include "core/config/Config.h"
#include "RmlUi/Core/DataModelHandle.h"

namespace glimmer {
    class MainThreadDispatcher;
    class SceneManager;
    class VirtualFileSystem;

    /**
     * SettingItemModel
     * 设置页数据模型中单个设置项的呈现模型。
     */
    struct SettingItemModel {
        std::string key;
        std::string label;
        bool isBool = false;
        bool isEnum = false;
        bool isRange = false;
        bool isText = false;
        std::string value;
        bool valueBool = false;
        float min = 0;
        float max = 0;
        float step = 0;
        std::vector<std::string> options;
    };

    /**
     * SettingSectionModel
     * 设置页数据模型中的一个分组。
     */
    struct SettingSectionModel {
        std::string title;
        std::vector<SettingItemModel> items;
    };

    /**
     * SettingsScene
     * 由 config.toml 自动生成的设置页。所有编辑先落到草稿（draft），点保存后写回
     * Config 并持久化到 config.toml，不保存则丢弃。
     */
    class SettingsScene final : public Scene {
        Config *config_ = nullptr;
        SceneManager *sceneManager_ = nullptr;
        MainThreadDispatcher *mainThreadDispatcher_ = nullptr;
        VirtualFileSystem *virtualFileSystem_ = nullptr;
        Rml::DataModelHandle modelHandle_;

        std::vector<TagDef> draftTags_;
        std::vector<SettingDef> draftSettings_;
        std::vector<SettingSectionModel> sections_;
        std::vector<SettingItemModel> selectedItems_;
        std::string selectedTitle_;
        int selectedIndex_ = 0;
        bool showAdvanced_ = false;

        void RebuildSections();

        void UpdateSelectedSection();

        [[nodiscard]] SettingDef *FindSetting(const std::string &key);

        static SettingItemModel ToItemModel(const SettingDef &setting);

        void OnValueChanged(Rml::DataModelHandle handle, Rml::Event &event, const Rml::VariantList &args);

        void OnBoolChanged(Rml::DataModelHandle handle, Rml::Event &event, const Rml::VariantList &args);

        void OnSectionSelect(Rml::DataModelHandle handle, Rml::Event &event, const Rml::VariantList &args);

        void OnShowAdvancedChanged(Rml::DataModelHandle handle, Rml::Event &event, const Rml::VariantList &args);

        void OnSaveClick(Rml::DataModelHandle handle, Rml::Event &event, const Rml::VariantList &args);

        void OnBackClick(Rml::DataModelHandle handle, Rml::Event &event, const Rml::VariantList &args);

        void PopScene();

        void PopSceneAndApplyConfig();

    public:
        explicit SettingsScene(AppContext *context);

        void LoadDocuments() override;

        void OnCreateDataModels() override;
    };
} // namespace glimmer
