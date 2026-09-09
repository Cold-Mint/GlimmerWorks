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
#include "DataPack.h"

#include <algorithm>
#include <utility>

#include "LanguageFileProcessor.h"
#include "PackSignatureVerifier.h"
#include "SpecialFileProcessingParams.h"
#include "core/config/Config.h"
#include "core/config/Constants.h"
#include "core/context/AppContext.h"
#include "core/context/GraphicsContext.h"
#include "core/context/ModContext.h"
#include "core/utils/StringUtils.h"
#include "core/utils/TomlUtils.h"
#include "toml11/parser.hpp"

std::optional<std::vector<char> > glimmer::DataPack::ReadFileContent(std::istream *stream) {
    if (stream->fail()) {
        return std::nullopt;
    }

    constexpr std::streamsize BUFFER_SIZE = 8192;
    std::vector<char> fileBuffer;
    std::vector<char> tempBuf(BUFFER_SIZE);

    while (stream->read(tempBuf.data(), BUFFER_SIZE) || stream->gcount() > 0) {
        fileBuffer.insert(fileBuffer.end(), tempBuf.data(), tempBuf.data() + stream->gcount());
    }
    return fileBuffer;
}

glimmer::DataPack::DataPack(std::filesystem::path path, const VirtualFileSystem *virtualFileSystem,
                            const TomlTemplateExpander *tomlTemplateExpander, const toml::spec &tomlVersion)
    : rootPath_(std::move(path)),
      manifest_(),
      tomlVersion_(tomlVersion),
      virtualFileSystem_(virtualFileSystem),
      tomlTemplateExpander_(tomlTemplateExpander),
      packVerifyState_(PackVerifyState::Unsigned),
      resourceFileLoader_(rootPath_, &manifest_, virtualFileSystem_, tomlTemplateExpander_, tomlVersion_) {
}

uint64_t glimmer::DataPack::GetUniqueId() const {
    return StringUtils::StringToUint64(manifest_.id);
}

bool glimmer::DataPack::LoadManifest() {
    const auto contentOptional = virtualFileSystem_->ReadFileAsString(rootPath_ / MANIFEST_FILE_NAME);
    if (!contentOptional.has_value()) {
        return false;
    }
    const toml::value value = toml::parse_str(contentOptional.value(), tomlVersion_);
    manifest_ = toml::get<DataPackManifest>(value);
    manifest_.name.SetSelfPackageId(manifest_.id);
    manifest_.description.SetSelfPackageId(manifest_.id);
    for (auto &packDependency: manifest_.packDependencies) {
        packDependency.packIdUint = StringUtils::StringToUint64(packDependency.packId);
    }
    return true;
}

glimmer::PackVerifyState glimmer::DataPack::GetPackVerifyState() const {
    return packVerifyState_;
}

int glimmer::DataPack::ProcessFile(const std::filesystem::path &file, const AppContext *appContext,
                                   PackSignatureVerifier &signatureVerifier,
                                   std::vector<std::filesystem::path> &defaultLanguageFiles,
                                   std::vector<std::filesystem::path> &targetLanguageFiles,
                                   std::vector<uint8_t> &allHashData) const {
    if (signatureVerifier.ProcessSpecialFiles(file)) {
        return 0;
    }
    auto fileNameOptional = virtualFileSystem_->GetFileOrFolderName(file);
    if (!fileNameOptional.has_value()) {
        return 0;
    }
    const auto &fileName = fileNameOptional.value();
    if (!fileName.empty() && fileName[0] == '.') {
        return 0;
    }

    auto istreamUniquePtr = virtualFileSystem_->ReadFileAsStream(file);
    if (istreamUniquePtr == nullptr) {
        return 0;
    }
    auto stream = istreamUniquePtr.get();
    const std::optional<std::vector<char> > fileBufferOptional = ReadFileContent(stream);
    if (!fileBufferOptional.has_value()) {
        return 0;
    }
    const std::vector<char> &fileBuffer = fileBufferOptional.value();
    signatureVerifier.ComputeAndAppendFileHash(fileBuffer, allHashData);

    const auto dataTypeOptional = LanguageFileProcessor::GetDataType(fileName);
    if (!dataTypeOptional.has_value()) {
        return 0;
    }
    const std::string content(fileBuffer.data(), fileBuffer.size());
    const auto &dataType = dataTypeOptional.value();
    if (LanguageFileProcessor::ProcessLanguageFile(file, dataType, fileName, defaultLanguageFiles,
                                                   targetLanguageFiles, appContext)) {
        return 0;
    }
    return resourceFileLoader_.LoadResourceByType(dataType, file.string(), content,
                                                  appContext->GetModContext(), appContext->GetGraphicsContext());
}

bool glimmer::DataPack::LoadPack(AppContext *appContext) {
    packVerifyState_ = PackVerifyState::Unsigned;
    if (appContext == nullptr) {
        return false;
    }
    ModContext *modContext = appContext->GetModContext();
    if (modContext == nullptr) {
        return false;
    }
    GraphicsContext *graphicsContext = appContext->GetGraphicsContext();
    if (graphicsContext == nullptr) {
        return false;
    }
    Config *config = appContext->GetConfig();
    if (config == nullptr) {
        return false;
    }
    int total = 0;
    std::vector<std::filesystem::path> files = virtualFileSystem_->ListFile(rootPath_, true);
    if (files.empty()) {
        return false;
    }
    std::ranges::sort(files);
    std::vector<std::filesystem::path> defaultLanguageFiles;
    std::vector<std::filesystem::path> targetLanguageFiles;
    std::vector<uint8_t> allHashData;
    SpecialFileProcessingParams specialFileProcessingParams{
        config->mods.enableSignVerify, rootPath_ / ".public", rootPath_ / ".sign", false, false,
        std::vector<uint8_t>(32), std::vector<uint8_t>(64)
    };
    PackSignatureVerifier signatureVerifier(virtualFileSystem_, specialFileProcessingParams);
    for (const auto &file: files) {
        total += ProcessFile(file, appContext, signatureVerifier, defaultLanguageFiles,
                             targetLanguageFiles, allHashData);
    }

    total += resourceFileLoader_.LoadLanguageFiles(defaultLanguageFiles, targetLanguageFiles, modContext);

    if (specialFileProcessingParams.enableSignVerify) {
        packVerifyState_ = signatureVerifier.Verify(allHashData);
    }

    if (config->mods.loadOnlyVerified && packVerifyState_ != PackVerifyState::VerifiedSuccess) {
        return false;
    }
    return total != 0;
}

const glimmer::DataPackManifest *glimmer::DataPack::GetManifest() const {
    return &manifest_;
}
