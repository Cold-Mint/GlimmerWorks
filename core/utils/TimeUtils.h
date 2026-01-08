//
// Created by Cold-Mint on 2026/1/8.
//

#ifndef GLIMMERWORKS_TIMEUTILS_H
#define GLIMMERWORKS_TIMEUTILS_H

#include <string>
#include <sstream>
#include <iomanip>

namespace glimmer {
    class TimeUtils {
    public:
        /**
         *Get the current time (in milliseconds) since Unix Epoch (1970-01-01 00:00:00 UTC)
         *获取自 Unix Epoch（1970-01-01 00:00:00 UTC）以来的当前时间（毫秒）
         *@return The number of milliseconds of the current time 当前时间的毫秒数
         */
        [[nodiscard]] static long GetCurrentTimeMs();

        /**
         * Format time in milliseconds to a human-readable string.
         * 格式化时间，将毫秒转换为人类可读的字符串。
         * @param ms
         * @return
         */
        [[nodiscard]] static std::string FormatTimeMs(uint64_t ms);
    };
}


#endif //GLIMMERWORKS_TIMEUTILS_H
