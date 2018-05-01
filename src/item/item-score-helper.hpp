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
#ifndef HEROESPATH_ITEM_ITEM_SCORE_HELPER_HPP_INCLUDE
#define HEROESPATH_ITEM_ITEM_SCORE_HELPER_HPP_INCLUDE
//
// item-score-helper.hpp
//
#include "item/armor-types.hpp"
#include "item/item-type-enum.hpp"
#include "item/weapon-types.hpp"
#include "misc/types.hpp"
#include "sfml-util/size-enum.hpp"
#include "stats/traits-set.hpp"

namespace heroespath
{
namespace item
{

    // Responsible for calculating scores for various item properties.
    struct ScoreHelper
    {
        static Score_t Score(const stats::TraitSet &);

        static Score_t
            Score(const material::Enum PRI, const material::Enum SEC = material::Nothing);

        static Score_t Score(const weapon::axe_type::Enum);
        static Score_t Score(const weapon::bladedstaff_type::Enum);
        static Score_t Score(const weapon::club_type::Enum);
        static Score_t Score(const weapon::projectile_type::Enum);
        static Score_t Score(const weapon::sword_type::Enum);
        static Score_t Score(const weapon::whip_type::Enum);

        static Score_t ScoreKnife(const sfml_util::Size::Enum);
        static Score_t ScoreDagger(const sfml_util::Size::Enum);
        static Score_t ScoreStaff();
        static Score_t ScoreQuarterStaff();

        static Score_t Score(const armor::cover_type::Enum);
        static Score_t Score(const armor::helm_type::Enum);
        static Score_t Score(const armor::shield_type::Enum);

        static Score_t Score(const armor_type::Enum, const armor::base_type::Enum);

        static Score_t Score(const armor::base_type::Enum);
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEM_SCORE_HELPER_HPP_INCLUDE
