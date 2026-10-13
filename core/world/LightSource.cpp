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
#include "LightSource.h"

#include <algorithm>
#include <cmath>


glimmer::LightDirection glimmer::LightSource::GetDirection() const {
    return direction_;
}

uint8_t glimmer::LightSource::GetMaxRadius() const {
    return maxRadius_;
}

void glimmer::LightSource::SetLightDirection(const LightDirection direction) {
    direction_ = direction;
}

void glimmer::LightSource::SetMaxRadius(const uint8_t maxRadius) {
    maxRadius_ = maxRadius;
}

const glimmer::TileVector2D &glimmer::LightSource::GetCenter() const {
    return center_;
}

void glimmer::LightSource::SetCenter(const TileVector2D &center) {
    center_ = center;
}

const glimmer::Color &glimmer::LightSource::GetEmissionColor() const {
    return emissionColor_;
}

void glimmer::LightSource::SetEmissionColor(const Color &emissionColor) {
    emissionColor_ = emissionColor;
}

glimmer::LightAttenuation glimmer::LightSource::GetAttenuation() const {
    return attenuation_;
}

void glimmer::LightSource::SetAttenuation(const LightAttenuation attenuation) {
    attenuation_ = attenuation;
}

bool glimmer::LightSource::operator==(const LightSource &other) const {
    return center_ == other.center_
           && maxRadius_ == other.maxRadius_
           && attenuation_ == other.attenuation_
           && direction_ == other.direction_
           && emissionColor_.GetFingerprint() == other.emissionColor_.GetFingerprint();
}


float glimmer::LightSource::GetAttenuationFactor(const int dx, const int dy) const {
    if (maxRadius_ <= 0) {
        return 0.0F;
    }
    const float distance = std::sqrt(static_cast<float>(dx * dx + dy * dy));
    switch (attenuation_) {
        case LightAttenuation::InverseSquare: {
            const float normalized = distance / static_cast<float>(maxRadius_);
            return std::clamp(1.0F / (1.0F + normalized * normalized * 4.0F), 0.0F, 1.0F);
        }
        case LightAttenuation::Linear:
        default: {
            return std::clamp(1.0F - distance / static_cast<float>(maxRadius_), 0.0F, 1.0F);
        }
    }
}
