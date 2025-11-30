//
// Created by Cold-Mint on 2025/11/29.
//

#ifndef ASSETSCPP_AASSETSTREAMBUF_H
#define ASSETSCPP_AASSETSTREAMBUF_H

#ifdef __ANDROID__
#include <streambuf>

#include <android/asset_manager.h>

namespace glimmer {
    /**
     * AAssetStreamBuf
     * Android 资源流缓冲区
     */
    class AAssetStreamBuf : public std::streambuf {
        AAsset *asset_;
        char buffer_[1024]{};

    public:
        explicit AAssetStreamBuf(AAsset *asset) : asset_(asset) {
        }

        ~AAssetStreamBuf() override {
            if (asset_) {
                AAsset_close(asset_);
            }
        }

    protected:
        /**
        * Read the next character block from AAsset
        * 从 AAsset 中读取下一个字符块
        *
         * When the input sequence is empty, underflow() will be called to fill the buffer.
        * 当输入序列为空时，underflow() 会被调用以填充缓冲区。
        * This implementation will read the maximum number of bytes of buffer_ from Android AAsset.
        * 本实现会从 Android AAsset 中读取最多 buffer_ 的字节数，
        * And set the input sequence through setg().
        * 并通过 setg() 设置输入序列。
        *
        * @return Return the int conversion value of the current character; If EOF has been reached, return traits_type::eof(). 返回当前字符的 int 转换值；如果已到达 EOF 则返回 traits_type::eof()。
        */
        int_type underflow() override {
            if (gptr() < egptr()) {
                return traits_type::to_int_type(*gptr());
            }

            // Read from AAsset
            // 从 AAsset 读取
            int bytesRead = AAsset_read(asset_, buffer_, sizeof(buffer_));
            if (bytesRead <= 0) {
                return traits_type::eof();
            }

            // Set up a new input area 设置新的输入区
            setg(buffer_, buffer_, buffer_ + bytesRead);

            return traits_type::to_int_type(*gptr());
        }

        /**
        * @brief 重写自 std::streambuf，用于在流中相对当前位置、开头或结尾移动读取位置。
        *
        * 此方法根据 off（偏移量）与 dir（起始位置）计算目标位置，然后调用
        * AAsset_seek() 在 Android 资产中调整读取指针。
        *
        * @param off  相对位移量，可以为正或负。
        * @param dir  起始点，std::ios_base::beg / cur / end。
        * @param which  操作模式，通常包含 std::ios_base::in。
        *
        * @return 新的位置（从流开头算起），失败时返回 pos_type(-1)。
        *
        * @note
        * - AAsset 对压缩资源可能不支持 SEEK_CUR 或 SEEK_END，因此此方法不一定总是成功。
        * - 成功 seek 后会重置缓冲区（setg(nullptr, nullptr, nullptr)），以便下一次读取会触发 underflow()。
        * - 仅在输入模式（which & std::ios_base::in）下支持 seek。
        */
        pos_type
        seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which) override {
            if (!(which & std::ios_base::in))
                return {off_type(-1)};

            off_type basePos = 0;

            switch (dir) {
                case std::ios_base::beg:
                    basePos = 0;
                    break;

                case std::ios_base::cur:
                    basePos = AAsset_getLength(asset_) - AAsset_getRemainingLength(asset_);
                    break;

                case std::ios_base::end:
                    basePos = AAsset_getLength(asset_);
                    break;

                default:
                    return {off_type(-1)};
            }

            off_type newPos = basePos + off;

            if (newPos < 0)
                return {off_type(-1)};

            // 尝试 seek — 使用 SEEK_SET 绝对定位
            off_type ret = AAsset_seek(asset_, newPos, SEEK_SET);
            if (ret < 0)
                return {off_type(-1)};

            // 清空缓冲区，等待重新 underflow()
            setg(buffer_, buffer_, buffer_);

            return {ret};
        }

        /**
        * @brief 重写自 std::streambuf，用于将读取位置移动到绝对位置 pos。
        *
        * 此方法直接调用 AAsset_seek() 来设置资产文件的读取偏移。
        *
        * @param pos   从文件开头计算的绝对偏移位置。
        * @param which 操作模式，通常包含 std::ios_base::in。
        *
        * @return 成功时返回新的位置，失败时返回 pos_type(-1)。
        *
        * @note
        * - 成功 seek 后将清空缓冲区，使下一次读取触发 underflow() 来重新填充数据。
        * - 对压缩的 AAsset 文件可能无法支持任意位置 seek。
        */
        pos_type seekpos(pos_type pos, std::ios_base::openmode which) override {
            if (!(which & std::ios_base::in))
                return {off_type(-1)};

            // 将 fpos<mbstate_t> 转换为字节偏移
            off_type bytePos = pos.operator off_type();

            if (bytePos < 0)
                return {off_type(-1)};

            // 尝试绝对定位
            off_type ret = AAsset_seek(asset_, bytePos, SEEK_SET);
            if (ret < 0)
                return {off_type(-1)};

            // 清空缓冲区，等待重新 underflow()
            setg(buffer_, buffer_, buffer_);

            return {ret};
        }
    };
}
#endif
#endif //ASSETSCPP_AASSETSTREAMBUF_H
