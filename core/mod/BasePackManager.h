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
#pragma once
#include <algorithm>
#include <filesystem>
#include <vector>

#include "PackScanRequest.h"
#include "core/log/LogCat.h"
#include "core/mod/PackManifest.h"
#include "resourcePack/BaseManager.h"

namespace glimmer {
    class AppContext;
    class Config;
    class VirtualFileSystem;

    template<typename ResourceType>
    class BasePackManager : public BaseManager<ResourceType> {
        std::vector<ResourceType *> resourceVector_;

    protected:
        void AfterRegister(ResourceType *resource) override;

        void BeforeUnRegister(ResourceType *resource) override;

        /**
         * GetEnabledPack
         * 获取所有的启用包
         * @param config
         * @return
         */
        virtual std::vector<uint64_t> *GetEnabledPack(Config *config) const = 0;

        /**
         * Obtain the path for package loading
         * 获取包加载的路径
         * @param config
         * @return
         */
        virtual std::filesystem::path GetPackPath(Config *config) const = 0;

        /**
        * Load a certain package
        * 加载某个包
        * @param packScanRequest
        * @param path
        * @return
        */
        virtual std::unique_ptr<ResourceType> LoadPack(const PackScanRequest *packScanRequest,
                                                       std::filesystem::path path) = 0;

        /**
        * Is the package included in the enabled list?
        * 包是否处于启用列表内。
        * @param uuid
        * @param enabledPack
        * @return
        */
        static bool IsPackEnabled(const uint64_t &uuid,
                                  std::vector<uint64_t> *enabledPack);

        /**
         * Check if the bag is usable
         * 检查包是否可用
         * @param packManifest
         * @return
         */
        [[nodiscard]] static bool IsPackAvailable(const PackManifest *packManifest);

    public:
        /**
         * List all the resources
         * 列出所有资源
         * @return
         */
        const std::vector<ResourceType *> *List() const;

        int Scan(const PackScanRequest *packScanRequest);
    };

    template<typename ResourceType>
    void BasePackManager<ResourceType>::AfterRegister(ResourceType *resource) {
        resourceVector_.emplace_back(resource);
    }

    template<typename ResourceType>
    void BasePackManager<ResourceType>::BeforeUnRegister(ResourceType *resource) {
        auto it = std::ranges::find(resourceVector_, resource);
        if (it != resourceVector_.end()) {
            resourceVector_.erase(it);
        }
    }

    template<typename ResourceType>
    const std::vector<ResourceType *> *BasePackManager<ResourceType>::List() const {
        return &resourceVector_;
    }

    template<typename ResourceType>
    bool BasePackManager<ResourceType>::IsPackEnabled(const uint64_t &uuid, std::vector<uint64_t> *enabledPack) {
        return std::ranges::find(*enabledPack, uuid) != enabledPack->end();
    }

    template<typename ResourceType>
    bool BasePackManager<ResourceType>::IsPackAvailable(const PackManifest *packManifest) {
        return packManifest->minGameVersion <= GAME_VERSION_NUMBER;
    }
}
