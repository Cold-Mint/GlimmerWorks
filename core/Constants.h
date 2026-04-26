//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <string>

#include "ecs/component/TileLayerComponent.h"

// Manifest file names of data packets and resource packets
// 数据包和资源包的清单文件名
static constexpr std::string MANIFEST_FILE_NAME = "manifest.toml";
static constexpr std::string MAP_MANIFEST_FILE_NAME = "mapManifest.bin";
static constexpr std::string PLAYER_FILE_NAME = "player.bin";


static constexpr std::string HELP_COMMAND_NAME = "help";
static constexpr std::string LICENSE_COMMAND_NAME = "license";
static constexpr std::string SEED_COMMAND_NAME = "seed";
static constexpr std::string SUMMON_COMMAND_NAME = "summon";
static constexpr std::string CONFIG_COMMAND_NAME = "config";
static constexpr std::string HEIGHT_MAP_COMMAND_NAME = "heightMap";
static constexpr std::string TP_COMMAND_NAME = "tp";
static constexpr std::string VFS_COMMAND_NAME = "vfs";
static constexpr std::string BOX2D_COMMAND_NAME = "box2d";
static constexpr std::string GIVE_COMMAND_NAME = "give";
static constexpr std::string ASSET_VIEWER_COMMAND_NAME = "assetViewer";
static constexpr std::string CLEAR_COMMAND_NAME = "clear";
static constexpr std::string LOOT_COMMAND_NAME = "loot";
static constexpr std::string PLACE_COMMAND_NAME = "place";
static constexpr std::string ECS_COMMAND_NAME = "ecs";
static constexpr std::string FLY_COMMAND_NAME = "fly";
static constexpr std::string ECHO_COMMAND_NAME = "echo";
static constexpr std::string SCREEN_SHOT_COMMAND_NAME = "screenShot";
static constexpr std::string LOCATE_COMMAND_NAME = "locate";
static constexpr std::string BOOL_DYNAMIC_SUGGESTIONS_NAME = "&bool";
static constexpr std::string BOOL_TOGGLE_DYNAMIC_SUGGESTIONS_NAME = "&bool_toggle";
static constexpr std::string X_DYNAMIC_SUGGESTIONS_NAME = "&x";
static constexpr std::string Y_DYNAMIC_SUGGESTIONS_NAME = "&y";
static constexpr std::string MOB_DYNAMIC_SUGGESTIONS_NAME = "&mob";
static constexpr std::string CONFIG_DYNAMIC_SUGGESTIONS_NAME = "&config";
static constexpr std::string VFS_DYNAMIC_SUGGESTIONS_NAME = "&vfs";
static constexpr std::string TILE_DYNAMIC_SUGGESTIONS_NAME = "&tile";
static constexpr std::string STRUCTURE_DYNAMIC_SUGGESTIONS_NAME = "&structure";
static constexpr std::string COMPOSABLE_ITEM_DYNAMIC_SUGGESTIONS_NAME = "&composeable";
static constexpr std::string ABILITY_ITEM_DYNAMIC_SUGGESTIONS_NAME = "&ability_item";
static constexpr std::string LOOT_DYNAMIC_SUGGESTIONS_NAME = "&loot";
static constexpr std::string BIOME_DYNAMIC_SUGGESTIONS_NAME = "&biome";

static constexpr std::string STRUCTURE_GENERATOR_ID_TREE = "tree";
static constexpr std::string STRUCTURE_GENERATOR_ID_STATIC = "static";
static constexpr std::string ABILITY_ID_NONE = "none";
static constexpr std::string ABILITY_ID_DIG = "dig";
static constexpr std::string ABILITY_ID_AREA_MARKER = "areaMarker";

static constexpr std::string STRUCTURE_PLACEMENT_CONDITIONS_BIOME = "biome";
static constexpr std::string STRUCTURE_PLACEMENT_CONDITIONS_SURFACE = "surface";
static constexpr std::string STRUCTURE_PLACEMENT_CONDITIONS_HEIGHT = "height";
static constexpr std::string STRUCTURE_PLACEMENT_CONDITIONS_HORIZONTAL_SPACING = "hSpacing";

