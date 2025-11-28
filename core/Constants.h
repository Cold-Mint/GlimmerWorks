//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <string>
#include <cstdint>

// Manifest file names of data packets and resource packets
// 数据包和资源包的清单文件名
#define MANIFEST_FILE_NAME "manifest.json"
#define RESOURCE_TYPE_STRING 1

#define HELP_COMMAND_NAME  "help"
#define TP_COMMAND_NAME  "tp"
#define VFS_COMMAND_NAME  "vfs"
#define BOX2D_COMMAND_NAME  "box2d"
#define ASSET_VIEWER_COMMAND_NAME  "assetViewer"
#define BOOL_DYNAMIC_SUGGESTIONS_NAME  "@bool"
#define VFS_DYNAMIC_SUGGESTIONS_NAME  "@vfs"

/**
 * Unit chunk size
 * 单位区块的尺寸
 */
static constexpr int CHUNK_SIZE = 16;
//世界高度
static constexpr int WORLD_MAX_Y = 256;
static constexpr int WORLD_MIN_Y = 0;
static constexpr int TILE_SIZE = 32;

/**
 * @brief Box2D 世界坐标到像素坐标的缩放因子
 *
 * Box2D 使用米作为单位，而屏幕渲染使用像素单位。
 * KSCALE 定义了 1 米对应多少像素，例如：
 *   - KSCALE = 32.0f 表示 1 米 = 32 像素
 *
 * 用法示例：
 *   float pixelX = meterX * KSCALE;
 */
static constexpr float KSCALE = TILE_SIZE;

/**
 * @brief 像素坐标到 Box2D 世界坐标（米）的缩放因子
 *
 * 等价于 1 / KSCALE，用于将像素坐标转换回米单位：
 *   meterX = pixelX * KINV_SCALE;
 */
static constexpr float KINV_SCALE = 1.0F / KSCALE;

/**
 * 玩家移动速度，单位：米/秒
 */
static constexpr float PLAYER_MOVE_SPEED = 6.0F;

static constexpr uint8_t RENDER_ORDER_TILE_LAYER = 1;
static constexpr uint8_t RENDER_ORDER_DEBUG_DRAW = 2;
static constexpr uint8_t RENDER_ORDER_DEBUG_PANEL = 3;
static constexpr uint8_t RENDER_ORDER_DEBUG_CHUNK = 4;
static constexpr uint8_t RENDER_ORDER_DEBUG_BOX2D = 5;


/**
 * Special tile Id: Air
 * 特殊的瓦片Id：空气
 */
static constexpr std::string TILE_ID_AIR = "air";

#endif //CONSTANTS_H
