# WorldContext 拆分重构计划

## Context

SonarQube 报告 `WorldContext` 类存在两个问题：
- **S1448**（64 方法，上限 35）：类过大，需拆分
- **S1820**（33 字段，上限 20）：字段过多，需拆分

`WorldContext` 是项目核心聚合根，被 66+ 文件引用。采用**组合子对象**方式：把职责拆成 4 个 Manager 子类，WorldContext 以 `unique_ptr` 持有并暴露 getter。高频核心访问器（`GetEntityManager`/`GetEntityShortCut`/`GetSaves`/`GetWorldId` 等，132 处调用）保留在 WorldContext，仅迁移区块/地形/系统调度/玩家相关逻辑（~48 处外部调用）。

预期结果：WorldContext 降至 17 字段 / 25 方法，满足两个规则上限。

## 设计总览

### 子对象分配

| 子对象 | 字段 | 方法 | 说明 |
|--------|------|------|------|
| **ChunkManager** | chunks_, chunksCache_, chunkSnapshot_, lastChunkSnapshot_, tileInstancePool_, lightBuffer_, worldContext_ | LoadChunkAt, UnloadChunkAt, GetChunk, GetAllChunks, HasChunk, SaveChunk, ChunkIsOutOfBounds(static), GetLightingBuffer, GetTileInstancePool, OnChunkTileChange(private), UpdateTileLight(private), UpdateChunkLight(private) | 区块存储+光照+tileInstancePool |
| **TerrainManager** | terrainTileData_, terrainTileDataCache_, processedTerrainTiles_, worldContext_ | GetTerrainData, GetOrCreateTerrainData, GetTerrainResults, LoadTerrainAt, UnloadTerrainAt | 地形数据 |
| **SystemScheduler** | activeSystems_, inactiveSystems_, activeSystemStack_, persistentGuiSystemCount_, allowRegisterSystem, onComponentCountChangedId_, onComponentCountChangeBuffer_, worldContext_ | RegisterSystem(private), MoveSystemsToActive(private), MoveSystemsToInactive(private), PushGuiSystemType, PushPersistentGuiSystem, PopGuiSystemType, GetGuiSystemType, GetAllActiveSystemType, HasAnyModalGuiOpen, HandleEvent, Update, OnBackPressed, Render, RenderImGui, OnFrameStart, InitSystem, OnConfigChanged, OnWindowSizeChanged, OnWatchedComponentChanged(private), NotifySystemsOfComponentChange(private), NotifyActiveSystems(private), NotifyInactiveSystems(private) + 析构 | 系统调度+GUI栈+组件变更通知 |
| **PlayerContext** | itemCallback_, itemBreakSFXResult_, audioManager_, worldContext_ | InitPlayer, CreateOrLoadPlayer(private), InitPlayerInventory(private), InitHotbar, InitInventory, OnPlayerItemChanged(private), HandleItemBreak(private), DropComposableItemAbilities(private) + 析构 | 玩家初始化+物品回调 |

### WorldContext 保留（17 字段 / 25 方法）

**字段**：worldSeed_, worldId_, mapManifest_, appContext_, saves_, entityManager_, entityShortCut_, chunkLoader_, chunkGenerator_, running, dragMode_, startTime_, saving_, chunkManager_, terrainManager_, systemScheduler_, playerContext_

**方法**：构造、析构、GetEntityManager、GetEntityShortCut、GetSaves、GetMapManifest、GetAppContext、GetWorldId、GetWorldSeed、GetChunkGenerator、GetChunkLoader(新增)、GetTileInstancePool(转发)、GetLightingBuffer(转发)、IsRuning、SetRuning、IsDragMode、SetDragMode、IsEmptyEntityId(static)、SaveGame、SaveEntity、GetChunkManager、GetTerrainManager、GetSystemScheduler、GetPlayerContext

