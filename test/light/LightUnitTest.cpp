//
// Created by coldmint on 2026/4/27.
//
#include <gtest/gtest.h>

#include "core/world/LightingBuffer.h"
#include "core/world/LightPropagationTraverser.h"

//Check the integrity of the light source
//检测光源完整性：验证光照传播遍历器覆盖所有圆形内的点
TEST(LightUnitTest, LightIntegrityCheck) {
    const TileVector2D center = {0, 0};
    std::unordered_set<TileVector2D, glimmer::Vector2DIHash> points;

    for (int radius = 1; radius <= CHUNK_SIZE; radius++) {
        points.clear();
        const int radiusSquared = radius * radius;
        for (int x = -radius; x <= radius; ++x) {
            for (int y = -radius; y <= radius; ++y) {
                // Distance squared <= Radius squared → Inside the circle
                // 距离平方 <= 半径平方 → 在圆形内
                if (TileVector2D p(x, y); p.DistanceSquared(center) <= radiusSquared) {
                    points.insert(p);
                }
            }
        }

        glimmer::LightPropagationTraverser light_traverser(center, radius,
                                                           [&points](const TileVector2D current,
                                                                     const TileVector2D next, bool center_of_circle,
                                                                     int) -> glimmer::TraverseAction {
                                                               if (center_of_circle) {
                                                                   points.erase(current);
                                                               }
                                                               points.erase(next);
                                                               return glimmer::TraverseAction::Continue;
                                                           });

        light_traverser.PropagateAllRays();
        EXPECT_TRUE(points.empty())
            << "Light source integrity check failed! Radius = " << radius
            << ", Unvisited points count: " << points.size()
            << ", Coordinates: " << [&]() {
                                       std::string s;
                                       for (auto &p: points)
                                           s += "(" + std::to_string(p.x) + "," + std::to_string(p.y) + ") ";
                                       return s;
                                   }();
    }
}




int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
