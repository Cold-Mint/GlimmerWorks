//
// Created by coldmint on 2026/5/16.
//

#pragma once
#include <cstdint>

namespace glimmer {
    /**
     * Objects that can generate signatures
     * 可生成签名的对象
     */
    class ISignable {
    public:
        virtual ~ISignable() = default;

        /**
         * Obtain the object signature. When the member variable changes, the signature will also change.
         * 获取对象签名，当成员变量改变时，签名也会发生变化。
         * The same member variables will have the same signature.
         * 相同的成员变量会获得相同的签名。
         * @return
         */
        [[nodiscard]] virtual uint64_t GetSignature() const = 0;
    };
}
