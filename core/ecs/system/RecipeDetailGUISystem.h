/*
* Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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
 */
#pragma once
#include <string>
#include <unordered_map>

#include "core/ecs/GuiStackGameSystem.h"
#include "core/rmi/dataModel/RecipeItemChoiceDataModel.h"
#include "core/rmi/dataModel/TagProgressDataModel.h"
#include "core/world/SystemScheduler.h"

namespace glimmer
{
    class RecipeDetailGUISystem : public GuiStackGameSystem
    {
        Rml::DataModelConstructor* constructor_ = nullptr;
        ItemContainer* itemContainer_ = nullptr;
        RecipeResource* currentRecipe_ = nullptr;
        RecipeSelectionComponent* recipeSelectionComponent_ = nullptr;

        std::vector<TagProgressDataModel> tagProgress_;
        std::vector<RecipeItemChoiceDataModel> itemChoices_;
        std::vector<int> originalRequiredWeights_;
        std::string outputImage_;
        std::string outputName_;
        SystemScheduler* systemScheduler_ = nullptr;
        int outputAmount_ = 0;
        bool canCraft_ = false;
        int craftCount_ = 1;
        int outputCount_ = 1;
        int maxCraftCount_ = 1;

        std::unordered_map<uint8_t, uint8_t> investedItems_;

        void LoadRecipeDetail();

        void UpdateTagProgress();

        void UpdateTagDisplayWeights();

        void UpdateCanCraft();

        void ExecuteCraftBatch(int count);

        void AutoFill();

        void ResetInvestment();

    public:
        ~RecipeDetailGUISystem() override;

        explicit RecipeDetailGUISystem(WorldContext* worldContext);

        void OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) override;

        void OnCreateDataModels(IDocumentRegistry* documentRegistry) override;

        void OnActivationChanged(bool activeStatus) override;

        SDL_Scancode GetHotKey() const override;

        [[nodiscard]] GameSystemType GetGameSystemType() const override;

        void LoadDocuments(IDocumentRegistry* documentRegistry) override;

        void OnBackClick(Rml::DataModelHandle handle, Rml::Event& event, const Rml::VariantList& args);

        void OnCraftClick(Rml::DataModelHandle handle, Rml::Event& event, const Rml::VariantList& args);

        void OnResetClick(Rml::DataModelHandle handle, Rml::Event& event, const Rml::VariantList& args);

        void OnAutoFillClick(Rml::DataModelHandle handle, Rml::Event& event, const Rml::VariantList& args);

        void OnCraftCountIncrease(Rml::DataModelHandle handle, Rml::Event& event, const Rml::VariantList& args);

        void OnCraftCountDecrease(Rml::DataModelHandle handle, Rml::Event& event, const Rml::VariantList& args);

        void OnItemIncrease(Rml::DataModelHandle handle, Rml::Event& event, const Rml::VariantList& args);

        void OnItemDecrease(Rml::DataModelHandle handle, Rml::Event& event, const Rml::VariantList& args);
    };
}
