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
#ifndef GAME_ITEM_ARMORTYPES_INCLUDED
#define GAME_ITEM_ARMORTYPES_INCLUDED
//
// armor-types.hpp
//  A set of enumerations that define a piece of armor.
//
#include <string>


namespace game
{
namespace item
{
namespace armor
{

    struct shield_type
    {
        enum Enum
        {
            Buckler = 0,
            Kite,
            Heater,
            Pavis,
            Count
        };

        static const std::string ToString(const shield_type::Enum);
        inline static const std::string ToStringFull(const shield_type::Enum E) { return ToString(E) + " Shield"; }
        static shield_type::Enum FromString(const std::string &);
    };


    struct helm_type
    {
        enum Enum
        {
            Leather = 0,
            MailCoif,
            Kettle,
            Archers,
            Bascinet,
            Great,
            Count
        };

        static const std::string ToString(const helm_type::Enum);
        inline static const std::string ToStringFull(const helm_type::Enum E) { return ToString(E) + " Helm"; }
        static helm_type::Enum FromString(const std::string &);
    };


    struct base_type
    {
        enum Enum
        {
            Plain = 0,
            Mail,
            Scale,
            Plate,
            Count
        };

        static const std::string ToString(const base_type::Enum);
        static base_type::Enum FromString(const std::string &);
    };


    struct cover_type
    {
        enum Enum
        {
            Cape = 0,
            Vest,
            Robe,
            Cloak,
            Count
        };

        static const std::string ToString(const cover_type::Enum);
        static cover_type::Enum FromString(const std::string &);
    };


}//namespace armor
}//namespace item
}//namespace game

#endif //GAME_ITEM_ARMORTYPES_INCLUDED
