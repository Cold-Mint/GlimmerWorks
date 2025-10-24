//
// Created by Cold-Mint on 2025/10/24.
//

#include "Chunk.h"

#include <fstream>

#include "../log/LogCat.h"
#include "../saves/Saves.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"
#include "../utils/JsonUtils.h"

void Glimmer::Chunk::place(const Vector2D position, const std::string& tileId) {
    tileIDs[position.x][position.y] = tileId;
}

void Glimmer::Chunk::save() {
#if defined(NDEBUG)
    //todo:实现保存函数(保存二进制)
#else
    // ✅ Debug 模式：保存为 JSON 文件
    namespace fs = std::filesystem;

    // 文件路径形如：saves/world1/chunk_16_0.json
    fs::path chunkPath = worldContext->saves->getPath()
                         / ("chunk_" + std::to_string(origin.x)
                            + "_" + std::to_string(origin.y)
                            + ".json");

    // 构造 JSON 数据
    nlohmann::json chunkData;
    chunkData["origin"] = origin;
    chunkData["tiles"] = tileIDs; // vector<vector<string>> 自动序列化

    // 写入文件
    fs::create_directories(chunkPath.parent_path());
    std::ofstream file(chunkPath);
    if (!file.is_open()) {
        LogCat::e("无法保存区块：", chunkPath.string());
        return;
    }

    file << chunkData.dump(2); // 缩进 2 格，便于调试查看
    file.close();

    LogCat::d("Saved chunk:", chunkPath.string());
#endif
}

void Glimmer::Chunk::load() {

}
