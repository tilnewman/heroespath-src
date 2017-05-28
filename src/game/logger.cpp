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
// logger.cpp
//
#include "logger.hpp"

#include "game/log-macros.hpp"

#include "misc/assertlogandthrow.hpp"


namespace game
{

    std::unique_ptr<Logger> Logger::instanceUPtr_{ nullptr };


    Logger::Logger()
    :
        LogBase(appbase::logbase::LogBase::FILE_NAME_DEFAULT,
                appbase::logbase::LogBase::FILE_NAME_EXT_DEFAULT,
                "")
    {}


    Logger::~Logger()
    {}


    Logger * Logger::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void Logger::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new Logger);
        }
    }


    void Logger::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr), "game::Logger::Release() found instanceUPtr that was null.");
        instanceUPtr_.reset();
    }

}
