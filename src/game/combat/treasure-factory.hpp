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
#ifndef GAME_COMBAT_TREASUREFACTORY_HPP_INCLUDED
#define GAME_COMBAT_TREASUREFACTORY_HPP_INCLUDED
//
// treasure-factory.hpp
//
#include "game/stats/types.hpp"
#include "game/stats/trait-enum.hpp"
#include "game/stats/traits-set.hpp"
#include "game/item/item-type-enum.hpp"
#include "game/item/weapon-types.hpp"
#include "game/item/armor-types.hpp"
#include "game/item/item-profile.hpp"

#include "sfml-util/size-enum.hpp"


namespace game
{
namespace combat
{

    struct TreasureFactory
    {
        static int Score(const stats::TraitSet &);

        static int Score(const item::material::Enum PRI,
                         const item::material::Enum SEC = item::material::Nothing);

        static int Score(const item::weapon::axe_type::Enum);
        static int Score(const item::weapon::bladedstaff_type::Enum);
        static int Score(const item::weapon::club_type::Enum);
        static int Score(const item::weapon::projectile_type::Enum);
        static int Score(const item::weapon::sword_type::Enum);
        static int Score(const item::weapon::whip_type::Enum);

        static int ScoreKnife(const sfml_util::Size::Enum);
        static int ScoreDagger(const sfml_util::Size::Enum);
        static int ScoreStaff();
        static int ScoreQuarterStaff();

        static int Score(const item::armor::base_type::Enum);
        static int Score(const item::armor::cover_type::Enum);
        static int Score(const item::armor::helm_type::Enum);
        static int Score(const item::armor::shield_type::Enum);

        static int ScoreAventail(item::armor::base_type::Enum);
        static int ScoreBracer(item::armor::base_type::Enum);
        static int ScoreShirt(item::armor::base_type::Enum);
        static int ScoreBoots(item::armor::base_type::Enum);
        static int ScorePants(item::armor::base_type::Enum);
        static int ScoreGauntlets(item::armor::base_type::Enum);
        
    };

}
}

#endif //GAME_COMBAT_TREASUREFACTORY_HPP_INCLUDED
