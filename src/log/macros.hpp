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


//defines that disable logging macros
#if defined(HEROESPATH_MACRO_DISABLE_ALL) || defined(HEROESPATH_MACRO_DISABLE_LOGGERS)

#warning "log macros M_LOG_...() have been disabled"
#define M_LOG_PRI(logger, priority, str) ;
#define M_LOG_PRI_SS(logger, priority, str) ;
#define M_LOG(logger, str) ;
#define M_LOG_NON(logger, str) ;
#define M_LOG_DBG(logger, str) ;
#define M_LOG_DEF(logger, str) ;
#define M_LOG_WRN(logger, str) ;
#define M_LOG_ERR(logger, str) ;
#define M_LOG_FAT(logger, str) ;

#else

#include "log/ilog.hpp"
#include <sstream>

#define M_LOG_PRI(logger, priority, string_static)                   \
{                                                                    \
    (logger).Log( (priority), __FILE__, __LINE__, (string_static) ); \
}

#define M_LOG_PRI_SS(logger, priority, string_stream)                       \
{                                                                           \
    std::ostringstream _m_oss_mlog_temp;                                    \
    _m_oss_mlog_temp << string_stream;                                      \
    (logger).Log( (priority), __FILE__, __LINE__, _m_oss_mlog_temp.str() ); \
}

#define M_LOG(logger, string_stream) M_LOG_PRI_SS(logger, heroespath::log::LogPri::Default, string_stream);
#define M_LOG_NON(logger, string_stream) M_LOG_PRI_SS(logger, heroespath::log::LogPri::None, string_stream);
#define M_LOG_DBG(logger, string_stream) M_LOG_PRI_SS(logger, heroespath::log::LogPri::Debug, string_stream);
#define M_LOG_DEF(logger, string_stream) M_LOG_PRI_SS(logger, heroespath::log::LogPri::Default, string_stream);
#define M_LOG_WRN(logger, string_stream) M_LOG_PRI_SS(logger, heroespath::log::LogPri::Warn, string_stream);
#define M_LOG_ERR(logger, string_stream) M_LOG_PRI_SS(logger, heroespath::log::LogPri::Error, string_stream);
#define M_LOG_FAT(logger, string_stream) M_LOG_PRI_SS(logger, heroespath::log::LogPri::Fatal, string_stream);

#endif //#if defined(HEROESPATH_MACRO_DISABLE_ALL) || defined(HEROESPATH_MACRO_DISABLE_LOGGERS)


#endif //HEROESPATH_LOG_MACROS_HPP_INCLUDED
