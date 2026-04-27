//
// Created by coldmint on 2026/4/27.
//
#include <gtest/gtest.h>

#include "core/world/LightingBuffer.h"
#include "core/math/Color.h"


// TEST(LightUnitTest, PositiveNumbers) {
//     glimmer::LightingBuffer lightingBuffer;
//     const TileVector2D tileVector = {0, 0};
//     glimmer::Color color = {0, 0, 0, 0};
//     auto lightMask = std::make_unique<glimmer::LightMask>(&color);
//     lightingBuffer.SetLightMask(tileVector, glimmer::Ground, std::move(lightMask));
//
//     // EXPECT_EQ(add(2, 3), 5);
//     // EXPECT_EQ(add(10, 20), 30);
// }

int main(int argc, char *argv[]) {

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
