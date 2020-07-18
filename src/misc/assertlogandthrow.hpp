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
#include "misc/string-stream-holder.hpp"

#if !defined(HEROESPATH_MACRO_DISABLE_ALL) && !defined(HEROESPATH_MACRO_DISABLE_ASSERT)

#if defined(NDEBUG)

// release mode

#define M_HP_ASSERT_OR_LOG_AND_THROW_HELPER_STR(exp, str)                                          \
    {                                                                                              \
        misc::StringStreamHolder::log(                                                             \
            heroespath::misc::LogPriority::Fatal, __FILE__, __func__, __LINE__, str);              \
                                                                                                   \
        std::ostringstream _m_hp_ass_ss;                                                           \
        _m_hp_ass_ss << "FATAL: " << __FILE__ << __func__ << __LINE__ << str;                      \
                                                                                                   \
        throw std::runtime_error(_m_hp_ass_ss.str());                                              \
    }

#define M_HP_ASSERT_OR_LOG_AND_THROW_HELPER_SS(exp, str_streams)                                   \
    {                                                                                              \
        std::ostringstream _m_hp_ass_ss;                                                           \
        _m_hp_ass_ss << str_streams;                                                               \
                                                                                                   \
        misc::StringStreamHolder::log(                                                             \
            heroespath::misc::LogPriority::Fatal,                                                  \
            __FILE__,                                                                              \
            __func__,                                                                              \
            __LINE__,                                                                              \
            _m_hp_ass_ss.str());                                                                   \
                                                                                                   \
        _m_hp_ass_ss.clear();                                                                      \
        _m_hp_ass_ss.str("");                                                                      \
        _m_hp_ass_ss << "FATAL: " << __FILE__ << __func__ << __LINE__ << str_streams;              \
                                                                                                   \
        throw std::runtime_error(_m_hp_ass_ss.str());                                              \
    }

#else

// debug mode
#include <cassert>

#define M_HP_ASSERT_OR_LOG_AND_THROW_HELPER_STR(exp, str)                                          \
    {                                                                                              \
        misc::StringStreamHolder::log(                                                             \
            heroespath::misc::LogPriority::Fatal, __FILE__, __func__, __LINE__, str);              \
                                                                                                   \
        assert((exp));                                                                             \
    }

#define M_HP_ASSERT_OR_LOG_AND_THROW_HELPER_SS(exp, str_streams)                                   \
    {                                                                                              \
        std::ostringstream _m_hp_ass_ss;                                                           \
        _m_hp_ass_ss << str_streams;                                                               \
                                                                                                   \
        misc::StringStreamHolder::log(                                                             \
            heroespath::misc::LogPriority::Fatal,                                                  \
            __FILE__,                                                                              \
            __func__,                                                                              \
            __LINE__,                                                                              \
            _m_hp_ass_ss.str());                                                                   \
                                                                                                   \
        assert((exp));                                                                             \
    }

#endif

#else

// macros turned off
#define M_HP_ASSERT_OR_LOG_AND_THROW_HELPER_STR(exp, str) ;
#define M_HP_ASSERT_OR_LOG_AND_THROW_HELPER_SS(exp, str_streams) ;

#endif

//

#define M_HP_ASSERT_OR_LOG_AND_THROW_STR(exp, str)                                                 \
    {                                                                                              \
        if (!(exp))                                                                                \
        {                                                                                          \
            M_HP_ASSERT_OR_LOG_AND_THROW_HELPER_STR(exp, str)                                      \
        }                                                                                          \
    }

#define M_HP_ASSERT_OR_LOG_AND_THROW_CONSTEXPR_STR(exp, str)                                       \
    {                                                                                              \
        if constexpr (!(exp))                                                                      \
        {                                                                                          \
            M_HP_ASSERT_OR_LOG_AND_THROW_HELPER_STR(exp, str)                                      \
        }                                                                                          \
    }

#define M_HP_ASSERT_OR_LOG_AND_THROW_SS(exp, str_streams)                                          \
    {                                                                                              \
        if (!(exp))                                                                                \
        {                                                                                          \
            M_HP_ASSERT_OR_LOG_AND_THROW_HELPER_SS(exp, str_streams)                               \
        }                                                                                          \
    }

#define M_HP_ASSERT_OR_LOG_AND_THROW_CONSTEXPR_SS(exp, str_streams)                                \
    {                                                                                              \
        if constexpr (!(exp))                                                                      \
        {                                                                                          \
            M_HP_ASSERT_OR_LOG_AND_THROW_HELPER_SS(exp, str_streams)                               \
        }                                                                                          \
    }

#define M_HP_ASSERT_OR_LOG_AND_THROW(exp, str_streams)                                             \
    M_HP_ASSERT_OR_LOG_AND_THROW_SS(exp, str_streams)

#define M_HP_ASSERT_OR_LOG_AND_THROW_CONSTEXPR(exp, str_streams)                                   \
    M_HP_ASSERT_OR_LOG_AND_THROW_CONSTEXPR_SS(exp, str_streams)

#endif // MISC_ASSERTORLOGANDTHROW_HPP_INCLUDED
