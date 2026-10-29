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
#pragma once
#include <string>

#include "EcsTypes.h"
#include "component/AreaMarkerComponent.h"
#include "component/BlueprintComponent.h"
#include "component/CameraComponent.h"
#include "component/MiningComponent.h"
#include "component/ItemContainerComponent.h"
#include "component/ItemToolTipComponent.h"
#include "component/RecipeSelectionComponent.h"
#include "component/Transform2DComponent.h"
#include "core/config/Constants.h"

namespace glimmer {
    class ItemContainer;

    class EntityShortCut {
        CameraComponent *cameraComponent_ = nullptr;
        Transform2DComponent *cameraTransform2DComponent_ = nullptr;
        BlueprintComponent *blueprintComponent_ = nullptr;
        ItemContainerComponent *itemContainerComponent_ = nullptr;
        MiningComponent *miningComponent_ = nullptr;
        AreaMarkerComponent *areaMarkerComponent_ = nullptr;
        ItemToolTipComponent *itemToolTipComponent_ = nullptr;
        RecipeSelectionComponent *recipeSelectionComponent_ = nullptr;
        ItemContainer *openedStorageContainer_ = nullptr;
        //The uuid of the currently opened loot box (empty = not a loot box).
        //当前打开的战利品箱的uuid（空表示不是战利品箱）。
        std::string openedLootBoxUuid_;
        GameEntityID player_ = GAME_ENTITY_ID_INVALID;

    public:
        void SetPlayer(GameEntityID player);

        [[nodiscard]] GameEntityID GetPlayer() const;

        [[nodiscard]] ItemToolTipComponent *GetItemToolTipComponent() const;

        void SetItemToolTipComponent(ItemToolTipComponent *itemToolTipComponent);

        void SetMiningComponent(MiningComponent *miningComponent);

        [[nodiscard]] MiningComponent *GetMiningComponent() const;

        void SetBlueprintComponent(BlueprintComponent *blueprintComponent);

        [[nodiscard]] BlueprintComponent *GetBlueprintComponent() const;

        void SetItemContainerComponent(ItemContainerComponent *itemContainerComponent);

        [[nodiscard]] ItemContainerComponent *GetItemContainerComponent() const;

        void SetCameraComponent(CameraComponent *cameraComponent);

        void SetAreaMarkerComponent(AreaMarkerComponent *areaMarkerComponent);

        [[nodiscard]] AreaMarkerComponent *GetAreaMarkerComponent() const;

        [[nodiscard]] CameraComponent *GetCameraComponent() const;

        void SetCameraTransform2DComponent(Transform2DComponent *cameraTransform2DComponent);

        [[nodiscard]] Transform2DComponent *GetCameraTransform2DComponent() const;

        void SetRecipeSelectionComponent(RecipeSelectionComponent *recipeSelectionComponent);

        [[nodiscard]] RecipeSelectionComponent *GetRecipeSelectionComponent() const;

        /**
         * Set the currently opened storage container (e.g. a chest block).
         * 设置当前打开的存储容器（例如箱子方块）。
         * @param container container 容器指针
         */
        void SetOpenedStorageContainer(ItemContainer *container);

        /**
         * Get the currently opened storage container.
         * 获取当前打开的存储容器。
         * @return 容器指针，未打开返回 nullptr
         */
        [[nodiscard]] ItemContainer *GetOpenedStorageContainer() const;

        /**
         * Set the uuid of the currently opened loot box.
         * 设置当前打开的战利品箱的uuid。
         * @param uuid uuid 箱子uuid，空字符串表示不是战利品箱
         */
        void SetOpenedLootBoxUuid(const std::string &uuid);

        /**
         * Get the uuid of the currently opened loot box.
         * 获取当前打开的战利品箱的uuid。
         * @return 箱子uuid，空字符串表示不是战利品箱
         */
        [[nodiscard]] const std::string &GetOpenedLootBoxUuid() const;
    };
}
