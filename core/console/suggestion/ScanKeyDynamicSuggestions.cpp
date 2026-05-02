//
// Created by coldmint on 2026/5/2.
//

#include "ScanKeyDynamicSuggestions.h"

#include "core/Constants.h"
#include "core/utils/ScanCodeUtils.h"

std::string glimmer::ScanKeyDynamicSuggestions::GetId() const {
    return SCAN_KEY_DYNAMIC_SUGGESTIONS_NAME;
}

std::vector<std::string> glimmer::ScanKeyDynamicSuggestions::GetSuggestions(std::string param) {
    return ScanCodeUtils::GetAllScanCodeKeys();
}

bool glimmer::ScanKeyDynamicSuggestions::Match(const std::string keyword, std::string param) {
    return ScanCodeUtils::ContainsKey(keyword);
}
