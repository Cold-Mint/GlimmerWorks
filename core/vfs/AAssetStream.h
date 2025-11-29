//
// Created by Cold-Mint on 2025/11/29.
//

#ifndef ASSETSCPP_AASSETSTREAM_H
#define ASSETSCPP_AASSETSTREAM_H

#include <streambuf>
#include "AAssetStreamBuf.h"
#include <istream>

namespace glimmer {
    /**
     * AAssetStream
     * 安卓资源流
     */
    class AAssetStream : public std::istream {
        AAssetStreamBuf buf_;

    public:
        explicit AAssetStream(AAsset *asset) : std::istream(&buf_), buf_(asset) {
        }
    };
}

#endif //ASSETSCPP_AASSETSTREAM_H
