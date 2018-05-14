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
// armor-types.cpp
//
#include "armor-types.hpp"

namespace heroespath
{
namespace item
{
    namespace armor
    {

        const std::string shield_type::ToString(const shield_type::Enum SHEILD_TYPE)
        {
            switch (SHEILD_TYPE)
            {
                case Buckler:
                {
                    return "Buckler";
                }
                case Kite:
                {
                    return "Kite";
                }
                case Heater:
                {
                    return "Heater";
                }
                case Pavis:
                {
                    return "Pavis";
                }
                case Count:
                default:
                {
                    ThrowInvalidValueForFunction(SHEILD_TYPE, "ToString");
                }
            }
        }

        const std::string helm_type::ToString(const helm_type::Enum HELM_TYPE)
        {
            switch (HELM_TYPE)
            {
                case Leather:
                {
                    return "Leather";
                }
                case MailCoif:
                {
                    return "MailCoif";
                }
                case Kettle:
                {
                    return "Kettle";
                }
                case Archers:
                {
                    return "Archers";
                }
                case Bascinet:
                {
                    return "Bascinet";
                }
                case Great:
                {
                    return "Great";
                }
                case Count:
                default:
                {
                    ThrowInvalidValueForFunction(HELM_TYPE, "ToString");
                }
            };
        }

        const std::string helm_type::Name(const helm_type::Enum HELM_TYPE)
        {
            if (HELM_TYPE == MailCoif)
            {
                return "Mail Coif";
            }
            else
            {
                return ToString(HELM_TYPE);
            }
        }

        const std::string base_type::ToString(const base_type::Enum BASE_TYPE)
        {
            switch (BASE_TYPE)
            {
                case Plain:
                {
                    return "Plain";
                }
                case Mail:
                {
                    return "Mail";
                }
                case Scale:
                {
                    return "Scale";
                }
                case Plate:
                {
                    return "Plate";
                }
                case Count:
                default:
                {
                    ThrowInvalidValueForFunction(BASE_TYPE, "ToString");
                }
            };
        }

        const std::string cover_type::ToString(const cover_type::Enum COVER_TYPE)
        {
            switch (COVER_TYPE)
            {
                case Cape:
                {
                    return "Cape";
                }
                case Vest:
                {
                    return "Vest";
                }
                case Robe:
                {
                    return "Robe";
                }
                case Cloak:
                {
                    return "Cloak";
                }
                case Count:
                default:
                {
                    ThrowInvalidValueForFunction(COVER_TYPE, "ToString");
                }
            };
        }

    } // namespace armor
} // namespace item
} // namespace heroespath
