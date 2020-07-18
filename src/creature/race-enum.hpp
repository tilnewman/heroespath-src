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
#include "creature/rank-class.hpp"
#include "creature/role-enum.hpp"
#include "item/treasure-score-set.hpp"
#include "misc/enum-common.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace creature
{

    struct origin_type : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Statue = 0,
            Egg,
            Embryo,
            Seeds,
            Count
        };

        static constexpr std::size_t UseCount(const origin_type::Enum ORIGIN_TYPE) noexcept
        {
            switch (ORIGIN_TYPE)
            {
                case Statue: return 20;
                case Seeds: return 10;

                case Egg:
                case Embryo: return 1;

                case Count:
                default: return 0;
            }
        }
    };

    using OriginTypeVec_t = std::vector<origin_type::Enum>;

    struct race : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Human = 0,
            Gnome,
            Pixie,
            Wolfen,
            Dragon,
            Goblin,
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

        static constexpr Enum Count_PlayerRaces = Goblin;

        static constexpr std::string_view Name(const Enum ENUM) noexcept
        {
            if (ENUM == CaveCrawler)
            {
                return "Cave Crawler";
            }
            else if (ENUM == LizardWalker)
            {
                return "Lizard Walker";
            }
            else if (ENUM == ThreeHeadedHound)
            {
                return "Three Headed Hound";
            }
            else
            {
                return NAMEOF_ENUM(ENUM);
            }
        }

        static const std::string Desc(const Enum);

        static constexpr std::string_view Abbr(const Enum ENUM) noexcept
        {
            if (ENUM == Wolfen)
            {
                return "Wlf";
            }
            else if (ENUM == Dragon)
            {
                return "Drg";
            }
            else if (ENUM == Werebat)
            {
                return "Wba";
            }
            else if (ENUM == Werebear)
            {
                return "Wbe";
            }
            else if (ENUM == Wereboar)
            {
                return "Wbo";
            }
            else if (ENUM == Werecat)
            {
                return "Wca";
            }
            else if (ENUM == Werewolf)
            {
                return "Wwf";
            }
            else
            {
                return NAMEOF_ENUM(ENUM).substr(0, 3);
            }
        }

        static constexpr bool HasTorso(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case Human:
                case Gnome:
                case Pixie:
                case Goblin:
                case Troll:
                case Orc:
                case Newt:
                case Bog:
                case LizardWalker:
                case Minotaur:
                case Ogre:
                case Shade:
                case Skeleton:
                case Demon:
                case Halfling:
                case Naga:
                case Pug:
                case Giant:
                case Witch:
                case Golem:
                case Harpy:
                case Ghoul: return true;

                case Wolfen:
                case Dragon:
                case Spider:
                case CaveCrawler:
                case Hydra:
                case Plant:
                case Griffin:
                case Lion:
                case LionBoar:
                case Ramonaut:
                case Serpent:
                case Cobra:
                case Werebear:
                case Wereboar:
                case Werecat:
                case Werewolf:
                case Werebat:
                case Wyvern:
                case Beetle:
                case Bat:
                case ThreeHeadedHound:
                case Boar: return false;

                case Count:
                default: return false;
            }
        }

        static constexpr bool CanFly(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case Human:
                case Gnome:
                case Pixie:
                case Goblin:
                case Troll:
                case Orc:
                case Newt:
                case Bog:
                case LizardWalker:
                case Minotaur:
                case Ogre:
                case Halfling:
                case Naga:
                case Pug:
                case Giant:
                case Ghoul:
                case Wolfen:
                case Spider:
                case CaveCrawler:
                case Hydra:
                case Plant:
                case Lion:
                case LionBoar:
                case Ramonaut:
                case Serpent:
                case Cobra:
                case Werebear:
                case Wereboar:
                case Werecat:
                case Werewolf:
                case Beetle:
                case Boar:
                case Witch:
                case Golem:
                case ThreeHeadedHound:
                case Skeleton: return false;

                case Demon:
                case Shade:
                case Griffin:
                case Werebat:
                case Wyvern:
                case Bat:
                case Harpy:
                case Dragon: return true;

                case Count:
                default: return false;
            }
        }

        static constexpr bool WillInitiallyFly(const Enum ENUM) noexcept
        {
            return (
                (ENUM == Pixie) || (ENUM == Shade) || (ENUM == Griffin) || (ENUM == Werebat)
                || (ENUM == Bat) || (ENUM == Harpy));
        }

        static const RoleVec_t Roles(const Enum);

        static constexpr bool
            RaceRoleMatch(const race::Enum RACE_ENUM, const role::Enum ROLE_ENUM) noexcept
        {
            return (NAMEOF_ENUM(RACE_ENUM).compare(NAMEOF_ENUM(ROLE_ENUM)) == 0);
        }

        static const OriginTypeVec_t OriginTypes(const Enum, const role::Enum);

        static constexpr item::TreasureScoreSet
            TreasureScoreSet(const race::Enum RACE_ENUM, const role::Enum ROLE_ENUM) noexcept
        {
            return (TreasureScoreSetByRace(RACE_ENUM) + TreasureScoreSetByRole(ROLE_ENUM));
        }

        // see Score() function above for where these numbers come from
        static constexpr item::TreasureScoreSet TreasureScoreSetMax {
            400_score, 400_score, 400_score, 400_score
        };

        static const std::string RaceRoleName(const Enum, const role::Enum);

        static const std::vector<std::string> CorpseImageKeys(const Enum);

        static constexpr RankRange_t
            RaceRoleRanks(const Enum RACE_ENUM, const role::Enum ROLE_ENUM) noexcept
        {
            const auto [RACE_MIN_ADJ, RACE_MAX_ADJ] = RankModifierBasedOnRace(RACE_ENUM);
            const auto [ROLE_MIN_ADJ, ROLE_MAX_ADJ] = RankModifierBasedOnRole(ROLE_ENUM);

            int min(1 + RACE_MIN_ADJ + ROLE_MIN_ADJ);
            int max(1 + RACE_MAX_ADJ + ROLE_MAX_ADJ);

            min = std::clamp(min, 1, 199);
            max = std::clamp(max, min, 200);

            return RankRange_t(Rank_t::Make(min), Rank_t::Make(max));
        }

    private:
        static constexpr std::tuple<int, int>
            RankModifierBasedOnRace(const race::Enum RACE_ENUM) noexcept
        {
            switch (RACE_ENUM)
            {
                case race::Human: return std::make_tuple(0, 99);
                case race::Orc: return std::make_tuple(4, 19);
                case race::Minotaur: return std::make_tuple(9, 99);
                case race::Ogre: return std::make_tuple(5, 99);
                case race::Halfling: return std::make_tuple(0, 19);
                case race::Pug: return std::make_tuple(0, 9);
                case race::Giant: return std::make_tuple(11, 99);
                case race::Wolfen: return std::make_tuple(14, 99);
                case race::Lion: return std::make_tuple(0, 9);
                case race::LionBoar: return std::make_tuple(4, 19);
                case race::Ramonaut: return std::make_tuple(7, 19);
                case race::Boar: return std::make_tuple(0, 7);
                case race::Demon: return std::make_tuple(19, 99);
                case race::Gnome: return std::make_tuple(0, 19);
                case race::Newt: return std::make_tuple(0, 19);
                case race::Bog: return std::make_tuple(0, 19);
                case race::Plant: return std::make_tuple(2, 19);
                case race::Pixie: return std::make_tuple(1, 49);
                case race::Goblin: return std::make_tuple(0, 19);
                case race::Troll: return std::make_tuple(4, 49);
                case race::Hydra: return std::make_tuple(19, 99);
                case race::LizardWalker: return std::make_tuple(2, 49);
                case race::Naga: return std::make_tuple(9, 59);
                case race::Harpy: return std::make_tuple(0, 49);
                case race::Dragon: return std::make_tuple(49, 99);
                case race::Griffin: return std::make_tuple(9, 39);
                case race::Serpent: return std::make_tuple(9, 49);
                case race::Cobra: return std::make_tuple(0, 9);
                case race::Wyvern: return std::make_tuple(9, 49);
                case race::Shade: return std::make_tuple(11, 99);
                case race::Skeleton: return std::make_tuple(0, 29);
                case race::Witch: return std::make_tuple(9, 49);
                case race::Golem: return std::make_tuple(29, 99);
                case race::Ghoul: return std::make_tuple(0, 19);
                case race::Spider: return std::make_tuple(0, 9);
                case race::CaveCrawler: return std::make_tuple(4, 24);
                case race::Werebear: return std::make_tuple(9, 49);
                case race::Wereboar: return std::make_tuple(7, 19);
                case race::Werecat: return std::make_tuple(0, 9);
                case race::Werewolf: return std::make_tuple(9, 49);
                case race::Werebat: return std::make_tuple(3, 19);
                case race::Beetle: return std::make_tuple(0, 12);
                case race::Bat: return std::make_tuple(0, 9);
                case race::ThreeHeadedHound: return std::make_tuple(14, 39);
                case race::Count:
                default: return std::make_tuple(0, 0);
            }
        }

        static constexpr std::tuple<int, int>
            RankModifierBasedOnRole(const role::Enum ROLE_ENUM) noexcept
        {
            switch (ROLE_ENUM)
            {
                case role::Archer: return std::make_tuple(1, 0);
                case role::Bard: return std::make_tuple(0, 0);
                case role::Beastmaster: return std::make_tuple(0, 0);
                case role::Cleric: return std::make_tuple(2, 20);
                case role::Knight: return std::make_tuple(2, 5);
                case role::Sorcerer: return std::make_tuple(2, 20);
                case role::Thief: return std::make_tuple(0, 0);
                case role::Firebrand: return std::make_tuple(0, 0);
                case role::Sylavin: return std::make_tuple(0, 0);
                case role::Wolfen: return std::make_tuple(0, 0);
                case role::Thug: return std::make_tuple(0, 3);
                case role::Mugger: return std::make_tuple(1, 6);
                case role::Drunk: return std::make_tuple(0, 0);
                case role::Grunt: return std::make_tuple(1, 10);
                case role::Brute: return std::make_tuple(3, 0);
                case role::Berserker: return std::make_tuple(4, 0);
                case role::Mountain: return std::make_tuple(4, 10);
                case role::Captain: return std::make_tuple(3, 10);
                case role::Chieftain: return std::make_tuple(6, 15);
                case role::Trader: return std::make_tuple(0, 0);
                case role::Warlord: return std::make_tuple(9, 20);
                case role::Shaman: return std::make_tuple(8, 10);
                case role::Smasher: return std::make_tuple(2, 2);
                case role::Strangler: return std::make_tuple(1, 0);
                case role::Soldier: return std::make_tuple(2, -5);
                case role::TwoHeaded: return std::make_tuple(5, 0);
                case role::Giant: return std::make_tuple(1, 0);
                case role::Elder: return std::make_tuple(11, 20);
                case role::FourArmed: return std::make_tuple(4, 0);
                case role::Tendrilus: return std::make_tuple(0, 0);
                case role::Wing: return std::make_tuple(-1, -5);
                case role::Whelp: return std::make_tuple(-1, -5);
                case role::Pod: return std::make_tuple(0, 0);
                case role::Spike: return std::make_tuple(4, 0);
                case role::Skeleton: return std::make_tuple(0, 0);
                case role::Ranger: return std::make_tuple(4, 0);
                case role::Water: return std::make_tuple(0, 0);
                case role::Blacksmith: return std::make_tuple(0, 0);
                case role::Spider: return std::make_tuple(0, 0);
                case role::Beetle: return std::make_tuple(0, 0);
                case role::Boar: return std::make_tuple(0, 0);
                case role::Lion: return std::make_tuple(0, 0);
                case role::Ramonaut: return std::make_tuple(0, 0);
                case role::Serpent: return std::make_tuple(0, 0);
                case role::Bat: return std::make_tuple(0, 0);
                case role::Ghost: return std::make_tuple(5, 0);
                case role::Cat: return std::make_tuple(0, 0);
                case role::Wolf: return std::make_tuple(1, 0);
                case role::Count:
                default: return std::make_tuple(0, 0);
            }
        }

        // these numbers intentionally max at 350, 300, 300, 300 -see comment below
        static constexpr item::TreasureScoreSet
            TreasureScoreSetByRace(const Enum RACE_ENUM) noexcept
        {
            // clang-format off
            switch (RACE_ENUM)
            {   
                case race::Human:           return item::TreasureScoreSet( 60_score,  50_score, 100_score,  50_score);
                case race::Orc:             return item::TreasureScoreSet( 30_score,  20_score,  60_score,  20_score);
                case race::Minotaur:        return item::TreasureScoreSet( 20_score,   5_score,  50_score,  10_score);
                case race::Ogre:            return item::TreasureScoreSet( 15_score,   5_score,  66_score,  25_score);
                case race::Halfling:        return item::TreasureScoreSet( 80_score,  75_score,  90_score,  50_score);
                case race::Pug:             return item::TreasureScoreSet( 45_score,  10_score,  20_score,  30_score);
                case race::Giant:           return item::TreasureScoreSet( 30_score,  10_score,  65_score,  30_score);
                case race::Wolfen:          return item::TreasureScoreSet(  0_score,   5_score,  10_score,  10_score);
                case race::Lion:            return item::TreasureScoreSet(  0_score,   0_score,   0_score,   0_score);
                case race::LionBoar:        return item::TreasureScoreSet(  0_score,   0_score,   0_score,   0_score);
                case race::Ramonaut:        return item::TreasureScoreSet(  0_score,   0_score,   0_score,   0_score);
                case race::Boar:            return item::TreasureScoreSet(  0_score,   0_score,   0_score,   0_score);
                case race::Demon:           return item::TreasureScoreSet(225_score, 125_score, 200_score, 200_score);
                case race::Gnome:           return item::TreasureScoreSet(175_score,  75_score, 120_score,  40_score);
                case race::Newt:            return item::TreasureScoreSet( 30_score,  10_score,  25_score,  20_score);
                case race::Bog:             return item::TreasureScoreSet( 10_score,   0_score,  20_score,   0_score);
                case race::Plant:           return item::TreasureScoreSet(  0_score,   0_score,   5_score,   0_score);
                case race::Pixie:           return item::TreasureScoreSet(125_score, 100_score, 125_score,  50_score);
                case race::Goblin:          return item::TreasureScoreSet( 75_score,  12_score,  50_score,  25_score);
                case race::Troll:           return item::TreasureScoreSet( 40_score,  25_score,  75_score,  30_score);
                case race::Hydra:           return item::TreasureScoreSet(200_score, 110_score, 180_score, 100_score);
                case race::LizardWalker:    return item::TreasureScoreSet(125_score,  20_score, 100_score, 40_score);
                case race::Naga:            return item::TreasureScoreSet(166_score,  35_score, 133_score,  60_score);
                case race::Harpy:           return item::TreasureScoreSet(125_score,  18_score, 100_score,  80_score);
                case race::Dragon:          return item::TreasureScoreSet(350_score, 300_score, 300_score, 150_score);
                case race::Griffin:         return item::TreasureScoreSet( 95_score,  20_score,  75_score,  20_score);
                case race::Serpent:         return item::TreasureScoreSet(  0_score,   0_score,   0_score,  30_score);
                case race::Cobra:           return item::TreasureScoreSet(  0_score,   0_score,   0_score,   0_score);
                case race::Wyvern:          return item::TreasureScoreSet(100_score,  35_score,  80_score,  50_score);
                case race::Shade:           return item::TreasureScoreSet( 85_score, 150_score, 150_score, 100_score);
                case race::Skeleton:        return item::TreasureScoreSet(120_score,  35_score, 100_score,  50_score);
                case race::Witch:           return item::TreasureScoreSet(105_score, 175_score, 180_score, 300_score);
                case race::Golem:           return item::TreasureScoreSet( 55_score, 100_score, 150_score,  50_score);
                case race::Ghoul:           return item::TreasureScoreSet( 30_score,  35_score,  60_score,  60_score);
                case race::Spider:          return item::TreasureScoreSet(  0_score,   0_score,   0_score,   0_score);
                case race::CaveCrawler:     return item::TreasureScoreSet( 10_score,   0_score,   0_score,   0_score);
                case race::Werebear:        return item::TreasureScoreSet(  0_score,   0_score,  50_score,   0_score);
                case race::Wereboar:        return item::TreasureScoreSet(  0_score,   0_score,   0_score,   0_score);
                case race::Werecat:         return item::TreasureScoreSet(  0_score,   0_score,   0_score,   0_score);
                case race::Werewolf:        return item::TreasureScoreSet(  0_score,  10_score, 100_score,  50_score);
                case race::Werebat:         return item::TreasureScoreSet(  0_score,   0_score,   0_score,   0_score);
                case race::Beetle:          return item::TreasureScoreSet(  0_score,   0_score,   0_score,   0_score);
                case race::Bat:             return item::TreasureScoreSet(  0_score,   0_score,   0_score,   0_score);
                case race::ThreeHeadedHound:return item::TreasureScoreSet(  0_score,   0_score,   0_score,   0_score);
                case race::Count:
                default:                    return item::TreasureScoreSet(  0_score, 0_score, 0_score, 0_score);
            }
            // clang-format on
        }

        // These numbers intentionally max at 50, 100, 100, 100 -see comment above
        // These numbers (and those above) intentionally combine to a max of 400, 400, 400, 400.
        // See TreasureScoreSetMax() above.
        static constexpr item::TreasureScoreSet
            TreasureScoreSetByRole(const role::Enum ROLE_ENUM) noexcept
        {
            // clang-format off
            switch (ROLE_ENUM)
            {   
                case role::Archer:      return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Bard:        return item::TreasureScoreSet( 20_score, 20_score, 30_score, 10_score);
                case role::Beastmaster: return item::TreasureScoreSet(  0_score, 10_score, 20_score, 20_score);
                case role::Cleric:      return item::TreasureScoreSet(  0_score, 60_score, 50_score, 50_score);
                case role::Knight:      return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Sorcerer:    return item::TreasureScoreSet(  0_score, 70_score, 50_score, 20_score);
                case role::Thief:       return item::TreasureScoreSet(  0_score,  5_score, 20_score,  0_score);
                case role::Firebrand:   return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Sylavin:     return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Wolfen:      return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Thug:        return item::TreasureScoreSet( 15_score,  2_score, 15_score,  0_score);
                case role::Mugger:      return item::TreasureScoreSet( 25_score,  3_score, 25_score,  0_score);
                case role::Drunk:       return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Grunt:       return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Brute:       return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Berserker:   return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Mountain:    return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Captain:     return item::TreasureScoreSet( 50_score, 10_score, 20_score,  5_score);
                case role::Chieftain:   return item::TreasureScoreSet( 50_score, 20_score, 50_score, 20_score);
                case role::Trader:      return item::TreasureScoreSet( 30_score, 10_score, 20_score, 15_score);
                case role::Warlord:     return item::TreasureScoreSet( 50_score, 30_score, 30_score, 10_score);
                case role::Shaman:      return item::TreasureScoreSet( 10_score,100_score, 50_score,100_score);
                case role::Smasher:     return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Strangler:   return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Soldier:     return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::TwoHeaded:   return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Giant:       return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Elder:       return item::TreasureScoreSet( 10_score, 75_score,100_score, 50_score);
                case role::FourArmed:   return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Tendrilus:   return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Wing:        return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Whelp:       return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Pod:         return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Spike:       return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Skeleton:    return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Ranger:      return item::TreasureScoreSet(  0_score, 10_score,  0_score,  0_score);
                case role::Water:       return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Blacksmith:  return item::TreasureScoreSet(  0_score,  0_score, 10_score,  0_score);
                case role::Spider:      return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Beetle:      return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Boar:        return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Lion:        return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Ramonaut:    return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Serpent:     return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Bat:         return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Ghost:       return item::TreasureScoreSet(  0_score, 50_score,  0_score,  0_score);
                case role::Cat:         return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Wolf:        return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
                case role::Count:       
                default:                return item::TreasureScoreSet(  0_score,  0_score,  0_score,  0_score);
            }
            // clang-format on
        }
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_RACEENUM_HPP_INCLUDED
