//
// Created by Cold-Mint on 2025/10/10.
//
#include <string>
#ifndef CONSTANTS_H
#define CONSTANTS_H


// Manifest file names of data packets and resource packets
// 数据包和资源包的清单文件名
#define MANIFEST_FILE_NAME "manifest.json"
#define RESOURCE_TYPE_STRING 1

#define HELP_COMMAND_NAME  "help"

// 单位区块的尺寸
static constexpr int CHUNK_SIZE = 256;
//世界高度
static constexpr int WORLD_HEIGHT = 256;
static constexpr int TILE_SIZE = 8;

/**
 * Special tile Id: Air
 * 特殊的瓦片Id：空气
 */
static constexpr std::string TILE_ID_AIR = "air";

#endif //CONSTANTS_H
