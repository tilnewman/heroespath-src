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
#ifndef HEROESPATH_LOG_MACROS_HPP_INCLUDED
#define HEROESPATH_LOG_MACROS_HPP_INCLUDED
//
// macros.hpp
//


//these defines will disable these macros
#if defined(APPBASE_MACRO_DISABLE_ALL) || defined(APPBASE_MACRO_DISABLE_LOGGERS)

#warning "log macros M_LOG_...() have been disabled"
#define M_LOG_PRI(log, pri, str) ;
#define M_LOG_PRI_SS(log, pri, str) ;
#define M_LOG(log, str) ;
#define M_LOG_NON(log, str) ;
#define M_LOG_DBG(log, str) ;
#define M_LOG_DEF(log, str) ;
#define M_LOG_WRN(log, str) ;
#define M_LOG_ERR(log, str) ;
#define M_LOG_FAT(log, str) ;

#else

#include <sstream>
#include "log/ilog.hpp"
#define M_LOG_PRI(logThing, pri, str_static)                   \
{                                                              \
    (logThing).Log( (pri), __FILE__, __LINE__, (str_static) ); \
}

#define M_LOG_PRI_SS(logThing, pri, str_stream)                          \
{                                                                        \
    std::ostringstream _m_oss_mlog_temp;                                 \
    _m_oss_mlog_temp << str_stream;                                      \
    (logThing).Log( (pri), __FILE__, __LINE__, _m_oss_mlog_temp.str() ); \
}

#define M_LOG(logThing, str)     M_LOG_PRI_SS(logThing, heroespath::log::LogPri::Default, str);
#define M_LOG_NON(logThing, str) M_LOG_PRI_SS(logThing, heroespath::log::LogPri::None, str);
#define M_LOG_DBG(logThing, str) M_LOG_PRI_SS(logThing, heroespath::log::LogPri::Debug, str);
#define M_LOG_DEF(logThing, str) M_LOG_PRI_SS(logThing, heroespath::log::LogPri::Default, str);
#define M_LOG_WRN(logThing, str) M_LOG_PRI_SS(logThing, heroespath::log::LogPri::Warn, str);
#define M_LOG_ERR(logThing, str) M_LOG_PRI_SS(logThing, heroespath::log::LogPri::Error, str);
#define M_LOG_FAT(logThing, str) M_LOG_PRI_SS(logThing, heroespath::log::LogPri::Fatal, str);

#endif //HEROESPATH_#if defined(APPBASE_MACRO_DISABLE_ALL) || defined(APPBASE_MACRO_DISABLE_LOGGERS)


#endif //HEROESPATH_LOG_MACROS_HPP_INCLUDED
