#ifndef GAME_LOGMACROS_HPP
#define GAME_LOGMACROS_HPP
//
// log-macros.hpp
//
#include "common/logmacros.hpp"
#include "game/logger.hpp"


//these defines will disable these macros
#if defined(MACRO_DISABLE_ALL) || defined(MACRO_DISABLE_ASSERTORTHROW)

//if disabled by ASSERT_OR_THROW_DISABLED, then produce no code
#define M_HP_LOG(str);

#else

#define M_HP_LOG(str_stream)                                                    \
{                                                                               \
    {                                                                           \
        std::ostringstream _m_oss_hp_log_temp;                                  \
        _m_oss_hp_log_temp << str_stream;                                       \
        M_LOG_DEF( * game::Logger::Instance(), _m_oss_hp_log_temp.str()); \
    }                                                                           \
}                                                                               \

#define M_HP_LOG_DBG(str_stream)                                                \
{                                                                               \
    {                                                                           \
        std::ostringstream _m_oss_hp_log_temp;                                  \
        _m_oss_hp_log_temp << str_stream;                                       \
        M_LOG_DBG( * game::Logger::Instance(), _m_oss_hp_log_temp.str()); \
    }                                                                           \
}                                                                               \

#define M_HP_LOG_WRN(str_stream)                                                \
{                                                                               \
    {                                                                           \
        std::ostringstream _m_oss_hp_log_temp;                                  \
        _m_oss_hp_log_temp << str_stream;                                       \
        M_LOG_WRN( * game::Logger::Instance(), _m_oss_hp_log_temp.str()); \
    }                                                                           \
}                                                                               \

#define M_HP_LOG_ERR(str_stream)                                                \
{                                                                               \
    {                                                                           \
        std::ostringstream _m_oss_hp_log_temp;                                  \
        _m_oss_hp_log_temp << str_stream;                                       \
        M_LOG_ERR( * game::Logger::Instance(), _m_oss_hp_log_temp.str()); \
    }                                                                           \
}                                                                               \

#define M_HP_LOG_FAT(str_stream)                                                \
{                                                                               \
    {                                                                           \
        std::ostringstream _m_oss_hp_log_temp;                                  \
        _m_oss_hp_log_temp << str_stream;                                       \
        M_LOG_FAT( * game::Logger::Instance(), _m_oss_hp_log_temp.str()); \
    }                                                                           \
}                                                                               \

#endif //#if defined(MACRO_DISABLE_ALL) || defined(MACRO_DISABLE_ASSERTORTHROW)

#endif //GAME_LOGMACROS_HPP