**关键设计决策**：
- `chunkLoader_`/`chunkGenerator_` 保留在 WorldContext 作共享资源——避免 ChunkManager↔TerrainManager 循环依赖，6 个 `GetChunkGenerator()` 外部调用方无需改动
- 光照方法（OnChunkTileChange/UpdateTileLight/UpdateChunkLight）移入 ChunkManager——它们是私有方法，仅 LoadChunkAt/UnloadChunkAt 调用
- `GetLightingBuffer()`/`GetTileInstancePool()` 保留为 WorldContext 转发方法——避免 9 个外部调用方改动
- `SaveGame`/`SaveEntity` 保留在 WorldContext——SaveGame 协调多子对象，不宜下放
- 删除死字段 `itemEditorPanel_`（仅声明，全项目无使用）

### 修复潜在 bug

当前析构函数（WorldContext.cpp:258-263）在析构时才赋值 `itemBreakSFXResult_` 和 `audioManager_`，导致 `HandleItemBreak` 运行时它们永远为 null。移到 PlayerContext 构造函数中初始化。

## 实现步骤

采用两阶段策略：Phase 1 建子对象+保留转发（行为不变，可单独验证），Phase 2 移除转发+更新调用方。

### Phase 1：创建子对象 + 转发（行为保持）

1. **创建 4 个子对象头文件和实现**（core/world/ChunkManager.h/.cpp、TerrainManager.h/.cpp、SystemScheduler.h/.cpp、PlayerContext.h/.cpp）
   - 遵循 ChunkLoader.h 的模式：持有 `WorldContext* worldContext_`，构造函数接收 `WorldContext*`
   - 从 WorldContext.cpp 移入对应方法的实现代码
   - ChunkManager 构造函数：创建 lightBuffer_、tileInstancePool_
   - SystemScheduler 构造函数：注册 `onComponentCountChanged` 回调（entityManager_ 此时已存在）
   - PlayerContext 构造函数：加载 itemBreakSFX 音频资源、获取 audioManager_（修复 bug）

2. **更新 WorldContext.h**
   - 移除已迁移的字段和方法声明
   - 新增 4 个 `unique_ptr` 成员（chunkManager_/terrainManager_/systemScheduler_/playerContext_），声明在 entityManager_ 等核心字段之后（确保子对象先析构）
   - 保留的方法改为转发（如 `GetChunk` → `return chunkManager_->GetChunk(pos);`）
   - 新增 GetChunkManager/GetTerrainManager/GetSystemScheduler/GetPlayerContext
   - 新增 GetChunkLoader（chunkLoader_ 原无 getter）

3. **更新 WorldContext.cpp 构造函数**
   - 装配顺序：基础配置 → b2World → entityManager_/entityShortCut_ → BindWorldContext → chunkLoader_/chunkGenerator_ → pause/tileLayer 实体 → 4 个子对象 → PlayerContext::InitPlayer → InitHotbar/InitInventory → SystemScheduler::InitSystem
   - 删除原析构函数中 itemBreakSFXResult_/audioManager_ 的错误赋值

4. **更新 WorldContext.cpp 析构函数**（使用显式 reset() 控制顺序）
   ```
   playerContext_.reset();      // 注销 itemCallback_（entityManager_ 仍存活）
   systemScheduler_.reset();    // 注销 onComponentCountChangedId_（entityManager_ 仍存活）
   entityManager_->Clear();
   chunkManager_.reset();       // 清空 chunks_/lightBuffer_/tileInstancePool_
   terrainManager_.reset();     // 清空 terrainTileData_
   b2DestroyWorld(worldId_);
   appContext_->GetCommandManager()->UnbindWorldContext();
   ```

5. **更新 CMakeLists.txt**：在 L363 后加 4 个 .cpp，在 L693 后加 4 个 .h

6. **构建并验证**——行为应与重构前完全一致

### Phase 2：移除转发 + 更新外部调用方

按子对象分 4 步，每步独立可编译可验证：

**Step 2a — ChunkManager**（15 处，8 文件）
- 从 WorldContext 移除 LoadChunkAt/UnloadChunkAt/GetChunk/GetAllChunks/HasChunk/SaveChunk/ChunkIsOutOfBounds 的转发方法
- 改写调用方：`worldContext->LoadChunkAt(pos)` → `worldContext->GetChunkManager()->LoadChunkAt(pos)`
- 静态方法：`WorldContext::ChunkIsOutOfBounds(...)` → `ChunkManager::ChunkIsOutOfBounds(...)`
- 文件：ChunkSystem.cpp(12处)、TileLayerComponent.cpp(4处)、TileLayerSystem.cpp(1处)、TileSnapshotCommand.cpp、PlaceCommand.cpp、DebugPanelSystem.cpp、各 command(各1-2处)

