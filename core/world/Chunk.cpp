//
// Created by Cold-Mint on 2025/10/24.
//

#include "Chunk.h"

#include <fstream>

#include "WorldContext.h"
#include "../log/LogCat.h"
#include "../saves/Saves.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"
#include "../utils/JsonUtils.h"

void Glimmer::Chunk::place(const Vector2D position, const std::string &tileId) {
    tileIDs[position.x][position.y] = tileId;
}

void Glimmer::Chunk::save() {
#if defined(NDEBUG)
    //todo:实现保存函数(保存二进制)
#else
    namespace fs = std::filesystem;

    fs::path chunksDir = worldContext->saves->getPath() / "chunks";
    if (!fs::exists(chunksDir)) {
        fs::create_directories(chunksDir);
    }
    fs::path chunkPath = chunksDir / ("chunk_" + std::to_string(origin.x) + "_" + std::to_string(origin.y) + ".json");
    nlohmann::json chunkData;
    chunkData["origin"] = origin;
    chunkData["tiles"] = tileIDs; // vector<vector<string>> 自动序列化

    fs::create_directories(chunkPath.parent_path());
    std::ofstream file(chunkPath);
    if (!file.is_open()) {
        LogCat::e("Chunk cannot be saved", chunkPath.string());
        return;
    }

    file << chunkData.dump(2); // 缩进 2 格，便于调试查看
    file.close();

    LogCat::d("Saved chunk:", chunkPath.string());
#endif
}

void Glimmer::Chunk::load() {
}
