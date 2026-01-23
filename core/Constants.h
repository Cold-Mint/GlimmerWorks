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
#define MAP_MANIFEST_FILE_NAME "mapManifest.bin"
#define PLAYER_FILE_NAME "player.bin"


#define HELP_COMMAND_NAME  "help"
#define LICENSE_COMMAND_NAME  "license"
#define SEED_COMMAND_NAME  "seed"
#define CONFIG_COMMAND_NAME  "config"
#define HEIGHT_MAP_COMMAND_NAME  "heightMap"
#define TP_COMMAND_NAME  "tp"
#define VFS_COMMAND_NAME  "vfs"
#define BOX2D_COMMAND_NAME  "box2d"
#define GIVE_COMMAND_NAME  "give"
#define ASSET_VIEWER_COMMAND_NAME  "assetViewer"
#define ECS_COMMAND_NAME  "ecs"
#define FULL_TILE_PLACER_ID "fillTilePlacer"
#define BOOL_DYNAMIC_SUGGESTIONS_NAME  "&bool"
#define CONFIG_DYNAMIC_SUGGESTIONS_NAME  "&config"
#define VFS_DYNAMIC_SUGGESTIONS_NAME  "&vfs"
#define TILE_DYNAMIC_SUGGESTIONS_NAME  "&tile"
#define COMPOSABLE_ITEM_DYNAMIC_SUGGESTIONS_NAME  "&composeable_item"
#define ABILITY_ITEM_DYNAMIC_SUGGESTIONS_NAME  "&ability_item"

static constexpr std::string ABILITY_ID_DIG = "dig";
/**
 * Resource ref
 * 资源引用
 */
static constexpr uint32_t RESOURCE_TYPE_NONE = 0;
static constexpr uint32_t RESOURCE_TYPE_STRING = 1;
static constexpr uint32_t RESOURCE_TYPE_TILE = 2;
static constexpr uint32_t RESOURCE_TYPE_COMPOSABLE_ITEM = 3;
static constexpr uint32_t RESOURCE_TYPE_ABILITY_ITEM = 4;
static constexpr uint32_t RESOURCE_REF_ARG_TYPE_NONE = 0;
static constexpr uint32_t RESOURCE_REF_ARG_TYPE_STRING = 1;
static constexpr uint32_t RESOURCE_REF_ARG_TYPE_INT = 2;
static constexpr uint32_t RESOURCE_REF_ARG_TYPE_FLOAT = 3;
static constexpr uint32_t RESOURCE_REF_ARG_TYPE_BOOL = 4;
static constexpr uint32_t RESOURCE_REF_ARG_TYPE_REF = 5;
static constexpr std::string RESOURCE_REF_SELF = "@self";
static constexpr std::string RESOURCE_REF_CORE = "@core";
static constexpr std::string DEBUG_FOLDER_NAME = "debug";

constexpr uint16_t BOX2D_CATEGORY_PLAYER = 0x0001;
constexpr uint16_t BOX2D_CATEGORY_TILE = 0x0002;
constexpr uint16_t BOX2D_CATEGORY_ITEM = 0x0004;
constexpr size_t ITEM_MAX_STACK = 64;
constexpr size_t HOT_BAR_SIZE = 9;
constexpr float ITEM_SLOT_SIZE = 40.0F;
constexpr float ITEM_SLOT_PADDING = 8.0F;
constexpr float DROP_INTERVAL = 1.0F / ITEM_MAX_STACK;

/**
 *Type of adsorbate
 * 被吸附物类型
 */
constexpr uint16_t MAGNETIC_TYPE_ITEM = 0x0001;
/**
 * Unit chunk size
 * 单位区块的尺寸
 */
static constexpr int CHUNK_SIZE = 16;
//世界高度
static constexpr int WORLD_MAX_Y = 512;
static constexpr int SKY_HEIGHT = 64;
static constexpr int WORLD_MIN_Y = 0;
/**
 * 地形变化的总范围
 */
constexpr int TERRAIN_HEIGHT_RANGE = WORLD_MAX_Y - WORLD_MIN_Y - SKY_HEIGHT;
constexpr int BASE_HEIGHT_OFFSET = 32; // 基础高度偏移
constexpr int SEA_LEVEL_HEIGHT = BASE_HEIGHT_OFFSET + 64; // 绝对海平面 Y 坐标
constexpr int MAX_LAND_HEIGHT = WORLD_MIN_Y + TERRAIN_HEIGHT_RANGE; // 陆地最大高度

