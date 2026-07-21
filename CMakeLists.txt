cmake_minimum_required(VERSION 3.31.6)
project(GlimmerWorks)
set(CMAKE_CXX_STANDARD 26)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Disable shared libs, use static only")

include("${CMAKE_CURRENT_SOURCE_DIR}/versions.cmake")
if (NOT DEFINED ENV{GITHUB_ACTIONS})
    if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/config.cmake")
        message(FATAL_ERROR "The file \"config.cmake\" was not found. Please copy \"config_template.cmake\", fill in its content, rename it to \"config.cmake\", and then run it again.")
    endif ()
    include("${CMAKE_CURRENT_SOURCE_DIR}/config.cmake")
endif ()

if (MSVC)
    # 禁用警告
    add_compile_options(/wd4834)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

if (NOT EXISTS ${PACK_SIGNER_EXE})
    message(FATAL_ERROR "The file pointed to by PACK_SIGNER_EXE does not exist.")
endif ()

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_BIT "64")
    set(LIB_DIR "lib64")
else ()
    set(ARCH_BIT "32")
    set(LIB_DIR "lib32")
endif ()
if (WIN32)
    #Place the dll and exe files in the same directory.
    #把dll和exe放到同级目录。
    set(LIB_DIR ".")
endif ()
set(CMAKE_INSTALL_RPATH "$ORIGIN/${LIB_DIR}")
set(CMAKE_PROJECT_HOMEPAGE_URL "https://github.com/Cold-Mint/GlimmerWorks")
set(CPACK_PACKAGE_VENDOR "Cold-Mint")
#Major version number
#主版本号
set(CPACK_PACKAGE_VERSION_MAJOR 0)
#Minor version number
#次要版本号
set(CPACK_PACKAGE_VERSION_MINOR 0)
#Patch version number
#补丁版本号
set(CPACK_PACKAGE_VERSION_PATCH 1)

math(EXPR GAME_VERSION_NUMBER
        "${CPACK_PACKAGE_VERSION_MAJOR} * 10000
   + ${CPACK_PACKAGE_VERSION_MINOR} * 100
   + ${CPACK_PACKAGE_VERSION_PATCH}"
)

set(GAME_VERSION_STRING
        "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${CMAKE_BUILD_TYPE}"
)


include(FetchContent)

set(SDL_THREADS ON)
set(SDL_CAMERA OFF)
set(SDL_TRAY OFF)
# JOYSTICK
# 游戏手柄
set(SDL_JOYSTICK OFF)
set(SDL_JACK OFF)
set(SDL_DISKAUDIO OFF)
set(SDL_DUMMYAUDIO OFF)
set(SDL_HAPTIC OFF)
set(SDL_HIDAPI OFF)
# SENSOR
# 传感器支持
set(SDL_SENSOR OFF)
set(SDL_POWER OFF)
set(SDL_VIRTUAL_JOYSTICK OFF)
set(SDL_X11 ${X11_SUPPORT})
set(SDL_X11_SHARED ${X11_SUPPORT})
set(SDL_X11_XCURSOR ${X11_SUPPORT})
set(SDL_X11_XDBE ${X11_SUPPORT})
set(SDL_X11_XFIXES ${X11_SUPPORT})
set(SDL_X11_XINPUT ${X11_SUPPORT})
set(SDL_X11_XRANDR ${X11_SUPPORT})
set(SDL_X11_XSCRNSAVER ${X11_SUPPORT})
set(SDL_X11_XSHAPE ${X11_SUPPORT})
set(SDL_X11_XSYNC ${X11_SUPPORT})
set(SDL_X11_XTEST ${X11_SUPPORT})

set(SDLMIXER_AIFF OFF)
set(SDLMIXER_WAVE OFF)
set(SDLMIXER_VOC OFF)
set(SDLMIXER_AU OFF)
set(SDLMIXER_FLAC OFF)
set(SDLMIXER_GME OFF)
set(SDLMIXER_MOD OFF)
set(SDLMIXER_MP3 OFF)
set(SDLMIXER_MIDI OFF)
set(SDLMIXER_OPUS OFF)
set(SDLMIXER_WAVPACK OFF)
set(SDLMIXER_VORBIS_STB ON)
set(SDLMIXER_VORBIS_VORBISFILE OFF)
set(SDLMIXER_VORBIS_TREMOR OFF)
set(SDLIMAGE_BACKEND_WIC OFF)
set(SDLIMAGE_BACKEND_IMAGEIO OFF)
set(SDLIMAGE_ANI OFF)
set(SDLIMAGE_AVIF OFF)
set(SDLIMAGE_BMP OFF)
set(SDLIMAGE_GIF OFF)
set(SDLIMAGE_JPG OFF)
set(SDLIMAGE_JXL OFF)
set(SDLIMAGE_LBM OFF)
set(SDLIMAGE_PCX OFF)
set(SDLIMAGE_PNM OFF)
set(SDLIMAGE_QOI OFF)
set(SDLIMAGE_SVG OFF)
set(SDLIMAGE_TGA OFF)
set(SDLIMAGE_TIF OFF)
set(SDLIMAGE_WEBP OFF)
set(SDLIMAGE_XCF OFF)
set(SDLIMAGE_XPM OFF)
set(SDLIMAGE_XV OFF)
set(SDLIMAGE_ANI_SAVE OFF)
set(SDLIMAGE_AVIF_SAVE OFF)
set(SDLIMAGE_BMP_SAVE OFF)
set(SDLIMAGE_GIF_SAVE OFF)
set(SDLIMAGE_JPG_SAVE OFF)
set(SDLIMAGE_TGA_SAVE OFF)
set(SDLIMAGE_WEBP_SAVE OFF)

# Read and save PNG
# 读取和保存png
set(SDLIMAGE_BACKEND_STB ON)
set(SDLIMAGE_PNG_SAVE ON)
set(SDLIMAGE_PNG ON)

set(FMT_TEST OFF)
set(FMT_FUZZ OFF)
set(FMT_CUDA_TEST OFF)
set(FMT_DOC OFF)
set(FMT_INSTALL OFF)

set(BOX2D_SAMPLES OFF)
set(BOX2D_BENCHMARKS OFF)
set(BOX2D_DOCS OFF)
set(BOX2D_PROFILE OFF)
set(BOX2D_VALIDATE OFF)
set(BOX2D_UNIT_TESTS OFF)
set(BOX2D_SANITIZE OFF)

if (X11_SUPPORT)
    set(SDL_WAYLAND OFF)
    set(SDL_WAYLAND_LIBDECOR OFF)
    set(SDL_WAYLAND_SHARED OFF)
    message(STATUS "Window backend: X11 (Wayland disabled)")
