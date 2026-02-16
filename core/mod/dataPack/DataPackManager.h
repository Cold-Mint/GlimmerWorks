//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef DATAPACKMANAGER_H
#define DATAPACKMANAGER_H
#include <string>
#include <vector>

#include "../../vfs/VirtualFileSystem.h"
#include "core/mod/PackManifest.h"
#include "toml11/spec.hpp"

namespace glimmer {
    class AppContext;
    class BiomesManager;
    class TileManager;
    class StringManager;
    class DataPack;
    class Config;


    class DataPackManager {
        VirtualFileSystem *virtualFileSystem_;
        std::vector<DataPackManifest> packManifestVector_;

        //Check whether the data packet is available (for example, determine whether the minimum game version declared by the data packet exceeds the game version)
        //检测数据包是否可用（例如判断数据包声明的最低游戏版本是否超过了游戏版本）
        [[nodiscard]] bool IsDataPackAvailable(const DataPack &pack) const;

        static bool IsDataPackEnabled(const DataPack &pack,
                                      const std::vector<std::string> &enabledDataPack);

    public:
        explicit DataPackManager(VirtualFileSystem *virtualFilesystem);

        /**
         * Determine whether data packet 1 and data packet 2 meet the dependency conditions (whether the list of data packet 1 includes the id of data packet 2, and whether the minimum version required by the version dependency of data packet 2 is equal to or greater than the specified dependency version in data packet 1.)
         * 判断数据包1和数据包2是否满足依赖条件(数据包1的清单是否包含数据包2的id，且数据包2版本依赖的最小版本等于或大于数据包1内指定的依赖版本。)
         * @return
         */
        bool IsDependencySatisfied(const std::string &pack1Id, const std::string &pack2Id);

        //Scan the data packets in the specified directory(Return the number of data packets successfully loaded)
        //扫描指定目录下的数据包（返回成功加载多少个数据包）
        int Scan(AppContext *appContext,
                 const toml::spec &tomlVersion);
    };
}


#endif //DATAPACKMANAGER_H
