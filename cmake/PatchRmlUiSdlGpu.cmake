# Fixes a name collision with the Windows SDK `interface` macro.
#
# On Windows, including <windows.h> (transitively, via <rpc.h>/<rpcndr.h> or
# <objbase.h>) defines:
#     #define interface struct
# RmlUi's SDL_GPU backend uses `interface` as a parameter and member-access
# identifier, which then expands to `struct` and fails to compile. Rename the
# identifier to `render_interface` to avoid the collision.
#
# This script is invoked from CMakeLists.txt as the rmlUi FetchContent
# PATCH_COMMAND. It is idempotent: if the source has already been patched it
# does nothing, so re-running CMake configure is safe.
#
# Usage: cmake -D SOURCE_DIR=<rmlui source dir> -P PatchRmlUiSdlGpu.cmake

if(NOT DEFINED SOURCE_DIR)
    message(FATAL_ERROR "PatchRmlUiSdlGpu.cmake: SOURCE_DIR is not defined")
endif()

set(_header "${SOURCE_DIR}/Backends/RmlUi_Renderer_SDL_GPU.h")
set(_source "${SOURCE_DIR}/Backends/RmlUi_Renderer_SDL_GPU.cpp")

foreach(_file IN ITEMS ${_header} ${_source})
    if(NOT EXISTS "${_file}")
        message(FATAL_ERROR "PatchRmlUiSdlGpu.cmake: file not found: ${_file}")
    endif()

    file(READ "${_file}" _content)

    # Skip if the file was already patched by a previous configure run.
    string(FIND "${_content}" "render_interface" _already_patched)
    if(NOT _already_patched EQUAL -1)
        continue()
    endif()

    # Parameter declarations: `RenderInterface_SDL_GPU& interface`.
    string(REPLACE "& interface" "& render_interface" _content "${_content}")
    # Member access: `interface.member`.
    string(REPLACE "interface." "render_interface." _content "${_content}")
    # Void-cast of the (otherwise unused) parameter.
    string(REPLACE "(void)interface;" "(void)render_interface;" _content "${_content}")

    file(WRITE "${_file}" "${_content}")
    message(STATUS "PatchRmlUiSdlGpu.cmake: patched ${_file}")
endforeach()
