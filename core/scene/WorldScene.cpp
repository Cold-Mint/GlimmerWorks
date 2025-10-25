//
// Created by Cold-Mint on 2025/10/25.
//

#include "WorldScene.h"

#include "../Constants.h"
#include "SDL3/SDL_oldnames.h"

bool Glimmer::WorldScene::HandleEvent(const SDL_Event &event) {
    return false;
}

void Glimmer::WorldScene::Update(float delta) {


}

void Glimmer::WorldScene::Render(SDL_Renderer *renderer) {
    // if (appContext->debugScreenCoords) {
    //     SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // 蓝色
    //
    //     // 四条边框线
    //     SDL_RenderDrawLine(renderer, 0, 0, screenWidth, 0); // 上
    //     SDL_RenderDrawLine(renderer, 0, 0, 0, screenHeight); // 左
    //     SDL_RenderDrawLine(renderer, 0, screenHeight-1, screenWidth, screenHeight-1); // 下
    //     SDL_RenderDrawLine(renderer, screenWidth-1, 0, screenWidth-1, screenHeight); // 右
    //
    //     // 四个角的屏幕坐标
    //     // 假设你有一个文本渲染函数 DrawText(renderer, x, y, text, color)
    //     DrawText(renderer, 5, 5, "0,0", {0,0,255,255}); // 左上
    //     DrawText(renderer, screenWidth-50, 5, std::to_string(screenWidth)+",0", {0,0,255,255}); // 右上
    //     DrawText(renderer, 5, screenHeight-20, "0,"+std::to_string(screenHeight), {0,0,255,255}); // 左下
    //     DrawText(renderer, screenWidth-50, screenHeight-20, std::to_string(screenWidth)+","+std::to_string(screenHeight), {0,0,255,255}); // 右下
    // }
}
