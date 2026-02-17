//
// Created by coldmint on 2026/2/17.
//

#include "BrowserInvoker.h"

#include <unistd.h>

void glimmer::BrowserInvoker::OpenUrl(const std::string &url) {
#ifdef _WIN32
    ShellExecuteA(nullptr, "open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#elif __APPLE__
    if (fork() == 0) {
        execlp("open", "open", url.c_str(), nullptr);
        _exit(1);
    }

#else
    if (fork() == 0) {
        execlp("xdg-open", "xdg-open", url.c_str(), nullptr);
        _exit(1);
    }
#endif
}
