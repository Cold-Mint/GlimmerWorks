//
// Created by Cold-Mint on 2025/12/29.
//

#include "DiggingComponent.h"

#include "core/Constants.h"

bool glimmer::DiggingComponent::IsEnable() const {
    return enable_;
}

void glimmer::DiggingComponent::SetEnable(const bool enable) {
    enable_ = enable;
}

void glimmer::DiggingComponent::SetMiningRangeData(MiningRangeData *miningRangeData) {
    miningRangeData_ = miningRangeData;
}

const glimmer::MiningRangeData * glimmer::DiggingComponent::GetMiningRangeData() const {
    return miningRangeData_;
}

void glimmer::DiggingComponent::SetStartPosition(WorldVector2D startPosition) {
    startPosition_ = startPosition;
}

const WorldVector2D &glimmer::DiggingComponent::GetStartPosition() const {
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

uint32_t glimmer::DiggingComponent::GetId() {
    return COMPONENT_ID_DIGGING;
}
