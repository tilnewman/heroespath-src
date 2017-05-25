///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef APPBASE_ASSERTORTHROW_HPP
#define APPBASE_ASSERTORTHROW_HPP
//
// assertorthrow.hpp
//
#include <exception>
#include <iostream>
#include <sstream>


//these defines will disable these macros
#if defined(APPBASE_MACRO_DISABLE_ALL) || defined(APPBASE_MACRO_DISABLE_ASSERTORTHROW)

//if disabled by APPBASE_ASSERT_OR_THROW_DISABLED, then produce no code
#define M_ASSERT_OR_THROW(exp, str) ;
#define M_ASSERT_OR_THROW_SS(exp, str) ;
#define M_TRY_ASSERT_OR_RETHROW_SS(might_throw, str_stream) (might_throw);

#elif defined(NDEBUG)

//in release builds use throw
#define M_ASSERT_OR_THROW(exp, str_static)          \
{                                                   \
    if(false == (exp))                              \
    {                                               \
        throw std::runtime_error( (str_static) );   \
    }                                               \
}


#define M_ASSERT_OR_THROW_SS(exp, str_stream)          \
{                                                      \
    if(false == (exp))                                 \
    {                                                  \
        std::ostringstream _m_oss_temp;                \
        _m_oss_temp << str_stream;                     \
        throw std::runtime_error( _m_oss_temp.str() ); \
    }                                                  \
}


#define M_TRY_ASSERT_OR_RETHROW_SS(might_throw, str_stream) \
try                                                         \
{                                                           \
    (might_throw);                                          \
}                                                           \
catch(const std::exception & E)                             \
{                                                           \
    std::ostringstream _m_oss_temp;                         \
    _m_oss_temp << str_stream << " in "                     \
                << __FILE__ << "::" << __LINE__             \
                << " threw std::exception ["                \
                << E.what() << "]";                         \
    throw std::runtime_error(_m_oss_temp.str().c_str());    \
}                                                           \
catch (...)                                                 \
{                                                           \
    std::ostringstream _m_oss_temp;                         \
    _m_oss_temp << str_stream << " in "                     \
                << __FILE__ << "::" << __LINE__             \
                << " threw UNKNOWN exception!";             \
    throw std::runtime_error(_m_oss_temp.str().c_str());    \
}


#else

//in debug (and all other builds) use assert
#include <cassert>
#define M_ASSERT_OR_THROW(exp, str_static)      \
{                                               \
    assert( (exp) && (str_static) );            \
}


#define M_ASSERT_OR_THROW_SS(exp, str_stream)   \
{                                               \
    if(false == (exp))                          \
    {                                           \
        std::cerr << "Assert failed "           \
                  << __FILE__ << "::"           \
                  << __LINE__ << " - "          \
                  << str_stream << std::endl;   \
        assert( (exp) );                        \
    }                                           \
}


#define M_TRY_ASSERT_OR_RETHROW_SS(might_throw, str_stream) \
try                                                         \
{                                                           \
    (might_throw);                                          \
}                                                           \
catch(const std::exception & E)                             \
{                                                           \
    std::cerr   << str_stream << " in "                     \
                << __FILE__ << "::" << __LINE__             \
                << " threw std::exception ["                \
                << E.what() << "]" << std::endl;            \
    assert(false);                                          \
}                                                           \
catch (...)                                                 \
{                                                           \
    std::cerr   << str_stream << " in "                     \
                << __FILE__ << "::" << __LINE__             \
                << " threw UNKNOWN exception!" << std::endl;\
    assert(false);                                          \
}


#endif //#if defined(APPBASE_MACRO_DISABLE_ALL) || defined(APPBASE_MACRO_DISABLE_ASSERTORTHROW)

#endif //APPBASE_ASSERTORTHROW_HPP
