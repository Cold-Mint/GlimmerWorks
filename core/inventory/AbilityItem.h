//
// Created by coldmint on 2025/12/28.
//

#ifndef GLIMMERWORKS_ABILITYITEM_H
#define GLIMMERWORKS_ABILITYITEM_H
#include "Item.h"

#include <utility>

#include "ComposableItem.h"

namespace glimmer {
    class AbilityItem : public Item {
        std::string id_;
        std::string name_;
        std::string description_;
        std::shared_ptr<SDL_Texture> icon_;

    public:
        explicit AbilityItem(std::string id, std::string name, std::string description,
                             std::shared_ptr<SDL_Texture> icon) : id_(std::move(id)), name_(std::move(name)),
                                                                  description_(std::move(description)),
                                                                  icon_(std::move(icon)) {
        }

        [[nodiscard]] std::string GetId() const override;

        [[nodiscard]] std::string GetName() const override;

        [[nodiscard]] std::string GetDescription() const override;

        [[nodiscard]] std::shared_ptr<SDL_Texture> GetIcon() const override;

        static std::unique_ptr<AbilityItem> FromItemResource(AppContext *appContext,
                                                             const AbilityItemResource *itemResource) {
            const auto nameRes = appContext->GetResourceLocator()->FindString(itemResource->name);
            if (!nameRes.has_value()) {
                LogCat::e("An error occurred when constructing ability items, and the name is empty.");
                return nullptr;
            }
            auto descriptionRes = appContext->GetResourceLocator()->FindString(itemResource->description);
            if (!descriptionRes.has_value()) {
                LogCat::e("An error occurred when constructing ability items, and the description is empty.");
                return nullptr;
            }
            auto texture = appContext->GetResourcePackManager()->LoadTextureFromFile(appContext, itemResource->texture);
            if (texture == nullptr) {
                LogCat::e("An error occurred when constructing ability items, and the texture is empty.");
                return nullptr;
            }
            return std::make_unique<AbilityItem>(Resource::GenerateId(*itemResource), nameRes.value()->value,
                                                 descriptionRes.value()->value, texture);
        }

        void OnUse(AppContext *appContext, WorldContext *worldContext, GameEntity *user) override;

        void OnDrop() override;
    };
}

#endif //GLIMMERWORKS_ABILITYITEM_H