endif ()
FetchContent_Declare(
        SDL
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL"
        GIT_TAG ${LIBSDL-ORG_SDL_VERSION}
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(SDL)

if (ANDROID)
    find_library(ANDROID_LOG_LIB log)
    target_link_libraries(SDL3-shared PRIVATE ${ANDROID_LOG_LIB})
endif ()

FetchContent_Declare(
        SDL_image
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL_image"
        GIT_TAG ${LIBSDL-ORG_SDL_IMAGE_VERSION}
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(SDL_image)


FetchContent_Declare(
        SDL_ttf
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL_ttf"
        GIT_TAG ${LIBSDL-ORG_SDL_TTF_VERSION}
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(SDL_ttf)

FetchContent_Declare(
        SDL_mixer
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL_mixer"
        GIT_TAG ${LIBSDL-ORG_SDL_MIXER_VERSION}
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(SDL_mixer)

FetchContent_Declare(
        tweeny
        GIT_REPOSITORY "https://github.com/mobius3/tweeny"
        GIT_TAG ${MOBIUS3_TWEENY_VERSION}
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(tweeny)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY "https://github.com/fmtlib/fmt"
        GIT_TAG ${FMTLIB_FMT_VERSION}
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
        fastNoiseLite
        GIT_REPOSITORY "https://github.com/Auburn/FastNoiseLite"
        GIT_TAG ${AUBURN_FASTNOISELITE_VERSION}
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(fastNoiseLite)

FetchContent_Declare(
        box2d
        GIT_REPOSITORY "https://github.com/erincatto/box2d"
        GIT_TAG ${ERINCATTO_BOX2D_VERSION}
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(box2d)

FetchContent_Declare(
        toml11
        GIT_REPOSITORY "https://github.com/ToruNiina/toml11"
        GIT_TAG ${TORUNIINA_TOML11_VERSION}
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(toml11)

FetchContent_Declare(
        glimmerWorksProtobuf
        GIT_REPOSITORY "https://github.com/Cold-Mint/GlimmerWorksProtobuf"
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(glimmerWorksProtobuf)
target_include_directories(GlimmerWorksProto PRIVATE "${CMAKE_BINARY_DIR}/_deps/protobuf-src/third_party/utf8_range")

FetchContent_Declare(
        blake3
        GIT_REPOSITORY "https://github.com/BLAKE3-team/BLAKE3"
        GIT_TAG ${BLAKE3-TEAM_BLAKE3_VERSION}
        SOURCE_SUBDIR c
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(blake3)

FetchContent_Declare(
        monoCypher
        GIT_REPOSITORY "https://github.com/LoupVaillant/Monocypher"
        GIT_TAG ${LOUPVAILLANT_MONOCYPHER_VERSION}
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(monoCypher)

FetchContent_Declare(
        rmlUi
        GIT_REPOSITORY "https://github.com/mikke89/RmlUi"
        GIT_TAG ${MIKKE89_RMLUI_VERSION}
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(rmlUi)

if (ANDROID)
    message(STATUS "Building for Android → Shared Library (.so)")
    add_library(GlimmerWorks SHARED)
else ()
    message(STATUS "Building for Desktop → Executable")
    add_executable(GlimmerWorks)
endif ()

target_sources(GlimmerWorks
        PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/monocypher-src/src/optional/monocypher-ed25519.c"
        "${CMAKE_BINARY_DIR}/_deps/monocypher-src/src/monocypher.c"
        "${CMAKE_BINARY_DIR}/_deps/rmlui-src/Backends/RmlUi_Platform_SDL.cpp"
        "${CMAKE_BINARY_DIR}/_deps/rmlui-src/Backends/RmlUi_Renderer_SDL.cpp"
        core/App.cpp
        core/AppEventLoop.cpp
        core/AppRenderer.cpp
        core/BoolOrToggle.cpp
        core/Box2dFilter.cpp
        core/CommandHookEntry.cpp
        core/CommandHookManager.cpp
        core/Config.cpp
        core/GameUIMessage.cpp
        core/Hyperlink.cpp
        core/IFingerprintable.cpp
        core/SelectedItemRuntimeData.cpp
        core/TagRuntimeData.cpp
        core/TilePlacementConfig.cpp
        core/TileStateBackup.cpp
        core/console/Command.cpp
        core/console/CommandArgs.cpp
        core/console/CommandHistoryManager.cpp
        core/console/CommandManager.cpp
        core/console/CommandRequest.cpp
        core/console/CommandResponse.cpp
        core/console/CommandResult.cpp
        core/console/CommandSender.cpp
        core/console/CommandStructure.cpp
        core/console/CommandSuggestions.cpp
        core/console/ConsoleMessage.cpp
        core/console/ConsoleWorker.cpp
        core/console/LocalConsoleInput.cpp
        core/console/command/SceneCommand.cpp
        core/console/command/InputCommand.cpp
        core/console/command/GuiCommand.cpp
        core/console/asset_enumerator/AbilityItemsAssetEnumerator.cpp
        core/console/asset_enumerator/BiomeDecorsAssetEnumerator.cpp
        core/console/asset_enumerator/BiomesAssetEnumerator.cpp
        core/console/asset_enumerator/ComposableItemsAssetEnumerator.cpp
        core/console/asset_enumerator/FixedColorAssetEnumerator.cpp
        core/console/asset_enumerator/LightMaskAssetEnumerator.cpp
        core/console/asset_enumerator/LightSourceAssetEnumerator.cpp
        core/console/asset_enumerator/LootTablesAssetEnumerator.cpp
        core/console/asset_enumerator/MaterialItemsAssetEnumerator.cpp
        core/console/asset_enumerator/MobsAssetEnumerator.cpp
        core/console/asset_enumerator/RecipesAssetEnumerator.cpp
        core/console/asset_enumerator/ShapesAssetEnumerator.cpp
        core/console/asset_enumerator/StartInvAssetEnumerator.cpp
        core/console/asset_enumerator/StringAssetEnumerator.cpp
        core/console/asset_enumerator/StructuresAssetEnumerator.cpp
        core/console/asset_enumerator/TextureAssetEnumerator.cpp
        core/console/asset_enumerator/TileAssetEnumerator.cpp
        core/console/command/AssetViewerCommand.cpp
        core/console/command/BiomeScoreCommand.cpp
        core/console/command/Box2DCommand.cpp
        core/console/command/ClearCommand.cpp
        core/console/command/ConfigCommand.cpp
        core/console/command/EchoCommand.cpp
        core/console/command/EcsCommand.cpp
        core/console/command/FlyCommand.cpp
        core/console/command/GiveCommand.cpp
        core/console/command/HelpCommand.cpp
        core/console/command/HookCommand.cpp
        core/console/command/ItemEditorCommand.cpp
        core/console/command/LicenseCommand.cpp
        core/console/command/LightCommand.cpp
        core/console/command/LocateCommand.cpp
        core/console/command/LootCommand.cpp
        core/console/command/PackVerifyCommand.cpp
        core/console/command/ParallaxBackgroundCommand.cpp
        core/console/command/PlaceCommand.cpp
        core/console/command/PlayCommand.cpp
        core/console/command/ScreenshotCommand.cpp
        core/console/command/SeedCommand.cpp
        core/console/command/SummonCommand.cpp
        core/console/command/TagCommand.cpp
        core/console/command/TechnologyCommand.cpp
        core/console/command/TileSnapshotCommand.cpp
        core/console/command/TpCommand.cpp
        core/console/command/UnlockedRecipesCommand.cpp
        core/console/command/VFSCommand.cpp
        core/console/suggestion/AbilityItemDynamicSuggestions.cpp
        core/console/suggestion/AllocStrategyTypeDynamicSuggestions.cpp
        core/console/suggestion/AudioTrackDynamicSuggestions.cpp
        core/console/suggestion/BiomeSuggestions.cpp
        core/console/suggestion/BoolDynamicSuggestions.cpp
        core/console/suggestion/BooleanToggleDynamicSuggestions.cpp
        core/console/suggestion/CommandHookIdDynamicSuggestions.cpp
        core/console/suggestion/CommandHookScopeDynamicSuggestions.cpp
        core/console/suggestion/ComposableItemDynamicSuggestions.cpp
        core/console/suggestion/ConfigSuggestions.cpp
        core/console/suggestion/CoordinateDynamicSuggestions.cpp
        core/console/suggestion/DataPackDynamicSuggestions.cpp
        core/console/suggestion/DynamicSuggestions.cpp
        core/console/suggestion/DynamicSuggestionsManager.cpp
        core/console/suggestion/EventTypeDynamicSuggestions.cpp
        core/console/suggestion/LootSuggestions.cpp
        core/console/suggestion/MaterialItemDynamicSuggestions.cpp
        core/console/suggestion/MobDynamicSuggestions.cpp
        core/console/suggestion/MouseButtonDynamicSuggestions.cpp
        core/console/suggestion/ScanKeyDynamicSuggestions.cpp
        core/console/suggestion/StructureDynamicSuggestions.cpp
        core/console/suggestion/TileDynamicSuggestions.cpp
        core/console/suggestion/VFSDynamicSuggestions.cpp
        core/context/AppContext.cpp
        core/context/AudioContext.cpp
        core/context/ConsoleContext.cpp
        core/context/GraphicsContext.cpp
        core/context/ModContext.cpp
        core/context/RmlContext.cpp
        core/context/WindowContext.cpp
        core/contributor/Contributor.cpp
        core/contributor/ContributorManager.cpp
        core/ecs/DroppedItemCreator.cpp
        core/ecs/EntityManager.cpp
        core/ecs/EntityShortCut.cpp
        core/ecs/FlowingTextCreator.cpp
        core/ecs/GameComponent.cpp
        core/ecs/GameEntity.cpp
        core/ecs/GameSystem.cpp
        core/ecs/GameSystemType.cpp
        core/ecs/IEntityCreator.cpp
        core/ecs/IPersistenceEntityCreator.cpp
        core/ecs/ISwitchable.cpp
        core/ecs/IVisible.cpp
        core/ecs/MobEntityCreator.cpp
        core/ecs/PlayerCapabilityHandler.cpp
        core/ecs/PlayerInputHandler.cpp
        core/ecs/PlayerTechnologyHandler.cpp
        core/ecs/component/AreaMarkerComponent.cpp
        core/ecs/component/AutoPickComponent.cpp
        core/ecs/component/BlueprintComponent.cpp
        core/ecs/component/ButtonComponent.cpp
        core/ecs/component/CameraComponent.cpp
        core/ecs/component/CraftPreviewSlotComponent.cpp
        core/ecs/component/DebugDrawComponent.cpp
        core/ecs/component/DiggingComponent.cpp
        core/ecs/component/DraggableComponent.cpp
        core/ecs/component/DroppedItemComponent.cpp
        core/ecs/component/FloatingTextComponent.cpp
        core/ecs/component/HotBarComponent.cpp
        core/ecs/component/ItemContainerComponent.cpp
        core/ecs/component/ItemEditorComponent.cpp
        core/ecs/component/ItemSlotComponent.cpp
        core/ecs/component/ItemSlotQuantityComponent.cpp
        core/ecs/component/ItemToolTipComponent.cpp
        core/ecs/component/MagnetComponent.cpp
        core/ecs/component/MagneticComponent.cpp
        core/ecs/component/MobComponent.cpp
        core/ecs/component/ParallaxBackgroundComponent.cpp
        core/ecs/component/PauseComponent.cpp
        core/ecs/component/PlayerComponent.cpp
        core/ecs/component/RayCast2DComponent.cpp
        core/ecs/component/RigidBody2DComponent.cpp
        core/ecs/component/SpiritRendererComponent.cpp
        core/ecs/component/TechProviderComponent.cpp
        core/ecs/component/TileLayerComponent.cpp
        core/ecs/component/TilePlacementForbiddenZoneComponent.cpp
        core/ecs/component/Transform2DComponent.cpp
        core/ecs/system/AndroidControlSystem.cpp
        core/ecs/system/AreaMarkerSystem.cpp
        core/ecs/system/AutoPickSystem.cpp
        core/ecs/system/BiomeBGMSystem.cpp
        core/ecs/system/BlueprintSystem.cpp
        core/ecs/system/Box2dSystemContext.cpp
        core/ecs/system/ButtonSystem.cpp
        core/ecs/system/CameraSystem.cpp
        core/ecs/system/ChunkSystem.cpp
        core/ecs/system/CraftPreviewSlotSystem.cpp
        core/ecs/system/DebugDrawBox2dSystem.cpp
        core/ecs/system/DebugDrawSystem.cpp
        core/ecs/system/DebugMultiMapSystem.cpp
        core/ecs/system/DebugPanelSystem.cpp
        core/ecs/system/DiggingSystem.cpp
        core/ecs/system/DraggableSystem.cpp
        core/ecs/system/DroppedItemSystem.cpp
        core/ecs/system/FloatingTextSystem.cpp
        core/ecs/system/ItemSlotQuantitySystem.cpp
        core/ecs/system/ItemSlotSystem.cpp
        core/ecs/system/ItemToolTipSystem.cpp
        core/ecs/system/Light2DSystem.cpp
        core/ecs/system/MagnetSystem.cpp
        core/ecs/system/ParallaxBackgroundSystem.cpp
        core/ecs/system/PauseSystem.cpp
        core/ecs/system/PhysicsSystem.cpp
        core/ecs/system/PlayerControlSystem.cpp
        core/ecs/system/RayCast2DSystem.cpp
        core/ecs/system/SpiritRendererSystem.cpp
        core/ecs/system/TechProviderSystem.cpp
        core/ecs/system/TileLayerSystem.cpp
        core/inventory/AbilityItem.cpp
        core/inventory/AbilityItemCreateParams.cpp
        core/inventory/ComposableItem.cpp
        core/inventory/ComposableItemCreateParams.cpp
        core/inventory/ContainerChangeType.cpp
        core/inventory/CraftPreview.cpp
        core/inventory/InitialInventoryManager.cpp
        core/inventory/Item.cpp
        core/inventory/ItemAbilityFactory.cpp
        core/inventory/ItemContainer.cpp
        core/inventory/ItemDurabilityModule.cpp
        core/inventory/ItemLockModule.cpp
        core/inventory/ItemStackModule.cpp
        core/inventory/ItemTagModule.cpp
        core/inventory/MaterialItem.cpp
        core/inventory/TileItem.cpp
        core/inventory/ability/AreaMarkerAbility.cpp
        core/inventory/ability/ItemAbility.cpp
        core/inventory/ability/MiningAbility.cpp
        core/inventory/ability/MiningRangeData.cpp
        core/inventory/ability/MiningRangeDataPoint.cpp
        core/inventory/ability/NoneAbility.cpp
        core/log/LogCat.cpp
        core/lootTable/LootEntry.cpp
        core/lootTable/LootTableManager.cpp
        core/math/BackwardAllocStrategy.cpp
        core/math/BalanceAllocStrategy.cpp
        core/math/Color.cpp
        core/math/ColorF.cpp
        core/math/CoordinateTransformer.cpp
        core/math/DesignVector2D.cpp
        core/math/ForwardAllocStrategy.cpp
        core/math/IAllocStrategy.cpp
        core/math/IAllocatable.cpp
        core/math/RandomAllocStrategy.cpp
        core/math/ScreenVector2D.cpp
        core/math/TileVector2D.cpp
        core/math/Vector2D.cpp
        core/math/Vector2DBase.cpp
        core/math/Vector2DI.cpp
        core/math/Vector2DIBase.cpp
        core/math/Vector2DIHash.cpp
        core/math/WorldVector2D.cpp
        core/mod/ITemplateCommand.cpp
        core/mod/PackManifest.cpp
        core/mod/PackVerifyState.cpp
        core/mod/Resource.cpp
        core/mod/ResourceLocator.cpp
        core/mod/ResourceRef.cpp
        core/mod/StructurePlacementConditionsManager.cpp
        core/mod/TomlTemplateExpander.cpp
        core/mod/dataPack/BiomeDecoratorResourcesManager.cpp
        core/mod/dataPack/BiomeDecoratorType.cpp
        core/mod/dataPack/BiomesManager.cpp
        core/mod/dataPack/DataPack.cpp
        core/mod/dataPack/DataPackManager.cpp
        core/mod/dataPack/FixedColorManager.cpp
        core/mod/dataPack/ItemManager.cpp
        core/mod/dataPack/LightMaskManager.cpp
        core/mod/dataPack/LightSourceManager.cpp
        core/mod/dataPack/MobManager.cpp
        core/mod/dataPack/PackDependence.cpp
        core/mod/dataPack/RecipeGroup.cpp
        core/mod/dataPack/RecipeManager.cpp
        core/mod/dataPack/SpecialFileProcessingParams.cpp
        core/mod/dataPack/StringManager.cpp
        core/mod/dataPack/StructureManager.cpp
        core/mod/dataPack/TileResourceManager.cpp
        core/mod/resourcePack/AudioManager.cpp
        core/mod/resourcePack/AudioResourceResult.cpp
        core/mod/resourcePack/AudioType.cpp
        core/mod/resourcePack/ResourcePack.cpp
        core/mod/resourcePack/ResourcePackConfig.cpp
        core/mod/resourcePack/ResourcePackManager.cpp
        core/mod/resourcePack/ResourceResult.cpp
        core/mod/resourcePack/RmlResourceResult.cpp
        core/mod/resourcePack/TextureResourceResult.cpp
        core/mod/templateCommand/InsertTemplateCommand.cpp
        core/mod/templateCommand/SetTemplateCommand.cpp
        core/mod/templateCommand/UnSetTemplateCommand.cpp
        core/rmi/GameFileInterface.cpp
        core/rmi/GameFontEngineInterface.cpp
        core/rmi/RenderInterfaceSDL3.cpp
        core/rmi/SystemInterfaceSDL3.cpp
        core/saves/MapManifest.cpp
        core/saves/Saves.cpp
        core/saves/SavesManager.cpp
        core/scene/ConsoleOverlay.cpp
        core/scene/CreateWorldScene.cpp
        core/scene/DebugOverlay.cpp
        core/scene/MainScene.cpp
        core/scene/MainThreadDispatcher.cpp
        core/scene/SavedGamesScene.cpp
        core/scene/Scene.cpp
        core/scene/SceneManager.cpp
        core/scene/SplashScene.cpp
        core/scene/WorldScene.cpp
        core/shape/ShapeManager.cpp
        core/shape/ShapeType.cpp
        core/utils/Box2DUtils.cpp
        core/utils/ColorUtils.cpp
        core/utils/EventTypeUtils.cpp
        core/utils/LanguageUtils.cpp
        core/utils/LightUtils.cpp
        core/utils/MouseButtonUtils.cpp
        core/utils/NodeTree.cpp
        core/utils/ScanCodeUtils.cpp
        core/utils/StringUtils.cpp
        core/utils/TimeUtils.cpp
        core/utils/TomlUtils.cpp
        core/utils/TransparentStringHash.cpp
        core/utils/UUIDUtils.cpp
        core/vfs/AAssetStream.cpp
        core/vfs/AAssetStreamBuf.cpp
        core/vfs/AndroidAssetsFileProvider.cpp
        core/vfs/FileProvider.cpp
        core/vfs/StdFileProvider.cpp
        core/vfs/VirtualFileSystem.cpp
        core/world/ChunkManager.cpp
        core/world/ChunkTask.cpp
        core/world/ChunkType.cpp
        core/world/LightBuffer.cpp
        core/world/LightContribution.cpp
        core/world/LightMask.cpp
        core/world/LightPropagationTraverser.cpp
        core/world/LightSource.cpp
        core/world/PlayerContext.cpp
        core/world/PreloadColors.cpp
        core/world/SystemScheduler.cpp
        core/world/TerrainManager.cpp
        core/world/Tile.cpp
        core/world/TileAnchorType.cpp
        core/world/TileBlueprintData.cpp
        core/world/TileDimensions.cpp
        core/world/TileInstancePool.cpp
        core/world/TileLightData.cpp
        core/world/TileLightResourceData.cpp
        core/world/TileLootData.cpp
        core/world/TileMiningData.cpp
        core/world/TileResourceData.cpp
        core/world/TraverseAction.cpp
        core/world/WorldContext.cpp
        core/world/generator/BiomeDecorator.cpp
        core/world/generator/BiomeDecoratorManager.cpp
        core/world/generator/Chunk.cpp
        core/world/generator/ChunkGenerator.cpp
        core/world/generator/ChunkLoader.cpp
        core/world/generator/ChunkPhysicsHelper.cpp
        core/world/generator/FillBiomeDecorator.cpp
        core/world/generator/MineralBiomeDecorator.cpp
        core/world/generator/SurfaceBiomeDecorator.cpp
        core/world/generator/TerrainResult.cpp
        core/world/generator/TerrainResultType.cpp
        core/world/generator/TerrainTileResult.cpp
        core/world/generator/TileLayerType.cpp
        core/world/generator/TilePhysicsType.cpp
        core/world/generator/TileSnapshot.cpp
        core/world/structure/BiomeStructureConditionProcessor.cpp
        core/world/structure/HeightStructureConditionProcessor.cpp
        core/world/structure/HorizontalSpacingStructureConditionProcessor.cpp
        core/world/structure/IStructureConditionProcessor.cpp
        core/world/structure/IStructureGenerator.cpp
        core/world/structure/StaticStructureGenerator.cpp
        core/world/structure/StructureGeneratorManager.cpp
        core/world/structure/StructureGeneratorType.cpp
        core/world/structure/StructureInfo.cpp
        core/world/structure/SurfaceStructureConditionProcessor.cpp
        core/world/structure/TreeStructureGenerator.cpp
        main.cpp
        PUBLIC
        FILE_SET HEADERS
        FILES
        "${CMAKE_BINARY_DIR}/_deps/monocypher-src/src/optional/monocypher-ed25519.h"
        "${CMAKE_BINARY_DIR}/_deps/monocypher-src/src/monocypher.h"
        "${CMAKE_BINARY_DIR}/_deps/rmlui-src/Backends/RmlUi_Platform_SDL.h"
        "${CMAKE_BINARY_DIR}/_deps/rmlui-src/Backends/RmlUi_Renderer_SDL.h"
        core/App.h
        core/AppEventLoop.h
        core/AppRenderer.h
        core/BoolOrToggle.h
        core/Box2dFilter.h
        core/CommandHookEntry.h
        core/CommandHookManager.h
        core/CommandHookScope.h
        core/Config.h
        core/Constants.h
        core/GameUIMessage.h
        core/Hyperlink.h
        core/IFingerprintable.h
        core/LangsResources.h
        core/SelectedItemRuntimeData.h
        core/TagRuntimeData.h
        core/TilePlacementConfig.h
        core/TileStateBackup.h
        core/console/Command.h
        core/console/CommandArgs.h
        core/console/CommandEnvironment.h
        core/console/CommandHistoryManager.h
        core/console/CommandManager.h
        core/console/CommandRequest.h
        core/console/CommandResponse.h
        core/console/CommandResult.h
        core/console/CommandSender.h
        core/console/CommandStructure.h
        core/console/CommandSuggestions.h
        core/console/ConsoleMessage.h
        core/console/ConsoleWorker.h
        core/console/LocalConsoleInput.h
        core/console/command/SceneCommand.h
        core/console/command/InputCommand.h
        core/console/command/GuiCommand.h
        core/console/asset_enumerator/AbilityItemsAssetEnumerator.h
        core/console/asset_enumerator/BiomeDecorsAssetEnumerator.h
        core/console/asset_enumerator/BiomesAssetEnumerator.h
        core/console/asset_enumerator/ComposableItemsAssetEnumerator.h
        core/console/asset_enumerator/FixedColorAssetEnumerator.h
        core/console/asset_enumerator/IAssetEnumerator.h
        core/console/asset_enumerator/LightMaskAssetEnumerator.h
        core/console/asset_enumerator/LightSourceAssetEnumerator.h
        core/console/asset_enumerator/LootTablesAssetEnumerator.h
        core/console/asset_enumerator/MaterialItemsAssetEnumerator.h
        core/console/asset_enumerator/MobsAssetEnumerator.h
        core/console/asset_enumerator/RecipesAssetEnumerator.h
        core/console/asset_enumerator/ShapesAssetEnumerator.h
        core/console/asset_enumerator/StartInvAssetEnumerator.h
        core/console/asset_enumerator/StringAssetEnumerator.h
        core/console/asset_enumerator/StructuresAssetEnumerator.h
        core/console/asset_enumerator/TextureAssetEnumerator.h
        core/console/asset_enumerator/TileAssetEnumerator.h
        core/console/command/AssetViewerCommand.h
        core/console/command/BiomeScoreCommand.h
        core/console/command/Box2DCommand.h
        core/console/command/ClearCommand.h
        core/console/command/ConfigCommand.h
        core/console/command/EchoCommand.h
        core/console/command/EcsCommand.h
        core/console/command/FlyCommand.h
        core/console/command/GiveCommand.h
        core/console/command/HelpCommand.h
        core/console/command/HookCommand.h
        core/console/command/ItemEditorCommand.h
        core/console/command/LicenseCommand.h
        core/console/command/LightCommand.h
        core/console/command/LocateCommand.h
        core/console/command/LootCommand.h
        core/console/command/PackVerifyCommand.h
        core/console/command/ParallaxBackgroundCommand.h
        core/console/command/PlaceCommand.h
        core/console/command/PlayCommand.h
        core/console/command/ScreenshotCommand.h
        core/console/command/SeedCommand.h
        core/console/command/SummonCommand.h
        core/console/command/TagCommand.h
        core/console/command/TechnologyCommand.h
        core/console/command/TileSnapshotCommand.h
        core/console/command/TpCommand.h
        core/console/command/UnlockedRecipesCommand.h
        core/console/command/VFSCommand.h
        core/console/suggestion/AbilityItemDynamicSuggestions.h
        core/console/suggestion/AllocStrategyTypeDynamicSuggestions.h
        core/console/suggestion/AudioTrackDynamicSuggestions.h
        core/console/suggestion/BiomeSuggestions.h
        core/console/suggestion/BoolDynamicSuggestions.h
        core/console/suggestion/BooleanToggleDynamicSuggestions.h
        core/console/suggestion/CommandHookIdDynamicSuggestions.h
        core/console/suggestion/CommandHookScopeDynamicSuggestions.h
        core/console/suggestion/ComposableItemDynamicSuggestions.h
        core/console/suggestion/ConfigSuggestions.h
        core/console/suggestion/CoordinateDynamicSuggestions.h
        core/console/suggestion/DataPackDynamicSuggestions.h
        core/console/suggestion/DynamicSuggestions.h
        core/console/suggestion/DynamicSuggestionsManager.h
        core/console/suggestion/EventTypeDynamicSuggestions.h
        core/console/suggestion/LootSuggestions.h
        core/console/suggestion/MaterialItemDynamicSuggestions.h
        core/console/suggestion/MobDynamicSuggestions.h
        core/console/suggestion/MouseButtonDynamicSuggestions.h
        core/console/suggestion/ScanKeyDynamicSuggestions.h
        core/console/suggestion/StructureDynamicSuggestions.h
        core/console/suggestion/TileDynamicSuggestions.h
        core/console/suggestion/VFSDynamicSuggestions.h
        core/context/AppContext.h
        core/context/AudioContext.h
        core/context/ConsoleContext.h
        core/context/GraphicsContext.h
        core/context/ModContext.h
        core/context/RmlContext.h
        core/context/WindowContext.h
        core/contributor/Contributor.h
        core/contributor/ContributorManager.h
        core/ecs/DroppedItemCreator.h
        core/ecs/EcsTypes.h
        core/ecs/EntityManager.h
        core/ecs/EntityShortCut.h
        core/ecs/FlowingTextCreator.h
        core/ecs/GameComponent.h
        core/ecs/GameEntity.h
        core/ecs/GameSystem.h
        core/ecs/GameSystemType.h
        core/ecs/IEntityCreator.h
        core/ecs/IPersistenceEntityCreator.h
        core/ecs/ISwitchable.h
        core/ecs/IVisible.h
        core/ecs/ItemSlotType.h
        core/ecs/MobEntityCreator.h
        core/ecs/PlayerCapabilityHandler.h
        core/ecs/PlayerInputHandler.h
        core/ecs/PlayerTechnologyHandler.h
        core/ecs/TileBreakParams.h
        core/ecs/component/AreaMarkerComponent.h
        core/ecs/component/AutoPickComponent.h
        core/ecs/component/BlueprintComponent.h
        core/ecs/component/ButtonComponent.h
        core/ecs/component/CameraComponent.h
        core/ecs/component/CraftPreviewSlotComponent.h
        core/ecs/component/DebugDrawComponent.h
        core/ecs/component/DiggingComponent.h
        core/ecs/component/DraggableComponent.h
        core/ecs/component/DroppedItemComponent.h
        core/ecs/component/FloatingTextComponent.h
        core/ecs/component/HotBarComponent.h
        core/ecs/component/ItemContainerComponent.h
        core/ecs/component/ItemEditorComponent.h
        core/ecs/component/ItemSlotComponent.h
        core/ecs/component/ItemSlotQuantityComponent.h
        core/ecs/component/ItemToolTipComponent.h
        core/ecs/component/MagnetComponent.h
        core/ecs/component/MagneticComponent.h
        core/ecs/component/MobComponent.h
        core/ecs/component/ParallaxBackgroundComponent.h
        core/ecs/component/PauseComponent.h
        core/ecs/component/RayCast2DComponent.h
        core/ecs/component/RigidBody2DComponent.h
        core/ecs/component/SpiritRendererComponent.h
        core/ecs/component/TechProviderComponent.h
        core/ecs/component/TileLayerComponent.h
        core/ecs/component/TilePlacementForbiddenZoneComponent.h
        core/ecs/component/Transform2DComponent.h
        core/ecs/system/AndroidControlSystem.h
        core/ecs/system/AreaMarkerSystem.h
        core/ecs/system/AutoPickSystem.h
        core/ecs/system/BiomeBGMSystem.h
        core/ecs/system/BlueprintSystem.h
        core/ecs/system/Box2dSystemContext.h
        core/ecs/system/ButtonSystem.h
        core/ecs/system/ChunkSystem.h
        core/ecs/system/CraftPreviewSlotSystem.h
        core/ecs/system/DebugDrawBox2dSystem.h
        core/ecs/system/DebugDrawSystem.h
        core/ecs/system/DebugMultiMapSystem.h
        core/ecs/system/DebugPanelSystem.h
        core/ecs/system/DiggingSystem.h
        core/ecs/system/DraggableSystem.h
        core/ecs/system/DroppedItemSystem.h
        core/ecs/system/FloatingTextSystem.h
        core/ecs/system/ItemSlotQuantitySystem.h
        core/ecs/system/ItemSlotSystem.h
        core/ecs/system/ItemToolTipSystem.h
        core/ecs/system/Light2DSystem.h
        core/ecs/system/MagnetSystem.h
        core/ecs/system/ParallaxBackgroundSystem.h
        core/ecs/system/PauseSystem.h
        core/ecs/system/PhysicsSystem.h
        core/ecs/system/RayCast2DSystem.h
        core/ecs/system/SpiritRendererSystem.h
        core/ecs/system/TechProviderSystem.h
        core/ecs/system/TileLayerSystem.h
        core/inventory/AbilityItem.h
        core/inventory/AbilityItemCreateParams.h
        core/inventory/ComposableItem.h
        core/inventory/ComposableItemCreateParams.h
        core/inventory/ContainerChangeType.h
        core/inventory/CraftPreview.h
        core/inventory/InitialInventoryManager.h
        core/inventory/Item.h
        core/inventory/ItemAbilityFactory.h
        core/inventory/ItemContainer.h
        core/inventory/ItemDurabilityModule.h
        core/inventory/ItemLockModule.h
        core/inventory/ItemStackModule.h
        core/inventory/ItemTagModule.h
        core/inventory/MaterialItem.h
        core/inventory/TileItem.h
        core/inventory/ability/AreaMarkerAbility.h
        core/inventory/ability/ItemAbility.h
        core/inventory/ability/MiningAbility.h
        core/inventory/ability/MiningRangeData.h
        core/inventory/ability/MiningRangeDataPoint.h
        core/inventory/ability/NoneAbility.h
        core/log/LogCat.h
        core/lootTable/LootEntry.h
        core/lootTable/LootTableManager.h
        core/math/BackwardAllocStrategy.h
        core/math/BalanceAllocStrategy.h
        core/math/Color.h
        core/math/ColorF.h
        core/math/CoordinateTransformer.h
        core/math/DesignVector2D.h
        core/math/ForwardAllocStrategy.h
        core/math/IAllocStrategy.h
        core/math/IAllocatable.h
        core/math/RandomAllocStrategy.h
        core/math/ScreenVector2D.h
        core/math/TileVector2D.h
        core/math/Vector2D.h
        core/math/Vector2DBase.h
        core/math/Vector2DI.h
        core/math/Vector2DIBase.h
        core/math/Vector2DIHash.h
        core/math/WorldVector2D.h
        core/mod/ITemplateCommand.h
        core/mod/PackManifest.h
        core/mod/PackVerifyState.h
        core/mod/Resource.h
        core/mod/ResourceLocator.h
        core/mod/ResourceRef.h
        core/mod/StructurePlacementConditionsManager.h
        core/mod/TomlTemplateExpander.h
        core/mod/dataPack/BiomeDecoratorResourcesManager.h
        core/mod/dataPack/BiomeDecoratorType.h
        core/mod/dataPack/BiomesManager.h
        core/mod/dataPack/DataPack.h
        core/mod/dataPack/DataPackManager.h
        core/mod/dataPack/FixedColorManager.h
        core/mod/dataPack/ItemManager.h
        core/mod/dataPack/LightMaskManager.h
        core/mod/dataPack/LightSourceManager.h
        core/mod/dataPack/MobManager.h
        core/mod/dataPack/PackDependence.h
        core/mod/dataPack/RecipeGroup.h
        core/mod/dataPack/RecipeManager.h
        core/mod/dataPack/SpecialFileProcessingParams.h
        core/mod/dataPack/StringManager.h
        core/mod/dataPack/StructureManager.h
        core/mod/dataPack/TileResourceManager.h
        core/mod/resourcePack/AudioManager.h
        core/mod/resourcePack/AudioResourceResult.h
        core/mod/resourcePack/AudioType.h
        core/mod/resourcePack/ResourcePack.h
        core/mod/resourcePack/ResourcePackConfig.h
        core/mod/resourcePack/ResourcePackManager.h
        core/mod/resourcePack/ResourceResult.h
        core/mod/resourcePack/RmlResourceResult.h
        core/mod/resourcePack/TextureResourceResult.h
        core/mod/templateCommand/InsertTemplateCommand.h
        core/mod/templateCommand/SetTemplateCommand.h
        core/mod/templateCommand/UnSetTemplateCommand.h
        core/rmi/GameFileInterface.h
        core/rmi/GameFontEngineInterface.h
        core/rmi/RenderInterfaceSDL3.h
        core/rmi/SystemInterfaceSDL3.h
        core/saves/MapManifest.h
        core/saves/Saves.h
        core/saves/SavesManager.h
        core/scene/ConsoleOverlay.h
        core/scene/CreateWorldScene.h
        core/scene/DebugOverlay.h
        core/scene/MainScene.h
        core/scene/MainThreadDispatcher.h
        core/scene/SavedGamesScene.h
        core/scene/Scene.h
        core/scene/SceneManager.h
        core/scene/SplashScene.h
        core/scene/WorldScene.h
        core/shape/ShapeManager.h
        core/shape/ShapeType.h
        core/utils/Box2DUtils.h
        core/utils/ColorUtils.h
        core/utils/EventTypeUtils.h
        core/utils/LanguageUtils.h
        core/utils/LightUtils.h
        core/utils/MouseButtonUtils.h
        core/utils/NodeTree.h
        core/utils/RandomUtils.h
        core/utils/ScanCodeUtils.h
        core/utils/StringUtils.h
        core/utils/TimeUtils.h
        core/utils/TomlUtils.h
        core/utils/TransparentStringHash.h
        core/utils/UUIDUtils.h
        core/vfs/AAssetStream.h
        core/vfs/AAssetStreamBuf.h
        core/vfs/AndroidAssetsFileProvider.h
        core/vfs/FileProvider.h
        core/vfs/StdFileProvider.h
        core/vfs/VirtualFileSystem.h
        core/world/BreakSource.h
        core/world/ChunkManager.h
        core/world/ChunkTask.h
        core/world/ChunkType.h
        core/world/LightBuffer.h
        core/world/LightContribution.h
        core/world/LightMask.h
        core/world/LightPropagationTraverser.h
        core/world/LightSource.h
        core/world/PlayerContext.h
        core/world/PreloadColors.h
        core/world/SystemScheduler.h
        core/world/TerrainManager.h
        core/world/Tile.h
        core/world/TileAnchorType.h
        core/world/TileBlueprintData.h
        core/world/TileDimensions.h
        core/world/TileInstancePool.h
        core/world/TileLightData.h
        core/world/TileLightResourceData.h
        core/world/TileLootData.h
        core/world/TileMiningData.h
        core/world/TileResourceData.h
        core/world/TraverseAction.h
        core/world/WorldContext.h
        core/world/generator/BiomeDecorator.h
        core/world/generator/BiomeDecoratorManager.h
        core/world/generator/Chunk.h
        core/world/generator/ChunkGenerator.h
        core/world/generator/ChunkLoader.h
        core/world/generator/ChunkPhysicsHelper.h
        core/world/generator/FillBiomeDecorator.h
        core/world/generator/MineralBiomeDecorator.h
        core/world/generator/SurfaceBiomeDecorator.h
        core/world/generator/TerrainResult.h
        core/world/generator/TerrainResultType.h
        core/world/generator/TerrainTileResult.h
        core/world/generator/TileLayerType.h
        core/world/generator/TilePhysicsType.h
        core/world/generator/TileSnapshot.h
        core/world/structure/BiomeStructureConditionProcessor.h
        core/world/structure/HeightStructureConditionProcessor.h
        core/world/structure/HorizontalSpacingStructureConditionProcessor.h
        core/world/structure/IStructureConditionProcessor.h
        core/world/structure/IStructureGenerator.h
        core/world/structure/StaticStructureGenerator.h
        core/world/structure/StructureGeneratorManager.h
        core/world/structure/StructureGeneratorType.h
        core/world/structure/StructureInfo.h
        core/world/structure/SurfaceStructureConditionProcessor.h
        core/world/structure/TreeStructureGenerator.h
)

#Define version information
#定义版本信息
set(CONFIG_FILE_NAME "config.toml")
set(DEBUG_CONFIG_FILE_NAME "config_debug.toml")
target_compile_definitions(GlimmerWorks PRIVATE
        CONFIG_FILE_NAME="${CONFIG_FILE_NAME}"
        GAME_VERSION_NUMBER=${GAME_VERSION_NUMBER}
        GAME_VERSION_STRING="${GAME_VERSION_STRING}"
        RMLUI_SDL_VERSION_MAJOR=3
        APP_PACKNAME="org.glimmerworks"
)

if (ANDROID)
    target_link_libraries(GlimmerWorks PRIVATE ${ANDROID_LOG_LIB} android SDL3::SDL3 SDL3_image::SDL3_image SDL3_mixer::SDL3_mixer fmt::fmt SDL3_ttf::SDL3_ttf box2d::box2d glimmerworks::proto toml11::toml11 BLAKE3::blake3 RmlUi::RmlUi)
else ()
    target_link_libraries(GlimmerWorks PRIVATE SDL3::SDL3 SDL3_image::SDL3_image SDL3_mixer::SDL3_mixer fmt::fmt SDL3_ttf::SDL3_ttf box2d::box2d glimmerworks::proto toml11::toml11 BLAKE3::blake3 RmlUi::RmlUi)
endif ()


target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/fmt-src/include"
)

target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/box2d-src/include"
)
target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/rmlui-src/Backends"
)
target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/rmlui-src/Source/Core"
)

target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/toml11-src/include"
)

target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/tweeny-src/include"
)

target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/fastnoiselite-src/Cpp"
)

target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/sdl-src/include"
)

target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/blake3-src/c"
)
target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/monocypher-src/src"
)
target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/monocypher-src/src/optional"
)
target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/sdl_mixer-src/include"
)

