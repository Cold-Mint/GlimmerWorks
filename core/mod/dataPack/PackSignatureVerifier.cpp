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
#include "PackSignatureVerifier.h"

#include "blake3.h"
#include "monocypher-ed25519.h"
#include "core/log/LogCat.h"
#include "core/vfs/VirtualFileSystem.h"

using enum glimmer::PackVerifyState;

glimmer::PackSignatureVerifier::PackSignatureVerifier(const VirtualFileSystem *virtualFileSystem,
                                                      SpecialFileProcessingParams &params)
    : virtualFileSystem_(virtualFileSystem), params_(params) {
}

bool glimmer::PackSignatureVerifier::IsEnabled() const {
    return params_.enableSignVerify;
}

bool glimmer::PackSignatureVerifier::ProcessPublicKeyFile(const std::filesystem::path &path) const {
    const auto publicKeyStreamUniquePtr = virtualFileSystem_->ReadFileAsStream(path);
    if (publicKeyStreamUniquePtr == nullptr) {
        LogCat::w(std::source_location::current(), "data_pack_public_key_read_failed",
                  "Failed to read public key file: {}", path.string());
        return false;
    }
    const auto publicKeyStream = publicKeyStreamUniquePtr.get();
    if (publicKeyStream == nullptr) {
        return false;
    }
    auto &pubStream = *publicKeyStream;
    pubStream.read(reinterpret_cast<char *>(params_.publicKey.data()), 32);
    if (pubStream.gcount() == 32) {
        params_.findPublicKey = true;
        LogCat::d("data_pack_public_key_loaded", "Loaded public key file: {}", path.string());
    }
    return true;
}

bool glimmer::PackSignatureVerifier::ProcessSignatureFile(const std::filesystem::path &path) const {
    const auto signStreamUniquePtr = virtualFileSystem_->ReadFileAsStream(path);
    if (signStreamUniquePtr == nullptr) {
        LogCat::w(std::source_location::current(), "data_pack_signature_read_failed",
                  "Failed to read signature file: {}", path.string());
        return false;
    }
    const auto signStream = signStreamUniquePtr.get();
    if (signStream == nullptr) {
        return false;
    }
    auto &sigStream = *signStream;
    sigStream.read(reinterpret_cast<char *>(params_.signature.data()), 64);
    if (sigStream.gcount() == 64) {
        params_.findSignature = true;
        LogCat::d("data_pack_signature_loaded", "Loaded signature file: {}", path.string());
    }
    return true;
}

void glimmer::PackSignatureVerifier::ComputeFileHash(const std::vector<char> &fileBuffer,
                                                     std::vector<uint8_t> &allHashData) {
    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    blake3_hasher_update(&hasher, fileBuffer.data(), fileBuffer.size());
    uint8_t singleHash[BLAKE3_OUT_LEN];
    blake3_hasher_finalize(&hasher, singleHash, BLAKE3_OUT_LEN);
    allHashData.insert(allHashData.end(), singleHash, singleHash + BLAKE3_OUT_LEN);
}

glimmer::PackVerifyState glimmer::PackSignatureVerifier::VerifySignature(const bool findPublicKey,
                                                                         const bool findSignature,
                                                                         const std::vector<uint8_t> &publicKey,
                                                                         const std::vector<uint8_t> &signature,
                                                                         const std::vector<uint8_t> &allHashData) {
    if (!findPublicKey || !findSignature) {
        LogCat::d("data_pack_signature_missing", "Signature verification skipped: missing public key or signature");
        return VerifiedFailed;
    }
    if (crypto_ed25519_check(signature.data(), publicKey.data(),
                             allHashData.data(), allHashData.size()) == 0) {
        LogCat::i("data_pack_signature_verified", "Data pack signature verified successfully");
        return VerifiedSuccess;
    }
    LogCat::w(std::source_location::current(), "data_pack_signature_invalid",
              "Data pack signature verification failed");
    return VerifiedFailed;
}

bool glimmer::PackSignatureVerifier::ProcessSpecialFiles(const std::filesystem::path &path) const {
    if (!params_.enableSignVerify) {
        return false;
    }
    if (!params_.findPublicKey && path == params_.publicPath) {
        return ProcessPublicKeyFile(path);
    }
    if (!params_.findSignature && path == params_.signPath) {
        return ProcessSignatureFile(path);
    }
    return false;
}

void glimmer::PackSignatureVerifier::ComputeAndAppendFileHash(const std::vector<char> &fileBuffer,
                                                              std::vector<uint8_t> &allHashData) const {
    if (params_.enableSignVerify) {
        ComputeFileHash(fileBuffer, allHashData);
    }
}

glimmer::PackVerifyState glimmer::PackSignatureVerifier::Verify(const std::vector<uint8_t> &allHashData) const {
    return VerifySignature(params_.findPublicKey, params_.findSignature,
                           params_.publicKey, params_.signature, allHashData);
}
