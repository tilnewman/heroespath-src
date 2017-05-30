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
#ifndef GAME_NONPLAYER_OWNERSHIPPROFILE_INCLUDED
#define GAME_NONPLAYER_OWNERSHIPPROFILE_INCLUDED
//
// ownership-profile.hpp
//  Code that defines the ownership traits of a creature that
//  are used to predict the chance of having particular items/coins/etc.
//
#include "game/non-player/ownership-chance-types.hpp"
#include "game/creature/rank.hpp"
#include "game/item/item-type-enum.hpp"

#include <utility>
#include <vector>
#include <string>


namespace game
{
namespace non_player
{
    class Character;
    using CharacterPtr_t = Character *;

namespace ownership
{

    //defines the value of items owned by a creature
    struct wealth_type
    {
        enum Enum
        {
            Destitute = 0,
            Poor,
            LowerMiddle,
            UpperMiddle,
            Rich,
            Lavish,
            Royal,
            Count
        };

        static const std::string ToString(const wealth_type::Enum);
        static wealth_type::Enum FromRankType(const creature::rank_class::Enum RANK_TYPE);
        static wealth_type::Enum FromRank(const stats::Rank_t RANK_VALUE);
        static wealth_type::Enum FromCreature(const CharacterPtr_t CHARACTER_PTR);
    };


    //defines the number of items a creature owns
    struct collector_type
    {
        enum Enum : unsigned int
        {
            //pure random chance to carry items, should not be used
            NoPreference = 0,

            //rarely carries duplicates, usually only carries essential items
            Minimalist = 1 << 0,

            //often carries items that can be used (torche/lantern/healingherbs/etc.)
            Practical = 1 << 1,

            //often carries rare items or items of unique/cool materials
            Collector = 1 << 2,

            //often carries duplicates, especially items that cannot be used
            Hoarder = 1 << 3
        };

        static const std::string ToString(const collector_type::Enum);
        static collector_type::Enum FromCreature(const CharacterPtr_t CHARACTER_PTR);
    };


    //defines the frequency/power of magical items owned by the creature
    struct owns_magic_type
    {
        enum Enum
        {
            Rarely = 0, //rarely carries magic items
            Religous, //often carries religous items and only occasionally magic items
            Magical, //often carries religous and magic items
            Count
        };

        static owns_magic_type::Enum FromCreature(const CharacterPtr_t CHARACTER_PTR);
    };


    //define the complexity of items owned by the creature
    struct complexity_type
    {
        enum Enum
        {
            //...nothing really
            Animal = 0,

            //shortbow/longbow/blowpipe, rings,
            //leather/stone/bone/obsidian/lapis/horn/hide/tooth materials only
            Simple,

            //composite bow, medallion, scale material, iron/steel/tin metals only
            Moderate,

            //crossbow, lantern, necklas, work with all metals
            Complex,

            Count
        };

        static const std::string ToString(const complexity_type::Enum);
        static complexity_type::Enum FromString(const std::string &);
        static complexity_type::Enum FromCreature(const CharacterPtr_t CHARACTER_PTR);
    };


    //wrapper for info that describes what a creature will own
    struct Profile
    {
        Profile(const wealth_type::Enum     WEALTH_TYPE,
                const collector_type::Enum  COLLECTOR_TYPE,
                const owns_magic_type::Enum MAGIC_OWN_TYPE,
                const complexity_type::Enum COMPLEXITY_TYPE);

        static const Profile Make_FromCreature( const CharacterPtr_t CHARACTER_PTR);

        wealth_type::Enum     wealthType;
        collector_type::Enum  collectorType;
        owns_magic_type::Enum magicType;
        complexity_type::Enum complexityType;
    };

}
}
}

#endif //GAME_NONPLAYER_OWNERSHIPPROFILE_INCLUDED
