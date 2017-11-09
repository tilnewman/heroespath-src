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
#ifndef HEROESPATH_CREATURE_SPELL_SPELLENUM_HPP_INCLUDED
#define HEROESPATH_CREATURE_SPELL_SPELLENUM_HPP_INCLUDED
//
// spell-enum.hpp
//
#include <string>
#include <vector>


namespace heroespath
{
namespace spell
{

    struct Spells
    {
        //Note:  Keep in sync with:
        //          spell::Warehouse::Fill()
        //          spells.hpp
        //          CombatAnimation::SpellAnimStart()
        //          CombatSoundEffects::PlaySpell()
        //          Initial::SetupInventory()
        enum Enum
        {
            Sparks = 0,
            Bandage,
            Sleep,
            Awaken,
            Trip,
            Lift,
            Daze,
            Panic,
            ClearMind,
            Poison,
            Antidote,//cures poison
            PoisonCloud,
            Count
        };

        //straight name without spaces
        static const std::string ToString(const Enum);

        //human readable with spaces
        static const std::string Name(const Enum);
        static const std::string ShortDesc(const Enum);
        static const std::string ExtraDesc(const Enum);
        static const std::string ImageFilename(const Enum);
    };

    using SpellVec_t = std::vector<Spells::Enum>;

}
}

#endif //HEROESPATH_CREATURE_SPELL_SPELLENUM_HPP_INCLUDED
