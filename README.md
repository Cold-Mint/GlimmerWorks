# GlimmerWorks

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
