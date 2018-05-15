// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
