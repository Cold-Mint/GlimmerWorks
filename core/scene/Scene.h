//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef SCENE_H
#define SCENE_H
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>


namespace Glimmer {
    class AppContext;

    class Scene {
        AppContext *appContext = nullptr;

        //Handle events (inputs, window messages, etc.)
        //处理事件（输入、窗口消息等）
        virtual void HandleEvent(const SDL_Event &event) = 0;

        // Logical update (physical frame) - Fixed time step call
        // 逻辑更新（物理帧） - 固定时间步长调用
        virtual void Update(SDL_Window *window, Uint64 deltaTime) = 0;

        // Render frame - Each loop call
        // 渲染帧 - 每次循环调用
        virtual void Render(SDL_Window *window, SDL_Renderer *renderer) = 0;

    public:
        virtual ~Scene() = default;

        explicit Scene(AppContext *context) : appContext(context) {
        }
    };
}

#endif //SCENE_H
