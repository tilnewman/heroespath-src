// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ITEM_SCORE_HELPER_HPP_INCLUDE
#define HEROESPATH_ITEM_ITEM_SCORE_HELPER_HPP_INCLUDE
//
// item-score-helper.hpp
//
#include "item/armor-types.hpp"
#include "item/item-type-enum.hpp"
#include "item/weapon-types.hpp"
#include "misc/types.hpp"
#include "stats/traits-set.hpp"

namespace heroespath
{
namespace item
{

    // Responsible for calculating scores for items that are not based on enchantments.  See
    // EnchantmentFactory for score calculations based on things like
    // unique_type/set_type/misc_type/named_type/element_type.
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

        static Score_t ScoreKnife();
        static Score_t ScoreDagger();
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
