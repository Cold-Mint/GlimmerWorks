//
// Created by Cold-Mint on 2026/5/27.
//

#include "MaterialItem.h"

glimmer::MaterialItem::MaterialItem(std::string id, std::string name, std::optional<std::string> description,
                                    std::shared_ptr<SDL_Texture> icon,
                                    const ResourceRef& resourceRef) : id_(std::move(id)),
                                                                      name_(std::move(name)),
                                                                      description_(std::move(description)),
                                                                      icon_(std::move(icon))
{
    resourceRef_ = resourceRef;
}

std::unique_ptr<glimmer::MaterialItem> glimmer::MaterialItem::FromItemResource(const AppContext* appContext,
                                                                               const MaterialItemResource* itemResource,
                                                                               const ResourceRef& resourceRef)
{
    std::string name = Resource::GenerateId(itemResource->packId, itemResource->resourceId);
    const auto nameRes = appContext->GetResourceLocator()->FindString(&itemResource->name);
    if (nameRes != nullptr)
    {
        name = nameRes->value;
    }
    std::optional<std::string> description;
    auto descriptionRes = appContext->GetResourceLocator()->FindString(&itemResource->description);
    if (descriptionRes != nullptr)
    {
        description = descriptionRes->value;
    }
    return std::make_unique<MaterialItem>(Resource::GenerateId(*itemResource), name,
                                          description,
                                          appContext->GetResourceLocator()->FindTexture(
                                              &itemResource->texture), resourceRef);
}

const std::string& glimmer::MaterialItem::GetId() const
{
    return id_;
}

const std::string& glimmer::MaterialItem::GetName() const
{
    return name_;
}

const std::optional<std::string>& glimmer::MaterialItem::GetDescription() const
{
    return description_;
}

const glimmer::AbilityConfig* glimmer::MaterialItem::GetAbilityConfig() const
{
    return nullptr;
}

void glimmer::MaterialItem::Reduce(unsigned value)
{
}


SDL_Texture* glimmer::MaterialItem::GetIcon() const
{
    return icon_.get();
}

uint32_t glimmer::MaterialItem::GetMaxDurability() const
{
    return 0;
}

bool glimmer::MaterialItem::IsUnbreakable() const
{
    return true;
}

void glimmer::MaterialItem::OnUse(WorldContext* worldContext, GameEntity::ID user, const AbilityConfig* abilityConfig,
                                  std::unordered_set<std::string>& popupAbility)
{
}


std::unique_ptr<glimmer::Item> glimmer::MaterialItem::Clone() const
{
    return std::make_unique<MaterialItem>(*this);
}