target_include_directories(GlimmerWorks
        PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/glimmerworksprotobuf-build/cpp"
        "${CMAKE_BINARY_DIR}/_deps/protobuf-src/third_party/utf8_range"
)

if (CMAKE_BUILD_TYPE MATCHES "^Debug$")
    file(READ "${CMAKE_SOURCE_DIR}/${CONFIG_FILE_NAME}" MAIN_CONFIG_CONTENT)
    file(READ "${CMAKE_SOURCE_DIR}/${DEBUG_CONFIG_FILE_NAME}" DEBUG_CONFIG_CONTENT)
    set(FINAL_CONFIG_CONTENT "${MAIN_CONFIG_CONTENT}\n${DEBUG_CONFIG_CONTENT}")
    file(WRITE "${CMAKE_BINARY_DIR}/${CONFIG_FILE_NAME}" "${FINAL_CONFIG_CONTENT}")
else ()
    file(COPY "${CMAKE_SOURCE_DIR}/${CONFIG_FILE_NAME}"
            DESTINATION "${CMAKE_BINARY_DIR}/")
endif ()


file(COPY "${CMAKE_SOURCE_DIR}/core_langs/"
        DESTINATION "${CMAKE_BINARY_DIR}/langs/")
if (CMAKE_BUILD_TYPE MATCHES "^Debug$")
    file(GLOB_RECURSE DEBUG_LANG_FILES "${CMAKE_SOURCE_DIR}/core_langs_debug/*")

    foreach (DEBUG_FILE ${DEBUG_LANG_FILES})
        file(RELATIVE_PATH REL_PATH "${CMAKE_SOURCE_DIR}/core_langs_debug" "${DEBUG_FILE}")
        set(TARGET_FILE "${CMAKE_BINARY_DIR}/langs/${REL_PATH}")
        file(READ "${DEBUG_FILE}" DEBUG_CONTENT)
        file(APPEND "${TARGET_FILE}" "\n${DEBUG_CONTENT}")
    endforeach ()
