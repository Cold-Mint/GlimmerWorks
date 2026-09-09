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
#include "InitSDLTask.h"

#include "core/log/LogCat.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_init.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "SDL3_ttf/SDL_ttf.h"

bool glimmer::InitSDLTask::Run(ISystemBucket *) {
#ifdef __ANDROID__
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
    SDL_SetHint("SDL_ANDROID_TRAP_BACK_BUTTON", "1");
#endif
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        LogCat::e(std::source_location::current(), "sdl_init_failed", "SDL_Init failed");
        return false;
    }
    initSDLSuccess_ = true;
    LogCat::i("sdl_init_succeeded", "SDL_Init succeeded");
    if (!MIX_Init()) {
        LogCat::e(std::source_location::current(), "mix_init_failed", "MIX_Init failed");
        return false;
    }
    initSDLMixSuccess_ = true;
    LogCat::i("mix_init_succeeded", "MIX_Init succeeded");
    if (!TTF_Init()) {
        LogCat::e(std::source_location::current(), "ttf_init_failed", "TTF_Init failed");
        return false;
    }
    initSDLTtfSuccess_ = true;
    LogCat::i("ttf_init_succeeded", "TTF_Init succeeded");
    return true;
}

void glimmer::InitSDLTask::QuitSubsystems() {
    if (initSDLMixSuccess_) {
        MIX_Quit();
        initSDLMixSuccess_ = false;
    }
    if (initSDLTtfSuccess_) {
        TTF_Quit();
        initSDLTtfSuccess_ = false;
    }
    if (initSDLSuccess_) {
        SDL_Quit();
        initSDLSuccess_ = false;
    }
}

void glimmer::InitSDLTask::Rollback(ISystemBucket *) {
    QuitSubsystems();
}

void glimmer::InitSDLTask::Shutdown() {
    QuitSubsystems();
}

std::string glimmer::InitSDLTask::GetTaskName() {
    return "InitSDLTask";
}
