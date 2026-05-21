//
// Created by coldmint on 2026/5/16.
//

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
