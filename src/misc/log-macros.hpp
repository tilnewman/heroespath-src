// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_LOG_MACROS_HPP_INCLUDED
#define HEROESPATH_MISC_LOG_MACROS_HPP_INCLUDED
//
// log-macros.hpp
//
#include "misc/log-pri-enum.hpp"
#include "misc/string-stream-holder.hpp"

#include <ostream>

#if !defined(MACRO_DISABLE_ALL) && !defined(HEROESPATH_MACRO_DISABLE_LOG)

#define M_HP_LOG_PRI_SS(priority, str_streams)                                                     \
    {                                                                                              \
        misc::stringStreamHolder::ostreamer() << str_streams;                                      \
        misc::stringStreamHolder::log(false, priority, __FILE__, __func__, __LINE__);              \
    }

#define M_HP_LOG_PRI_STR(priority, str)                                                            \
    {                                                                                              \
        misc::stringStreamHolder::log(false, priority, __FILE__, __func__, __LINE__, str);         \
    }

#define M_HP_LOG_DBG(str_streams) M_HP_LOG_PRI_SS(heroespath::misc::LogPriority::Debug, str_streams)

#define M_HP_LOG_DEF(str_streams)                                                                  \
    M_HP_LOG_PRI_SS(heroespath::misc::LogPriority::Default, str_streams)

#define M_HP_LOG_WRN(str_streams) M_HP_LOG_PRI_SS(heroespath::misc::LogPriority::Warn, str_streams)
#define M_HP_LOG_ERR(str_streams) M_HP_LOG_PRI_SS(heroespath::misc::LogPriority::Error, str_streams)
#define M_HP_LOG_FAT(str_streams) M_HP_LOG_PRI_SS(heroespath::misc::LogPriority::Fatal, str_streams)
#define M_HP_LOG(str_streams) M_HP_LOG_DEF(str_streams)

#define M_HP_LOG_DBG_STR(str) M_HP_LOG_PRI_STR(heroespath::misc::LogPriority::Debug, str)
#define M_HP_LOG_DEF_STR(str) M_HP_LOG_PRI_STR(heroespath::misc::LogPriority::Default, str)
#define M_HP_LOG_WRN_STR(str) M_HP_LOG_PRI_STR(heroespath::misc::LogPriority::Warn, str)
#define M_HP_LOG_ERR_STR(str) M_HP_LOG_PRI_STR(heroespath::misc::LogPriority::Error, str)
#define M_HP_LOG_FAT_STR(str) M_HP_LOG_PRI_STR(heroespath::misc::LogPriority::Fatal, str)
#define M_HP_LOG_STR(str) M_HP_LOG_DEF_STR(str)

#else

#define M_HP_LOG_PRI(str_streams) ;
#define M_HP_LOG_DBG(str_streams) ;
#define M_HP_LOG_DEF(str_streams) ;
#define M_HP_LOG_WRN(str_streams) ;
#define M_HP_LOG_ERR(str_streams) ;
#define M_HP_LOG_FAT(str_streams) ;
#define M_HP_LOG(str_streams) ;

#define M_HP_LOG_PRI_STR(str) ;
#define M_HP_LOG_DBG_STR(str) ;
#define M_HP_LOG_DEF_STR(str) ;
#define M_HP_LOG_WRN_STR(str) ;
#define M_HP_LOG_ERR_STR(str) ;
#define M_HP_LOG_FAT_STR(str) ;
#define M_HP_LOG_STR(str) ;

#endif // end check if logging macros enabled

#endif // HEROESPATH_MISC_LOG_MACROS_HPP_INCLUDED
