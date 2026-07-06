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
#include "VFSCommand.h"

#include "core/Constants.h"
#include "core/scene/AppContext.h"
#include "fmt/color.h"

void glimmer::VFSCommand::InitSuggestions(NodeTree<std::string>* suggestionsTree)
{
    if (suggestionsTree == nullptr)
    {
        return;
    }
    //List all the mount points
    //列出所有的挂载点
    suggestionsTree->AddChild("listMount");
    //Obtain the absolute path of a certain virtual path
    //获取某个虚拟路径的绝对路径
    suggestionsTree->AddChild("actualPath");
    //Check whether a certain path exists
    //检查某个路径是否存在
    suggestionsTree->AddChild("exists");
    //Read the content of a certain file and list it on the console
    //读取某个文件内容并将其列在控制台上
    suggestionsTree->AddChild("readFile");
}

glimmer::VFSCommand::VFSCommand(AppContext* appContext)
    : Command(appContext)
{
}

const std::string& glimmer::VFSCommand::GetName() const
{
    return VFS_COMMAND_NAME;
}

void glimmer::VFSCommand::PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings)
{
    if (commandArgs == nullptr || strings == nullptr)
    {
        return;
    }
    strings->emplace_back("[operation type:string]");
    strings->emplace_back("[path:string]");
    if (commandArgs->GetSize() > 1)
    {
        const std::string type = commandArgs->AsString(1);
        if (type == "readFile")
        {
            strings->emplace_back("[message type:string]");
        }
    }
}

glimmer::NodeTree<std::string>* glimmer::VFSCommand::GetSuggestionsTree(const CommandArgs* commandArgs)
{
    if (commandArgs == nullptr)
    {
        return &GetPrivateSuggestionsTree();
    }
    if (commandArgs->GetSize() > 1)
    {
        auto type = commandArgs->AsString(1);
        if (type == "readFile" || type == "actualPath" || type == "exists")
        {
            //Attach dynamic suggestion parameters to the second-level tree
            //为第二层树附加动态建议参数
            int size = GetPrivateSuggestionsTree().GetSize();
            for (int i = 0; i < size; i++)
            {
                auto child = GetPrivateSuggestionsTree().GetChild(i);
                child->ClearChildren();
                child->AddChild(
                    std::string(VFS_DYNAMIC_SUGGESTIONS_NAME) + ":" + commandArgs->AsString(2));
            }
        }
    }

    return &GetPrivateSuggestionsTree();
}


bool glimmer::VFSCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                                  const std::function<void(const std::string& text)>* onMessage)
{
    const AppContext* appContext = GetAppContext();
    if (appContext == nullptr || commandArgs == nullptr || onMessage == nullptr)
    {
        return false;
    }
    const std::function<void(const std::string& text)>& onMessageRef = *onMessage;
    const int size = commandArgs->GetSize();
    if (size < 2)
    {
        onMessageRef(fmt::format(
            fmt::runtime(appContext->GetLangsResources()->insufficientParameterLength),
            2, size));
        return false;
    }
    const VirtualFileSystem* virtualFileSystem = appContext->GetVirtualFileSystem();
    const auto type = commandArgs->AsString(1);
    if (type == "listMount")
    {
        onMessageRef(virtualFileSystem->ListMounts());
        return true;
    }
    if (size > 2 && type == "actualPath")
    {
        auto path = commandArgs->AsString(2);
        auto actualPath = virtualFileSystem->GetActualPath(path);
        onMessageRef(actualPath.value_or(appContext->GetLangsResources()->getActualPathError));
        return true;
    }
    if (size > 2 && type == "exists")
    {
        if (auto path = commandArgs->AsString(2); virtualFileSystem->Exists(path))
        {
            onMessageRef("true");
        }
        else
        {
            onMessageRef("false");
        }
        return true;
    }
    if (size > 2 && type == "readFile")
    {
        auto path = commandArgs->AsString(2);
        auto text = virtualFileSystem->ReadFile(path);
        onMessageRef(text.value_or("null"));
        return true;
    }
    return false;
}
#endif
