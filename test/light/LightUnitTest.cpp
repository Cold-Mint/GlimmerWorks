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
            EXPECT_TRUE(finalLightColor != nullptr)
                  << "Light rendering failed! radius = " << radius
                  << ", coordinates (" << point.x << "," << point.y << ") → light color is nullptr";

            if (finalLightColor != nullptr) {
                EXPECT_TRUE(finalLightColor->a > 0)
             << "Light rendering failed! radius = " << radius
             << ", coordinates (" << point.x << "," << point.y << ") → light alpha is 0";
            }
        }
    }
}


//Check if there are any remnants after the light damage.
//检测光照破坏后是否有残留。
TEST(LightUnitTest, CheckLightRemainAfterBreak) {
    // Check the case of placing alone.
    TileVector2D originPoint = {0, 0};
    auto redColor = glimmer::Color(255, 0, 0, 255);
    for (int radius = 0; radius <= CHUNK_SIZE; radius++) {
        glimmer::LightingBuffer lightingBuffer;
        lightingBuffer.SetLightSource(originPoint, glimmer::Ground,
                                      std::make_unique<glimmer::LightSource>(originPoint, radius, redColor));
        lightingBuffer.ClearLightSource(originPoint, glimmer::Ground);
        // Is there still any light remaining within the detection range?
        for (int x = -radius; x <= radius; x++) {
            for (int y = -radius; y <= radius; y++) {
                const glimmer::Color *lightColor = lightingBuffer.GetFinalLightColor({x, y});
                // If it is not empty, then an error will be reported.
                EXPECT_TRUE(lightColor == nullptr) << "Found residual radius = " << radius << " Position x=" << x << ",y="
                    << y << " color a =" << lightColor->a << ",r = " << lightColor->r << ",g = " << lightColor->g << ",b = " << lightColor->b;
            }
        }
    }

    for (int radius = 0; radius <= CHUNK_SIZE; radius++) {
        for (int interval = 1; interval <= radius; interval++) {
            glimmer::LightingBuffer lightingBuffer;
            lightingBuffer.SetLightSource(originPoint, glimmer::Ground,
                                          std::make_unique<glimmer::LightSource>(originPoint, radius, redColor));

            // Store the color values of each coordinate (for subsequent comparison) before deleting B
            std::map<std::pair<int, int>, glimmer::Color> beforeColorMap;
            int beforeACount = 0;
            for (int x = -radius; x <= radius; x++) {
                for (int y = -radius; y <= radius; y++) {
                    const glimmer::Color *lightColor = lightingBuffer.GetFinalLightColor({x, y});
                    // Record color
                    if (lightColor != nullptr) {
                        beforeColorMap[{x, y}] = *lightColor;
                        if (lightColor->a > 0) {
                            beforeACount++;
                        }
                    } else {
                        // Null pointers are also recorded (to ensure consistency throughout).
                        beforeColorMap[{x, y}] = glimmer::Color{0, 0, 0, 0};
                    }
                }
            }

            // 2. Place light source B → Immediately delete B
            TileVector2D pointB = {interval, 0};
            lightingBuffer.SetLightSource(pointB, glimmer::Ground,
                                          std::make_unique<glimmer::LightSource>(pointB, radius, redColor));
            lightingBuffer.ClearLightSource(pointB, glimmer::Ground);
            int afterACount = 0;
            bool colorMatch = true;
            std::string errorMsg;

            for (int x = -radius; x <= radius; x++) {
                for (int y = -radius; y <= radius; y++) {
                    const glimmer::Color *currColor = lightingBuffer.GetFinalLightColor({x, y});
                    glimmer::Color oldColor = beforeColorMap[{x, y}];

                    if (currColor != nullptr && currColor->a > 0) {
                        afterACount++;
                    }
                    if (currColor == nullptr) {
                        // Current is null → original must also be null (all 0)
                        if (oldColor.a != 0 || oldColor.r != 0 || oldColor.g != 0 || oldColor.b != 0) {
                            colorMatch = false;
                            errorMsg = "Position(" + std::to_string(x) + "," + std::to_string(y) + ") Original has color, current is null";
                        }
                    } else {
                        // Current has color → must be exactly the same as the original
                        if (currColor->r != oldColor.r ||
                            currColor->g != oldColor.g ||
                            currColor->b != oldColor.b ||
                            currColor->a != oldColor.a) {
                            colorMatch = false;
                            errorMsg = "Position(" + std::to_string(x) + "," + std::to_string(y) + ") Color mismatch\n"
                                       "Original R:" + std::to_string(oldColor.r) + " G:" + std::to_string(oldColor.g) +
                                       " B:" + std::to_string(oldColor.b) + " A:" + std::to_string(oldColor.a) + "\n"
                                       "Current  R:" + std::to_string(currColor->r) + " G:" + std::to_string(currColor->g) +
                                       " B:" + std::to_string(currColor->b) + " A:" + std::to_string(currColor->a);
                        }
                    }
                }
            }

            // 4. Double assertion: equal count + all pixel colors match
            EXPECT_TRUE(beforeACount == afterACount)
            << "Radius " << radius << " Light count is not equal";
            EXPECT_TRUE(colorMatch)
            << "Radius " << radius << " Color value mismatch: " << errorMsg;

            // Clear light source A
            lightingBuffer.ClearLightSource(originPoint, glimmer::Ground);
        }
    }
}



int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