endif ()

file(COPY "${CMAKE_SOURCE_DIR}/LICENSE"
        DESTINATION "${CMAKE_BINARY_DIR}/")

file(WRITE "${CMAKE_BINARY_DIR}/.gitignore" "*")

#Clone data packets and resource packets.
#克隆数据包和资源包
FetchContent_Declare(
        VanillaDataPack
        GIT_REPOSITORY "https://github.com/Cold-Mint/GlimmerworksData"
        SOURCE_DIR "${CMAKE_BINARY_DIR}/mods/data_packs/glimmerworks"
)

FetchContent_GetProperties(VanillaDataPack)
if (NOT VanillaDataPack_POPULATED)
    FetchContent_MakeAvailable(VanillaDataPack)
endif ()

FetchContent_Declare(
        VanillaResourcePack
        GIT_REPOSITORY "https://github.com/Cold-Mint/GlimmerworksAssets"
        SOURCE_DIR "${CMAKE_BINARY_DIR}/mods/resource_packs/glimmerworks_res"
)

FetchContent_GetProperties(VanillaResourcePack)
if (NOT VanillaDataPack_POPULATED)
    FetchContent_MakeAvailable(VanillaResourcePack)
endif ()

if (UNIX AND NOT APPLE)
    if (X11_SUPPORT)
        set(WINDOW_BACKEND_NAME "x11")
    else ()
        set(WINDOW_BACKEND_NAME "wayland")
    endif ()

    set(CPACK_PACKAGE_FILE_NAME
            "${PROJECT_NAME}-${GAME_VERSION_STRING}-Linux-${WINDOW_BACKEND_NAME}-x${ARCH_BIT}")
    set(CPACK_GENERATOR "TZST")
