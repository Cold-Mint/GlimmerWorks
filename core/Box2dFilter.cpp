//
// Created by Cold-Mint on 2026/3/9.
//

#include "Box2dFilter.h"

b2QueryFilter glimmer::Box2dFilter::Tob2QueryFilter() const {
    return {categoryBits, maskBits};
}
