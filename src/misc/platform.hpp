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

namespace heroespath
{
namespace misc
{

    enum class platform_enum
    {
        Unknown,
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
    constexpr auto platform = platform_enum::Windows;
    constexpr auto platform_name = "Windows";
#elif (defined(macintosh) || defined(Macintosh))
    constexpr auto platform = platform_enum::Apple9;
    constexpr auto platform_name = "Apple9";
#elif (defined(__APPLE__) || defined(__MACH__))
    constexpr auto platform = platform_enum::Apple;
    constexpr auto platform_name = "Apple";
#elif (defined(linux) || defined(__linux) || defined(__linux__))
    constexpr auto platform = platform_enum::Linux;
    constexpr auto platform_name = "Linux";
#elif (defined(__unix) || defined(__unix__))
    constexpr auto platform = platform_enum::Linux;
    constexpr auto platform_name = "Linux";
#elif (defined(__posix) || defined(__posix__))
    constexpr auto platform = platform_enum::Posix;
    constexpr auto platform_name = "Posix";
#elif defined(__CYGWIN__)
    constexpr auto platform = platform_enum::Cygwin;
    constexpr auto platform_name = "Cygwin";
#elif defined(__gnu_linux__)
    constexpr auto platform = platform_enum::LinuxGNU;
    constexpr auto platform_name = "LinuxGNU";
#else
    constexpr auto platform = platform_enum::Unknown;
    constexpr auto platform_name = "Unknown";
#endif

    constexpr bool platform_is_any_windows = (platform == platform_enum::Windows);

    constexpr bool platform_is_any_linux
        = ((platform == platform_enum::Linux) || (platform == platform_enum::LinuxGNU));

    constexpr bool platform_is_any_apple
        = ((platform == platform_enum::Apple) || (platform == platform_enum::Apple9));

    constexpr bool platform_is_supported
        = ((platform == platform_enum::Windows) || (platform == platform_enum::Apple)
           || (platform == platform_enum::Linux));

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_PLATFORM_HPP_INCLUDED
