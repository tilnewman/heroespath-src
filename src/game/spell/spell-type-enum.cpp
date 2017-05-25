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
// spell-type-enum.cpp
//
#include "spell-type-enum.hpp"

#include <sstream>
#include <exception>


namespace game
{
namespace spell
{

    const std::string SpellType::ToString(const Enum E)
    {
        switch (E)
        {
            case Attack:                    { return "Attack"; }
            case Heal:                      { return "Heal"; }
            case EffectItem:                { return "EffectItem"; }
            case EnchantItemHelpful:        { return "EnchantItemHelpful"; }
            case EnchantItemHarmful:        { return "EnchantItemHarmful"; }
            case EffectCreature:            { return "EffectCreature"; }
            case EnchantCreatureHelpful:    { return "EnchantCreatureHelpful"; }
            case EnchantCreatureHarmful:    { return "EnchantCreatureHarmful"; }
            case Misc:                      { return "Misc"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::spell::ToString(" << E << ")_InvalidValueError";
                throw std::runtime_error(ss.str());
            }
        }
    }


    const std::string SpellType::Name(const Enum E)
    {
        switch (E)
        {
            case Attack:                    { return "attack"; }
            case Heal:                      { return "heal"; }
            case EffectItem:                { return "effect item"; }
            case EnchantItemHelpful:        { return "helpful enchant item"; }
            case EnchantItemHarmful:        { return "harmful enchant item"; }
            case EffectCreature:            { return "effect creature"; }
            case EnchantCreatureHelpful:    { return "helpful enchant creature"; }
            case EnchantCreatureHarmful:    { return "harmful enchant creature"; }
            case Misc:                      { return "miscellaneous"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::spell::ToString(" << E << ")_InvalidValueError";
                throw std::runtime_error(ss.str());
            }
        }
    }

}
}
