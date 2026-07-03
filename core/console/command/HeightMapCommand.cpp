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
#if  !defined(NDEBUG)
#include "HeightMapCommand.h"

#include "core/Constants.h"
#include "core/world/WorldContext.h"
#include "fmt/color.h"

void glimmer::HeightMapCommand::InitSuggestions(NodeTree<std::string>* suggestionsTree)
{
}

glimmer::HeightMapCommand::HeightMapCommand(AppContext* appContext) : Command(appContext)
{
}

const std::string& glimmer::HeightMapCommand::GetName() const
{
    return HEIGHT_MAP_COMMAND_NAME;
}

bool glimmer::HeightMapCommand::RequiresWorldContext() const
{
    return true;
}

void glimmer::HeightMapCommand::PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings)
//skipcq: CXX-C2014
{
    if (commandArgs == nullptr || strings == nullptr)
    {
        return;
    }
    strings->emplace_back("[minX:int]");
    strings->emplace_back("[maxX:int]");
    strings->emplace_back("[fileName:string]");
}


bool glimmer::HeightMapCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                                        const std::function<void(const std::string& text)>* onMessage)
{
    if (appContext_ == nullptr || commandArgs == nullptr || onMessage == nullptr)
    {
        return false;
    }
    const std::function<void(const std::string& text)>& onMessageRef = *onMessage;
    if (worldContext_ == nullptr)
    {
        onMessageRef(appContext_->GetLangsResources()->worldContextIsNull);
        return false;
    }
    if (size_t size = commandArgs->GetSize(); size < 3)
    {
        onMessageRef(fmt::format(
            fmt::runtime(appContext_->GetLangsResources()->insufficientParameterLength),
            3, size));
        return false;
    }
    const int minX = commandArgs->AsInt(1);
    const int maxX = commandArgs->AsInt(2);
    if (minX > maxX)
    {
        onMessageRef(appContext_->GetLangsResources()->minXIsGreaterThanMaxX);
        return false;
    }
    std::ostringstream ss;
    ss << "{\n";
    ss << "  \"seed\": " << worldContext_->GetWorldSeed() << ",\n";
    ss << R"(  "range": { "minX": )" << minX
        << ", \"maxX\": " << maxX << " },\n";
    ss << "  \"heightMap\": [\n";

    for (int x = minX; x <= maxX; ++x)
    {
        const int height = worldContext_->GetChunkGenerator()->GetFirstTileTerrainY(x);
        ss << "    { \"x\": " << x
            << ", \"height\": " << height << " }";

        if (x != maxX)
            ss << ",";

        ss << "\n";
    }
    ss << "  ]\n";
    ss << "}\n";
    onMessageRef(ss.str());
    LogCat::d(ss.str());
    if (commandArgs->GetSize() >= 4)
    {
        const VirtualFileSystem* virtualFileSystem = appContext_->GetVirtualFileSystem();
        const std::string fileName = commandArgs->AsString(3);
        if (!virtualFileSystem->Exists(DEBUG_FOLDER_NAME))
        {
            if (!virtualFileSystem->CreateFolder(DEBUG_FOLDER_NAME))
            {
                onMessageRef(fmt::format(
                    fmt::runtime(appContext_->GetLangsResources()->folderCreationFailed),
                    DEBUG_FOLDER_NAME));
                return false;
            }
        }
        std::string path = DEBUG_FOLDER_NAME + "/heightMap_" + std::to_string(worldContext_->GetWorldSeed()) + "_" +
            fileName
            + ".json";
        const bool write = virtualFileSystem->WriteFile(
            path,
            ss.str());
        if (!write)
        {
            onMessageRef(fmt::format(
                fmt::runtime(appContext_->GetLangsResources()->fileWritingFailed),
                path));
            return false;
        }
    }
    return true;
}
#endif
