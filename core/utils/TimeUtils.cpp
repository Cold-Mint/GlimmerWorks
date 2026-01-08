//
// Created by Cold-Mint on 2026/1/8.
//

#include "TimeUtils.h"

#include <chrono>


long glimmer::TimeUtils::GetCurrentTimeMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

std::string glimmer::TimeUtils::FormatTimeMs(uint64_t ms) {
    if (ms < 1000) {
        double seconds = static_cast<double>(ms) / 1000.0F;
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << seconds << "s";
        return oss.str();
    }

    uint64_t totalSeconds = ms / 1000;
    if (totalSeconds < 60) {
        return std::to_string(totalSeconds) + "s";
    }

    uint64_t minutes = totalSeconds / 60;
    uint64_t seconds = totalSeconds % 60;

    if (minutes < 60) {
        std::ostringstream oss;
        oss << minutes << "m," << seconds << "s";
        return oss.str();
    }
    uint64_t hours = minutes / 60;
    minutes = minutes % 60;
    std::ostringstream oss;
    oss << hours << "h," << minutes << "m," << seconds << "s";
    return oss.str();
}
