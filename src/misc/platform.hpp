// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_PLATFORM_HPP_INCLUDED
#define HEROESPATH_MISC_PLATFORM_HPP_INCLUDED
//
// platform.hpp
//
#include "misc/not-null.hpp"

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

    // Responsible for detecting, logging, and storing the platform this app is running on.
    class Platform
    {
    public:
        Platform(const Platform &) = delete;
        Platform(Platform &&) = delete;
        Platform & operator=(const Platform &) = delete;
        Platform & operator=(Platform &&) = delete;

        Platform();

        void Log() const;
        platform_type::Enum Get() const { return platform_; }
        const std::string GetName() const { return platform_type::ToString(platform_); }
        bool IsWindows() const { return (platform_type::Windows == platform_); }
        bool IsLinux() const { return (platform_type::Linux == platform_); }
        bool IsMacOS() const { return (platform_type::Apple == platform_); }

        bool IsSupported() const
        {
            return (platform_type::Unknown != platform_)
                && (platform_type::Unsupported != platform_);
        }

    private:
        platform_type::Enum Detect() const;

        platform_type::Enum platform_;
    };

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_PLATFORM_HPP_INCLUDED
