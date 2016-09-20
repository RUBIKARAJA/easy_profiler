/************************************************************************
* file name         : profiler_aux.h
* ----------------- :
* creation time     : 2016/06/11
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : This file contains auxiliary profiler macros and funcitons.
* ----------------- :
* change log        : * 2016/06/11 Victor Zarubkin: Moved sources from profiler.h
*                   :
*                   : *
* ----------------- :
* license           : Lightweight profiler library for c++
*                   : Copyright(C) 2016  Sergey Yagovtsev, Victor Zarubkin
*                   :
*                   : This program is free software : you can redistribute it and / or modify
*                   : it under the terms of the GNU General Public License as published by
*                   : the Free Software Foundation, either version 3 of the License, or
*                   : (at your option) any later version.
*                   :
*                   : This program is distributed in the hope that it will be useful,
*                   : but WITHOUT ANY WARRANTY; without even the implied warranty of
*                   : MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
*                   : GNU General Public License for more details.
*                   :
*                   : You should have received a copy of the GNU General Public License
*                   : along with this program.If not, see <http://www.gnu.org/licenses/>.
************************************************************************/

#ifndef EASY_PROFILER__AUX__H_______
#define EASY_PROFILER__AUX__H_______

#include <stdint.h>
#include <cstddef>
#include "profiler/profiler_colors.h"

//////////////////////////////////////////////////////////////////////////

#ifdef _WIN32

# define __func__ __FUNCTION__
# if defined(_MSC_VER) && _MSC_VER <= 1800
// There is no support for C++11 thread_local keyword prior to Visual Studio 2015. Use __declspec(thread) instead.
#  define EASY_THREAD_LOCAL __declspec(thread)
# endif
# ifdef _BUILD_PROFILER
#  define PROFILER_API __declspec(dllexport)
# else
#  define PROFILER_API __declspec(dllimport)
# endif

#elif defined (__clang__)

# if (__clang_major__ == 3 && __clang_minor__ < 3) || (__clang_major__ < 3)
#  define EASY_THREAD_LOCAL __thread
# endif

#elif defined(__GNUC__)

# if (__GNUC__ == 4 && __GNUC_MINOR__ < 8) || (__GNUC__ < 4)
// There is no support for C++11 thread_local keyword prior to gcc 4.8. Use __thread instead.
#  define EASY_THREAD_LOCAL __thread
# endif

#endif

// TODO: Check thread_local support for clang earlier than 3.3

#ifndef EASY_THREAD_LOCAL
# define EASY_THREAD_LOCAL thread_local
# define EASY_THREAD_LOCAL_CPP11
#endif

#ifndef PROFILER_API
# define PROFILER_API
#endif

//////////////////////////////////////////////////////////////////////////

namespace profiler {

    enum EasyBlockStatus : uint8_t {
        OFF = 0, ///< The block is OFF
        ON = 1, ///< The block is ON (but if it's parent block is off recursively then this block will be off too)
        FORCE_ON = ON | 2, ///< The block is ALWAYS ON (even if it's parent has turned off all children)
        OFF_RECURSIVE = 4, ///< The block is OFF and all of it's children by call-stack are also OFF.
        ON_WITHOUT_CHILDREN = ON | OFF_RECURSIVE, ///< The block is ON but all of it's children are OFF.
        FORCE_ON_WITHOUT_CHILDREN = FORCE_ON | OFF_RECURSIVE, ///< The block is ALWAYS ON but all of it's children are OFF.
    };

    struct passthrough_hash final {
        template <class T> inline size_t operator () (T _value) const {
            return static_cast<size_t>(_value);
        }
    };

}

//////////////////////////////////////////////////////////////////////////

#ifndef FULL_DISABLE_PROFILER

#include <type_traits>

# define EASY_STRINGIFY(a) #a
# define EASY_STRINGIFICATION(a) EASY_STRINGIFY(a)
# define EASY_TOKEN_JOIN(x, y) x ## y
# define EASY_TOKEN_CONCATENATE(x, y) EASY_TOKEN_JOIN(x, y)
# define EASY_UNIQUE_BLOCK(x) EASY_TOKEN_CONCATENATE(unique_profiler_mark_name_, x)
# define EASY_UNIQUE_DESC(x) EASY_TOKEN_CONCATENATE(unique_profiler_descriptor_, x)

namespace profiler {

    template <const bool IS_REF> struct NameSwitch final {
        static const char* runtime_name(const char* name) { return name; }
        static const char* compiletime_name(const char*, const char* autoGeneratedName) { return autoGeneratedName; }
    };

    template <> struct NameSwitch<true> final {
        static const char* runtime_name(const char*) { return ""; }
        static const char* compiletime_name(const char* name, const char*) { return name; }
    };

    //***********************************************

    inline color_t extract_color() {
        return ::profiler::colors::Default;
    }

    template <class ... TArgs>
    inline color_t extract_color(::profiler::EasyBlockStatus, TArgs...) {
        return ::profiler::colors::Default;
    }

    template <class ... TArgs>
    inline color_t extract_color(color_t _color, TArgs...) {
        return _color;
    }

    template <class T, class ... TArgs>
    inline color_t extract_color(T, color_t _color, TArgs...) {
        return _color;
    }

    template <class ... TArgs>
    inline color_t extract_color(TArgs...) {
        static_assert(sizeof...(TArgs) < 2, "No profiler::color_t in arguments list for EASY_BLOCK(name, ...)!");
        return ::profiler::colors::Default;
    }

    //***********************************************

    inline EasyBlockStatus extract_enable_flag() {
        return ::profiler::ON;
    }

    template <class T, class ... TArgs>
    inline EasyBlockStatus extract_enable_flag(T, ::profiler::EasyBlockStatus _flag, TArgs...) {
        return _flag;
    }

    template <class ... TArgs>
    inline EasyBlockStatus extract_enable_flag(::profiler::EasyBlockStatus _flag, TArgs...) {
        return _flag;
    }

    template <class ... TArgs>
    inline EasyBlockStatus extract_enable_flag(TArgs...) {
        static_assert(sizeof...(TArgs) < 2, "No EasyBlockStatus in arguments list for EASY_BLOCK(name, ...)!");
        return ::profiler::ON;
    }

    //***********************************************

} // END of namespace profiler.

# define EASY_UNIQUE_LINE_ID __FILE__ ":" EASY_STRINGIFICATION(__LINE__)
# define EASY_COMPILETIME_NAME(name) ::profiler::NameSwitch<::std::is_reference<decltype(name)>::value>::compiletime_name(name, EASY_UNIQUE_LINE_ID)
# define EASY_RUNTIME_NAME(name) ::profiler::NameSwitch<::std::is_reference<decltype(name)>::value>::runtime_name(name)

#endif // FULL_DISABLE_PROFILER

//////////////////////////////////////////////////////////////////////////

#endif // EASY_PROFILER__AUX__H_______
