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
#ifndef HEROESPATH_MISC_BOOST_OPTIONAL_THAT_THROWS_HPP_INCLUDED
#define HEROESPATH_MISC_BOOST_OPTIONAL_THAT_THROWS_HPP_INCLUDED
//
// boost-optional-that-throws.hpp
//
#include <boost/exception/to_string.hpp>
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
}

#define BOOST_ENABLE_ASSERT_HANDLER
#include <boost/optional.hpp>
#undef BOOST_ENABLE_ASSERT_HANDLER

#endif // HEROESPATH_MISC_BOOST_OPTIONAL_THAT_THROWS_HPP_INCLUDED
