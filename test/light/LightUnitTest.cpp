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

    for (int radius = 0; radius <= CHUNK_SIZE; radius++) {
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


//Check whether the LightingBuffer can correctly render a complete light source.
//检测LightingBuffer是否能够正确的渲染完整光源。
TEST(LightUnitTest, lightingComplete) {
    const TileVector2D center = {0, 0};
    std::unordered_set<TileVector2D, glimmer::Vector2DIHash> points;
    auto redColor = glimmer::Color(255, 0, 0, 255);
    for (int radius = 0; radius <= CHUNK_SIZE; radius++) {
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
        auto pointSource = std::make_unique<glimmer::LightSource>(center, radius, redColor);
        glimmer::LightingBuffer lightingBuffer;
        lightingBuffer.SetLightSource(center, glimmer::Ground, std::move(pointSource));
        for (auto &point: points) {
            const glimmer::Color *finalLightColor = lightingBuffer.GetFinalLightColor(point);
            EXPECT_NE(nullptr, finalLightColor)
                  << "Light rendering failed! radius = " << radius
                  << ", coordinates (" << point.x << "," << point.y << ") → light color is nullptr";

            if (finalLightColor != nullptr) {
                EXPECT_NE(0, finalLightColor->a)
             << "Light rendering failed! radius = " << radius
             << ", coordinates (" << point.x << "," << point.y << ") → light alpha is 0";
            }
        }
    }
}


//检测之前发现的一个问题，两个光照
TEST(LightUnitTest, DetectingLightAndShadow) {
    glimmer::LightingBuffer lightingBuffer;
    //Place two light sources with a radius of 8.
    //放置两个半径为8的光源。
    TileVector2D pointA = {0, 0};
    auto redColor = glimmer::Color(255, 0, 0, 255);
    auto pointASource = std::make_unique<glimmer::LightSource>(pointA, 8, redColor);
    lightingBuffer.SetLightSource(pointA, glimmer::Ground, std::move(pointASource));
    TileVector2D pointB = {2, 0};
    auto pointBSource = std::make_unique<glimmer::LightSource>(pointB, 8, redColor);
    lightingBuffer.SetLightSource(pointA, glimmer::Ground, std::move(pointBSource));
}


int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
