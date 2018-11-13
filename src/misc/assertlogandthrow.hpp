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
#include "misc/log-macros.hpp"

#include <cassert>
#include <exception>
#include <sstream>

#if !defined(HEROESPATH_MACRO_DISABLE_ALL) && !defined(HEROESPATH_MACRO_DISABLE_ASSERT)

#if defined(NDEBUG)

// in release builds use throw

#define M_HP_ASSERT_OR_LOG_AND_THROW_HELPER(str_stuff)                                             \
    {                                                                                              \
        M_HP_LOG_FAT(str_stuff);                                                                   \
                                                                                                   \
        std::ostringstream _m_oss_assertorlogandthrow_temp;                                        \
                                                                                                   \
        _m_oss_assertorlogandthrow_temp << "M_HP_ASSERT_OR_LOG_AND_THROW  FAILED: " << str_stuff   \
                                        << " (" << __FILE__ << ":" << __LINE__ << ")";             \
                                                                                                   \
        throw std::runtime_error(_m_oss_assertorlogandthrow_temp.str());                           \
    }

#define M_HP_ASSERT_OR_LOG_AND_THROW(exp, str_stuff)                                               \
    {                                                                                              \
        {                                                                                          \
            if (!(exp))                                                                            \
            {                                                                                      \
                M_HP_ASSERT_OR_LOG_AND_THROW_HELPER(str_stuff)                                     \
            }                                                                                      \
        }                                                                                          \
    }

#define M_HP_ASSERT_OR_LOG_AND_THROW_CONSTEXPR(exp, str_stuff)                                     \
    {                                                                                              \
        {                                                                                          \
            if constexpr (!(exp))                                                                  \
            {                                                                                      \
                M_HP_ASSERT_OR_LOG_AND_THROW_HELPER(str_stuff)                                     \
            }                                                                                      \
        }                                                                                          \
    }

#else

// in debug (and all other builds) use assert

#define M_HP_ASSERT_OR_LOG_AND_THROW(exp, str_stuff)                                               \
    {                                                                                              \
        {                                                                                          \
            if (!(exp))                                                                            \
            {                                                                                      \
                M_HP_LOG_FAT(str_stuff);                                                           \
                assert((exp));                                                                     \
            }                                                                                      \
        }                                                                                          \
    }

#define M_HP_ASSERT_OR_LOG_AND_THROW_CONSTEXPR(exp, str_stuff)                                     \
    {                                                                                              \
        {                                                                                          \
            if constexpr (!(exp))                                                                  \
            {                                                                                      \
                M_HP_LOG_FAT(str_stuff);                                                           \
                assert((exp));                                                                     \
            }                                                                                      \
        }                                                                                          \
    }

#endif // end if debug or not

#else

// if disabled by ASSERT_OR_THROW_DISABLED, then produce no code
#define M_HP_ASSERT_OR_LOG_AND_THROW(exp, str_stuff) ;
#define M_HP_ASSERT_OR_LOG_AND_THROW_CONSTEXPR(exp, str_stuff) ;

#endif // end if assert macros enabled

#endif // MISC_ASSERTORLOGANDTHROW_HPP_INCLUDED
