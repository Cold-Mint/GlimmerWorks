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
#include <cstdint>

namespace glimmer {
    /**
      * Objects that can generate unique fingerprints
      * 可生成唯一指纹的对象
      */
    class IFingerprintable {
    public:
        virtual ~IFingerprintable() = default;

        /**
         * Get the object fingerprint. The fingerprint will change when any member variable is modified.
         * 获取对象指纹，当成员变量发生改变时，指纹也会随之改变。
         * Objects with identical member variables will produce the same fingerprint.
         * 成员变量完全相同时，生成的指纹保持一致。
         * @return 64-bit unique fingerprint value
         */
        [[nodiscard]] virtual uint64_t GetFingerprint() const = 0;
    };
}
