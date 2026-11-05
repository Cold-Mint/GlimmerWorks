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
#include "InitCrashpadTask.h"
#ifdef WITH_CRASHPAD


#include <filesystem>
#include <map>

#include "crashpad_client.h"
#include "core/config/Config.h"
#include "core/context/ISystemBucket.h"
#include "core/log/LogCat.h"
#include "core/utils/StringUtils.h"
#include "core/vfs/VirtualFileSystem.h"

bool glimmer::InitCrashpadTask::Run(ISystemBucket *systemBucket) {
    const Config *config = systemBucket->GetConfig();
    if (config == nullptr) {
        return false;
    }
    const VirtualFileSystem *virtualFileSystem = systemBucket->GetVirtualFileSystem();
    if (virtualFileSystem == nullptr) {
        return false;
    }
    std::optional<std::string> runtimePathOptional = config->GetAsString(StringUtils::StringToUint64("runtime_path"));
    if (!runtimePathOptional.has_value()) {
        return false;
    }
    auto handlerPathOptional = virtualFileSystem->GetActualPath(std::filesystem::path(CRASHPAD_NAME));
    if (!handlerPathOptional.has_value()) {
        return false;
    }
    auto runtimeActualPathOptional = virtualFileSystem->GetActualPath(runtimePathOptional.value());
    if (!runtimeActualPathOptional.has_value()) {
        return false;
    }

    const std::filesystem::path &runtimeActualPath = runtimeActualPathOptional.value();
    std::filesystem::path databasePath = runtimeActualPath / ".crashpad";
    std::filesystem::create_directories(databasePath);
    auto databaseFilePath = base::FilePath(databasePath.string());
    std::map<std::string, std::string> annotations;
    annotations["product"] = "GlimmerWorks";
    annotations["version"] = GAME_VERSION_STRING; // 如果你有版本宏

    std::vector<std::string> arguments;
    // 可以加 --no-rate-limit 等参数，但不是必须的

    crashpad::CrashpadClient client;
    bool success = client.StartHandler(
        base::FilePath(handlerPathOptional.value()),
        databaseFilePath,
        databaseFilePath, // metrics_dir，通常和 database 相同即可
        "",
        annotations,
        arguments,
        /*restartable=*/true,
        /*asynchronous_start=*/false
    );

    if (!success) {
        // 启动失败，记录日志，但不要阻止程序主流程
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(),
                  "crashpad_init_failed", "Crashpad StartHandler failed");
    }
    return success;
}

void glimmer::InitCrashpadTask::Rollback(ISystemBucket *systemBucket) {
}

std::string glimmer::InitCrashpadTask::GetTaskName() {
    return "InitCrashpadTask";
}
#endif
