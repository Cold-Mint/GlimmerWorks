//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef DATAPACKMANAGER_H
#define DATAPACKMANAGER_H
#include <string>
#include <vector>

#include "DataPack.h"


namespace Glimmer {
    class Config;

    class DataPackManager {
        //Check whether the data packet is available (for example, determine whether the minimum game version declared by the data packet exceeds the game version)
        //检测数据包是否可用（例如判断数据包声明的最低游戏版本是否超过了游戏版本）
        static bool isDataPackAvailable(const DataPack &pack);

        static bool isDataPackEnabled(const DataPack &pack,
                                      const std::vector<std::string> &enabledDataPack);

    public:
        //Scan the data packets in the specified directory(Return the number of data packets successfully loaded)
        //扫描指定目录下的数据包（返回成功加载多少个数据包）
        int scan(const std::string &path, const std::vector<std::string> &enabledDataPack,
                 const std::string &language, StringManager &stringManager);
    };
}


#endif //DATAPACKMANAGER_H
