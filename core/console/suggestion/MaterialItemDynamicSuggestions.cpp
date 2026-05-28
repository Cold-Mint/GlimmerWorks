//
// Created by Cold-Mint on 2026/5/27.
//

#include "MaterialItemDynamicSuggestions.h"

glimmer::MaterialItemDynamicSuggestions::MaterialItemDynamicSuggestions(ItemManager* itemManager)
{
    itemManager_ = itemManager;
}

std::string glimmer::MaterialItemDynamicSuggestions::GetId() const
{
    return MATERIAL_ITEM_DYNAMIC_SUGGESTIONS_NAME;
}

std::vector<std::string> glimmer::MaterialItemDynamicSuggestions::GetSuggestions(std::string param)
{
    return itemManager_->GetMaterialItemIDList();
}

bool glimmer::MaterialItemDynamicSuggestions::Match(std::string keyword, std::string param)
{
    for (const auto& itemId : itemManager_->GetMaterialItemIDList())
    {
        if (itemId == keyword)
        {
            return true;
        }
    }
    return false;
}
