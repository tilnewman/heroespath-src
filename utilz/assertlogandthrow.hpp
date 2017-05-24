#ifndef UTILZ_ASSERTORLOGANDTHROW_HPP_INCLUDED
#define UTILZ_ASSERTORLOGANDTHROW_HPP_INCLUDED
//
// assertlogandthrow.hpp
//
#include "common/logmacros.hpp"
#include "game/logger.hpp"

#include <exception>
#include <iostream>
#include <sstream>


//these defines will disable these macros
#if defined(MACRO_DISABLE_ALL) || defined(MACRO_DISABLE_ASSERTORTHROW)

//if disabled by ASSERT_OR_THROW_DISABLED, then produce no code
#define M_ASSERT_OR_LOGANDTHROW(exp, str) ;
#define M_ASSERT_OR_LOGANDTHROW_SS(exp, str) ;

#elif defined(NDEBUG)

//in release builds use throw
#define M_ASSERT_OR_LOGANDTHROW(exp, str_static)                        \
{                                                                       \
    {                                                                   \
        if(false == (exp))                                              \
        {                                                               \
            M_LOG_FAT( * game::Logger::Instance(), (str_static));       \
            throw std::runtime_error( (str_static) );                   \
        }                                                               \
    }                                                                   \
}                                                                       \

#define M_ASSERT_OR_LOGANDTHROW_SS(exp, str_stream)                                       \
{                                                                                         \
    {                                                                                     \
        if(false == (exp))                                                                \
        {                                                                                 \
            std::ostringstream _m_oss_logandthrow_temp;                                   \
            _m_oss_logandthrow_temp << str_stream;                                        \
            M_LOG_FAT( * game::Logger::Instance(), _m_oss_logandthrow_temp.str() );       \
            throw std::runtime_error( _m_oss_logandthrow_temp.str() );                    \
        }                                                                                 \
    }                                                                                     \
}                                                                                         \

#else

//in debug (and all other builds) use assert
#include <cassert>
#define M_ASSERT_OR_LOGANDTHROW(exp, str_static)                        \
{                                                                       \
    {                                                                   \
        if (false == (exp))                                             \
        {                                                               \
            std::cerr << (str_static) << std::endl;                     \
            M_LOG_FAT( * game::Logger::Instance(), (str_static));       \
            assert( (exp) );                                            \
        }                                                               \
    }                                                                   \
}                                                                       \

#define M_ASSERT_OR_LOGANDTHROW_SS(exp, str_stream)                                      \
{                                                                                        \
    {                                                                                    \
        if(false == (exp))                                                               \
        {                                                                                \
            std::ostringstream _m_oss_logandthrow_temp;                                  \
            _m_oss_logandthrow_temp << "Assert failed "                                  \
                                    << __FILE__ << "::"                                  \
                                    << __LINE__ << " - "                                 \
                                    << str_stream;                                       \
                                                                                         \
            std::cerr << _m_oss_logandthrow_temp.str() << std::endl;                     \
            M_LOG_FAT( * game::Logger::Instance(), _m_oss_logandthrow_temp.str());       \
            assert( (exp) );                                                             \
        }                                                                                \
    }                                                                                    \
}                                                                                        \

#endif //#if defined(MACRO_DISABLE_ALL) || defined(MACRO_DISABLE_ASSERTORTHROW)

#endif //UTILZ_ASSERTORLOGANDTHROW_HPP_INCLUDED
