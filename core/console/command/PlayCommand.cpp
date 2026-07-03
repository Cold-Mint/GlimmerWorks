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
#include "PlayCommand.h"

#include "core/mod/resourcePack/AudioResourceResult.h"
#include "core/scene/AppContext.h"
#include "fmt/xchar.h"

void glimmer::PlayCommand::InitSuggestions(NodeTree<std::string>* suggestionsTree)
{
    suggestionsTree->AddChild(AUDIO_TRACK_DYNAMIC_SUGGESTIONS_NAME);
}

glimmer::PlayCommand::PlayCommand(AppContext* appContext) : Command(appContext)
{
    audioManager_ = appContext->GetAudioManager();
}

const std::string& glimmer::PlayCommand::GetName() const
{
    return PLAY_COMMAND_NAME;
}

void glimmer::PlayCommand::PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings)
//skipcq: CXX-C2014
{
    if (strings == nullptr)
    {
        return;
    }
    strings->emplace_back("[audio_track:string]");
    strings->emplace_back("[path:string]");
}

bool glimmer::PlayCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                                   const std::function<void(const std::string& text)>* onMessage)
{
    if (appContext_ == nullptr || commandArgs == nullptr || onMessage == nullptr)
    {
        return false;
    }
    const std::function<void(const std::string& text)>& onMessageRef = *onMessage;
    const LangsResources* langsResources = appContext_->GetLangsResources();
    if (langsResources == nullptr)
    {
        return false;
    }
    int size = commandArgs->GetSize();
    if (size < 3)
    {
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            3, size));
        return false;
    }
    const std::string audioTrack = commandArgs->AsString(1);
    AudioType audioType = AMBIENT;
    if (audioTrack == AUDIO_TRACK_BGM)
    {
        audioType = BGM;
    }
    const auto resourceRef = commandArgs->AsResourceRef(2, RESOURCE_AUDIO);
    if (!resourceRef.has_value())
    {
        return false;
    }
    const std::shared_ptr<AudioResourceResult> audioResourceResult = appContext_->GetResourceLocator()->FindAudio(
        &resourceRef.value());
    if (audioResourceResult == nullptr)
    {
        return false;
    }
    MIX_Audio* audio = audioResourceResult->GetResource();
    if (audio == nullptr)
    {
        return false;
    }
    audioManager_->ForcePlayReplace(audioType, audio, 0);
    return true;
}
