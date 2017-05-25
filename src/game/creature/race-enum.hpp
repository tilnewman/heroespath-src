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
#ifndef GAME_CREATURE_RACEENUM_INCLUDED
#define GAME_CREATURE_RACEENUM_INCLUDED
//
// race-enum.hpp
//  An enumeration of all Races.
//
#include <string>


namespace game
{
namespace creature
{

    struct race
    {
        enum Enum
        {
            Human = 0,
            Gnome,
            Pixie,
            Wolfen,
            Dragon,
            Count_PlayerRaces,
            Goblin = Count_PlayerRaces,
            Troll,
            Orc,
            Newt,
            Spider,
            Bog,
            CaveCrawler,
            Hydra,
            LizardWalker,
            Minotaur,
            Ogre,
            Plant,
            Shade,
            Skeleton,
            Beetle,
            Boar,
            Demon,
            Griffin,
            Halfling,
            Lion,
            LionBoar,
            Naga,
            Ramonaut,
            Serpent,
            Cobra,
            Werebear,
            Wereboar,
            Werecat,
            Werewolf,
            Werebat,
            Pug,
            Wyvern,
            Giant,
            Ghoul,
            Count
        };

        static const std::string ToString(const race::Enum);
        inline static const std::string Name(const race::Enum E) { return ToString(E); }
        static const std::string Desc(const race::Enum);
        static const std::string Abbr(const race::Enum);
        static bool HasTorso(const race::Enum);
        static bool CanFly(const race::Enum);
        static bool WillInitiallyFly(const race::Enum);
    };

}
}
#endif //GAME_CREATURE_RACEENUM_INCLUDED
