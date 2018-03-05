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
#ifndef HEROESPATH_MISC_STRINGS_HPP_INCLUDED
#define HEROESPATH_MISC_STRINGS_HPP_INCLUDED
//
// strings.hpp
//
#include <sstream>
#include <string>

namespace heroespath
{
namespace misc
{

    struct String
    {

        template <typename T>
        static const std::string DecorateNumber(const T NUMBER)
        {
            std::ostringstream ss;
            ss << NUMBER;

            if (NUMBER == 1)
            {
                ss << "st";
            }
            else if (NUMBER == 2)
            {
                ss << "nd";
            }
            else if (NUMBER == 3)
            {
                ss << "rd";
            }
            else
            {
                ss << "th";
            }

            return ss.str();
        }
    };
} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_STRINGS_HPP_INCLUDED
