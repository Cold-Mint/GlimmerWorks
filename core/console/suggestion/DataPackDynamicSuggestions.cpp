//
// Created by Cold-Mint on 2026/5/21.
//

#include "DataPackDynamicSuggestions.h"

#include "core/Constants.h"

glimmer::DataPackDynamicSuggestions::DataPackDynamicSuggestions(DataPackManager *dataPackManager) : dataPackManager_(
    dataPackManager) {
}

std::string glimmer::DataPackDynamicSuggestions::GetId() const {
    return DATA_PACK_SUGGESTIONS_NAME;
}

std::vector<std::string> glimmer::DataPackDynamicSuggestions::GetSuggestions(std::string param) {
    return dataPackManager_->GetPackIdList();
}

bool glimmer::DataPackDynamicSuggestions::Match(std::string keyword, std::string param) {
    return dataPackManager_->Contains(keyword);
}
