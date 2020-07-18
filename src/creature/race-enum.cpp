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

    const std::string race::Desc(const race::Enum ENUM)
    {
        return misc::ConfigFile::Instance()->Value("creature-race-desc-" + NAMEOF_ENUM_STR(ENUM));
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
            case Pixie: return { role::Beastmaster, role::Sorcerer, role::Cleric, role::Bard };
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
            case Halfling: return { role::Shaman, role::Chieftain, role::Captain, role::Grunt };
            case Naga: return { role::Grunt };
            case Pug: return { role::Archer, role::Cleric, role::Grunt };
            case Giant:
            {
                return { role::Smasher,  role::Strangler, role::Warlord,
                         role::Mountain, role::Grunt,     role::Brute };
            }
            case Ghoul: return { role::Strangler, role::Spike, role::Mountain, role::Grunt };
            case Wolfen: return { role::Wolfen, role::TwoHeaded };
            case Spider: return { role::Spider };
            case CaveCrawler: return { role::Mountain };
            case Hydra: return { role::Wing };
            case Plant: return { role::Smasher, role::Tendrilus, role::Pod, role::Strangler };

            case Lion:
            case LionBoar: return { role::Lion };

            case Ramonaut: return { role::Ramonaut };
            case Serpent: return { role::Serpent, role::Water };
            case Cobra: return { role::Serpent };
            case Werebear: return { role::Elder, role::Grunt, role::Mountain };
            case Wereboar: return { role::Boar };
            case Werecat: return { role::Cat };
            case Werewolf: return { role::Wolf };
            case Beetle: return { role::Beetle };
            case Boar: return { role::Boar };
            case Witch:
            {
                return {
                    role::Elder, role::Ghost, role::Strangler, role::Mountain, role::Sorcerer
                };
            }
            case Golem: return { role::Brute };
            case ThreeHeadedHound: return { role::Mountain };
            case Skeleton:
            {
                return { role::Chieftain, role::FourArmed, role::Mountain, role::Grunt };
            }
            case Demon:
            {
                return { role::Skeleton, role::Spike, role::Strangler,
                         role::Whelp,    role::Wing,  role::Grunt };
            }
            case Shade: return { role::Ghost };
            case Griffin: return { role::Whelp, role::Wing };
            case Werebat: return { role::Bat };
            case Wyvern: return { role::Whelp, role::Wing };
            case Bat: return { role::Bat };
            case Harpy: return { role::Wing };
            case Dragon: return { role::Firebrand, role::Sylavin };
            case Count:
            default: return {};
        }
    }

    const OriginTypeVec_t race::OriginTypes(const race::Enum RACE, const role::Enum ROLE)
    {
        OriginTypeVec_t types;

        if ((RACE >= race::Count) && (ROLE >= role::Count))
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

    const std::string race::RaceRoleName(const race::Enum RACE_ENUM, const role::Enum ROLE_ENUM)
    {
        std::string str(race::Name(RACE_ENUM));

        if (RACE_ENUM == race::ThreeHeadedHound)
        {
            return str;
        }

        const auto IS_ROLE_FIRST { (
            (ROLE_ENUM == role::Firebrand) || (ROLE_ENUM == role::FourArmed)
            || (ROLE_ENUM == role::Giant) || (ROLE_ENUM == role::Mountain)
            || (ROLE_ENUM == role::Spike) || (ROLE_ENUM == role::Sylavin)
            || (ROLE_ENUM == role::Tendrilus) || (ROLE_ENUM == role::TwoHeaded)
            || (ROLE_ENUM == role::Water)) };

        str.reserve(32);

        if (IS_ROLE_FIRST)
        {
            str += role::Name(ROLE_ENUM);
        }
        else
        {
            str += race::Name(RACE_ENUM);
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
                    str += ' ';

                    if (IS_ROLE_FIRST)
                    {
                        str += race::Name(RACE_ENUM);
                    }
                    else
                    {
                        str += role::Name(ROLE_ENUM);
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

        return str;
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