static constexpr std::string TOGGLE_KEY_WORD = "toggle";
static constexpr int HOTKEY_COUNT = 12;

/**
 * Resource ref
 * 资源引用
 */
static constexpr uint32_t RESOURCE_TYPE_NONE = 0;
static constexpr uint32_t RESOURCE_TYPE_STRING = 1;
static constexpr uint32_t RESOURCE_TYPE_TILE = 2;
static constexpr uint32_t RESOURCE_TYPE_COMPOSABLE_ITEM = 3;
static constexpr uint32_t RESOURCE_TYPE_ABILITY_ITEM = 4;
static constexpr uint32_t RESOURCE_TYPE_LOOT_TABLE = 5;
static constexpr uint32_t RESOURCE_TYPE_STRUCTURE = 6;
static constexpr uint32_t RESOURCE_TYPE_TEXTURES = 7;
static constexpr uint32_t RESOURCE_TYPE_BIOME = 8;
static constexpr uint32_t RESOURCE_TYPE_COLOR = 9;
static constexpr uint32_t RESOURCE_TYPE_MOB = 10;
static constexpr uint32_t RESOURCE_TYPE_SHAPE = 11;
static constexpr uint32_t RESOURCE_TYPE_AUDIO = 12;
//This dropped item resource type is used for serialization within the game save file. Currently, this type of resource is not supported for definition within the mod.
//这个掉落物资源类型用作存档内序列化。模组内暂不支持定义此类型的资源。
static constexpr uint32_t RESOURCE_TYPE_DROPPED_ITEM = 13;
static constexpr uint32_t RESOURCE_TYPE_BIOME_DECORATOR = 14;
static constexpr uint32_t RESOURCE_TYPE_FIXED_COLOR = 15;
static constexpr uint32_t RESOURCE_TYPE_LIGHT_SOURCE = 16;
static constexpr uint32_t RESOURCE_TYPE_LIGHT_MASK = 17;


static constexpr uint32_t RESOURCE_REF_ARG_TYPE_NONE = 0;
static constexpr uint32_t RESOURCE_REF_ARG_TYPE_STRING = 1;
static constexpr uint32_t RESOURCE_REF_ARG_TYPE_INT = 2;
static constexpr uint32_t RESOURCE_REF_ARG_TYPE_FLOAT = 3;
static constexpr uint32_t RESOURCE_REF_ARG_TYPE_BOOL = 4;
static constexpr uint32_t RESOURCE_REF_ARG_TYPE_REF_PB = 5;
static constexpr uint32_t RESOURCE_REF_ARG_TYPE_REF_TOML = 6;

static constexpr std::string DEV_DISPLAY_NAME_KEY_COLD_MINT = "dev@Cold-Mint";
static const std::string DEV_UUID_COLD_MINT = "4a724ce8-ec7f-411e-833c-b0a5e2529263";
static constexpr std::string DEV_NAME_COLO_MINT = "Cold-Mint";
static constexpr std::string RESOURCE_REF_SELF = "@self";
static constexpr std::string RESOURCE_REF_CORE = "@core";
static constexpr std::uint32_t CORE_DATA_PACK_VERSION_NUMBER = 1;
static constexpr std::string DEBUG_FOLDER_NAME = "debug";
static constexpr std::string DATA_FILE_TYPE_STRINGS = "strings";
static constexpr std::string DATA_FILE_TYPE_TILE = "tile";
static constexpr std::string DATA_FILE_TYPE_BIOME = "biome";
static constexpr std::string DATA_FILE_TYPE_COMPOSABLE_ITEM = "composable";
static constexpr std::string DATA_FILE_TYPE_ABILITY_ITEM = "ability";
static constexpr std::string DATA_FILE_TYPE_LOOT_TABLE = "loot";
static constexpr std::string DATA_FILE_TYPE_STRUCTURE = "structure";
static constexpr std::string DATA_FILE_TYPE_INITIAL_INVENTORY = "startinv";
static constexpr std::string DATA_FILE_TYPE_CONTRIBUTOR = "contributor";
static constexpr std::string DATA_FILE_TYPE_COLOR = "color";
static constexpr std::string DATA_FILE_TYPE_MOB = "mob";
static constexpr std::string DATA_FILE_TYPE_TEMPLATE = "template";
static constexpr std::string DATA_FILE_TYPE_SHAPE_RECTANGLE = "rect";
static constexpr std::string DATA_FILE_TYPE_SHAPE_ROUNDED_RECTANGLE = "round_rect";
static constexpr std::string DATA_FILE_TYPE_SHAPE_CIRCLE = "circle";
static constexpr std::string DATA_FILE_TYPE_DECORATOR_FILL = "decor_fill";
static constexpr std::string DATA_FILE_TYPE_DECORATOR_MINERAL = "decor_mineral";
static constexpr std::string DATA_FILE_TYPE_DECORATOR_SURFACE = "decor_surface";
static constexpr std::string DATA_FILE_TYPE_FIXED_COLOR = "fixed_color";
static constexpr std::string DATA_FILE_TYPE_LIGHT_MASK = "light_mask";
static constexpr std::string DATA_FILE_TYPE_LIGHT_SOURCE = "light_source";

