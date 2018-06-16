// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_RACEENUM_HPP_INCLUDED
#define HEROESPATH_CREATURE_RACEENUM_HPP_INCLUDED
//
// race-enum.hpp
//
#include "misc/boost-serialize-includes.hpp"

#include "creature/rank.hpp"
#include "creature/role-enum.hpp"
#include "creature/trait.hpp"
#include "item/treasure-scores.hpp"
#include "misc/enum-util.hpp"
#include "misc/handy-types.hpp"
#include "misc/types.hpp"

#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace creature
{

    struct origin_type : public misc::EnumBaseCounting<origin_type, misc::EnumFirstValueValid>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            Statue = 0,
            Egg,
            Embryo,
            Seeds,
            Count
        };

        static const std::string ToString(const Enum);
        static std::size_t UseCount(const Enum);
    };

    using OriginTypeVec_t = std::vector<origin_type::Enum>;

    struct race : public misc::EnumBaseCounting<race, misc::EnumFirstValueValid>
    {
        enum Enum : misc::EnumUnderlying_t
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
            Bat,
            Witch,
            Golem,
            Harpy,
            ThreeHeadedHound,
            Count
        };

        static const std::string ToString(const race::Enum);
        static const std::string Name(const race::Enum);
        static const std::string Desc(const race::Enum);
        static const std::string Abbr(const race::Enum);
        static bool HasTorso(const race::Enum);
        static bool CanFly(const race::Enum);
        static bool WillInitiallyFly(const race::Enum);
        static const RoleVec_t Roles(const race::Enum);
        static bool RaceRoleMatch(const race::Enum, const role::Enum);
        static const OriginTypeVec_t OriginTypes(const race::Enum, const role::Enum);
        static const item::TreasureScores TreasureScore(const race::Enum, const role::Enum);
        static const item::TreasureScores TreasureScoreMax();
        static const std::string RaceRoleName(const race::Enum, const role::Enum);
        static const RankRange_t RaceRoleRanks(const race::Enum, const role::Enum);
        static const misc::StrVec_t CorpseImageKeys(const race::Enum);
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_RACEENUM_HPP_INCLUDED
