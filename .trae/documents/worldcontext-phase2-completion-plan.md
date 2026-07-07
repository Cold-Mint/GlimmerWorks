# WorldContext 拆分重构 — Phase 2 完成计划

## Summary

承接之前会话已批准的 WorldContext 拆分工作（Phase 1 已完成并构建通过）。本计划仅完成**剩余的 Phase 2 收尾工作**：移除 `WorldContext.cpp` 中的 30 个转发方法实现，并更新 50 处外部调用方改用子对象 getter。

完成本计划后，`WorldContext.h` 将最终满足 SonarQube 规则：
- **S1448**：24 方法 ≤ 35（已达成，需 WorldContext.cpp 同步以保持编译一致）
- **S1820**：17 字段 ≤ 20（已达成）

用户已明确："仅处理 WorldContext 这个任务处理完成后结束，其他工作暂时不动。"

## Current State Analysis

### 已完成（Phase 1 + 头文件更新）

| 项 | 状态 |
|----|------|
| [core/world/ChunkManager.h](file:///home/coldmint/projects/GlimmerWorks/core/world/ChunkManager.h) / [.cpp](file:///home/coldmint/projects/GlimmerWorks/core/world/ChunkManager.cpp) | ✓ 已创建并编译通过 |
| [core/world/TerrainManager.h](file:///home/coldmint/projects/GlimmerWorks/core/world/TerrainManager.h) / [.cpp](file:///home/coldmint/projects/GlimmerWorks/core/world/TerrainManager.cpp) | ✓ 已创建并编译通过 |
| [core/world/SystemScheduler.h](file:///home/coldmint/projects/GlimmerWorks/core/world/SystemScheduler.h) / [.cpp](file:///home/coldmint/projects/GlimmerWorks/core/world/SystemScheduler.cpp) | ✓ 已创建并编译通过 |
| [core/world/PlayerContext.h](file:///home/coldmint/projects/GlimmerWorks/core/world/PlayerContext.h) / [.cpp](file:///home/coldmint/projects/GlimmerWorks/core/world/PlayerContext.cpp) | ✓ 已创建并编译通过 |
| [core/world/WorldContext.h](file:///home/coldmint/projects/GlimmerWorks/core/world/WorldContext.h) | ✓ **已更新为最终状态**：17 字段 / 24 方法（含 `GetLightingBuffer`/`GetTileInstancePool` 转发） |
| [CMakeLists.txt](file:///home/coldmint/projects/GlimmerWorks/CMakeLists.txt) | ✓ 已添加 4 个 .cpp 和 4 个 .h |
| 构造函数装配顺序 | ✓ 已正确：基础配置→核心资源→chunkLoader/Generator→pause/tileLayer→4 子对象→InitPlayer→InitHotbar/Inventory→InitSystem |
| 析构函数 reset 顺序 | ✓ 已正确：playerContext_.reset()→systemScheduler_.reset()→entityManager_->Clear()→chunkManager_.reset()→terrainManager_.reset()→b2DestroyWorld→UnbindWorldContext |

### 待完成（代码当前处于不一致状态，构建会失败）

1. **[core/world/WorldContext.cpp](file:///home/coldmint/projects/GlimmerWorks/core/world/WorldContext.cpp)**（481 行）：仍包含 30 个转发方法实现（L228-L403），但 `WorldContext.h` 已删除这些方法声明。需要移除这 30 个实现，保留 `GetLightingBuffer`（L267-L270）和 `GetTileInstancePool`（L272-L275）的转发。

2. **50 处外部调用方**：仍通过 `worldContext->Method()` 形式调用已删除的转发方法，需改为 `worldContext->GetChunkManager()->Method()` 等子对象 getter 链式调用。

## Proposed Changes

### Step 1: 更新 WorldContext.cpp — 移除 30 个转发方法实现

**删除**以下 4 组共 30 个转发实现（保留分组注释或一并清理）：

**ChunkManager 转发（7 个，L231-L265）**：
- `LoadChunkAt`, `UnloadChunkAt`, `GetChunk`, `GetAllChunks`, `HasChunk`, `SaveChunk`, `ChunkIsOutOfBounds`(static)

**TerrainManager 转发（5 个，L281-L305）**：
- `GetTerrainData`, `GetOrCreateTerrainData`, `GetTerrainResults`, `LoadTerrainAt`, `UnloadTerrainAt`

**SystemScheduler 转发（15 个，L311-L384）**：
- `PushGuiSystemType`, `PushPersistentGuiSystem`, `PopGuiSystemType`, `GetGuiSystemType`, `GetAllActiveSystemType`, `HasAnyModalGuiOpen`, `HandleEvent`, `Update`, `OnBackPressed`, `Render`, `RenderImGui`, `OnFrameStart`, `InitSystem`, `OnConfigChanged`, `OnWindowSizeChanged`

**PlayerContext 转发（3 个，L390-L403）**：
- `InitPlayer`, `InitHotbar`, `InitInventory`

**保留**：
- `GetLightingBuffer`（L267-L270）和 `GetTileInstancePool`（L272-L275）— 这两个转发覆盖 9 处外部调用，避免改动 6 个额外文件
- 构造函数（L409-L461）中所有对子对象方法的调用（如 `playerContext_->InitPlayer(...)`、`systemScheduler_->InitSystem()`）— 这些是内部使用，已正确

**清理**：移除不再需要的 `#include`（如果有的话）。检查 `#include "core/ecs/GameSystem.h"`（仅 SystemScheduler 转发使用 `SDL_Event`/`SDL_Renderer`/`GameSystemType`）— 但 `SaveEntity` 仍使用 `EntityItemMessage`，谨慎评估，保守起见保留所有现有 include。

### Step 2: 更新 ChunkManager 外部调用方（17 处 / 6 文件）

| 文件 | 行号 | 当前代码 | 改为 |
|------|------|----------|------|
| [core/ecs/system/ChunkSystem.cpp](file:///home/coldmint/projects/GlimmerWorks/core/ecs/system/ChunkSystem.cpp) | L83 | `worldContext->LoadChunkAt(...)` | `worldContext->GetChunkManager()->LoadChunkAt(...)` |
| 同上 | L97 | `worldContext->LoadChunkAt(...)` | `worldContext->GetChunkManager()->LoadChunkAt(...)` |
| 同上 | L116 | `worldContext->UnloadChunkAt(...)` | `worldContext->GetChunkManager()->UnloadChunkAt(...)` |
| 同上 | L130 | `worldContext->UnloadChunkAt(...)` | `worldContext->GetChunkManager()->UnloadChunkAt(...)` |
| 同上 | L227 | `worldContext->GetChunk(...)` | `worldContext->GetChunkManager()->GetChunk(...)` |
| 同上 | L273 | `WorldContext::ChunkIsOutOfBounds(...)` | `ChunkManager::ChunkIsOutOfBounds(...)` |
| 同上 | L296 | `WorldContext::ChunkIsOutOfBounds(...)` | `ChunkManager::ChunkIsOutOfBounds(...)` |
| 同上 | L300 | `worldContext->HasChunk(...)` | `worldContext->GetChunkManager()->HasChunk(...)` |
| 同上 | L318 | `worldContext->GetAllChunks()` | `worldContext->GetChunkManager()->GetAllChunks()` |
| [core/ecs/system/DebugPanelSystem.cpp](file:///home/coldmint/projects/GlimmerWorks/core/ecs/system/DebugPanelSystem.cpp) | L297 | `worldContext->GetAllChunks()` | `worldContext->GetChunkManager()->GetAllChunks()` |
| [core/ecs/system/TileLayerSystem.cpp](file:///home/coldmint/projects/GlimmerWorks/core/ecs/system/TileLayerSystem.cpp) | L204 | `worldContext->GetChunk(...)` | `worldContext->GetChunkManager()->GetChunk(...)` |
| [core/ecs/component/TileLayerComponent.cpp](file:///home/coldmint/projects/GlimmerWorks/core/ecs/component/TileLayerComponent.cpp) | L46, L118, L155, L171 | `worldContext_->GetChunk(...)` | `worldContext_->GetChunkManager()->GetChunk(...)` |
| [core/console/command/PlaceCommand.cpp](file:///home/coldmint/projects/GlimmerWorks/core/console/command/PlaceCommand.cpp) | L149 | `worldContext->GetChunk(...)` | `worldContext->GetChunkManager()->GetChunk(...)` |
| [core/console/command/TileSnapshotCommand.cpp](file:///home/coldmint/projects/GlimmerWorks/core/console/command/TileSnapshotCommand.cpp) | L135 | `worldContext->GetChunk(...)` | `worldContext->GetChunkManager()->GetChunk(...)` |

**需添加 include**：每个文件需添加 `#include "core/world/ChunkManager.h"`（因为调用 `ChunkManager::ChunkIsOutOfBounds` 或 `GetChunkManager()->Method()` 需要完整类型）。

**例外**：TileLayerComponent.cpp 已通过 `worldContext_->GetChunkManager()` 调用，需确认是否已有 ChunkManager.h include；若没有则添加。

### Step 3: 更新 TerrainManager 外部调用方（9 处 / 3 文件）

| 文件 | 行号 | 当前代码 | 改为 |
|------|------|----------|------|
| [core/ecs/system/ChunkSystem.cpp](file:///home/coldmint/projects/GlimmerWorks/core/ecs/system/ChunkSystem.cpp) | L49 | `worldContext->LoadTerrainAt(...)` | `worldContext->GetTerrainManager()->LoadTerrainAt(...)` |
| 同上 | L63 | `worldContext->LoadTerrainAt(...)` | `worldContext->GetTerrainManager()->LoadTerrainAt(...)` |
| 同上 | L149 | `worldContext->UnloadTerrainAt(...)` | `worldContext->GetTerrainManager()->UnloadTerrainAt(...)` |
| 同上 | L163 | `worldContext->UnloadTerrainAt(...)` | `worldContext->GetTerrainManager()->UnloadTerrainAt(...)` |
| 同上 | L339 | `worldContext->GetTerrainResults()` | `worldContext->GetTerrainManager()->GetTerrainResults()` |
| [core/ecs/system/BiomeBGMSystem.cpp](file:///home/coldmint/projects/GlimmerWorks/core/ecs/system/BiomeBGMSystem.cpp) | L82 | `worldContext->GetTerrainData(...)` | `worldContext->GetTerrainManager()->GetTerrainData(...)` |
| [core/world/generator/ChunkGenerator.cpp](file:///home/coldmint/projects/GlimmerWorks/core/world/generator/ChunkGenerator.cpp) | L179 | `worldContext_->GetOrCreateTerrainData(...)` | `worldContext_->GetTerrainManager()->GetOrCreateTerrainData(...)` |
| 同上 | L315 | `worldContext_->GetOrCreateTerrainData(...)` | `worldContext_->GetTerrainManager()->GetOrCreateTerrainData(...)` |
| 同上 | L615 | `worldContext_->GetTerrainData(...)` | `worldContext_->GetTerrainManager()->GetTerrainData(...)` |

**需添加 include**：`#include "core/world/TerrainManager.h"`

### Step 4: 更新 SystemScheduler 外部调用方（24 处 / 5 文件）

| 文件 | 行号 | 当前代码 | 改为 |
|------|------|----------|------|
| [core/scene/WorldScene.cpp](file:///home/coldmint/projects/GlimmerWorks/core/scene/WorldScene.cpp) | L48 | `worldContext_->OnFrameStart()` | `worldContext_->GetSystemScheduler()->OnFrameStart()` |
| 同上 | L57 | `worldContext_->HasAnyModalGuiOpen()` | `worldContext_->GetSystemScheduler()->HasAnyModalGuiOpen()` |
| 同上 | L63 | `worldContext_->PushGuiSystemType(...)` | `worldContext_->GetSystemScheduler()->PushGuiSystemType(...)` |
| 同上 | L66 | `worldContext_->HandleEvent(...)` | `worldContext_->GetSystemScheduler()->HandleEvent(...)` |
| 同上 | L75 | `worldContext_->OnBackPressed()` | `worldContext_->GetSystemScheduler()->OnBackPressed()` |
| 同上 | L93 | `worldContext_->Update(delta)` | `worldContext_->GetSystemScheduler()->Update(delta)` |
| 同上 | L102 | `worldContext_->OnWindowSizeChanged(...)` | `worldContext_->GetSystemScheduler()->OnWindowSizeChanged(...)` |
| 同上 | L111 | `worldContext_->OnConfigChanged(...)` | `worldContext_->GetSystemScheduler()->OnConfigChanged(...)` |
| 同上 | L120 | `worldContext_->Render(renderer)` | `worldContext_->GetSystemScheduler()->Render(renderer)` |
| 同上 | L129 | `worldContext_->RenderImGui(renderer)` | `worldContext_->GetSystemScheduler()->RenderImGui(renderer)` |
| [core/console/command/EcsCommand.cpp](file:///home/coldmint/projects/GlimmerWorks/core/console/command/EcsCommand.cpp) | L157 | `worldContext->GetAllActiveSystemType()` | `worldContext->GetSystemScheduler()->GetAllActiveSystemType()` |
| [core/ecs/system/MaterialSelectCraftUISystem.cpp](file:///home/coldmint/projects/GlimmerWorks/core/ecs/system/MaterialSelectCraftUISystem.cpp) | L81, L88, L95, L147, L154, L230, L237, L271, L418, L425, L454 | `worldContext->PopGuiSystemType()` / `GetWorldContext()->PopGuiSystemType()` | `worldContext->GetSystemScheduler()->PopGuiSystemType()` / `GetWorldContext()->GetSystemScheduler()->PopGuiSystemType()` |
| [core/ecs/system/GUISystem.cpp](file:///home/coldmint/projects/GlimmerWorks/core/ecs/system/GUISystem.cpp) | L43 | `worldContext->GetGuiSystemType()` | `worldContext->GetSystemScheduler()->GetGuiSystemType()` |
| [core/ecs/system/CraftPreviewSlotSystem.cpp](file:///home/coldmint/projects/GlimmerWorks/core/ecs/system/CraftPreviewSlotSystem.cpp) | L230 | `worldContext->PushGuiSystemType(...)` | `worldContext->GetSystemScheduler()->PushGuiSystemType(...)` |

**需添加 include**：`#include "core/world/SystemScheduler.h"`

**注意**：`PushPersistentGuiSystem` 无外部调用（仅 SystemScheduler.cpp 内部使用），所以无需更新外部调用方。

### Step 5: PlayerContext 外部调用方（0 处）

无需更新外部调用方。仅删除 WorldContext.cpp 中的 3 个转发实现即可。

### Step 6: 构建验证

执行 `cmake --build build`，修复任何编译错误。预期可能问题：
- 遗漏的 include（最常见）
- 静态方法调用语法（`WorldContext::ChunkIsOutOfBounds` → `ChunkManager::ChunkIsOutOfBounds` 需 ChunkManager.h）
- WorldContext.cpp 中可能遗留未使用的 include（编译警告但不会失败）

## Assumptions & Decisions

1. **保留 `GetLightingBuffer` / `GetTileInstancePool` 转发**：避免改动 6 个额外文件中的 9 处调用（ResourceLocator.cpp, Light2DSystem.cpp, ChunkGenerator.cpp, Chunk.cpp, TileLayerSystem.cpp, DebugPanelSystem.cpp, LightCommand.cpp, GiveCommand.cpp, PlaceCommand.cpp）。这两个方法不计入 S1448 超限（24 方法 ≤ 35）。
2. **`PushPersistentGuiSystem` 无外部调用**：仅 SystemScheduler.cpp 内部调用，可直接删除 WorldContext 中的转发，无需更新外部调用方。
3. **PlayerContext 三个 Init 方法无外部调用**：仅 WorldContext 构造函数内部使用（已通过 `playerContext_->InitXxx()` 调用），可直接删除转发。
4. **静态方法 `ChunkIsOutOfBounds`**：调用方使用 `WorldContext::ChunkIsOutOfBounds(...)`，改为 `ChunkManager::ChunkIsOutOfBounds(...)`。需在调用方文件 include ChunkManager.h。
5. **不删除 WorldContext.cpp 中现有 include**：保守起见保留所有 `#include`（即使部分头文件不再被 WorldContext.cpp 直接使用），避免引入编译问题。如构建通过且有明显未使用 include 警告，可在最后清理。
6. **构造函数中的 `playerContext_->InitPlayer(...)` 等调用保持不变**：这些是 WorldContext 内部通过子对象指针直接调用，不是转发方法，无需修改。
7. **执行顺序**：按 Step 1→2→3→4→6 顺序执行，每个 Step 完成后可单独验证（但完整构建验证留到最后）。

## Verification

1. **编译验证**：`cmake --build build` 成功，无错误。允许有少量未使用 include 警告。
2. **字段数验证**：`WorldContext.h` 字段数 = 17（≤ 20，满足 S1820）。
3. **方法数验证**：`WorldContext.h` public + private 方法数 = 24（≤ 35，满足 S1448）：
   - 析构 + 构造：2
   - 核心访问器：9（GetEntityManager, GetEntityShortCut, GetSaves, GetMapManifest, GetAppContext, GetWorldId, GetWorldSeed, GetChunkGenerator, GetChunkLoader）
   - 子对象访问器：4（GetChunkManager, GetTerrainManager, GetSystemScheduler, GetPlayerContext）
   - 状态控制：4（IsRuning, SetRuning, IsDragMode, SetDragMode）
   - 实体/存档：3（SaveEntity, SaveGame, IsEmptyEntityId static）
   - 保留转发：2（GetLightingBuffer, GetTileInstancePool）
   - 总计：24
4. **转发方法清理验证**：`WorldContext.cpp` 中不再有 `chunkManager_->LoadChunkAt`、`terrainManager_->LoadTerrainAt`、`systemScheduler_->Update` 等转发实现（保留 `chunkManager_->GetLightingBuffer` 和 `chunkManager_->GetTileInstancePool`）。
5. **行为验证**（可选）：启动游戏，加载存档，确认玩家、区块、地形、GUI 系统正常工作。如无法运行，至少确认编译链接成功。

## 任务范围声明

按用户明确指示："仅处理 WorldContext 这个任务处理完成后结束，其他工作暂时不动。"

完成本 Phase 2 计划后即结束，不处理 sonarqube_issues.csv 中的其他问题。
