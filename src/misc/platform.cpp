// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// platform.cpp
//
#include "platform.hpp"

#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace misc
{

    const std::string platform_type::ToString(const platform_type::Enum E)
    {
        switch (E)
        {
            case Unknown:
            {
                return "Unknown";
            }
            case Windows:
            {
                return "Windows";
            }
            case Linux:
            {
                return "Linux";
            }
            case Apple:
            {
                return "Apple";
            }
            case Unsupported:
            {
                return "Unsupported";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "misc::Platform::ToString(" << E << ")_InvalidValueError.";
                throw std::runtime_error(ss.str());
            }
        }
    }

    Platform::Platform()
        : platform_(Detect())
    {}

    platform_type::Enum Platform::Detect() const
    {
        platform_type::Enum platform{ platform_type::Unknown };

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_WINDOWS
        platform = platform_type::Windows;
#endif

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_APPLE9
        platform = platform_type::Unsupported;
#endif

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_APPLE
        platform = platform_type::Apple;
#endif

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_LINUX
        platform = platform_type::Linux;
#endif

        return platform;
    }

    void Platform::Log() const
    {
        std::ostringstream ss;

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_WINDOWS
        ss << "  Windows";
#endif

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_APPLE9
        ss << "  APPLE (OS9)";
#endif

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_APPLE
        ss << "  APPLE (OS)";
#endif

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_LINUX
        ss << "  Linux";
#endif

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_UNIX
        ss << "  Unix";
#endif

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_POSIX
        ss << "  POSIX";
#endif

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_CYGWIN
        ss << "  CYGWIN";
#endif

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_LINUX_GNU
        ss << "  GNU/Linux";
#endif

        if (ss.str().empty())
        {
            ss << "(None!  This unknown platform is probably not supported...)";
        }

        M_HP_LOG("Platform(s) Detected:  " << ss.str());
    }

} // namespace misc
} // namespace heroespath