static constexpr uint16_t BOX2D_CATEGORY_PLAYER = 0x0001;
static constexpr uint16_t BOX2D_CATEGORY_TILE = 0x0002;
static constexpr uint16_t BOX2D_CATEGORY_ITEM = 0x0004;
static constexpr size_t ITEM_MAX_STACK = 64;
static constexpr size_t HOT_BAR_SIZE = 9;
static constexpr float ITEM_SLOT_SIZE = 40.0F;
static constexpr float ITEM_SLOT_PADDING = 8.0F;
static constexpr float DROP_INTERVAL = 1.0F / ITEM_MAX_STACK;
//Maximum suction (slightly larger than the original 80, compensating for attenuation)
//最大吸力（比原80稍大，补偿衰减）
static constexpr float MAX_MAGNET_FORCE = 120.0F;
static constexpr float MIN_SAFE_DISTANCE = 0.1F;
/**
 *Type of adsorbate
 * 被吸附物类型
 */
static constexpr uint16_t MAGNETIC_TYPE_ITEM = 0x0001;
/**
 * Unit chunk size
 * 单位区块的尺寸（边长）
 * Designed as a power of 2 (16 = 2^4), which facilitates quick calculation through bit operations in the subsequent stages.
 * 设计为2的幂次(16=2^4)，便于后续通过位运算快速计算
 */
static constexpr int CHUNK_SIZE = 16;
static constexpr int HALF_CHUNK_SIZE = CHUNK_SIZE * 0.5;

/**
 * The area of a single block (total pixels / total elements)
 * 单个区块的面积（总像素/总元素数）
 * By calculating the square of the block side length, we get: 16 * 16 = 256
 * 由区块边长的平方计算得出：16 * 16 = 256
 */
static constexpr int CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
static constexpr int TILE_LAYER_TYPE_COUNT = 2;

/**
 * Block mask (used for fast modulo operation / obtaining offset within the block)
 * 区块掩码（用于快速取模/获取区块内偏移）
 * CHUNK_SIZE = 16 (binary 0b10000), and the mask is 0b1111 (15)
 * CHUNK_SIZE=16(二进制0b10000)，掩码为0b1111(15)
 * Function: x & CHUNK_MASK is equivalent to x % CHUNK_SIZE (this holds true only when CHUNK_SIZE is a power of 2)
 * 作用：x & CHUNK_MASK 等价于 x % CHUNK_SIZE（仅当CHUNK_SIZE为2的幂时成立）
 */
static constexpr int CHUNK_MASK = CHUNK_SIZE - 1;
/**
 * Block alignment mask (used for quickly aligning to block boundaries)
 * 区块对齐掩码（用于快速对齐到区块边界）
 *  ~CHUNK_MASK = ~0b1111 = 0xFFFFFFF0 (32-bit)
 * ~CHUNK_MASK = ~0b1111 = 0xFFFFFFF0（32位）
 * Function: x & CHUNK_ALIGN can round the coordinates down to the nearest starting position of a block.
 * 作用：x & CHUNK_ALIGN 可将坐标向下取整到最近的区块起始位置
 * 19 & 0xFFFFFFF0 = 16 (Aligned to the 16th position, which is the starting point of the next block)
 * 示例：19 & 0xFFFFFFF0 = 16（对齐到第16号位置，即下一个区块起始）
 * 35 & 0xFFFFFFF0 = 32 (Aligned to the 16th position, which is the starting point of the next block)
 * 示例：35 & 0xFFFFFFF0 = 32（对齐到第16号位置，即下一个区块起始）
 */
