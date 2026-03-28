#include "FloatingTextComponent.h"

#include "tweeny.h"
#include "core/Constants.h"
#include "SDL3/SDL_timer.h"

glimmer::FloatingTextComponent::FloatingTextComponent() {
    expireTime_ = SDL_GetTicks() + 25000;
    tween_ = tweeny::tween(tweeny::from(0.0f)
        .to(1.0f).during(200)
        .to(1.0f).during(2000)
        .to(0.0f).during(300));
}

void glimmer::FloatingTextComponent::SetText(const std::string &text) {
    text_ = text;
}

tweeny::tween<float> &glimmer::FloatingTextComponent::GetTween() {
    return tween_;
}

uint64_t glimmer::FloatingTextComponent::GetExpireTime() const {
    return expireTime_;
}

void glimmer::FloatingTextComponent::SetAlpha(float alpha) {
    alpha_ = alpha;
}

float glimmer::FloatingTextComponent::GetAlpha() const {
    return alpha_;
}

std::string &glimmer::FloatingTextComponent::GetText() {
    return text_;
}

uint32_t glimmer::FloatingTextComponent::GetId() {
    return COMPONENT_ID_FLOATING_TEXT;
}
