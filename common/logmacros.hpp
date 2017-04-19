#ifndef APPBASE_LOGMACROS_HPP
#define APPBASE_LOGMACROS_HPP
//
// logmacros.hpp
//


//these defines will disable these macros
#if defined(APPBASE_MACRO_DISABLE_ALL) || defined(APPBASE_MACRO_DISABLE_LOGGERS)

#warning "appbase macros M_LOG_...() have been disabled"
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
#include "common/ilog.hpp"
#define M_LOG_PRI(log, pri, str_static)                   \
{                                                         \
    (log).Log( (pri), __FILE__, __LINE__, (str_static) ); \
}

#define M_LOG_PRI_SS(log, pri, str_stream)                          \
{                                                                   \
    std::ostringstream _m_oss_mlog_temp;                            \
    _m_oss_mlog_temp << str_stream;                                 \
    (log).Log( (pri), __FILE__, __LINE__, _m_oss_mlog_temp.str() ); \
}

#define M_LOG(log, str)     M_LOG_PRI_SS(log, appbase::LogPri::Default, str);
#define M_LOG_NON(log, str) M_LOG_PRI_SS(log, appbase::LogPri::None, str);
#define M_LOG_DBG(log, str) M_LOG_PRI_SS(log, appbase::LogPri::Debug, str);
#define M_LOG_DEF(log, str) M_LOG_PRI_SS(log, appbase::LogPri::Default, str);
#define M_LOG_WRN(log, str) M_LOG_PRI_SS(log, appbase::LogPri::Warn, str);
#define M_LOG_ERR(log, str) M_LOG_PRI_SS(log, appbase::LogPri::Error, str);
#define M_LOG_FAT(log, str) M_LOG_PRI_SS(log, appbase::LogPri::Fatal, str);

#endif //#if defined(APPBASE_MACRO_DISABLE_ALL) || defined(APPBASE_MACRO_DISABLE_LOGGERS)


#endif //APPBASE_LOGMACROS_HPP
