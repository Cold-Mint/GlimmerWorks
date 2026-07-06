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
#include "ResourcePack.h"

#include "core/Constants.h"
#include "core/log/LogCat.h"
#include "core/utils/TomlUtils.h"
#include "toml11/parser.hpp"


glimmer::ResourcePack::ResourcePack(std::string path, const VirtualFileSystem* virtualFileSystem,
                                    const toml::spec& tomlVersion) : path_(std::move(path)),
                                                                     virtualFileSystem_(virtualFileSystem), manifest_(),
                                                                     tomlVersion_(tomlVersion)
{
}

bool glimmer::ResourcePack::LoadResourceConfig()
{
    auto data =
        virtualFileSystem_->ReadFile(path_ + "/" + RESOURCE_PACK_CONFIG_FILE_NAME);
    if (!data.has_value())
    {
        return false;
    }
    toml::value value = toml::parse_str(data.value(), tomlVersion_);
    try
    {
        resourcePackConfig_ = toml::get<ResourcePackConfig>(value);
    }
    catch (const toml::exception& e)
    {
        LogCat::e("DataPack::LoadResourceConfig - Failed to parse res_config.toml: ", e.what());
        return false;
    }
    return true;
}

bool glimmer::ResourcePack::LoadManifest()
{
    auto data =
        virtualFileSystem_->ReadFile(path_ + "/" + MANIFEST_FILE_NAME);
    if (!data.has_value())
    {
        LogCat::e("DataPack::loadManifest - Failed to load manifest: ", path_ + "/" + MANIFEST_FILE_NAME);
        return false;
    }
    toml::value value = toml::parse_str(data.value(), tomlVersion_);
    try
    {
        manifest_ = toml::get<ResourcePackManifest>(value);
    }
    catch (const toml::exception& e)
    {
        LogCat::e("DataPack::loadManifest - Failed to parse manifest toml: ", e.what());
        return false;
    }
    manifest_.name.SetSelfPackageId(manifest_.id);
    manifest_.description.SetSelfPackageId(manifest_.id);
    LogCat::d("ResourcePack::loadManifest - Loaded manifest for data pack: ", path_);
    LogCat::d("ID: ", manifest_.id);
    LogCat::d("Name: ", manifest_.name.GetResourceKey(), " (packId: ", manifest_.name.GetPackageId(), ")");
    LogCat::d("Description: ", manifest_.description.GetResourceKey(), " (packId: ",
              manifest_.description.GetPackageId(),
              ")");
    LogCat::d("Author: ", manifest_.author);
    LogCat::d("Version: ", manifest_.versionName, " (Number: ", manifest_.versionNumber, ")");
    LogCat::d("Minimum Game Version: ", manifest_.minGameVersion);
    LogCat::d("Is Resource Pack: ", manifest_.resPack ? "true" : "false");
    return true;
}

const glimmer::ResourcePackConfig& glimmer::ResourcePack::GetResourcePackConfig() const
{
    return resourcePackConfig_;
}

const glimmer::ResourcePackManifest& glimmer::ResourcePack::GetManifest() const
{
    return manifest_;
}

std::string glimmer::ResourcePack::GetPath() const
{
    return path_;
}
