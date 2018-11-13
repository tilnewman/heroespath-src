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
#include "misc/timing-scoped.hpp"

#include <cstddef> //for std::size_t
#include <memory>
#include <sstream>
#include <string>

//

#define M_HP_FILE_FUNC_LINE_STR                                                                    \
    (std::string(__FILE__) + ":" + std::string(__func__) + "():" + std::to_string(__LINE__))

#define M_HP_FILE_FUNC_LINE_STR_VAR                                                                \
    const std::string FILE_FUNC_LINE_STR { M_HP_FILE_FUNC_LINE_STR };

//

namespace heroespath
{
namespace misc
{
    class TimeTrials;
}

struct LogMacroHelper
{
    static void Append(
        const misc::LogPriority::Enum PRIORITY,
        const std::string & MSG,
        const std::string & FILE,
        const std::string & FUNCTION,
        const int LINE);

    static void LogTimingStart();
    static void LogTimingStop();
    static misc::TimeTrials * GetLoggingTimeTrialsPtr();

private:
    static std::unique_ptr<misc::TimeTrials> loggingTimeTrialsUPtr_;
};

} // namespace heroespath

//

#if !defined(MACRO_DISABLE_ALL) && !defined(HEROESPATH_MACRO_DISABLE_LOG)

#define M_HP_LOG_PRI_ACTUAL(priority, str_stuff)                                                   \
    {                                                                                              \
        {                                                                                          \
            std::ostringstream _m_oss_hp_log_temp;                                                 \
            _m_oss_hp_log_temp << str_stuff;                                                       \
                                                                                                   \
            heroespath::LogMacroHelper::Append(                                                    \
                priority, _m_oss_hp_log_temp.str(), __FILE__, __func__, __LINE__);                 \
        }                                                                                          \
    }

#define M_HP_LOG_PRI(priority, str_stuff)                                                          \
    {                                                                                              \
        if (LogMacroHelper::GetLoggingTimeTrialsPtr() != nullptr)                                  \
        {                                                                                          \
            M_HP_SCOPED_TIME_TRIAL(*LogMacroHelper::GetLoggingTimeTrialsPtr(), 0);                 \
            M_HP_LOG_PRI_ACTUAL(priority, str_stuff);                                              \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            M_HP_LOG_PRI_ACTUAL(priority, str_stuff);                                              \
        }                                                                                          \
    }

#define M_HP_LOG_DBG(str_stuff) M_HP_LOG_PRI(heroespath::misc::LogPriority::Debug, str_stuff)
#define M_HP_LOG_DEF(str_stuff) M_HP_LOG_PRI(heroespath::misc::LogPriority::Default, str_stuff)
#define M_HP_LOG_WRN(str_stuff) M_HP_LOG_PRI(heroespath::misc::LogPriority::Warn, str_stuff)
#define M_HP_LOG_ERR(str_stuff) M_HP_LOG_PRI(heroespath::misc::LogPriority::Error, str_stuff)
#define M_HP_LOG_FAT(str_stuff) M_HP_LOG_PRI(heroespath::misc::LogPriority::Fatal, str_stuff)
#define M_HP_LOG(str_stuff) M_HP_LOG_DEF(str_stuff)

#else

#define M_HP_LOG_PRI(str_stuff) ;
#define M_HP_LOG_DBG(str_stuff) ;
#define M_HP_LOG_DEF(str_stuff) ;
#define M_HP_LOG_WRN(str_stuff) ;
#define M_HP_LOG_ERR(str_stuff) ;
#define M_HP_LOG_FAT(str_stuff) ;
#define M_HP_LOG(str_stuff) ;

#endif // end check if logging macros enabled

#endif // HEROESPATH_MISC_LOG_MACROS_HPP_INCLUDED
