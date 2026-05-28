/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
检测光照破坏后是否有残留。
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
