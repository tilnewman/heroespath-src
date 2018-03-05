// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// sex-enum.hpp
//
#include "sex-enum.hpp"
#include <exception>
#include <sstream>

namespace heroespath
{
namespace creature
{

    const std::string sex::ToString(const sex::Enum E)
    {
        switch (E)
        {
            case Unknown:
            {
                return "Unknown";
            }
            case Male:
            {
                return "Male";
            }
            case Female:
            {
                return "Female";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "creature::sex::Enum::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    bool sex::IsValid(const sex::Enum E)
    {
        switch (E)
        {
            case Unknown:
            case Male:
            case Female:
            {
                return true;
            }
            case Count:
            default:
            {
                return false;
            }
        }
    }

    const std::string sex::HeSheIt(const sex::Enum E, const bool WILL_CAPITALIZE)
    {
        switch (E)
        {
            case Unknown:
            {
                if (WILL_CAPITALIZE)
                    return "It";
                else
                    return "it";
            }
            case Male:
            {
                if (WILL_CAPITALIZE)
                    return "He";
                else
                    return "he";
            }
            case Female:
            {
                if (WILL_CAPITALIZE)
                    return "She";
                else
                    return "she";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "creature::sex::Enum::HisHersIts(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    const std::string sex::HisHerIts(
        const sex::Enum E, const bool WILL_CAPITALIZE, const bool WILL_POSSESSIVE_HER)
    {
        switch (E)
        {
            case Unknown:
            {
                if (WILL_CAPITALIZE)
                    return "Its";
                else
                    return "its";
            }
            case Male:
            {
                if (WILL_CAPITALIZE)
                    return "His";
                else
                    return "his";
            }
            case Female:
            {
                if (WILL_CAPITALIZE)
                    return std::string("Her").append((WILL_POSSESSIVE_HER) ? "s" : "");
                else
                    return std::string("her").append((WILL_POSSESSIVE_HER) ? "s" : "");
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "creature::sex::Enum::HisHersIts(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    const std::string sex::HimHerIt(const sex::Enum E, const bool WILL_CAPITALIZE)
    {
        switch (E)
        {
            case Unknown:
            {
                if (WILL_CAPITALIZE)
                    return "It";
                else
                    return "it";
            }
            case Male:
            {
                if (WILL_CAPITALIZE)
                    return "Him";
                else
                    return "him";
            }
            case Female:
            {
                if (WILL_CAPITALIZE)
                    return "Her";
                else
                    return "her";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "creature::sex::Enum::HimHerIt(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }
} // namespace creature
} // namespace heroespath