// 噪声权重
constexpr float MOUNTAIN_WEIGHT = 0.70F; // 山脉噪声在陆地起伏中的权重
constexpr float HILLS_WEIGHT = 0.30F; // 丘陵噪声在陆地起伏中的权重

// 高原/山脉的额外抬升参数
constexpr float PEAK_LIFT_THRESHOLD = 0.60F; // 陆地起伏噪声高于此值时开始抬升
constexpr float MAX_PEAK_LIFT = 0.30F; // 山峰额外抬升的最大噪声比例 (例如，1.0 + 0.3 = 1.3 倍的幅度)

static constexpr int TILE_SIZE = 32;
/**
 * The placement distance of the tiles(Unit: Number of Tile Squares)
 * 瓦片的放置距离（单位：瓦片格数）
 */
static constexpr float TILE_PLACE_RANGE = 10;

static constexpr int DROPPED_ITEM_SIZE = TILE_SIZE * 0.8F;

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
static constexpr float PLAYER_MOVE_SPEED = 60.0F;

static constexpr uint8_t RENDER_ORDER_TILE_LAYER = 1;
static constexpr uint8_t RENDER_ORDER_DROPPED_ITEM = 2;
static constexpr uint8_t RENDER_ORDER_DEBUG_DRAW = 3;
static constexpr uint8_t RENDER_ORDER_DEBUG_PANEL = 4;
static constexpr uint8_t RENDER_ORDER_DEBUG_CHUNK = 5;
static constexpr uint8_t RENDER_ORDER_DEBUG_BOX2D = 6;
static constexpr uint8_t RENDER_ORDER_DEBUG_HOTBAR = 7;
static constexpr uint8_t RENDER_ORDER_ITEM_EDITOR = 8;
static constexpr uint8_t RENDER_ORDER_ANDROID_CTRL = 9;
static constexpr uint8_t RENDER_ORDER_DIGGING = 10;
static constexpr uint8_t RENDER_ORDER_PAUSE = 11;


/**
 * The component IDs defined here should be auto-incremented and cannot be reused.
 * 这里定义的组件id应该是自增的。不能重复利用。
 */
static constexpr uint32_t COMPONENT_ID_AUTO_PICK = 1;
static constexpr uint32_t COMPONENT_ID_CAMERA = 2;
static constexpr uint32_t COMPONENT_ID_DEBUG_DRAW = 3;
static constexpr uint32_t COMPONENT_ID_DIGGING = 4;
static constexpr uint32_t COMPONENT_ID_DROPPED_ITEM = 5;
// Serialization is not supported.
// 不支持序列化。
static constexpr uint32_t COMPONENT_ID_HOTBAR = 6;
static constexpr uint32_t COMPONENT_ID_ITEM_CONTAINER = 7;
// Serialization is not supported.
// 不支持序列化。
static constexpr uint32_t COMPONENT_ID_ITEM_SLOT = 8;
static constexpr uint32_t COMPONENT_ID_MAGNET = 9;
static constexpr uint32_t COMPONENT_ID_MAGNETIC = 10;
// Serialization is not supported.
// 不支持序列化。
static constexpr uint32_t COMPONENT_ID_PAUSE = 11;
// Serialization is not supported.
// 不支持序列化。
static constexpr uint32_t COMPONENT_ID_PLAYER_CONTROL = 12;
static constexpr uint32_t COMPONENT_ID_RIGID_BODY_2D = 13;
// Serialization is not supported.
// 不支持序列化。
static constexpr uint32_t COMPONENT_ID_TILE_LAYER = 14;
static constexpr uint32_t COMPONENT_ID_TRANSFORM_2D = 15;
static constexpr uint32_t COMPONENT_ID_GUI_TRANSFORM_2D = 16;


/**
 * Special tile Id: Air
 * 特殊的瓦片Id：空气
 */
static constexpr std::string TILE_ID_AIR = "air";
static constexpr std::string TILE_ID_WATER = "water";
static constexpr std::string TILE_ID_BEDROCK = "bedrock";

#endif //CONSTANTS_H
