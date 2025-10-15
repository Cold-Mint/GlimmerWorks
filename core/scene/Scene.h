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
    protected:
        AppContext *appContext = nullptr;

    public:
        //Processing events (input, window messages, etc.) returns whether to intercept the event. If it is true, it will not be passed down.
        //处理事件（输入、窗口消息等）返回是否拦截事件，如果为true则不会向下传递。
        virtual bool HandleEvent(const SDL_Event &event) = 0;

        // Logical update (physical frame) - Fixed time step call
        // 逻辑更新（物理帧） - 固定时间步长调用
        virtual void Update(float delta) = 0;

        // Render frame - Each loop call
        // 渲染帧 - 每次循环调用
        virtual void Render(SDL_Renderer *renderer) = 0;


        virtual ~Scene() = default;

        explicit Scene(AppContext *context) : appContext(context) {
        }
    };
}

#endif //SCENE_H
