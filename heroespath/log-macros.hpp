#ifndef HEROESPATH_LOGMACROS_HPP
#define HEROESPATH_LOGMACROS_HPP
//
// log-macros.hpp
//
#include "common/logmacros.hpp"
#include "heroespath/logger.hpp"


//these defines will disable these macros
#if defined(APPBASE_MACRO_DISABLE_ALL) || defined(APPBASE_MACRO_DISABLE_ASSERTORTHROW)

//if disabled by APPBASE_ASSERT_OR_THROW_DISABLED, then produce no code
#define M_HP_LOG(str);

#else

#define M_HP_LOG(str_stream)                                                \
{                                                                           \
    {                                                                       \
        std::ostringstream _m_oss_hp_log_temp;                              \
        _m_oss_hp_log_temp << str_stream;                                   \
        M_LOG( * heroespath::Logger::Instance(), _m_oss_hp_log_temp.str()); \
    }                                                                       \
}                                                                           \

#endif //#if defined(APPBASE_MACRO_DISABLE_ALL) || defined(APPBASE_MACRO_DISABLE_ASSERTORTHROW)

#endif //HEROESPATH_LOGMACROS_HPP
