//
// Created by Cold-Mint on 2025/10/15.
//

#include "CommandArgs.h"

#include <stdexcept>

bool glimmer::CommandArgs::AsBool(const int index) const {
    if (index < 0 || index >= static_cast<int>(tokens.size())) {
        throw std::out_of_range("CommandArgs::asBool index out of range");
    }
    const std::string &val = tokens[index];
    if (val == "1" || val == "true" || val == "yes") return true;
    if (val == "0" || val == "false" || val == "no") return false;
    throw std::invalid_argument("Cannot convert to bool: " + val);
}

int glimmer::CommandArgs::AsInt(const int index) const {
    if (index < 0 || index >= static_cast<int>(tokens.size())) {
        throw std::out_of_range("CommandArgs::asInt index out of range");
    }
    return std::stoi(tokens[index]);
}

float glimmer::CommandArgs::AsFloat(const int index) const {
    if (index < 0 || index >= static_cast<int>(tokens.size())) {
        throw std::out_of_range("CommandArgs::asFloat index out of range");
    }
    return std::stof(tokens[index]);
}

double glimmer::CommandArgs::AsDouble(const int index) const {
    if (index < 0 || index >= static_cast<int>(tokens.size())) {
        throw std::out_of_range("CommandArgs::asDouble index out of range");
    }
    return std::stod(tokens[index]);
}

std::string glimmer::CommandArgs::AsString(const int index) const {
    if (index < 0 || index >= static_cast<int>(tokens.size())) {
        throw std::out_of_range("CommandArgs::asString index out of range");
    }
    return tokens[index];
}
