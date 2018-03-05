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
#ifndef HEROESPATH_LOG_LOGMACROS_HPP_INCLUDED
#define HEROESPATH_LOG_LOGMACROS_HPP_INCLUDED
//
// log-macros.hpp
//
#include "log/logger.hpp"
#include "log/macros.hpp"

// these defines will disable these macros
#if defined(MACRO_DISABLE_ALL) || defined(MACRO_DISABLE_ASSERTORTHROW)

// if disabled by ASSERT_OR_THROW_DISABLED, then produce no code
#define M_HP_LOG(str) ;

#else

#define M_HP_LOG(str_stream)                                                                       \
    {                                                                                              \
        {                                                                                          \
            std::ostringstream _m_oss_hp_log_temp;                                                 \
            _m_oss_hp_log_temp << str_stream;                                                      \
            M_LOG_DEF(*log::Logger::Instance(), _m_oss_hp_log_temp.str());                         \
        }                                                                                          \
    }

#define M_HP_LOG_DBG(str_stream)                                                                   \
    {                                                                                              \
        {                                                                                          \
            std::ostringstream _m_oss_hp_log_temp;                                                 \
            _m_oss_hp_log_temp << str_stream;                                                      \
            M_LOG_DBG(*log::Logger::Instance(), _m_oss_hp_log_temp.str());                         \
        }                                                                                          \
    }

#define M_HP_LOG_WRN(str_stream)                                                                   \
    {                                                                                              \
        {                                                                                          \
            std::ostringstream _m_oss_hp_log_temp;                                                 \
            _m_oss_hp_log_temp << str_stream;                                                      \
            M_LOG_WRN(*log::Logger::Instance(), _m_oss_hp_log_temp.str());                         \
        }                                                                                          \
    }

#define M_HP_LOG_ERR(str_stream)                                                                   \
    {                                                                                              \
        {                                                                                          \
            std::ostringstream _m_oss_hp_log_temp;                                                 \
            _m_oss_hp_log_temp << str_stream;                                                      \
            M_LOG_ERR(*log::Logger::Instance(), _m_oss_hp_log_temp.str());                         \
        }                                                                                          \
    }

#define M_HP_LOG_FAT(str_stream)                                                                   \
    {                                                                                              \
        {                                                                                          \
            std::ostringstream _m_oss_hp_log_temp;                                                 \
            _m_oss_hp_log_temp << str_stream;                                                      \
            M_LOG_FAT(*log::Logger::Instance(), _m_oss_hp_log_temp.str());                         \
        }                                                                                          \
    }

#endif //#if defined(MACRO_DISABLE_ALL) || defined(MACRO_DISABLE_ASSERTORTHROW)

#endif // HEROESPATH_LOG_LOGMACROS_HPP_INCLUDED
