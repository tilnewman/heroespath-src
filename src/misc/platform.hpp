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
#include <string_view>

namespace heroespath
{
namespace misc
{

    enum class platforms : unsigned
    {
        Unknown = 0,
        Windows,
        Linux,
        LinuxGNU,
        Posix,
        Apple9,
        Apple,
        Cygwin
    };

#if (                                                                                              \
    defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(WIN64) || defined(_WIN64)   \
    || defined(__WINDOWS__))
    constexpr auto platform_enum = platforms::Windows;
    constexpr std::string_view platform_name = "Windows";
#elif (defined(macintosh) || defined(Macintosh))
    constexpr auto platform_enum = platforms::Apple9;
    constexpr std::string_view platform_name = "Apple9";
#elif (defined(__APPLE__) || defined(__MACH__))
    constexpr auto platform_enum = platforms::Apple;
    constexpr std::string_view platform_name = "Apple";
#elif (defined(linux) || defined(__linux) || defined(__linux__))
    constexpr auto platform_enum = platforms::Linux;
    constexpr std::string_view platform_name = "Linux";
#elif (defined(__unix) || defined(__unix__))
    constexpr auto platform_enum = platforms::Linux;
    constexpr std::string_view platform_name = "Unix";
#elif (defined(__posix) || defined(__posix__))
    constexpr auto platform_enum = platforms::Posix;
    constexpr std::string_view platform_name = "Posix";
#elif defined(__CYGWIN__)
    constexpr auto platform_enum = platforms::Cygwin;
    constexpr std::string_view platform_name = "Cygwin";
#elif defined(__gnu_linux__)
    constexpr auto platform_enum = platforms::LinuxGNU;
    constexpr std::string_view platform_name = "GNULinux";
#else
    constexpr auto platform = platforms::Unknown;
    constexpr std::string_view platform_name = "Unknown";
#endif

    constexpr bool platform_is_any_windows = (platform_enum == platforms::Windows);

    constexpr bool platform_is_any_linux
        = ((platform_enum == platforms::Linux) || (platform_enum == platforms::LinuxGNU));

    constexpr bool platform_is_any_apple
        = ((platform_enum == platforms::Apple) || (platform_enum == platforms::Apple9));

    constexpr bool platform_is_supported
        = ((platform_enum == platforms::Windows) || (platform_enum == platforms::Apple)
           || (platform_enum == platforms::Linux));

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_PLATFORM_HPP_INCLUDED
