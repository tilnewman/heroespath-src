// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_BOOST_OPTIONAL_THAT_THROWS_HPP_INCLUDED
#define HEROESPATH_MISC_BOOST_OPTIONAL_THAT_THROWS_HPP_INCLUDED
//
// boost-optional-that-throws.hpp
//
#include <boost/exception/to_string.hpp>

#include <cstddef>
#include <string>

namespace boost
{
[[noreturn]] inline void
    assertion_failed(char const * expr, char const * function, char const * file, long line)
{
    throw std::runtime_error(
        std::string() + "Boost Assertion Failed:  "
        + ((std::string("this->is_initialized()") == expr)
               ? "(a boost::optional was accessed without being initialized)  "
               : "")
        + "expr=" + expr + " at " + function + ":" + file + ":" + boost::to_string(line));
}
} // namespace boost

#define BOOST_ENABLE_ASSERT_HANDLER
#include <boost/optional.hpp>
#undef BOOST_ENABLE_ASSERT_HANDLER

namespace heroespath
{

using CharOpt_t = boost::optional<char>;
using UCharOpt_t = boost::optional<unsigned char>;

using IntOpt_t = boost::optional<int>;
using UIntOpt_t = boost::optional<unsigned int>;

using SizetOpt_t = boost::optional<std::size_t>;

using FloatOpt_t = boost::optional<float>;
using DoubleOpt_t = boost::optional<double>;

} // namespace heroespath

#endif // HEROESPATH_MISC_BOOST_OPTIONAL_THAT_THROWS_HPP_INCLUDED
