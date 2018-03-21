///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef HEROESPATH_MISC_PLATFORM_HPP_INCLUDED
#define HEROESPATH_MISC_PLATFORM_HPP_INCLUDED
//
// platform.hpp
//  Code that detects the platform and stores it.
//
#include <memory>
#include <string>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(WIN64) || defined(_WIN64)   \
    || defined(__WINDOWS__)
#define HEROESPATH_PLATFORM_DETECTED_IS_WINDOWS
#elif defined(macintosh) || defined(Macintosh)
#define HEROESPATH_PLATFORM_DETECTED_IS_APPLE9
#elif defined(__APPLE__) || defined(__MACH__)
#define HEROESPATH_PLATFORM_DETECTED_IS_APPLE
#elif defined(linux) || defined(__linux) || defined(__linux__)
#define HEROESPATH_PLATFORM_DETECTED_IS_LINUX
#endif

#if defined(__unix) || defined(__unix__)
#define HEROESPATH_PLATFORM_DETECTED_IS_UNIX
#endif

#if defined(__posix) || defined(__posix__)
#define HEROESPATH_PLATFORM_DETECTED_IS_POSIX
#endif

#if defined(__CYGWIN__)
#define HEROESPATH_PLATFORM_DETECTED_IS_CYGWIN
#endif

#if defined(__gnu_linux__)
#define HEROESPATH_PLATFORM_DETECTED_IS_LINUX_GNU
#endif

namespace heroespath
{
namespace misc
{

    // defines supported platforms
    struct platform_type
    {
        enum Enum
        {
            Unknown = 0,
            Windows,
            Linux,
            Apple,
            Unsupported,
            Count
        };

        static const std::string ToString(const platform_type::Enum);
    };

    // singleton responsible for detecting, logging, and storing the system platform
    class Platform
    {
    public:
        Platform(const Platform &) = delete;
        Platform(const Platform &&) = delete;
        Platform & operator=(const Platform &) = delete;
        Platform & operator=(const Platform &&) = delete;

        Platform();

        static Platform * Instance();
        static void Acquire();
        static void Release();

        void DetectAndLog();

        platform_type::Enum Get() const { return platform_; }

        const std::string GetName() const { return platform_type::ToString(platform_); }

        bool IsWindows() const { return (platform_type::Windows == platform_); }

        bool IsLinux() const { return (platform_type::Linux == platform_); }

        bool IsApple() const { return (platform_type::Apple == platform_); }

        bool IsSupported() const
        {
            return (platform_type::Unknown != platform_)
                && (platform_type::Unsupported != platform_);
        }

    private:
        static std::unique_ptr<Platform> instanceUPtr_;
        platform_type::Enum platform_;
    };
} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_PLATFORM_HPP_INCLUDED
