/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#pragma once

// A safe wrapper around the Windows SDK's <windows.h>.
//
// The Windows SDK leaks a couple of macros into user code that cause
// identifier collisions:
//   * `interface` -> `struct`  (from <rpc.h>/<rpcndr.h>/<objbase.h>)
//   * `min` / `max`            (from <windef.h>, unless NOMINMAX is defined)
//
// Include this header instead of <windows.h> directly to avoid both.
//
// Windows SDK <windows.h> 的安全封装。
// Windows SDK 会把一些宏泄漏到用户代码里，导致标识符冲突：
//   * `interface` -> `struct`（来自 <rpc.h>/<rpcndr.h>/<objbase.h>）
//   * `min` / `max`（来自 <windef.h>，除非定义了 NOMINMAX）
// 请用本头文件代替直接包含 <windows.h> 以避开它们。
#if !defined(_WIN32)
#error "core/platform/WindowsApi.h can only be included on Windows"
#endif

// Target Windows 7 (0x0601) so SetThreadDescription and friends are declared.
#if !defined(_WIN32_WINNT) || _WIN32_WINNT < 0x0601
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>

// `interface` can still leak in if some earlier header pulled in <objbase.h>
// (and therefore <rpc.h>) before us. It is never legitimately required by C++
// code, so remove it defensively.
#ifdef interface
#undef interface
#endif
