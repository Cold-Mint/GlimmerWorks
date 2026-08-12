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
#include "LocalConsoleInput.h"

#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <poll.h>
#include <unistd.h>
#endif

#include "core/log/LogCat.h"

#ifdef _WIN32
void glimmer::LocalConsoleInput::InputLoop(std::stop_token stopToken)
{
    LogCat::i("LocalConsoleInput thread started");
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hWakeup = static_cast<HANDLE>(wakeupEvent_);
    HANDLE handles[2] = {hStdin, hWakeup};
    std::wstring wline;

    while (!stopToken.stop_requested())
    {
        DWORD ret = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
        if (ret == WAIT_OBJECT_0 + 1)
        {
            ResetEvent(hWakeup);
            continue;
        }
        if (ret != WAIT_OBJECT_0)
        {
            continue;
        }

        INPUT_RECORD records[16];
        DWORD numRead;
        if (!ReadConsoleInputW(hStdin, records, 16, &numRead))
        {
            continue;
        }

        for (DWORD i = 0; i < numRead && !stopToken.stop_requested(); ++i)
        {
            if (records[i].EventType != KEY_EVENT) continue;
            const auto& ke = records[i].Event.KeyEvent;
            if (!ke.bKeyDown) continue;

            WCHAR ch = ke.uChar.UnicodeChar;
            if (ch == L'\r')
            {
                DWORD written;
                WriteConsoleW(hConsoleOut, L"\r\n", 2, &written, nullptr);
                if (!wline.empty())
                {
                    std::string line;
                    int len = WideCharToMultiByte(CP_UTF8, 0, wline.c_str(), (int)wline.size(),
                                                  nullptr, 0, nullptr, nullptr);
                    if (len > 0)
                    {
                        line.resize(len);
                        WideCharToMultiByte(CP_UTF8, 0, wline.c_str(), (int)wline.size(),
                                            &line[0], len, nullptr, nullptr);
                    }
                    LogCat::i("Received command from local console: ", line);
                    if (onCommandCallback_)
                    {
                        onCommandCallback_(line);
                    }
                }
                wline.clear();
            }
            else if (ch == L'\b')
            {
                if (!wline.empty())
                {
                    wline.pop_back();
                    DWORD written;
                    WriteConsoleW(hConsoleOut, L"\b \b", 3, &written, nullptr);
                }
            }
            else if (ch == L'\t' || ch >= L' ')
            {
                wline += ch;
                DWORD written;
                WriteConsoleW(hConsoleOut, &ch, 1, &written, nullptr);
            }
        }
    }
    LogCat::i("LocalConsoleInput thread stopped");
}

glimmer::LocalConsoleInput::LocalConsoleInput(std::function<void(const std::string&)> onCommandCallback)
    : onCommandCallback_(std::move(onCommandCallback))
{
    wakeupEvent_ = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    thread_ = std::jthread([this](const std::stop_token& stopToken) { this->InputLoop(stopToken); });
}

glimmer::LocalConsoleInput::~LocalConsoleInput()
{
    CloseHandle(static_cast<HANDLE>(wakeupEvent_));
}

void glimmer::LocalConsoleInput::Stop()
{
    thread_.request_stop();
    SetEvent(static_cast<HANDLE>(wakeupEvent_));
}
#else
void glimmer::LocalConsoleInput::InputLoop(std::stop_token stopToken)
{
    LogCat::i("LocalConsoleInput thread started");
    std::string line;
    pollfd pfds[2];
    pfds[0].fd = STDIN_FILENO;
    pfds[0].events = POLLIN;
    pfds[1].fd = wakeupPipe_[0];
    pfds[1].events = POLLIN;
    while (!stopToken.stop_requested())
    {
        int ret = poll(pfds, 2, -1);
        if (ret < 0)
        {
            continue;
        }

        if (pfds[1].revents & POLLIN)
        {
            char dummy;
            read(wakeupPipe_[0], &dummy, 1);
            continue;
        }

        if (pfds[0].revents & POLLIN)
        {
            if (!std::getline(std::cin, line))
            {
                continue;
            }
            if (line.empty())
            {
                continue;
            }
            LogCat::i("Received command from local console: ", line);
            if (onCommandCallback_)
            {
                onCommandCallback_(line);
            }
        }
    }
    LogCat::i("LocalConsoleInput thread stopped");
}

glimmer::LocalConsoleInput::LocalConsoleInput(std::function<void(const std::string&)> onCommandCallback)
    : onCommandCallback_(std::move(onCommandCallback))
{
    pipe(wakeupPipe_);
    thread_ = std::jthread([this](const std::stop_token& stopToken) { this->InputLoop(stopToken); });
}

glimmer::LocalConsoleInput::~LocalConsoleInput()
{
    close(wakeupPipe_[0]);
    close(wakeupPipe_[1]);
}

void glimmer::LocalConsoleInput::Stop()
{
    thread_.request_stop();
    char dummy = 0;
    write(wakeupPipe_[1], &dummy, 1);
}
#endif