endif ()
if (WIN32)
    set(CPACK_PACKAGE_FILE_NAME
            "${PROJECT_NAME}-${GAME_VERSION_STRING}-Windows-x${ARCH_BIT}")
    set(CPACK_GENERATOR "7Z")
endif ()

set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/dist/${CMAKE_BUILD_TYPE}")
include(CPack)
install(TARGETS GlimmerWorks RUNTIME DESTINATION .)
install(FILES "${CMAKE_SOURCE_DIR}/${CONFIG_FILE_NAME}" DESTINATION .)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/core_langs/"
        DESTINATION "langs")
install(DIRECTORY "${CMAKE_BINARY_DIR}/mods/"
        DESTINATION "mods"
        PATTERN ".*" EXCLUDE
)

install(CODE "
    # 安装目标根目录（由 CMAKE_INSTALL_PREFIX 和 DESTDIR 共同决定）
    set(INSTALL_MODS_DIR \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/mods\")
    set(DATA_PACKS_DIR \"\${INSTALL_MODS_DIR}/data_packs\")

    if(NOT EXISTS \"\${DATA_PACKS_DIR}\")
        message(WARNING \"Data packs directory not found: \${DATA_PACKS_DIR}, skip signing.\")
    else()
        file(GLOB SUB_DIRS LIST_DIRECTORIES true \"\${DATA_PACKS_DIR}/*\")
        foreach(SUB_DIR \${SUB_DIRS})
            if(IS_DIRECTORY \${SUB_DIR})
                get_filename_component(PACK_NAME \${SUB_DIR} NAME)
                message(STATUS \"Signing data pack: \${PACK_NAME} at \${SUB_DIR}\")
                execute_process(
                    COMMAND \"${PACK_SIGNER_EXE}\" sign
                            -x
                            -k \"${SIGN_PRIVATE_KEY}\"
                            -p \"${SIGN_PUBLIC_KEY}\"
                            -d \"\${SUB_DIR}\"
                    RESULT_VARIABLE SIGN_RESULT
                )
                if(NOT SIGN_RESULT EQUAL 0)
                    message(FATAL_ERROR \"Signing failed for \${SUB_DIR}\")
                endif()
            endif()
        endforeach()
    endif()
")

install(FILES "${CMAKE_SOURCE_DIR}/LICENSE"
        DESTINATION .)