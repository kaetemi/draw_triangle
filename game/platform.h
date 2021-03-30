/*

Copyright (C) 2019-2021  Jan BOON (Kaetemi) <jan.boon@kaetemi.be>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

/*

It's a header named `platform.h`. This better be good.

*/

#pragma once
#ifndef GAME_PLATFORM_H
#define GAME_PLATFORM_H

// Use C math defines for M_PI
#define _USE_MATH_DEFINES

#ifdef _WIN32
// Include Win32.
// Ensure malloc is included before anything else,
// There are some odd macros that may conflict otherwise.
// Include STL algorithm to ensure std::min and std::max
// are used everywhere, instead of min and max macros.
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0600
#ifdef __cplusplus
#define NOMINMAX
#endif /* __cplusplus */
#include <malloc.h>
#ifdef __cplusplus
#include <algorithm>
using std::max;
using std::min;
#endif /* __cplusplus */
#include <Windows.h>
#ifdef _MSC_VER
// #include <codeanalysis\sourceannotations.h>
#endif
#endif /* _WIN32 */

// C++
#ifdef __cplusplus

// Require C++17
#if defined(_MSC_VER) && (!defined(_HAS_CXX17) || !_HAS_CXX17)
static_assert(false, "C++17 is required");
#endif

// Define null, with color highlight
constexpr decltype(nullptr) null = nullptr;
#define null null

// Include STL string and allow string literals.
// Always use sv suffix when declaring string literals.
// Ideally, assign them as `constexpr std::string_view`.
#include <string>
#include <string_view>
using namespace std::string_literals;
using namespace std::string_view_literals;

// Include GSL
// auto _ = gsl::finally([&] { delete xyz; });
#include "gsl/gsl_util"

// The usual
#include <functional>

// Force inline
#ifdef _MSC_VER
#	define GAME_FORCE_INLINE __forceinline
#else
#	define GAME_FORCE_INLINE inline __attribute__((always_inline))
#endif

#if defined(_DEBUG) && !defined(NDEBUG)
#define GAME_DEBUG
#else
#define GAME_RELEASE
#endif

// Include debug_break
#include <debugbreak.h>
#define GAME_RELEASE_BREAK() debug_break()
#define GAME_RELEASE_ASSERT(cond) do { if (!(cond)) GAME_RELEASE_BREAK(); } while (false)
#define GAME_RELEASE_VERIFY(cond) do { if (!(cond)) GAME_RELEASE_BREAK(); } while (false)

#ifdef GAME_DEBUG
#define GAME_DEBUG_BREAK() debug_break()
#define GAME_DEBUG_ASSERT(cond) do { if (!(cond)) GAME_DEBUG_BREAK(); } while (false)
#define GAME_DEBUG_VERIFY(cond) do { if (!(cond)) GAME_DEBUG_BREAK(); } while (false)
#else
#define GAME_DEBUG_BREAK() do { } while (false)
#define GAME_DEBUG_ASSERT(cond) do { } while (false)
#define GAME_DEBUG_VERIFY(cond) do { cond; } while (false)
#endif

#define GAME_CONCAT_IMPL(a, b) x##y
#define GAME_CONCAT(a, b) GAME_CONCAT_IMPL(a, b)

#define GAME_FINALLY(f) auto GAME_CONCAT(finally__, __COUNTER__) = gsl::finally(f)

#endif /* __cplusplus */

#endif /* GAME_PLATFORM_H */

/* end of file */