static constexpr int CHUNK_ALIGN = ~CHUNK_MASK;
/**
 * Block displacement bits (used for quickly calculating block indices)
 * 区块位移位数（用于快速计算区块索引）
 * std::countr_zero(CHUNK_SIZE) calculates the number of zeros at the end of the binary representation of CHUNK_SIZE.
 * std::countr_zero(CHUNK_SIZE) 计算CHUNK_SIZE二进制末尾的0的个数
 * CHUNK_SIZE = 16 (0b10000), with 4 zeros at the end, so CHUNK_SHIFT = 4
 * CHUNK_SIZE=16(0b10000)，末尾有4个0，故CHUNK_SHIFT=4
 * Function: x >> CHUNK_SHIFT is equivalent to x / CHUNK_SIZE (this holds true only when CHUNK_SIZE is a power of 2)
 * 作用：x >> CHUNK_SHIFT 等价于 x / CHUNK_SIZE（仅当CHUNK_SIZE为2的幂时成立）
 */
static constexpr int CHUNK_SHIFT = std::countr_zero(static_cast<uint32_t>(CHUNK_SIZE));
static constexpr int SKY_HEIGHT = 64;
static constexpr int WORLD_MAX_Y = 1200;
static constexpr int WORLD_MIN_Y = 0;
static constexpr int WORLD_MIN_X = -29984;
static constexpr int WORLD_MAX_X = 29984;

//Total distributable height.
//总的可供分配高度。
static constexpr int TERRAIN_HEIGHT_RANGE = WORLD_MAX_Y - WORLD_MIN_Y - SKY_HEIGHT;
//The height of the lowest point on the ground.
//地面最低点的高度。
static constexpr int GROUND_START_HEIGHT = WORLD_MIN_Y + TERRAIN_HEIGHT_RANGE * 0.45F;
//Sea level height
//海平面高度
static constexpr int SEA_LEVEL_HEIGHT = WORLD_MIN_Y + TERRAIN_HEIGHT_RANGE * 0.55F;
//Continent noise, the maximum height that can be allocated.
//大陆噪声，可分配的最大高度。
static constexpr int CONTINENT_MAX_HEIGHT = TERRAIN_HEIGHT_RANGE * 0.2F;

static constexpr int MAX_LAND_HEIGHT = WORLD_MIN_Y + TERRAIN_HEIGHT_RANGE;

// 噪声权重
static constexpr float MOUNTAIN_WEIGHT = 0.70F; // 山脉噪声在陆地起伏中的权重
static constexpr float HILLS_WEIGHT = 0.30F; // 丘陵噪声在陆地起伏中的权重

// 高原/山脉的额外抬升参数
static constexpr float PEAK_LIFT_THRESHOLD = 0.60F; // 陆地起伏噪声高于此值时开始抬升
static constexpr float MAX_PEAK_LIFT = 0.30F; // 山峰额外抬升的最大噪声比例 (例如，1.0 + 0.3 = 1.3 倍的幅度)

static constexpr int TILE_SIZE = 32;
static constexpr int HALF_TILE_SIZE = TILE_SIZE / 2;
static constexpr std::string ERROR_TEXTURE_KEY = "@error";
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

/**
 * Invalid game ID indication
 * 表示无效的游戏ID
 */
static constexpr uint32_t GAME_ENTITY_ID_INVALID = 0;

