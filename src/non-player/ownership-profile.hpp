// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_NONPLAYER_OWNERSHIPPROFILE_HPP_INCLUDED
#define HEROESPATH_NONPLAYER_OWNERSHIPPROFILE_HPP_INCLUDED
//
// ownership-profile.hpp
//  Code that defines the ownership traits of a creature that
//  are used to predict the chance of having particular items/coins/etc.
//
#include "creature/rank.hpp"
#include "item/item-type-enum.hpp"
#include "misc/enum-util.hpp"
#include "misc/not-null.hpp"
#include "misc/types.hpp"
#include "non-player/ownership-chance-types.hpp"

#include <string>
#include <utility>
#include <vector>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
} // namespace creature
namespace non_player
{
    namespace ownership
    {

        // defines the value of items owned by a creature
        struct wealth_type : public misc::EnumBaseCounting<wealth_type, misc::EnumFirstValueValid>
        {
            enum Enum : misc::EnumUnderlying_t
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
            static wealth_type::Enum FromRankType(const creature::rank_class::Enum RANK_CLASS);
            static wealth_type::Enum FromRank(const Rank_t & RANK);
            static wealth_type::Enum FromCreature(const creature::CreaturePtr_t CHARACTER_PTR);
        };

        // defines the number of items a creature owns
        struct collector_type : misc::EnumBaseBitField<collector_type>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                // pure random chance to carry items, should not be used
                None = 0,

                // rarely carries duplicates, usually only carries essential items
                Minimalist = 1 << 0,

                // often carries items that can be used (torche/lantern/healingherbs/etc.)
                Practical = 1 << 1,

                // often carries rare items or items of unique/cool materials
                Collector = 1 << 2,

                // often carries duplicates, especially items that cannot be used
                Hoarder = 1 << 3,

                Last = Hoarder
            };

            static void ToStringPopulate(
                std::ostringstream & ss,
                const misc::EnumUnderlying_t ENUM_VALUE,
                const std::string & SEPARATOR = ", ");

            static collector_type::Enum FromCreature(const creature::CreaturePtr_t CHARACTER_PTR);
        };

        // defines the frequency/power of magical items owned by the creature
        struct owns_magic_type
            : public misc::EnumBaseCounting<owns_magic_type, misc::EnumFirstValueValid>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                Rarely = 0, // rarely carries magic items
                Religious, // often carries religous items and only occasionally magic items
                Magical, // often carries religous and magic items
                Count
            };

            static const std::string ToString(const owns_magic_type::Enum);
            static owns_magic_type::Enum FromCreature(const creature::CreaturePtr_t CHARACTER_PTR);
        };

        // define the complexity of items owned by the creature
        struct complexity_type
            : public misc::EnumBaseCounting<complexity_type, misc::EnumFirstValueValid>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                //...nothing
                Animal = 0,

                // shortbow/longbow/blowpipe, rings,
                // leather/stone/bone/obsidian/lapis/horn/hide/tooth materials only
                Simple,

                // composite bow, medallion, scale material, iron/steel/tin metals only
                Moderate,

                // crossbow, lantern, necklas, work with all metals
                Complex,

                Count
            };

            static const std::string ToString(const complexity_type::Enum);
            static complexity_type::Enum FromCreature(const creature::CreaturePtr_t CHARACTER_PTR);
        };

        // wrapper for info that describes what a creature will own
        struct Profile
        {
            Profile(
                const wealth_type::Enum WEALTH_TYPE,
                const collector_type::Enum COLLECTOR_TYPE,
                const owns_magic_type::Enum MAGIC_OWN_TYPE,
                const complexity_type::Enum COMPLEXITY_TYPE);

            static const Profile Make_FromCreature(const creature::CreaturePtr_t CHARACTER_PTR);

            wealth_type::Enum wealthType;
            collector_type::Enum collectorType;
            owns_magic_type::Enum magicType;
            complexity_type::Enum complexityType;
        };

        float ConvertStringToFloat(const std::string & KEY, const std::string & STR_FLOAT);

    } // namespace ownership
} // namespace non_player
} // namespace heroespath

#endif // HEROESPATH_NONPLAYER_OWNERSHIPPROFILE_HPP_INCLUDED
