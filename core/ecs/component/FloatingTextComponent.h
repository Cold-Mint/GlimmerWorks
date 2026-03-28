//
// Created by coldmint on 2026/3/28.
//

#ifndef GLIMMERWORKS_FLOATINGTEXTCOMPONENT_H
#define GLIMMERWORKS_FLOATINGTEXTCOMPONENT_H
#include "tween.h"
#include "core/ecs/GameComponent.h"


namespace glimmer {
    class FloatingTextComponent : public GameComponent {
        std::string text_;
        float alpha_ = 0.0F;
        tweeny::tween<float> tween_;
        uint64_t expireTime_ = 0;

    public:
        explicit FloatingTextComponent();

        void SetText(const std::string &text);

        tweeny::tween<float> &GetTween();

        [[nodiscard]] uint64_t GetExpireTime() const;


        void SetAlpha(float alpha);

        [[nodiscard]] float GetAlpha() const;

        [[nodiscard]] std::string &GetText();

        [[nodiscard]] uint32_t GetId() override;
    };
}


#endif //GLIMMERWORKS_FLOATINGTEXTCOMPONENT_H
