//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef DATAPACKMANAGER_H
#define DATAPACKMANAGER_H
#include <string>
#include <vector>

#include "ItemManager.h"
#include "../../vfs/VirtualFileSystem.h"
#include "core/lootTable/LootTableManager.h"
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
        std::vector<std::string> packIdVector_;

        //Check whether the data packet is available (for example, determine whether the minimum game version declared by the data packet exceeds the game version)
        //检测数据包是否可用（例如判断数据包声明的最低游戏版本是否超过了游戏版本）
        [[nodiscard]] bool IsDataPackAvailable(const DataPack &pack) const;

        static bool IsDataPackEnabled(const DataPack &pack,
                                      const std::vector<std::string> &enabledDataPack);

    public:
        explicit DataPackManager(VirtualFileSystem *virtualFilesystem);

        //Scan the data packets in the specified directory(Return the number of data packets successfully loaded)
        //扫描指定目录下的数据包（返回成功加载多少个数据包）
        int Scan(AppContext *appContext,
                 const toml::spec &tomlVersion);
    };
}


#endif //DATAPACKMANAGER_H