**Step 2b — TerrainManager**（9 处，3 文件）
- 文件：ChunkSystem.cpp(5处)、BiomeBGMSystem.cpp(1处)、ChunkGenerator.cpp(3处)
- 改写：`worldContext->LoadTerrainAt(pos)` → `worldContext->GetTerrainManager()->LoadTerrainAt(pos)`

**Step 2c — SystemScheduler**（24 处，5 文件）
- 文件：WorldScene.cpp(10处)、MaterialSelectCraftUISystem.cpp(11处)、CraftPreviewSlotSystem.cpp(1处)、GUISystem.cpp(1处)、EcsCommand.cpp(1处)
- 改写：`GetWorldContext()->PushGuiSystemType(t)` → `GetWorldContext()->GetSystemScheduler()->PushGuiSystemType(t)`

**Step 2d — PlayerContext**（0 外部调用）
- 仅移除 WorldContext 中的转发方法，无外部文件改动

## 关键文件

- [WorldContext.h](file:///home/coldmint/projects/GlimmerWorks/core/world/WorldContext.h) / [WorldContext.cpp](file:///home/coldmint/projects/GlimmerWorks/core/world/WorldContext.cpp) — 核心重构
- [CMakeLists.txt](file:///home/coldmint/projects/GlimmerWorks/CMakeLists.txt) — 注册新文件（L363、L693 附近）
- [ChunkSystem.cpp](file:///home/coldmint/projects/GlimmerWorks/core/ecs/system/ChunkSystem.cpp) — 调用最密集（17 处）
- [WorldScene.cpp](file:///home/coldmint/projects/GlimmerWorks/core/scene/WorldScene.cpp) — 系统调度调用（10 处）
- [MaterialSelectCraftUISystem.cpp](file:///home/coldmint/projects/GlimmerWorks/core/ecs/system/MaterialSelectCraftUISystem.cpp) — GUI 栈调用（11 处）
- [ChunkLoader.h](file:///home/coldmint/projects/GlimmerWorks/core/world/generator/ChunkLoader.h) — 子对象代码模板
- [ChunkGenerator.cpp](file:///home/coldmint/projects/GlimmerWorks/core/world/generator/ChunkGenerator.cpp) — 调用 TerrainManager（3 处）

## 风险点

1. **析构顺序（最高风险）**：回调注销必须在 entityManager_->Clear() 之前。用显式 reset() 控制，子对象析构函数内注销回调。
2. **lambda 捕获 this**：SystemScheduler 的 onComponentCountChanged 回调和 PlayerContext 的 itemCallback_ 都捕获 this。子对象析构时先注销回调。
3. **ChunkGenerator 循环调用**：ChunkGenerator 调用 TerrainManager，TerrainManager 又调用 ChunkGenerator。此循环已存在（只是原来经过 WorldContext），重构后路径变为 `ChunkGenerator → worldContext_->GetTerrainManager() → worldContext_->GetChunkGenerator()`，逻辑不变。
4. **InitSystem 的 this 指针**：InitSystem 移到 SystemScheduler 后，构造 GameSystem 子类时传 `worldContext_`（SystemScheduler 持有的 WorldContext*），而非 SystemScheduler 自身。

## 验证

- [ ] Phase 1 完成后：构建通过，运行游戏验证行为不变（新建世界、加载存档、开关 GUI、暂停、退出）
- [ ] Step 2a：区块加载/卸载/保存正常
- [ ] Step 2b：地形生成正常
- [ ] Step 2c：GUI 推送/弹出、事件处理、渲染、暂停正常
- [ ] Step 2d：玩家初始化、物品破坏/掉落正常
- [ ] 全部完成后：SonarQube 扫描确认 S1448（≤35 方法）和 S1820（≤20 字段）已解决
