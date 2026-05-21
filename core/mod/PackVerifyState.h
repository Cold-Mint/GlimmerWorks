//
// Created by coldmint on 2026/5/20.
//
#pragma once
#include <cstdint>

namespace glimmer {
    enum class PackVerifyState : uint8_t {
        Unsigned,
        VerifiedSuccess,
        VerifiedFailed
    };
}
