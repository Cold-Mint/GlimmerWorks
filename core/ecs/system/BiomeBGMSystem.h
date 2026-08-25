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
#include "core/ecs/GameSystem.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/mod/Resource.h"
#include "core/mod/ResourceLocator.h"
#include "core/mod/resourcePack/AudioManager.h"

namespace glimmer {
    class BiomeBGMSystem final : public GameSystem {
        // Fallback debounce used only when the runtime config is unavailable.
        // The authoritative value lives in config.toml ([biome_bgm].debounce_seconds).
        // 仅在运行时配置不可用时使用的防抖回退值。
        // 权威值位于 config.toml（[biome_bgm].debounce_seconds）。
        static constexpr float kDefaultBiomeBGMDebounceSeconds = 3.0F;

        BiomeResource *biomeResource_ = nullptr;
        // Candidate biome the player is lingering in; committed once the debounce timer elapses.
        // 玩家正在停留的候选生物群系；防抖计时结束后正式切换。
        BiomeResource *candidateBiomeResource_ = nullptr;
        float candidateTimeAccumulator_ = 0.0F;
        std::shared_ptr<AudioResourceResult> audioResult_ = nullptr;
        Transform2DComponent *playerTransform2DComponent_ = nullptr;
        AudioManager *audioManager_ = nullptr;
        ResourceLocator *resourceLocator_ = nullptr;

        // Loads the biome's BGM, starts playback, pops a toast, and commits biomeResource_.
        // 加载生物群系 BGM、开始播放、弹出弹幕并提交 biomeResource_。
        void SwitchToBiome(BiomeResource *biomeResource);

    public:
        explicit BiomeBGMSystem(WorldContext *worldContext);

        void OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) override;

        void Update(float delta) override;

        [[nodiscard]] GameSystemType GetGameSystemType() const override;
    };
}
