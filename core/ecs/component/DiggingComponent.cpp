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
#include "DiggingComponent.h"

#include <utility>

bool glimmer::DiggingComponent::IsEnable() const {
    return enable_;
}

void glimmer::DiggingComponent::SetEnable(const bool enable) {
    enable_ = enable;
}

void glimmer::DiggingComponent::SetMiningRangeData(MiningRangeData *miningRangeData) {
    miningRangeData_ = miningRangeData;
}

const glimmer::MiningRangeData *glimmer::DiggingComponent::GetMiningRangeData() const {
    return miningRangeData_;
}

void glimmer::DiggingComponent::SetStartPosition(TileVector2D startPosition) {
    startPosition_ = std::move(startPosition);
}

const glimmer::TileVector2D &glimmer::DiggingComponent::GetStartPosition() const {
    return startPosition_;
}

float glimmer::DiggingComponent::GetProgress() const {
    return progress_;
}

void glimmer::DiggingComponent::SetLayerType(const TileLayerType tileLayerType) {
    layerType_ = tileLayerType;
}

glimmer::TileLayerType glimmer::DiggingComponent::GetLayerType() const {
    return layerType_;
}

void glimmer::DiggingComponent::SetChainMiningRadius(const int chainMiningRadius) {
    chainMiningRadius_ = chainMiningRadius;
}

int glimmer::DiggingComponent::GetChainMiningRadius() const {
    return chainMiningRadius_;
}

void glimmer::DiggingComponent::SetProgress(const float progress) {
    progress_ = progress;
}

void glimmer::DiggingComponent::SetPrecisionMining(bool precisionMining) {
    precisionMining_ = precisionMining;
}

bool glimmer::DiggingComponent::IsPrecisionMining() const {
    return precisionMining_;
}

void glimmer::DiggingComponent::AddProgress(float progress) {
    progress_ += progress;
}

void glimmer::DiggingComponent::SetEfficiency(float efficiency) {
    efficiency_ = efficiency;
}

float glimmer::DiggingComponent::GetEfficiency() const {
    return efficiency_;
}

void glimmer::DiggingComponent::MarkActive() {
    activeSignal_ = true;
}

bool glimmer::DiggingComponent::CheckAndResetActive() {
    const bool active = activeSignal_;
    activeSignal_ = false;
    return active;
}

GameComponentTypeMessage glimmer::DiggingComponent::GetComponentType() {
    return COMPONENT_DIGGING;
}
