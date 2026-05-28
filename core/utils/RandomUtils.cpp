//
// Created by Cold-Mint on 2026/5/28.
//

#include "RandomUtils.h"

static std::mt19937 gen(std::random_device{}());

std::mt19937& glimmer::RandomUtils::GetGenerator()
{
    return gen;
}
