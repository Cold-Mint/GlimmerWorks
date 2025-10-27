# GlimmerWorks

**GlimmerWorks** is a 2D pixel sandbox game built with C++ and modern libraries, offering a customizable and open-ended gameplay experience.

## Features

- **Charming 2D pixel graphics** with smooth, fluid animations
- **Sandbox gameplay** that lets players explore, craft, and build freely
- **Extensible Mod support** with customizable assets and content
- **Cross-platform compatibility**, currently optimized for Linux, with other platforms planned
- **Open-source and community-driven**, licensed under [GNU Affero General Public License](LICENSE), encouraging contributions and modifications


## Dependencies

GlimmerWorks uses the following libraries:

- [SDL3](https://github.com/libsdl-org/SDL) – for graphics, input, and window management  
- [Dear ImGui](https://github.com/ocornut/imgui) – for GUI overlays and debugging tools  
- [Tweeny](https://github.com/mobius3/tweeny) – for smooth animations and transitions  
- [fmt](https://github.com/fmtlib/fmt) – for safe and efficient string formatting  
- [nlohmann/json](https://github.com/nlohmann/json) – for configuration and data serialization  

## Building from Source (Linux)

### 1. Clone the repository
```bash
git clone https://github.com/Cold-Mint/GlimmerWorks.git
cd GlimmerWorks
````

### 2. Create a Release build with Ninja

```bash
mkdir -p build_release
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -S . -B build_release
cmake --build build_release
```

### 3. Run the game

```bash
./build_release/GlimmerWorks
```

## Contributing

Contributions are welcome! You can help by:

* Submitting bug reports and feature requests
* Creating mods, assets, or localization packs
* Improving the codebase and documentation


## License

This project is licensed under the [GNU Affero General Public License](LICENSE) License – see the LICENSE file for details.
