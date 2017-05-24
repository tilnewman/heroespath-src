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

#elif NDEBUG

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
