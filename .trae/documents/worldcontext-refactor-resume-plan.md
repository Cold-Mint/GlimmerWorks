# WorldContext 拆分重构 - 续接计划

## Context

SonarQube 报告 `WorldContext` 类违反两条规则：
- **S1448**（64 方法，上限 35）
- **S1820**（33 字段，上限 20）

采用**组合子对象**方式拆分：把职责拆成 4 个 Manager 子类，WorldContext 以 `unique_ptr` 持有并暴露 getter。目标降至 17 字段 / 25 方法。

本计划承接之前会话已批准并部分实施的工作，仅完成剩余步骤。

## Current State Analysis（已完成进度）

### 已完成（Phase 1 步骤 1）

4 个子对象文件已全部创建（已验证内容与原计划一致）：

| 文件 | 字段 | 公有方法 | 私有方法 | 状态 |
|------|------|----------|----------|------|
| [core/world/ChunkManager.h](file:///home/coldmint/projects/GlimmerWorks/core/world/ChunkManager.h) / [.cpp](file:///home/coldmint/projects/GlimmerWorks/core/world/ChunkManager.cpp) | 7 | 9 | 3 | ✓ |
| [core/world/TerrainManager.h](file:///home/coldmint/projects/GlimmerWorks/core/world/TerrainManager.h) / [.cpp](file:///home/coldmint/projects/GlimmerWorks/core/world/TerrainManager.cpp) | 4 | 5 | 0 | ✓ |
| [core/world/SystemScheduler.h](file:///home/coldmint/projects/GlimmerWorks/core/world/SystemScheduler.h) / [.cpp](file:///home/coldmint/projects/GlimmerWorks/core/world/SystemScheduler.cpp) | 8 | 16 | 7 | ✓ |
| [core/world/PlayerContext.h](file:///home/coldmint/projects/GlimmerWorks/core/world/PlayerContext.h) / [.cpp](file:///home/coldmint/projects/GlimmerWorks/core/world/PlayerContext.cpp) | 4 | 3 | 5 | ✓（已修复 itemBreakSFX 初始化 bug） |

### 待完成（Phase 1 步骤 2-6）

- [core/world/WorldContext.h](file:///home/coldmint/projects/GlimmerWorks/core/world/WorldContext.h)：仍是原始 33 字段 / 64 方法状态
- [core/world/WorldContext.cpp](file:///home/coldmint/projects/GlimmerWorks/core/world/WorldContext.cpp)：仍是原始 1304 行，构造函数 L1230-1282，析构函数 L228-264（含 bug L258-263）
- [CMakeLists.txt](file:///home/coldmint/projects/GlimmerWorks/CMakeLists.txt)：未添加 4 个新 .cpp/.h（L363 是 WorldContext.cpp，L693 附近是头文件区）

## Proposed Changes

### Step 1: 更新 WorldContext.h

**删除**（已迁移到子对象的字段，共 16 个）：
- `chunkSnapshot_`, `lastChunkSnapshot_`（→ ChunkManager）
- `chunks_`, `chunksCache_`, `lightBuffer_`, `tileInstancePool_`（→ ChunkManager）
- `terrainTileData_`, `terrainTileDataCache_`, `processedTerrainTiles_`（→ TerrainManager）
- `activeSystems_`, `inactiveSystems_`, `activeSystemStack_`, `persistentGuiSystemCount_`, `allowRegisterSystem`, `onComponentCountChangedId_`, `onComponentCountChangeBuffer_`（→ SystemScheduler）
- `itemCallback_`, `itemBreakSFXResult_`, `audioManager_`（→ PlayerContext）
- `itemEditorPanel_`（死字段，全项目无使用，直接删除）

**新增 4 个 unique_ptr 成员**（紧随 entityManager_/entityShortCut_ 之后，确保子对象先于核心资源析构）：
```cpp
std::unique_ptr<ChunkManager> chunkManager_;
std::unique_ptr<TerrainManager> terrainManager_;
std::unique_ptr<SystemScheduler> systemScheduler_;
std::unique_ptr<PlayerContext> playerContext_;
```

**删除**已迁移到子对象的私有方法声明：`OnChunkTileChange`, `UpdateTileLight`, `UpdateChunkLight`, `OnWatchedComponentChanged`, `NotifySystemsOfComponentChange`, `NotifyActiveSystems`, `NotifyInactiveSystems`, `RegisterSystem`, `MoveSystemsToActive`, `MoveSystemsToInactive`, `CreateOrLoadPlayer`, `InitPlayerInventory`, `OnPlayerItemChanged`, `HandleItemBreak`, `DropComposableItemAbilities`

**保留并改为转发**（Phase 1 期间保留转发以保持外部调用方不变）：
- 区块相关：`LoadChunkAt`, `UnloadChunkAt`, `GetChunk`, `GetAllChunks`, `HasChunk`, `SaveChunk`, `ChunkIsOutOfBounds`(static→转发到 ChunkManager::ChunkIsOutOfBounds), `GetLightingBuffer`, `GetTileInstancePool`
- 地形相关：`GetTerrainData`, `GetOrCreateTerrainData`, `GetTerrainResults`, `LoadTerrainAt`, `UnloadTerrainAt`
- 系统调度相关：`PushGuiSystemType`, `PushPersistentGuiSystem`, `PopGuiSystemType`, `GetGuiSystemType`, `GetAllActiveSystemType`, `HasAnyModalGuiOpen`, `HandleEvent`, `Update`, `OnBackPressed`, `Render`, `RenderImGui`, `OnFrameStart`, `InitSystem`, `OnConfigChanged`, `OnWindowSizeChanged`
- 玩家相关：`InitPlayer`, `InitHotbar`, `InitInventory`

**新增 getter**：
```cpp
[[nodiscard]] ChunkManager* GetChunkManager() const;
[[nodiscard]] TerrainManager* GetTerrainManager() const;
[[nodiscard]] SystemScheduler* GetSystemScheduler() const;
[[nodiscard]] PlayerContext* GetPlayerContext() const;
[[nodiscard]] ChunkLoader* GetChunkLoader() const;  // chunkLoader_ 原无 getter
```

**保留的核心方法**（不转发）：`~WorldContext`, `GetEntityManager`, `GetEntityShortCut`, `GetSaves`, `GetMapManifest`, `GetAppContext`, `GetWorldId`, `GetWorldSeed`, `GetChunkGenerator`, `IsRuning`, `SetRuning`, `IsDragMode`, `SetDragMode`, `IsEmptyEntityId`(static), `SaveGame`, `SaveEntity`, 构造函数

**清理头文件**：移除已不再直接使用的的前向声明（`ParallaxBackgroundComponent` 若已不用），保留必要的 `#include`。子对象类型用前向声明即可（成员是 unique_ptr）。

最终 WorldContext.h 应为：**17 字段 / 25 方法**（含构造、析构、4 个子对象 getter、GetChunkLoader）。

### Step 2: 更新 WorldContext.cpp 构造函数（L1230-1282）

**装配顺序调整**（关键：4 个子对象必须在 entityManager_/chunkLoader_/chunkGenerator_ 之后、InitPlayer/InitHotbar/InitInventory/InitSystem 之前创建）：

```cpp
// 1. 基础配置
worldSeed_ = mapManifest->seed;
mapManifest_ = mapManifest;
b2WorldDef worldDef = b2DefaultWorldDef();
worldDef.gravity = b2Vec2(0.0F, -10.0F);
worldId_ = b2CreateWorld(&worldDef);
appContext_ = appContext;
appContext->GetCommandManager()->BindWorldContext(this);
appContext_->GetBiomeDecoratorManager()->SetWorldSeed(worldSeed_);

// 2. 核心资源
entityManager_ = std::make_unique<EntityManager>();
entityShortCut_ = std::make_unique<EntityShortCut>();
entityManager_->SetEntityIndex(mapManifest_->entityIDIndex);

// 3. 共享资源（保留在 WorldContext 以避免循环依赖）
chunkLoader_ = std::make_unique<ChunkLoader>(this, saves);
chunkGenerator_ = std::make_unique<ChunkGenerator>(this, worldSeed_);
startTime_ = TimeUtils::GetCurrentTimeMs();

// 4. pause / tileLayer 实体
auto pause = entityManager_->AddEntity();
entityManager_->AddComponent<PauseComponent>(pause);
// ... groundTileLayerEntity / backgroundTileLayerEntity 创建

// 5. 4 个子对象（在玩家/系统初始化前创建）
chunkManager_ = std::make_unique<ChunkManager>(this);
terrainManager_ = std::make_unique<TerrainManager>(this);
systemScheduler_ = std::make_unique<SystemScheduler>(this);
playerContext_ = std::make_unique<PlayerContext>(this);

// 6. 玩家初始化（通过 PlayerContext）
playerContext_->InitPlayer(playerResourceRef);
auto itemContainerPtr = entityManager_->GetComponent<ItemContainerComponent>(entityShortCut_->GetPlayer());
entityShortCut_->SetItemContainerComponent(itemContainerPtr);
if (itemContainer != nullptr) {
    playerContext_->InitHotbar(itemContainer);
    playerContext_->InitInventory(itemContainer);
}

// 7. 系统初始化（通过 SystemScheduler）
systemScheduler_->InitSystem();
```

**删除**：
- `lightBuffer_ = std::make_unique<LightBuffer>();`（已移入 ChunkManager 构造函数）
- `tileInstancePool_ = std::make_unique<TileInstancePool>();`（已移入 ChunkManager 构造函数）
- `onComponentCountChangedId_ = entityManager_->RegisterOnComponentCountChanged(...)`（已移入 SystemScheduler 构造函数）

**删除已迁移方法的实现**：从 .cpp 中移除所有已迁移到子对象的方法体（约 1000 行），保留转发方法实现（每个 1-3 行）。

### Step 3: 更新 WorldContext.cpp 析构函数（L228-264）

使用显式 `reset()` 控制顺序（确保回调注销在 entityManager_ 销毁前完成）：

```cpp
WorldContext::~WorldContext() {
    playerContext_.reset();      // 注销 itemCallback_（entityManager_ 仍存活）
    systemScheduler_.reset();    // 注销 onComponentCountChangedId_（entityManager_ 仍存活）
    if (entityManager_) entityManager_->Clear();
    chunkManager_.reset();       // 清空 chunks_/lightBuffer_/tileInstancePool_
    terrainManager_.reset();     // 清空 terrainTileData_
    if (b2WorldId_IsValid(worldId_)) {
        b2DestroyWorld(worldId_);
        worldId_ = b2_nullWorldId;
    }
    if (appContext_) appContext_->GetCommandManager()->UnbindWorldContext();
}
```

**删除**原析构函数 L258-263 的 bug 赋值（itemBreakSFXResult_/audioManager_ 已在 PlayerContext 构造函数正确初始化）。

### Step 4: 实现 SaveGame 的转发（[WorldContext.cpp:1170-1217](file:///home/coldmint/projects/GlimmerWorks/core/world/WorldContext.cpp#L1170-L1217)）

`SaveGame` 调用 `GetAllChunks()` 和 `SaveChunk(pos)`，改为：
```cpp
auto allChunks = chunkManager_->GetAllChunks();
for (const auto& pos : *allChunks | std::views::keys) {
    (void)chunkManager_->SaveChunk(pos);
}
```

### Step 5: 更新 CMakeLists.txt

- L363 后添加 4 个 .cpp：
  ```
  core/world/ChunkManager.cpp
  core/world/TerrainManager.cpp
  core/world/SystemScheduler.cpp
  core/world/PlayerContext.cpp
  ```
- 头文件区（L693 附近 PUBLIC FILE_SET HEADERS）添加 4 个 .h：
  ```
  core/world/ChunkManager.h
  core/world/TerrainManager.h
  core/world/SystemScheduler.h
  core/world/PlayerContext.h
  ```

### Step 6: 构建验证（Phase 1 完成）

执行 cmake 构建，修复任何编译错误。**行为应与重构前完全一致**（因为外部调用方仍使用 WorldContext 的转发方法）。

预期可能问题：
- 头文件循环依赖：WorldContext.h 需前向声明 4 个子对象类（unique_ptr 成员只需前向声明）
- WorldContext.cpp 需 `#include` 4 个子对象头文件（构造时 make_unique 需完整类型）
- 子对象 .cpp 已 include WorldContext.h，无循环（WorldContext.h 不 include 子对象 .h）

## Phase 2（可选，由难到简建议跳过）

移除 WorldContext 中的转发方法并更新外部调用方（约 48 处）。**建议本次先完成 Phase 1 并通过构建验证**，Phase 2 留待后续 PR。理由：
- Phase 1 后 WorldContext 已满足 S1448/S1820 规则（17 字段 / 25 方法）
- Phase 2 涉及 8+ 文件的调用点修改，风险与收益不成比例
- 转发方法的开销可忽略（编译期内联）

## Assumptions & Decisions

1. **chunkLoader_ / chunkGenerator_ 保留在 WorldContext**：作为共享资源避免 ChunkManager↔TerrainManager 循环依赖。原 6 处 `GetChunkGenerator()` 外部调用无需改动。
2. **GetLightingBuffer / GetTileInstancePool 保留为转发**：避免 9 处外部调用改动。
3. **SaveGame / SaveEntity 保留在 WorldContext**：SaveGame 协调多子对象，不宜下放。
4. **`itemEditorPanel_` 直接删除**：全项目无使用（grep 已确认）。
5. **Phase 2 不在本任务范围内**：仅完成 Phase 1 即可满足 SonarQube 规则。

## Verification

1. **编译验证**：`cmake --build build` 成功，无错误无新警告
2. **字段数验证**：WorldContext.h 字段数 ≤ 20（目标 17）
3. **方法数验证**：WorldContext.h public+private 方法数 ≤ 35（目标 25）
4. **行为验证**：启动游戏，加载存档，确认玩家、区块、地形、GUI 系统正常工作
