//
// Created by Cold-Mint on 2025/12/1.
//

#ifndef GLIMMERWORKS_ANDROIDCONTROLSYSTEM_H
#define GLIMMERWORKS_ANDROIDCONTROLSYSTEM_H
#ifdef __ANDROID__
#include "../GameSystem.h"
#include <map>
#include <memory>

namespace glimmer {
    class AndroidControlSystem : public GameSystem {
        std::shared_ptr<SDL_Texture> leftTexture = nullptr;
        std::shared_ptr<SDL_Texture> rightTexture = nullptr;
        std::shared_ptr<SDL_Texture> jumpTexture = nullptr;
        std::shared_ptr<SDL_Texture> leftPressedTexture = nullptr;
        std::shared_ptr<SDL_Texture> rightPressedTexture = nullptr;
        std::shared_ptr<SDL_Texture> jumpPressedTexture = nullptr;

        SDL_FRect leftRect{};
        SDL_FRect rightRect{};
        SDL_FRect jumpRect{};

        enum class ButtonType {
            None,
            Left,
            Right,
            Jump
        };

        std::map<SDL_FingerID, ButtonType> activeTouches;

    public:
        AndroidControlSystem(AppContext *appContext, WorldContext *worldContext);

        bool HandleEvent(const SDL_Event &event) override;

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        uint8_t GetRenderOrder() override;

        std::string GetName() override;
    };
}
#endif
#endif //GLIMMERWORKS_ANDROIDCONTROLSYSTEM_H
