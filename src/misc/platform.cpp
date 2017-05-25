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

#include "game/log-macros.hpp"

#include "misc/assertlogandthrow.hpp"

#include <exception>
#include <sstream>


namespace misc
{

    const std::string platform_type::ToString(const platform_type::Enum E)
    {
        switch (E)
        {
            case Unknown:     { return "Unknown"; }
            case Windows:     { return "Windows"; }
            case Linux:       { return "Linux"; }
            case Apple:       { return "Apple"; }
            case Unsupported: { return "Unsupported"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "misc::Platform::ToString(" << E << ")_InvalidValueError.";
                throw std::runtime_error(ss.str());
            }
        }
    }


    std::unique_ptr<Platform> Platform::instanceUPtr_{ nullptr };


    Platform::Platform()
    :
        platform_(platform_type::Unknown)
    {}


    Platform * Platform::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void Platform::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new Platform);
        }
    }


    void Platform::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr), "misc::Platform::Release() found instanceUPtr that was null.");
        instanceUPtr_.reset();
    }


    void Platform::DetectAndLog()
    {
        std::ostringstream ss;
        platform_ = platform_type::Unknown;

#ifdef PLATFORM_DETECTED_IS_WINDOWS
        ss << "  Windows";
        platform_ = platform_type::Windows;
#endif

#ifdef PLATFORM_DETECTED_IS_APPLE9
        ss << "  APPLE (OS9)";
        platform_ = platform_type::Unsupported;
#endif

#ifdef PLATFORM_DETECTED_IS_APPLE
        ss << "  APPLE (OS)";
        platform_ = platform_type::Apple;
#endif

#ifdef PLATFORM_DETECTED_IS_LINUX
        ss << "  Linux";
        platform_ = platform_type::Linux;
#endif

#ifdef PLATFORM_DETECTED_IS_UNIX
        ss << "  Unix";
#endif

#ifdef PLATFORM_DETECTED_IS_POSIX
        ss << "  POSIX";
#endif

#ifdef PLATFORM_DETECTED_IS_CYGWIN
        ss << "  CYGWIN";
#endif

#ifdef PLATFORM_DETECTED_IS_LINUX_GNU
        ss << "  GNU/Linux";
#endif

        if (ss.str().empty())
            ss << "(None?  This platform is probably unsupported...)";

        M_HP_LOG("Platform(s) Detected:  " << ss.str() << "  The platform_type has been set to " << GetName() << ".");
    }

}
