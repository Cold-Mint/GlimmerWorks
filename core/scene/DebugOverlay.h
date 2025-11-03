//
// Created by Cold-Mint on 2025/10/25.
//

#ifndef GLIMMERWORKS_DEBUGSCENE_H
#define GLIMMERWORKS_DEBUGSCENE_H
#include "Scene.h"

namespace glimmer
{
    /**
     * Debug the overlay layer
     * 调试叠加层
     * It can be displayed on any scene. Display frps, screen coordinates and other information.
     * 可以显示在任意场景上。显示frps，屏幕坐标等信息。
     */
    class DebugOverlay : public Scene
    {
        bool show_ = false;
        float fps_ = 0.0f;
        float frameTimeMs_ = 0.0f;
        float fpsAccumTime_ = 0.0f;
        int fpsFrameCount_ = 0;

    public:
        explicit DebugOverlay(AppContext* context)
            : Scene(context)
        {
        }

        bool HandleEvent(const SDL_Event& event) override;

        void Update(float delta) override;

        void Render(SDL_Renderer* renderer) override;

        ~DebugOverlay() override = default;
    };
}


#endif //GLIMMERWORKS_DEBUGSCENE_H
