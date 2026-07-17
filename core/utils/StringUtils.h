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
#include <string>
#include <cstdint>
#include <optional>
#include <span>

namespace glimmer
{
    class StringUtils
    {
    public:
        /**
         * Convert any UTF-8 string to a cross-platform secure archive directory name (in English + numbers)
         * 将任意 UTF-8 字符串 转换为 跨平台安全的存档目录名（英文+数字）
         * @param utf8Str
         * @return
         */
        static std::string ToSafeSaveName(const std::string& utf8Str);


        /**
         * String to hash value
         * 字符串转哈希值
         * @param string string 字符串
         * @return Return the fixed hash value. 返回固定的哈希值。
         */
        static uint64_t StringToUint64Blake3(const std::string& string);

        /**
         * Check if the input string is an integer
         * 输入字符串判断是否为整数
         * @param str
         * @return
         */
        static bool IsInteger(const std::string& str);

        /**
         * String to hash value
         * 字符串转哈希值
         *
         * This function is faster than StringToUint64Blake3, but after the game restarts, the hash value generated from the same input will be different from the previous one. It has random perturbations.
         * 此函数比StringToUint64Blake3更快，但是游戏重启后，相同的输入产生的哈希值和上次会有所不同。带有随机扰动。
         * @param string
         * @return
         */
        static uint64_t StringToUint64(const std::string& string);

        /**
         * String to byte data
         * 字符串转字节数据
         * @param string
         * @return
         */
        static std::span<const std::byte> StringToByteData(std::string_view string);

        /**
         * MakeRawText
         * 生成原始文本
         * @param string 文本
         * @return 原始文本（不被引擎翻译的）
         */
        static std::string MakeRawText(std::string_view string);

        /**
         * Write the byte stream to a string
         * 将字节流写入到字符串
         * @param stream
         * @return
         */
        static std::optional<std::string> StreamToString(const std::istream* stream);

        static void ReplaceAll(std::string& str, std::string_view from, std::string_view to);
    };
}
