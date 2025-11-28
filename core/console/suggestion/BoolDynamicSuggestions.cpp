//
// Created by Cold-Mint on 2025/11/27.
//

#include "BoolDynamicSuggestions.h"

#include "../../Constants.h"


std::string glimmer::BoolDynamicSuggestions::GetId() const {
    return BOOL_DYNAMIC_SUGGESTIONS_NAME;
}

std::vector<std::string> glimmer::BoolDynamicSuggestions::GetSuggestions(std::string param) {
    return {"true", "false"};
}
