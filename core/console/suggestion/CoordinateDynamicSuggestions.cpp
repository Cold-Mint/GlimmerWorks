//
// Created by Cold-Mint on 2026/3/3.
//

#include "CoordinateDynamicSuggestions.h"

#include <stdexcept>
#include <utility>

glimmer::CoordinateDynamicSuggestions::CoordinateDynamicSuggestions(std::string id) : id_(std::move(id)) {
}

std::string glimmer::CoordinateDynamicSuggestions::GetId() const {
    return id_;
}

std::vector<std::string> glimmer::CoordinateDynamicSuggestions::GetSuggestions(std::string param) {
    return {"~", "~+1", "~-1"};
}

bool glimmer::CoordinateDynamicSuggestions::Match(std::string keyword, std::string param) {
    if (!keyword.empty() && keyword[0] == '~') {
        return true;
    }
    try {
        std::stoi(keyword);
        return true;
    } catch (const std::invalid_argument &) {
        return false;
    } catch (const std::out_of_range &) {
        return false;
    }
}
