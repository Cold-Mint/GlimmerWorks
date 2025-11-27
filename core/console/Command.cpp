//
// Created by Cold-Mint on 2025/10/15.
//

#include "Command.h"

bool glimmer::Command::RequiresWorldContext() const {
    return false;
}

void glimmer::Command::BindWorldContext(WorldContext *worldContext) {
    worldContext_ = worldContext;
}

void glimmer::Command::UnBindWorldContext() {
    worldContext_ = nullptr;
}

glimmer::NodeTree<std::string> glimmer::Command::GetSuggestionsTree() const {
    return suggestionsTree_;
}