static constexpr uint8_t RENDER_ORDER_TILE_LAYER = 1;
static constexpr uint8_t RENDER_ORDER_DROPPED_ITEM = 2;
static constexpr uint8_t RENDER_ORDER_HOTBAR = 3;
static constexpr uint8_t RENDER_ORDER_ITEM_EDITOR = 4;
static constexpr uint8_t RENDER_ORDER_DIGGING = 10;
static constexpr uint8_t RENDER_ORDER_AREA_MARKER = 11;
static constexpr uint8_t RENDER_ORDER_SPIRIT_RENDERER = 12;
static constexpr uint8_t RENDER_ORDER_FLOATING_TEXT = 13;
static constexpr uint8_t RENDER_ORDER_DRAGGABLE = 14;

static constexpr uint8_t RENDER_ORDER_LIGHT2D = 93;
static constexpr uint8_t RENDER_ORDER_DEBUG_DRAW = 94;
static constexpr uint8_t RENDER_ORDER_DEBUG_CHUNK = 95;
static constexpr uint8_t RENDER_ORDER_DEBUG_BOX2D = 96;
static constexpr uint8_t RENDER_ORDER_DEBUG_MAP = 97;
static constexpr uint8_t RENDER_ORDER_DEBUG_PANEL = 98;
static constexpr uint8_t RENDER_ORDER_ANDROID_CTRL = 99;
static constexpr uint8_t RENDER_ORDER_PAUSE = 100;


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
static constexpr uint32_t COMPONENT_ID_PLAYER = 12;
static constexpr uint32_t COMPONENT_ID_RIGID_BODY_2D = 13;
// Serialization is not supported.
// 不支持序列化。
static constexpr uint32_t COMPONENT_ID_TILE_LAYER = 14;
static constexpr uint32_t COMPONENT_ID_TRANSFORM_2D = 15;
static constexpr uint32_t COMPONENT_ID_GUI_TRANSFORM_2D = 16;
static constexpr uint32_t COMPONENT_ID_AREA_MARKER = 17;
static constexpr uint32_t COMPONENT_ID_SPIRIT_RENDERER = 18;
static constexpr uint32_t COMPONENT_ID_ITEM_EDITOR = 19;
static constexpr uint32_t COMPONENT_ID_MOB = 20;
static constexpr uint32_t COMPONENT_ID_RAY_CAST_2D = 21;
static constexpr uint32_t COMPONENT_ID_FLOATING_TEXT = 22;
static constexpr uint32_t COMPONENT_ID_TILE_PLACEMENT_FORBIDDEN_ZONE = 23;
static constexpr uint32_t COMPONENT_ID_DRAGGABLE = 24;


static constexpr std::string TILE_ID_AIR = "air";
static constexpr std::string TILE_ID_WATER = "water";
static constexpr std::string TILE_ID_BEDROCK = "bedrock";
static constexpr std::string TILE_ID_ERROR = "error";
static constexpr std::string SHAPE_ID_DROPPED_ITEM = "droppedItem";
static constexpr std::string DROPPED_ITEM_ID_DEFAULT = "default";
// Complete occlusion (solid/transparent obstruction)
// 完全遮照（实心/不透明遮挡）
static constexpr std::string LIGHT_MASK_FULL = "fill";
static constexpr std::string LIGHT_MASK_FULL_COLOR = "fill";

// Transparent (without blocking light)
// 无遮照（透明/不遮挡光线）
static constexpr std::string LIGHT_MASK_NONE = "none";
static constexpr std::string LIGHT_MASK_NONE_COLOR = "none";

static constexpr std::string LIGHT_NONE = "none";
static constexpr std::string LIGHT_NONE_COLOR = "none";
static constexpr std::string TEMPLATE_CURRENT = "@current";
static constexpr std::string TEMPLATE_ROOT = "@root";
static constexpr std::string PROJECT_NAME = "GlimmerWorks";

static constexpr float GRAVITY_SCALE = 1.8F; // 重力缩放（优化下落手感）
static constexpr int JUMP_BUFFER_FRAMES = 4;
static constexpr int FLY_SPEED = TILE_SIZE * 25;

//How often should the information of picked-up items be accumulated and settled (unit: seconds)?
//累积多长时间结算一次捡起的物品信息（单位：秒）
static constexpr float MERGE_DURATION = 0.35F;
static constexpr int LIGHT_CONTRIBUTION_CENTER_RAY_INDEX = -1;

#endif //CONSTANTS_H
