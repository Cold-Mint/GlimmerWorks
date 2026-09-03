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
#include "core/context/AppContext.h"

#include "InitResourcePackTask.h"

#include "core/context/SystemBucket.h"
#include "core/log/LogCat.h"

glimmer::InitResourcePackTask::InitResourcePackTask(AppContext *appContext) {
    appContext_ = appContext;
}

bool glimmer::InitResourcePackTask::Run(ISystemBucket *systemBucket) {
    VirtualFileSystem *virtualFileSystem = systemBucket->GetVirtualFileSystem();
    if (virtualFileSystem == nullptr) {
        LogCat::e(std::source_location::current(), "virtualFileSystem is nullptr");
        return false;
    }
    const Config *config = systemBucket->GetConfig();
    if (config == nullptr) {
        LogCat::e(std::source_location::current(), "config is nullptr");
        return false;
    }
    auto resourcePackManager = std::make_unique<ResourcePackManager>();
    PackScanRequest packScanRequest;
    packScanRequest.SetAppContext(appContext_);
    if (resourcePackManager->Scan(&packScanRequest) == 0) {
        LogCat::e(std::source_location::current(), "The resource package cannot be found.");
        return false;
    }
    systemBucket->SetResourcePackManager(std::move(resourcePackManager));
    return true;
}

void glimmer::InitResourcePackTask::Rollback(ISystemBucket *systemBucket) {
    systemBucket->SetResourcePackManager(nullptr);
}
