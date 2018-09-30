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

#include <sstream>
#include <string>

namespace heroespath
{
struct LogMacroHelper
{
    static void Append(
        const misc::LogPriority::Enum PRIORITY,
        const std::string & MSG,
        const std::string & FILE,
        const int LINE);
};
} // namespace heroespath

// these defines will disable these macros
#if defined(MACRO_DISABLE_ALL) || defined(HEROESPATH_MACRO_DISABLE_LOG)

// if disabled by HEROESPATH_MACRO_DISABLE_LOG, then produce no code
#define M_HP_LOG_PRI(str_stuff) ;
#define M_HP_LOG_DBG(str_stuff) ;
#define M_HP_LOG_DEF(str_stuff) ;
#define M_HP_LOG_WRN(str_stuff) ;
#define M_HP_LOG_ERR(str_stuff) ;
#define M_HP_LOG_FAT(str_stuff) ;
#define M_HP_LOG(str_stuff) ;

#else

#define M_HP_LOG_PRI(priority, str_stuff)                                                          \
    {                                                                                              \
        {                                                                                          \
            std::ostringstream _m_oss_hp_log_temp;                                                 \
            _m_oss_hp_log_temp << str_stuff;                                                       \
                                                                                                   \
            heroespath::LogMacroHelper::Append(                                                    \
                priority, _m_oss_hp_log_temp.str(), __FILE__, __LINE__);                           \
        }                                                                                          \
    }

#define M_HP_LOG_DBG(str_stuff) M_HP_LOG_PRI(heroespath::misc::LogPriority::Debug, str_stuff)
#define M_HP_LOG_DEF(str_stuff) M_HP_LOG_PRI(heroespath::misc::LogPriority::Default, str_stuff)
#define M_HP_LOG_WRN(str_stuff) M_HP_LOG_PRI(heroespath::misc::LogPriority::Warn, str_stuff)
#define M_HP_LOG_ERR(str_stuff) M_HP_LOG_PRI(heroespath::misc::LogPriority::Error, str_stuff)
#define M_HP_LOG_FAT(str_stuff) M_HP_LOG_PRI(heroespath::misc::LogPriority::Fatal, str_stuff)
#define M_HP_LOG(str_stuff) M_HP_LOG_DEF(str_stuff)

#endif //#if defined(MACRO_DISABLE_ALL) || defined(HEROESPATH_MACRO_DISABLE_LOG)

#endif // HEROESPATH_MISC_LOG_MACROS_HPP_INCLUDED
