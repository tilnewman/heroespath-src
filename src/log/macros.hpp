// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_LOG_MACROS_HPP_INCLUDED
#define HEROESPATH_LOG_MACROS_HPP_INCLUDED
//
// macros.hpp
//

// defines that disable logging macros
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

#define M_LOG_PRI(logger, priority, string_static)                                                 \
    {                                                                                              \
        (logger).Log((priority), __FILE__, __LINE__, (string_static));                             \
    }

#define M_LOG_PRI_SS(logger, priority, string_stream)                                              \
    {                                                                                              \
        std::ostringstream _m_oss_mlog_temp;                                                       \
        _m_oss_mlog_temp << string_stream;                                                         \
        (logger).Log((priority), __FILE__, __LINE__, _m_oss_mlog_temp.str());                      \
    }

#define M_LOG(logger, string_stream)                                                               \
    M_LOG_PRI_SS(logger, heroespath::log::LogPri::Default, string_stream);
#define M_LOG_NON(logger, string_stream)                                                           \
    M_LOG_PRI_SS(logger, heroespath::log::LogPri::None, string_stream);
#define M_LOG_DBG(logger, string_stream)                                                           \
    M_LOG_PRI_SS(logger, heroespath::log::LogPri::Debug, string_stream);
#define M_LOG_DEF(logger, string_stream)                                                           \
    M_LOG_PRI_SS(logger, heroespath::log::LogPri::Default, string_stream);
#define M_LOG_WRN(logger, string_stream)                                                           \
    M_LOG_PRI_SS(logger, heroespath::log::LogPri::Warn, string_stream);
#define M_LOG_ERR(logger, string_stream)                                                           \
    M_LOG_PRI_SS(logger, heroespath::log::LogPri::Error, string_stream);
#define M_LOG_FAT(logger, string_stream)                                                           \
    M_LOG_PRI_SS(logger, heroespath::log::LogPri::Fatal, string_stream);

#endif //#if defined(HEROESPATH_MACRO_DISABLE_ALL) || defined(HEROESPATH_MACRO_DISABLE_LOGGERS)

#endif // HEROESPATH_LOG_MACROS_HPP_INCLUDED
