// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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
