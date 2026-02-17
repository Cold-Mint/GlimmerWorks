//
// Created by coldmint on 2026/2/17.
//

#ifndef GLIMMERWORKS_URLUTILS_H
#define GLIMMERWORKS_URLUTILS_H
#include <string>

namespace glimmer {
    class BrowserInvoker {
    public:
        static void OpenUrl(const std::string &url);
    };
}

#endif //GLIMMERWORKS_URLUTILS_H
