// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// race-enum.hpp
//
#include "race-enum.hpp"

#include "misc/config-file.hpp"

#include <algorithm>

namespace heroespath
{
namespace creature
{

    const std::string origin_type::ToString(const origin_type::Enum ORIGIN_TYPE)
    {
        switch (ORIGIN_TYPE)
        {
            case Statue:
            {
                return "Statue";
            }
            case Egg:
            {
                return "Egg";
            }
            case Embryo:
            {
                return "Embryo";
            }
            case Seeds:
            {
                return "Seeds";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(ORIGIN_TYPE)
                                  << " is invalid. (count=" << static_cast<EnumUnderlying_t>(Count)
                                  << ")");

                return "";
            }
        }
    }

    std::size_t origin_type::UseCount(const origin_type::Enum ORIGIN_TYPE)
    {
        switch (ORIGIN_TYPE)
        {
            case Statue:
            {
                return 20;
            }
            case Seeds:
            {
                return 10;
            }
            case Egg:
            case Embryo:
            {
                return 1;
            }
            case Count:
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(ORIGIN_TYPE)
                                  << " is invalid. (count=" << static_cast<EnumUnderlying_t>(Count)
                                  << ")");

                return 0;
            }
        }
    }

    const std::string race::ToString(const race::Enum RACE)
    {
        switch (RACE)
        {
            case Human:
            {
                return "Human";
            }
            case Gnome:
            {
                return "Gnome";
            }
            case Pixie:
            {
                return "Pixie";
            }
            case Wolfen:
            {
                return "Wolfen";
            }
            case Dragon:
            {
                return "Dragon";
            }
            case Goblin:
            {
                return "Goblin";
            }
            case Troll:
            {
                return "Troll";
            }
            case Orc:
            {
                return "Orc";
            }
            case Newt:
            {
                return "Newt";
            }
            case Spider:
            {
                return "Spider";
            }
            case Bog:
            {
                return "Bog";
            }
            case CaveCrawler:
            {
                return "CaveCrawler";
            }
            case Hydra:
            {
                return "Hydra";
            }
            case LizardWalker:
            {
                return "LizardWalker";
            }
            case Minotaur:
            {
                return "Minotaur";
            }
            case Ogre:
            {
                return "Ogre";
            }
            case Plant:
            {
                return "Plant";
            }
            case Shade:
            {
                return "Shade";
            }
            case Skeleton:
            {
                return "Skeleton";
            }
            case Demon:
            {
                return "Demon";
            }
            case Griffin:
            {
                return "Griffin";
            }
            case Halfling:
            {
                return "Halfling";
            }
            case Lion:
            {
                return "Lion";
            }
            case LionBoar:
            {
                return "LionBoar";
            }
            case Naga:
            {
                return "Naga";
            }
            case Ramonaut:
            {
                return "Ramonaut";
            }
            case Serpent:
            {
                return "Serpent";
            }
            case Cobra:
            {
                return "Cobra";
            }
            case Werebear:
            {
                return "Werebear";
            }
            case Wereboar:
            {
                return "Wereboar";
            }
            case Werecat:
            {
                return "Werecat";
            }
            case Werewolf:
            {
                return "Werewolf";
            }
            case Werebat:
            {
                return "Werebat";
            }
            case Pug:
            {
                return "Pug";
            }
            case Wyvern:
            {
                return "Wyvern";
            }
            case Giant:
            {
                return "Giant";
            }
            case Ghoul:
            {
                return "Ghoul";
            }
            case Beetle:
            {
                return "Beetle";
            }
            case Boar:
            {
                return "Boar";
            }
            case Bat:
            {
                return "Bat";
            }
            case Witch:
            {
                return "Witch";
            }
            case Golem:
            {
                return "Golem";
            }
            case Harpy:
            {
                return "Harpy";
            }
            case ThreeHeadedHound:
            {
                return "ThreeHeadedHound";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(RACE) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

    const std::string race::Name(const race::Enum RACE)
    {
        if (RACE == Spider)
        {
            return "Giant Spider";
        }
        else
        {
            return ToString(RACE);
        }
    }

    const std::string race::Desc(const race::Enum ENUM)
    {
        std::ostringstream ss;
        ss << "creature-race-desc-" << ToString(ENUM);
        return misc::ConfigFile::Instance()->Value(ss.str());
    }

    const std::string race::Abbr(const race::Enum ENUM)
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
            return Name(ENUM).substr(0, 3);
    }

    bool race::HasTorso(const race::Enum ENUM)
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
            case Ghoul:
            {
                return true;
            }
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
            case Boar:
            {
                return false;
            }
            case Count:
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(ENUM) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return false;
            }
        }
    }

    bool race::CanFly(const race::Enum ENUM)
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
            case Skeleton:
            {
                return false;
            }
            case Demon:
            case Shade:
            case Griffin:
            case Werebat:
            case Wyvern:
            case Bat:
            case Harpy:
            case Dragon:
            {
                return true;
            }
            case Count:
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(ENUM) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return false;
            }
        }
    }

    bool race::WillInitiallyFly(const race::Enum ENUM)
    {
        return (
            (ENUM == race::Pixie) || (ENUM == race::Shade) || (ENUM == race::Griffin)
            || (ENUM == race::Werebat) || (ENUM == race::Bat) || (ENUM == race::Harpy));
    }

    // At last count on 2017-8-3 there were 167 unique race/roles, and 24 unique player race/roles.
    const RoleVec_t race::Roles(const race::Enum ENUM)
    {
        switch (ENUM)
        {
            case Human:
            {
                return { role::Knight,  role::Archer,     role::Bard,     role::Beastmaster,
                         role::Cleric,  role::Thief,      role::Sorcerer, role::Ranger,
                         role::Trader,  role::Drunk,      role::Brute,    role::Smasher,
                         role::Soldier, role::Blacksmith, role::Shaman,   role::Grunt };
            }
            case Gnome:
            {
                return { role::Bard,        role::Cleric, role::Sorcerer, role::Knight,
                         role::Beastmaster, role::Archer, role::Thief,    role::Grunt };
            }
            case Pixie:
            {
                return { role::Beastmaster, role::Sorcerer, role::Cleric, role::Bard };
            }
            case Goblin:
            {
                return { role::Thief,     role::Sorcerer, role::Brute,  role::Archer, role::Captain,
                         role::Chieftain, role::Shaman,   role::Trader, role::Grunt };
            }
            case Troll:
            {
                return { role::Berserker, role::Brute,    role::Chieftain, role::Grunt,
                         role::Mountain,  role::Shaman,   role::Smasher,   role::Strangler,
                         role::Warlord,   role::TwoHeaded };
            }
            case Orc:
            {
                return { role::Berserker, role::Chieftain, role::Elder,  role::FourArmed,
                         role::Captain,   role::Mountain,  role::Spike,  role::Grunt,
                         role::Shaman,    role::Smasher,   role::Warlord };
            }
            case Newt:
            {
                return { role::Chieftain, role::Cleric, role::Elder,
                         role::Sorcerer,  role::Trader, role::Grunt };
            }
            case Bog:
            {
                return { role::Chieftain, role::Smasher, role::Sorcerer, role::Shaman, role::Spike,
                         role::Tendrilus, role::Wing,    role::Whelp,    role::Grunt };
            }
            case LizardWalker:
            {
                return { role::Whelp, role::Sorcerer, role::Captain, role::Chieftain,
                         role::Spike, role::Shaman,   role::Grunt };
            }
            case Minotaur:
            {
                return { role::Grunt,    role::Brute,  role::FourArmed,
                         role::Mountain, role::Shaman, role::Warlord };
            }
            case Ogre:
            {
                return { role::Berserker, role::Brute,    role::Elder,   role::FourArmed,
                         role::Giant,     role::Mountain, role::Warlord, role::Captain,
                         role::Chieftain, role::Shaman,   role::Spike,   role::Grunt };
            }
            case Halfling:
            {
                return { role::Shaman, role::Chieftain, role::Captain, role::Grunt };
            }
            case Naga:
            {
                return { role::Grunt };
            }
            case Pug:
            {
                return { role::Archer, role::Cleric, role::Grunt };
            }
            case Giant:
            {
                return { role::Smasher,  role::Strangler, role::Warlord,
                         role::Mountain, role::Grunt,     role::Brute };
            }
            case Ghoul:
            {
                return { role::Strangler, role::Spike, role::Mountain, role::Grunt };
            }
            case Wolfen:
            {
                return { role::Wolfen, role::TwoHeaded };
            }
            case Spider:
            {
                return { role::Spider };
            }
            case CaveCrawler:
            {
                return { role::Mountain };
            }
            case Hydra:
            {
                return { role::Wing };
            }
            case Plant:
            {
                return { role::Smasher, role::Tendrilus, role::Pod, role::Strangler };
            }
            case Lion:
            case LionBoar:
            {
                return { role::Lion };
            }
            case Ramonaut:
            {
                return { role::Ramonaut };
            }
            case Serpent:
            {
                return { role::Serpent, role::Water };
            }
            case Cobra:
            {
                return { role::Serpent };
            }
            case Werebear:
            {
                return { role::Elder, role::Grunt, role::Mountain };
            }
            case Wereboar:
            {
                return { role::Boar };
            }
            case Werecat:
            {
                return { role::Cat };
            }
            case Werewolf:
            {
                return { role::Wolf };
            }
            case Beetle:
            {
                return { role::Beetle };
            }
            case Boar:
            {
                return { role::Boar };
            }
            case Witch:
            {
                return {
                    role::Elder, role::Ghost, role::Strangler, role::Mountain, role::Sorcerer
                };
            }
            case Golem:
            {
                return { role::Brute };
            }
            case ThreeHeadedHound:
            {
                return { role::Mountain };
            }
            case Skeleton:
            {
                return { role::Chieftain, role::FourArmed, role::Mountain, role::Grunt };
            }
            case Demon:
            {
                return { role::Skeleton, role::Spike, role::Strangler,
                         role::Whelp,    role::Wing,  role::Grunt };
            }
            case Shade:
            {
                return { role::Ghost };
            }
            case Griffin:
            {
                return { role::Whelp, role::Wing };
            }
            case Werebat:
            {
                return { role::Bat };
            }
            case Wyvern:
            {
                return { role::Whelp, role::Wing };
            }
            case Bat:
            {
                return { role::Bat };
            }
            case Harpy:
            {
                return { role::Wing };
            }
            case Dragon:
            {
                return { role::Firebrand, role::Sylavin };
            }
            case Count:
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(ENUM) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return {};
            }
        }
    }

    bool race::RaceRoleMatch(const race::Enum RACE_ENUM, const role::Enum ROLE_ENUM)
    {
        return (race::ToString(RACE_ENUM) == role::ToString(ROLE_ENUM));
    }

    const OriginTypeVec_t race::OriginTypes(const race::Enum RACE, const role::Enum ROLE)
    {
        OriginTypeVec_t types;

        if ((RACE == race::Count) && (ROLE == role::Count))
        {
            return types;
        }

        types.reserve(6);
        if (RACE != race::Spider)
        {
            types.emplace_back(origin_type::Statue);
        }

        switch (RACE)
        {
            case race::Human:
            case race::Orc:
            case race::Minotaur:
            case race::Ogre:
            case race::Halfling:
            case race::Pug:
            case race::Giant:
            case race::Wolfen:
            case race::Lion:
            case race::LionBoar:
            case race::Ramonaut:
            case race::Boar:
            {
                types.emplace_back(origin_type::Embryo);
                break;
            }
            case race::Demon:
            case race::Gnome:
            case race::Newt:
            case race::Bog:
            case race::Plant:
            {
                types.emplace_back(origin_type::Seeds);
                break;
            }
            case race::Pixie:
            case race::Goblin:
            case race::Troll:
            case race::Hydra:
            {
                types.emplace_back(origin_type::Egg);
                types.emplace_back(origin_type::Seeds);
                break;
            }
            case race::LizardWalker:
            case race::Naga:
            case race::Harpy:
            case race::Dragon:
            case race::Griffin:
            case race::Serpent:
            case race::Cobra:
            case race::Wyvern:
            case race::Spider:
            {
                types.emplace_back(origin_type::Egg);
                break;
            }
            case race::Shade:
            case race::Skeleton:
            case race::Witch:
            case race::Golem:
            case race::Ghoul:
            case race::CaveCrawler:
            case race::Werebear:
            case race::Wereboar:
            case race::Werecat:
            case race::Werewolf:
            case race::Werebat:
            case race::Beetle:
            case race::Bat:
            case race::ThreeHeadedHound:
            case race::Count:
            default:
            {
                break;
            }
        }

        if (ROLE == role::Tendrilus)
        {
            types.emplace_back(origin_type::Seeds);
        }
        else if ((ROLE == role::Wing) || (ROLE == role::Whelp) || (ROLE == role::Tendrilus))
        {
            types.emplace_back(origin_type::Egg);
        }

        std::sort(std::begin(types), std::end(types));
        types.erase(std::unique(std::begin(types), std::end(types)), std::end(types));
        return types;
    }

    const item::TreasureScores
        race::TreasureScore(const race::Enum RACE_ENUM, const role::Enum ROLE_ENUM)
    {
        Score_t coin { 0_score };
        Score_t gem { 0_score };
        Score_t magic { 0_score };
        Score_t rel { 0_score };

        // these numbers intentionally max at 350, 300, 300, 300 -see comment below
        switch (RACE_ENUM)
        {
            case race::Human:
            {
                coin += 60_score;
                gem += 50_score;
                magic += 100_score;
                rel += 50_score;
                break;
            }
            case race::Orc:
            {
                coin += 30_score;
                gem += 20_score;
                magic += 60_score;
                rel += 20_score;
                break;
            }
            case race::Minotaur:
            {
                coin += 20_score;
                gem += 5_score;
                magic += 50_score;
                rel += 10_score;
                break;
            }
            case race::Ogre:
            {
                coin += 15_score;
                gem += 5_score;
                magic += 66_score;
                rel += 25_score;
                break;
            }
            case race::Halfling:
            {
                coin += 80_score;
                gem += 75_score;
                magic += 90_score;
                rel += 50_score;
                break;
            }
            case race::Pug:
            {
                coin += 45_score;
                gem += 10_score;
                magic += 20_score;
                rel += 30_score;
                break;
            }
            case race::Giant:
            {
                coin += 30_score;
                gem += 10_score;
                magic += 65_score;
                rel += 30_score;
                break;
            }
            case race::Wolfen:
            {
                coin += 0_score;
                gem += 5_score;
                magic += 10_score;
                rel += 10_score;
                break;
            }
            case race::Lion:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case race::LionBoar:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case race::Ramonaut:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case race::Boar:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case race::Demon:
            {
                coin += 225_score;
                gem += 125_score;
                magic += 200_score;
                rel += 200_score;
                break;
            }
            case race::Gnome:
            {
                coin += 175_score;
                gem += 75_score;
                magic += 120_score;
                rel += 40_score;
                break;
            }
            case race::Newt:
            {
                coin += 30_score;
                gem += 10_score;
                magic += 25_score;
                rel += 20_score;
                break;
            }
            case race::Bog:
            {
                coin += 10_score;
                gem += 0_score;
                magic += 20_score;
                rel += 0_score;
                break;
            }
            case race::Plant:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 5_score;
                rel += 0_score;
                break;
            }
            case race::Pixie:
            {
                coin += 125_score;
                gem += 100_score;
                magic += 125_score;
                rel += 50_score;
                break;
            }
            case race::Goblin:
            {
                coin += 75_score;
                gem += 12_score;
                magic += 50_score;
                rel += 25_score;
                break;
            }
            case race::Troll:
            {
                coin += 40_score;
                gem += 25_score;
                magic += 75_score;
                rel += 30_score;
                break;
            }
            case race::Hydra:
            {
                coin += 200_score;
                gem += 110_score;
                magic += 180_score;
                rel += 100_score;
                break;
            }
            case race::LizardWalker:
            {
                coin += 125_score;
                gem += 20_score;
                magic += 100_score;
                rel += 40_score;
                break;
            }
            case race::Naga:
            {
                coin += 166_score;
                gem += 35_score;
                magic += 133_score;
                rel += 60_score;
                break;
            }
            case race::Harpy:
            {
                coin += 125_score;
                gem += 18_score;
                magic += 100_score;
                rel += 80_score;
                break;
            }
            case race::Dragon:
            {
                coin += 350_score;
                gem += 300_score;
                magic += 300_score;
                rel += 150_score;
                break;
            }
            case race::Griffin:
            {
                coin += 95_score;
                gem += 20_score;
                magic += 75_score;
                rel += 20_score;
                break;
            }
            case race::Serpent:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 30_score;
                break;
            }
            case race::Cobra:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case race::Wyvern:
            {
                coin += 100_score;
                gem += 35_score;
                magic += 80_score;
                rel += 50_score;
                break;
            }
            case race::Shade:
            {
                coin += 85_score;
                gem += 150_score;
                magic += 150_score;
                rel += 100_score;
                break;
            }
            case race::Skeleton:
            {
                coin += 120_score;
                gem += 35_score;
                magic += 100_score;
                rel += 50_score;
                break;
            }
            case race::Witch:
            {
                coin += 105_score;
                gem += 175_score;
                magic += 180_score;
                rel += 300_score;
                break;
            }
            case race::Golem:
            {
                coin += 55_score;
                gem += 100_score;
                magic += 150_score;
                rel += 50_score;
                break;
            }
            case race::Ghoul:
            {
                coin += 30_score;
                gem += 35_score;
                magic += 60_score;
                rel += 60_score;
                break;
            }
            case race::Spider:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case race::CaveCrawler:
            {
                coin += 10_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case race::Werebear:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 50_score;
                rel += 0_score;
                break;
            }
            case race::Wereboar:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case race::Werecat:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case race::Werewolf:
            {
                coin += 0_score;
                gem += 10_score;
                magic += 100_score;
                rel += 50_score;
                break;
            }
            case race::Werebat:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case race::Beetle:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case race::Bat:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case race::ThreeHeadedHound:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case race::Count:
            default:
            {
                break;
            }
        }

        // These numbers intentionally max at 50, 100, 100, 100 -see comment above
        // These numbers (and those above) intentionally combine to a max of 400, 400, 400, 400.
        // See TreasureScoreMax() below.
        switch (ROLE_ENUM)
        {
            case role::Archer:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Bard:
            {
                coin += 20_score;
                gem += 20_score;
                magic += 30_score;
                rel += 10_score;
                break;
            }
            case role::Beastmaster:
            {
                coin += 0_score;
                gem += 10_score;
                magic += 20_score;
                rel += 20_score;
                break;
            }
            case role::Cleric:
            {
                coin += 0_score;
                gem += 60_score;
                magic += 50_score;
                rel += 50_score;
                break;
            }
            case role::Knight:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Sorcerer:
            {
                coin += 0_score;
                gem += 70_score;
                magic += 50_score;
                rel += 20_score;
                break;
            }
            case role::Thief:
            {
                coin += 0_score;
                gem += 5_score;
                magic += 20_score;
                rel += 0_score;
                break;
            }
            case role::Firebrand:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Sylavin:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Wolfen:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Thug:
            {
                coin += 15_score;
                gem += 2_score;
                magic += 15_score;
                rel += 0_score;
                break;
            }
            case role::Mugger:
            {
                coin += 25_score;
                gem += 3_score;
                magic += 25_score;
                rel += 0_score;
                break;
            }
            case role::Drunk:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Grunt:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Brute:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Berserker:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Mountain:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Captain:
            {
                coin += 50_score;
                gem += 10_score;
                magic += 20_score;
                rel += 5_score;
                break;
            }
            case role::Chieftain:
            {
                coin += 50_score;
                gem += 20_score;
                magic += 50_score;
                rel += 20_score;
                break;
            }
            case role::Trader:
            {
                coin += 30_score;
                gem += 10_score;
                magic += 20_score;
                rel += 15_score;
                break;
            }
            case role::Warlord:
            {
                coin += 50_score;
                gem += 30_score;
                magic += 30_score;
                rel += 10_score;
                break;
            }
            case role::Shaman:
            {
                coin += 10_score;
                gem += 100_score;
                magic += 50_score;
                rel += 100_score;
                break;
            }
            case role::Smasher:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Strangler:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Soldier:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::TwoHeaded:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Giant:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Elder:
            {
                coin += 10_score;
                gem += 75_score;
                magic += 100_score;
                rel += 50_score;
                break;
            }
            case role::FourArmed:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Tendrilus:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Wing:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Whelp:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Pod:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Spike:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Skeleton:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Ranger:
            {
                coin += 0_score;
                gem += 10_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Water:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Blacksmith:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 10_score;
                rel += 0_score;
                break;
            }
            case role::Spider:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Beetle:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Boar:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Lion:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Ramonaut:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Serpent:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Bat:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Ghost:
            {
                coin += 0_score;
                gem += 50_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Cat:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Wolf:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            case role::Count:
            {
                coin += 0_score;
                gem += 0_score;
                magic += 0_score;
                rel += 0_score;
                break;
            }
            default:
            {
                break;
            }
        }

        return item::TreasureScores(coin, gem, magic, rel);
    }

    const item::TreasureScores race::TreasureScoreMax()
    {
        // see TreasureScore() function above for where these numbers come from
        return item::TreasureScores(400_score, 400_score, 400_score, 400_score);
    }

    const std::string race::RaceRoleName(const race::Enum RACE_ENUM, const role::Enum ROLE_ENUM)
    {
        if (RACE_ENUM == race::ThreeHeadedHound)
        {
            return race::Name(RACE_ENUM);
        }

        const auto IS_ROLE_FIRST { (
            (ROLE_ENUM == role::Firebrand) || (ROLE_ENUM == role::FourArmed)
            || (ROLE_ENUM == role::Giant) || (ROLE_ENUM == role::Mountain)
            || (ROLE_ENUM == role::Spike) || (ROLE_ENUM == role::Sylavin)
            || (ROLE_ENUM == role::Tendrilus) || (ROLE_ENUM == role::TwoHeaded)
            || (ROLE_ENUM == role::Water)) };

        std::ostringstream ss;

        if (IS_ROLE_FIRST)
        {
            ss << role::Name(ROLE_ENUM);
        }
        else
        {
            ss << race::Name(RACE_ENUM);
        }

        if (race::RaceRoleMatch(RACE_ENUM, ROLE_ENUM) == false)
        {
            switch (RACE_ENUM)
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
                case Pug:
                case Giant:
                case Ghoul:
                case Plant:
                case Wolfen:
                case Serpent:
                case Werebear:
                case Witch:
                case Skeleton:
                case Demon:
                case Griffin:
                case Wyvern:
                case Dragon:
                {
                    if (IS_ROLE_FIRST)
                    {
                        ss << " " << race::Name(RACE_ENUM);
                    }
                    else
                    {
                        ss << " " << role::Name(ROLE_ENUM);
                    }

                    break;
                }

                case ThreeHeadedHound:
                case Naga:
                case Spider:
                case CaveCrawler:
                case Hydra:
                case Lion:
                case LionBoar:
                case Ramonaut:
                case Cobra:
                case Wereboar:
                case Werecat:
                case Werewolf:
                case Beetle:
                case Boar:
                case Golem:
                case Shade:
                case Werebat:
                case Bat:
                case Harpy:
                case Count:
                default:
                {
                    break;
                }
            }
        }

        return ss.str();
    }

    const RankRange_t race::RaceRoleRanks(const race::Enum RACE_ENUM, const role::Enum ROLE_ENUM)
    {
        Rank_t min { 1_rank };
        Rank_t max { 1_rank };

        switch (RACE_ENUM)
        {
            case race::Human:
            {
                min += 0_rank;
                max += 99_rank;
                break;
            }
            case race::Orc:
            {
                min += 4_rank;
                max += 19_rank;
                break;
            }
            case race::Minotaur:
            {
                min += 9_rank;
                max += 99_rank;
                break;
            }
            case race::Ogre:
            {
                min += 5_rank;
                max += 99_rank;
                break;
            }
            case race::Halfling:
            {
                min += 0_rank;
                max += 19_rank;
                break;
            }
            case race::Pug:
            {
                min += 0_rank;
                max += 9_rank;
                break;
            }
            case race::Giant:
            {
                min += 11_rank;
                max += 99_rank;
                break;
            }
            case race::Wolfen:
            {
                min += 14_rank;
                max += 99_rank;
                break;
            }
            case race::Lion:
            {
                min += 0_rank;
                max += 9_rank;
                break;
            }
            case race::LionBoar:
            {
                min += 4_rank;
                max += 19_rank;
                break;
            }
            case race::Ramonaut:
            {
                min += 7_rank;
                max += 19_rank;
                break;
            }
            case race::Boar:
            {
                min += 0_rank;
                max += 7_rank;
                break;
            }
            case race::Demon:
            {
                min += 19_rank;
                max += 99_rank;
                break;
            }
            case race::Gnome:
            {
                min += 0_rank;
                max += 19_rank;
                break;
            }
            case race::Newt:
            {
                min += 0_rank;
                max += 19_rank;
                break;
            }
            case race::Bog:
            {
                min += 0_rank;
                max += 19_rank;
                break;
            }
            case race::Plant:
            {
                min += 2_rank;
                max += 19_rank;
                break;
            }
            case race::Pixie:
            {
                min += 1_rank;
                max += 49_rank;
                break;
            }
            case race::Goblin:
            {
                min += 0_rank;
                max += 19_rank;
                break;
            }
            case race::Troll:
            {
                min += 4_rank;
                max += 49_rank;
                break;
            }
            case race::Hydra:
            {
                min += 19_rank;
                max += 99_rank;
                break;
            }
            case race::LizardWalker:
            {
                min += 2_rank;
                max += 49_rank;
                break;
            }
            case race::Naga:
            {
                min += 9_rank;
                max += 59_rank;
                break;
            }
            case race::Harpy:
            {
                min += 0_rank;
                max += 49_rank;
                break;
            }
            case race::Dragon:
            {
                min += 49_rank;
                max += 99_rank;
                break;
            }
            case race::Griffin:
            {
                min += 9_rank;
                max += 39_rank;
                break;
            }
            case race::Serpent:
            {
                min += 9_rank;
                max += 49_rank;
                break;
            }
            case race::Cobra:
            {
                min += 0_rank;
                max += 9_rank;
                break;
            }
            case race::Wyvern:
            {
                min += 9_rank;
                max += 49_rank;
                break;
            }
            case race::Shade:
            {
                min += 11_rank;
                max += 99_rank;
                break;
            }
            case race::Skeleton:
            {
                min += 0_rank;
                max += 29_rank;
                break;
            }
            case race::Witch:
            {
                min += 9_rank;
                max += 49_rank;
                break;
            }
            case race::Golem:
            {
                min += 29_rank;
                max += 99_rank;
                break;
            }
            case race::Ghoul:
            {
                min += 0_rank;
                max += 19_rank;
                break;
            }
            case race::Spider:
            {
                min += 0_rank;
                max += 9_rank;
                break;
            }
            case race::CaveCrawler:
            {
                min += 4_rank;
                max += 24_rank;
                break;
            }
            case race::Werebear:
            {
                min += 9_rank;
                max += 49_rank;
                break;
            }
            case race::Wereboar:
            {
                min += 7_rank;
                max += 19_rank;
                break;
            }
            case race::Werecat:
            {
                min += 0_rank;
                max += 9_rank;
                break;
            }
            case race::Werewolf:
            {
                min += 9_rank;
                max += 49_rank;
                break;
            }
            case race::Werebat:
            {
                min += 3_rank;
                max += 19_rank;
                break;
            }
            case race::Beetle:
            {
                min += 0_rank;
                max += 12_rank;
                break;
            }
            case race::Bat:
            {
                min += 0_rank;
                max += 9_rank;
                break;
            }
            case race::ThreeHeadedHound:
            {
                min += 14_rank;
                max += 39_rank;
                break;
            }
            case race::Count:
            default:
            {
                break;
            }
        }

        switch (ROLE_ENUM)
        {
            case role::Archer:
            {
                min += 1_rank;
                max += 0_rank;
                break;
            }
            case role::Bard:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            case role::Beastmaster:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            case role::Cleric:
            {
                min += 2_rank;
                max += 20_rank;
                break;
            }
            case role::Knight:
            {
                min += 2_rank;
                max += 5_rank;
                break;
            }
            case role::Sorcerer:
            {
                min += 2_rank;
                max += 20_rank;
                break;
            }
            case role::Thief:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            case role::Firebrand:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            case role::Sylavin:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            case role::Wolfen:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            case role::Thug:
            {
                min += 0_rank;
                max += 3_rank;
                break;
            }
            case role::Mugger:
            {
                min += 1_rank;
                max += 6_rank;
                break;
            }
            case role::Drunk:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            case role::Grunt:
            {
                min += 1_rank;
                max += 10_rank;
                break;
            }
            case role::Brute:
            {
                min += 3_rank;
                max += 0_rank;
                break;
            }
            case role::Berserker:
            {
                min += 4_rank;
                max += 0_rank;
                break;
            }
            case role::Mountain:
            {
                min += 4_rank;
                max += 10_rank;
                break;
            }
            case role::Captain:
            {
                min += 3_rank;
                max += 10_rank;
                break;
            }
            case role::Chieftain:
            {
                min += 6_rank;
                max += 15_rank;
                break;
            }
            case role::Trader:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            case role::Warlord:
            {
                min += 9_rank;
                max += 20_rank;
                break;
            }
            case role::Shaman:
            {
                min += 8_rank;
                max += 10_rank;
                break;
            }
            case role::Smasher:
            {
                min += 2_rank;
                max += 2_rank;
                break;
            }
            case role::Strangler:
            {
                min += 1_rank;
                max += 0_rank;
                break;
            }
            case role::Soldier:
            {
                min += 2_rank;
                max -= 5_rank;
                break;
            }
            case role::TwoHeaded:
            {
                min += 5_rank;
                max += 0_rank;
                break;
            }
            case role::Giant:
            {
                min += 1_rank;
                max += 0_rank;
                break;
            }
            case role::Elder:
            {
                min += 11_rank;
                max += 20_rank;
                break;
            }
            case role::FourArmed:
            {
                min += 4_rank;
                max += 0_rank;
                break;
            }
            case role::Tendrilus:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            case role::Wing:
            {
                min -= 1_rank;
                max -= 5_rank;
                break;
            }
            case role::Whelp:
            {
                min -= 1_rank;
                max -= 5_rank;
                break;
            }
            case role::Pod:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            case role::Spike:
            {
                min += 4_rank;
                max += 0_rank;
                break;
            }
            case role::Skeleton:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            case role::Ranger:
            {
                min += 4_rank;
                max += 0_rank;
                break;
            }
            case role::Water:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            case role::Blacksmith:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            case role::Spider:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            case role::Beetle:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            case role::Boar:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            case role::Lion:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            case role::Ramonaut:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            case role::Serpent:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            case role::Bat:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            case role::Ghost:
            {
                min += 5_rank;
                max += 0_rank;
                break;
            }
            case role::Cat:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            case role::Wolf:
            {
                min += 1_rank;
                max += 0_rank;
                break;
            }
            case role::Count:
            {
                min += 0_rank;
                max += 0_rank;
                break;
            }
            default:
            {
                break;
            }
        }

        if (min < 1_rank)
        {
            min = 1_rank;
        }

        if (min > 199_rank)
        {
            min = 199_rank;
        }

        if (max > 200_rank)
        {
            max = 200_rank;
        }

        if (max <= min)
        {
            max = min + 1_rank;
        }

        return RankRange_t(min, max);
    }

    const std::vector<std::string> race::CorpseImageKeys(const race::Enum ENUM)
    {
        switch (ENUM)
        {
            case race::Shade:
            case race::Ghoul:
            case race::Human:
            case race::Gnome:
            case race::Pixie:
            case race::Witch:
            case race::Halfling:
            case race::Skeleton:
            {
                return { "media-image-bone-pile-skull-humanoid",
                         "media-image-bone-pile-skull-humanoid-pile-1",
                         "media-image-bone-pile-skull-humanoid-pile-2",
                         "media-image-bone-pile-skull-humanoid-pile-3" };
            }

            case race::Wyvern:
            case race::Dragon:
            case race::Hydra:
            {
                return { "media-image-bone-pile-skull-dragon-1",
                         "media-image-bone-pile-skull-dragon-2",
                         "media-image-bone-pile-skull-dragon-3",
                         "media-image-bone-pile-skull-dragon-4" };
            }

            case race::Orc:
            case race::Goblin:
            {
                return { "media-image-bone-pile-skull-goblin" };
            }

            case race::Pug:
            case race::Newt:
            {
                return { "media-image-bone-pile-skull-animal-2" };
            }

            case race::Naga:
            case race::LizardWalker:
            {
                // the orc skull looked better as a naga/lizard skull
                return { "media-image-bone-pile-skull-orc" };
            }

            case race::Bog:
            {
                return { "media-image-bone-pile-skull-bog" };
            }

            case race::Spider:
            case race::CaveCrawler:
            {
                return { "media-image-bone-pile-cave-crawler" };
            }

            case race::Minotaur:
            {
                return { "media-image-bone-pile-skull-minotaur" };
            }

            case race::Plant:
            {
                return { "media-image-bone-pile-skull-animal-1" };
            }

            case race::Beetle:
            {
                return { "media-image-bone-pile-beetle" };
            }

            case race::Demon:
            {
                return { "media-image-bone-pile-skull-demon" };
            }

            case race::Harpy:
            case race::Griffin:
            {
                return { "media-image-bone-pile-griffin" };
            }

            case race::Boar:
            case race::LionBoar:
            case race::Ramonaut:
            case race::Wereboar:
            {
                return { "media-image-bone-pile-skull-animal-3" };
            }

            case race::Wolfen:
            case race::Lion:
            case race::Werebear:
            case race::Werewolf:
            {
                return { "media-image-bone-pile-wolfen" };
            }

            case race::Serpent:
            case race::Cobra:
            {
                return { "media-image-bone-pile-skull-snake" };
            }

            case race::Werecat:
            {
                return { "media-image-bone-pile-cat" };
            }

            case race::Ogre:
            case race::Golem:
            case race::Troll:
            case race::Giant:
            {
                return { "media-image-bone-pile-skull-giant" };
            }

            case race::Bat:
            case race::Werebat:
            {
                return { "media-image-bone-pile-bat" };
            }

            case race::ThreeHeadedHound:
            {
                return { "media-image-bone-pile-three-headed-hound" };
            }

            case race::Count:
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(ENUM) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return {};
            }
        }
    }

} // namespace creature
} // namespace heroespath
