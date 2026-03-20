# GlimmerWorks
[Discord](https://discord.gg/CfppC9WHw8) | [QQ 频道](https://pd.qq.com/s/cntb09fr1?b=9)

**GlimmerWorks** 是一款使用 C++ 与现代库开发的 2D 像素沙盒游戏，提供高度可定制、自由开放的游戏体验。

## 游戏特性
- **精美的 2D 像素画面**，搭配流畅自然的动画
- **沙盒玩法**，支持玩家自由探索、合成与建造
- **强大的模组扩展支持**，可自定义资源与游戏内容
- **跨平台兼容性**，目前已针对 Linux 优化，其他平台规划中
- **开源且由社区驱动**，基于 [GNU Affero General Public License](LICENSE) 授权，鼓励贡献与修改

## 依赖库
GlimmerWorks 使用以下开源库：
- [SDL3](https://github.com/libsdl-org/SDL) – 负责图形、输入与窗口管理
- [Dear ImGui](https://github.com/ocornut/imgui) – 用于界面绘制与调试工具
- [Tweeny](https://github.com/mobius3/tweeny) – 实现平滑动画与过渡效果
- [fmt](https://github.com/fmtlib/fmt) – 安全高效的字符串格式化
- [nlohmann/json](https://github.com/nlohmann/json) – 配置与数据序列化
- [FastNoiseLite](https://github.com/Auburn/FastNoiseLite) – 程序化地形与噪声生成
- [box2d](https://github.com/erincatto/box2d) – 2D 物理模拟，包括刚体、碰撞与关节

## 从源码编译
请查看 [编译指南](https://github.com/Cold-Mint/GlimmerWorks/wiki/Build-Guide)

## 参与贡献
欢迎任何形式的贡献！你可以通过以下方式帮助项目：
- 提交 Bug 报告与功能建议
- 制作模组、美术资源或语言包
- 优化代码与完善文档

### 代码风格 — Glimmer C++ 命名规范
为保持项目整洁、易读且风格统一，所有 C++ 代码请遵循以下命名规则：

| 元素类型                | 命名风格                          | 示例                                         |
|-------------------------|-----------------------------------|----------------------------------------------|
| **命名空间**            | 小写 或 小写加下划线              | `glimmer`、`glimmer::ecs`、`glimmer::world`  |
| **类 / 结构体 / 枚举**  | PascalCase（大驼峰）              | `GameEntity`、`WorldContext`、`RenderSystem` |
| **函数 / 方法**         | PascalCase（大驼峰）              | `Update()`、`Render()`、`GetID()`            |
| **局部变量 / 函数参数** | camelCase（小驼峰）               | `deltaTime`、`playerPosition`                |
| **成员变量**            | camelCase（小驼峰）+ 末尾下划线 `_` | `id_`、`seed_`、`heightMapNoise_`            |
| **常量 / 宏定义**       | SCREAMING_SNAKE_CASE（全大写下划线） | `MAX_ENTITIES`、`CHUNK_SIZE`                 |
| **模板参数**            | PascalCase（大驼峰）              | `template <typename ComponentType>`          |

---

#### 命名空间
- 统一使用 **小写** 命名，命名空间表示模块，而非类型。
- 嵌套命名空间使用 `::` 语法组织结构：
  ```cpp
  namespace glimmer::ecs {
      class GameSystem {};
  }
  namespace glimmer::render {
      class Renderer {};
  }
  ```

## 开源协议
本项目基于 [GNU Affero General Public License](LICENSE) 开源协议授权 – 详情请查看 LICENSE 文件。
***
# GlimmerWorks

[Discord](https://discord.gg/CfppC9WHw8) | [QQ Channel](https://pd.qq.com/s/cntb09fr1?b=9)

**GlimmerWorks** is a 2D pixel sandbox game built with C++ and modern libraries, offering a customizable and open-ended
gameplay experience.

## Features

- **Charming 2D pixel graphics** with smooth, fluid animations
- **Sandbox gameplay** that lets players explore, craft, and build freely
- **Extensible Mod support** with customizable assets and content
- **Cross-platform compatibility**, currently optimized for Linux, with other platforms planned
- **Open-source and community-driven**, licensed under [GNU Affero General Public License](LICENSE), encouraging
  contributions and modifications

## Dependencies

GlimmerWorks uses the following libraries:

- [SDL3](https://github.com/libsdl-org/SDL) – for graphics, input, and window management
- [Dear ImGui](https://github.com/ocornut/imgui) – for GUI overlays and debugging tools
- [Tweeny](https://github.com/mobius3/tweeny) – for smooth animations and transitions
- [fmt](https://github.com/fmtlib/fmt) – for safe and efficient string formatting
- [nlohmann/json](https://github.com/nlohmann/json) – for configuration and data serialization
- [FastNoiseLite](https://github.com/Auburn/FastNoiseLite) – for procedural terrain and noise generation
- [box2d](https://github.com/erincatto/box2d) – for 2D physics simulation, including rigid bodies, collisions, and
  joints

## Building from Source

See [Build-Guide](https://github.com/Cold-Mint/GlimmerWorks/wiki/Build-Guide)

## Contributing

Contributions are welcome! You can help by:

* Submitting bug reports and feature requests
* Creating mods, assets, or localization packs
* Improving the codebase and documentation

### Code Style — Glimmer C++ Naming Conventions

To keep the Glimmer project clean, readable, and consistent, all C++ code should follow these naming conventions.

| Element Type                       | Naming Style                            | Example                                      |
|------------------------------------|-----------------------------------------|----------------------------------------------|
| **Namespaces**                     | lowercase or lowercase_with_underscores | `glimmer`, `glimmer::ecs`, `glimmer::world`  |
| **Classes / Structs / Enums**      | PascalCase                              | `GameEntity`, `WorldContext`, `RenderSystem` |
| **Functions / Methods**            | PascalCase                              | `Update()`, `Render()`, `GetID()`            |
| **Variables (local / parameters)** | camelCase                               | `deltaTime`, `playerPosition`                |
| **Member Variables**               | camelCase + trailing `_`                | `id_`, `seed_`, `heightMapNoise_`            |
| **Constants / Macros**             | SCREAMING_SNAKE_CASE                    | `MAX_ENTITIES`, `CHUNK_SIZE`                 |
| **Template Parameters**            | PascalCase                              | `template <typename ComponentType>`          |

---

#### Namespaces

- Always **lowercase** — namespaces represent modules, not types.
- Nested namespaces should use the `::` syntax for structure:
  ```cpp
  namespace glimmer::ecs {
      class GameSystem {};
  }
  namespace glimmer::render {
      class Renderer {};
  }

## License

This project is licensed under the [GNU Affero General Public License](LICENSE) License – see the LICENSE file for
details.
