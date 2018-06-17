// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_ASSERTORLOGANDTHROW_HPP_INCLUDED
#define HEROESPATH_MISC_ASSERTORLOGANDTHROW_HPP_INCLUDED
//
// assertlogandthrow.hpp
//
#include "log/logger.hpp"
#include "log/macros.hpp"

#include <exception>
#include <iostream>
#include <sstream>

// these defines will disable these macros
#if defined(HEROESPATH_MACRO_DISABLE_ALL) || defined(HEROESPATH_MACRO_DISABLE_ASSERTORTHROW)

// if disabled by ASSERT_OR_THROW_DISABLED, then produce no code
#define M_ASSERT_OR_LOGANDTHROW(exp, str) ;
#define M_ASSERT_OR_LOGANDTHROW_SS(exp, str) ;

#elif defined(NDEBUG)

// in release builds use throw
#define M_ASSERT_OR_LOGANDTHROW(exp, str_static)                                                   \
    {                                                                                              \
        {                                                                                          \
            if (!(exp))                                                                            \
            {                                                                                      \
                M_LOG_FAT(*heroespath::log::Logger::Instance(), (str_static));                     \
                throw std::runtime_error((str_static));                                            \
            }                                                                                      \
        }                                                                                          \
    }

#define M_ASSERT_OR_LOGANDTHROW_SS(exp, str_stream)                                                \
    {                                                                                              \
        {                                                                                          \
            if (!(exp))                                                                            \
            {                                                                                      \
                std::ostringstream _m_oss_logandthrow_temp;                                        \
                _m_oss_logandthrow_temp << str_stream;                                             \
                M_LOG_FAT(*heroespath::log::Logger::Instance(), _m_oss_logandthrow_temp.str());    \
                throw std::runtime_error(_m_oss_logandthrow_temp.str());                           \
            }                                                                                      \
        }                                                                                          \
    }

#define M_ASSERT_OR_LOGANDTHROW_CONSTEXPR(exp, str_static)                                         \
    {                                                                                              \
        {                                                                                          \
            if constexpr (!(exp))                                                                  \
            {                                                                                      \
                M_LOG_FAT(*heroespath::log::Logger::Instance(), (str_static));                     \
                throw std::runtime_error((str_static));                                            \
            }                                                                                      \
        }                                                                                          \
    }

#define M_ASSERT_OR_LOGANDTHROW_SS_CONSTEXPR(exp, str_stream)                                      \
    {                                                                                              \
        {                                                                                          \
            if constexpr (!(exp))                                                                  \
            {                                                                                      \
                std::ostringstream _m_oss_logandthrow_temp;                                        \
                _m_oss_logandthrow_temp << str_stream;                                             \
                M_LOG_FAT(*heroespath::log::Logger::Instance(), _m_oss_logandthrow_temp.str());    \
                throw std::runtime_error(_m_oss_logandthrow_temp.str());                           \
            }                                                                                      \
        }                                                                                          \
    }

#else

// in debug (and all other builds) use assert
#include <cassert>
#define M_ASSERT_OR_LOGANDTHROW(exp, str_static)                                                   \
    {                                                                                              \
        {                                                                                          \
            if (!(exp))                                                                            \
            {                                                                                      \
                std::cerr << (str_static) << std::endl;                                            \
                M_LOG_FAT(*heroespath::log::Logger::Instance(), (str_static));                     \
                assert((exp));                                                                     \
            }                                                                                      \
        }                                                                                          \
    }

#define M_ASSERT_OR_LOGANDTHROW_SS(exp, str_stream)                                                \
    {                                                                                              \
        {                                                                                          \
            if (!(exp))                                                                            \
            {                                                                                      \
                std::ostringstream _m_oss_logandthrow_temp;                                        \
                _m_oss_logandthrow_temp << "Assert failed " << __FILE__ << "::" << __LINE__        \
                                        << " - " << str_stream;                                    \
                                                                                                   \
                std::cerr << _m_oss_logandthrow_temp.str() << std::endl;                           \
                M_LOG_FAT(*heroespath::log::Logger::Instance(), _m_oss_logandthrow_temp.str());    \
                assert((exp));                                                                     \
            }                                                                                      \
        }                                                                                          \
    }

#define M_ASSERT_OR_LOGANDTHROW_CONSTEXPR(exp, str_static)                                         \
    {                                                                                              \
        {                                                                                          \
            if constexpr (!(exp))                                                                  \
            {                                                                                      \
                std::cerr << (str_static) << std::endl;                                            \
                M_LOG_FAT(*heroespath::log::Logger::Instance(), (str_static));                     \
                assert((exp));                                                                     \
            }                                                                                      \
        }                                                                                          \
    }

#define M_ASSERT_OR_LOGANDTHROW_SS_CONSTEXPR(exp, str_stream)                                      \
    {                                                                                              \
        {                                                                                          \
            if constexpr (!(exp))                                                                  \
            {                                                                                      \
                std::ostringstream _m_oss_logandthrow_temp;                                        \
                _m_oss_logandthrow_temp << "Assert failed " << __FILE__ << "::" << __LINE__        \
                                        << " - " << str_stream;                                    \
                                                                                                   \
                std::cerr << _m_oss_logandthrow_temp.str() << std::endl;                           \
                M_LOG_FAT(*heroespath::log::Logger::Instance(), _m_oss_logandthrow_temp.str());    \
                assert((exp));                                                                     \
            }                                                                                      \
        }                                                                                          \
    }

#endif //#if defined(MACRO_DISABLE_ALL) || defined(MACRO_DISABLE_ASSERTORTHROW)

#endif // MISC_ASSERTORLOGANDTHROW_HPP_INCLUDED
