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
#include <filesystem>
#include <optional>
#include <vector>

#include "core/mod/PackVerifyState.h"
#include "SpecialFileProcessingParams.h"

namespace glimmer {
    class VirtualFileSystem;

    /**
     * PackSignatureVerifier
     * 数据包签名校验器
     * Handles public key / signature file loading, file hashing (Blake3) and Ed25519 verification.
     * 负责公钥/签名文件加载、文件哈希（Blake3）与 Ed25519 验签。
     */
    class PackSignatureVerifier {
        const VirtualFileSystem *virtualFileSystem_;
        SpecialFileProcessingParams &params_;

        /**
         * ProcessPublicKeyFile
         * 处理公钥文件
         * @param path path 公钥文件路径
         * @return Whether the file was consumed 是否处理了该文件
         */
        bool ProcessPublicKeyFile(const std::filesystem::path &path) const;

        /**
         * ProcessSignatureFile
         * 处理签名文件
         * @param path path 签名文件路径
         * @return Whether the file was consumed 是否处理了该文件
         */
        bool ProcessSignatureFile(const std::filesystem::path &path) const;

        /**
         * ComputeFileHash
         * 计算文件哈希
         * @param fileBuffer fileBuffer 文件内容
         * @param allHashData allHashData 追加哈希数据的目标缓冲区
         */
        static void ComputeFileHash(const std::vector<char> &fileBuffer, std::vector<uint8_t> &allHashData);

        /**
         * VerifySignature
         * 校验签名
         * @param findPublicKey findPublicKey 是否找到公钥
         * @param findSignature findSignature 是否找到签名
         * @param publicKey publicKey 公钥
         * @param signature signature 签名
         * @param allHashData allHashData 全部哈希数据
         * @return The verification result 校验结果
         */
        static PackVerifyState VerifySignature(bool findPublicKey, bool findSignature,
                                               const std::vector<uint8_t> &publicKey,
                                               const std::vector<uint8_t> &signature,
                                               const std::vector<uint8_t> &allHashData);

    public:
        /**
         * PackSignatureVerifier
         * 签名校验器构造
         * @param virtualFileSystem virtualFileSystem 虚拟文件系统
         * @param params params 特殊文件处理参数（公钥/签名等）
         */
        PackSignatureVerifier(const VirtualFileSystem *virtualFileSystem, SpecialFileProcessingParams &params);

        /**
         * IsEnabled
         * 是否启用签名校验
         * @return True if signature verification is enabled 是否启用签名校验
         */
        [[nodiscard]] bool IsEnabled() const;

        /**
         * ProcessSpecialFiles
         * 处理特殊文件（公钥/签名）
         * @param path path 当前文件路径
         * @return True if the file was a special file 该文件是否为特殊文件
         */
        bool ProcessSpecialFiles(const std::filesystem::path &path) const;

        /**
         * ComputeAndAppendFileHash
         * 在启用签名校验时计算并追加文件哈希
         * @param fileBuffer fileBuffer 文件内容
         * @param allHashData allHashData 追加哈希数据的目标缓冲区
         */
        void ComputeAndAppendFileHash(const std::vector<char> &fileBuffer, std::vector<uint8_t> &allHashData) const;

        /**
         * Verify
         * 执行整体签名校验
         * @param allHashData allHashData 全部哈希数据
         * @return The verification result 校验结果
         */
        [[nodiscard]] PackVerifyState Verify(const std::vector<uint8_t> &allHashData) const;
    };
}
