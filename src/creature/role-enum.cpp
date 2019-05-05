// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// role-enum.cpp
//
#include "role-enum.hpp"

#include "misc/config-file.hpp"
#include "misc/enum-util.hpp"
#include "misc/log-macros.hpp"

namespace heroespath
{
namespace creature
{

    const std::string role::ToString(const role::Enum ROLE)
    {
        switch (ROLE)
        {
            case Beastmaster: { return "Beastmaster";
            }
            case Sorcerer: { return "Sorcerer";
            }
            case Cleric: { return "Cleric";
            }
            case Knight: { return "Knight";
            }
            case Archer: { return "Archer";
            }
            case Bard: { return "Bard";
            }
            case Thief: { return "Thief";
            }
            case Wolfen: { return "Wolfen";
            }
            case Firebrand: { return "Firebrand";
            }
            case Sylavin: { return "Sylavin";
            }
            //
            case Thug: { return "Thug";
            }
            case Mugger: { return "Mugger";
            }
            case Drunk: { return "Drunk";
            }
            case Brute: { return "Brute";
            }
            case Berserker: { return "Berserker";
            }
            case Mountain: { return "Mountain";
            }
            case Grunt: { return "Grunt";
            }
            case Captain: { return "Captain";
            }
            case Chieftain: { return "Chieftain";
            }
            case Trader: { return "Trader";
            }
            case Warlord: { return "Warlord";
            }
            case Shaman: { return "Shaman";
            }
            case Smasher: { return "Smasher";
            }
            case Strangler: { return "Strangler";
            }
            case Soldier: { return "Soldier";
            }
            case TwoHeaded: { return "TwoHeaded";
            }
            case Giant: { return "Giant";
            }
            case Elder: { return "Elder";
            }
            case FourArmed: { return "FourArmed";
            }
            case Tendrilus: { return "Tendrilus";
            }
            case Wing: { return "Wing";
            }
            case Whelp: { return "Whelp";
            }
            case Pod: { return "Pod";
            }
            case Spike: { return "Spike";
            }
            case Skeleton: { return "Skeleton";
            }
            case Ranger: { return "Ranger";
            }
            case Water: { return "Water";
            }
            case Blacksmith: { return "Blacksmith";
            }
            case Spider: { return "Spider";
            }
            case Beetle: { return "Beetle";
            }
            case Boar: { return "Boar";
            }
            case Lion: { return "Lion";
            }
            case Ramonaut: { return "Ramonaut";
            }
            case Serpent: { return "Serpent";
            }
            case Bat: { return "Bat";
            }
            case Ghost: { return "Ghost";
            }
            case Cat: { return "Cat";
            }
            case Wolf: { return "Wolf";
            }
            case Count: { return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(ROLE) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

    const std::string role::Desc(const role::Enum ENUM)
    {
        return misc::ConfigFile::Instance()->Value("creature-role-" + ToString(ENUM) + "-desc");
    }

    const std::string role::Abbr(const role::Enum ENUM)
    {
        if (ENUM == Beastmaster)
        {
            return "Bsm";
        }
        else if (ENUM == Thief)
        {
            return "Thf";
        }
        else if (ENUM == Wolfen)
        {
            return "Wlf";
        }
        else
        {
            return Name(ENUM).substr(0, 3);
        }
    }

    bool role::CanFly(const role::Enum ENUM)
    {
        return (
            (ENUM == role::Firebrand) || (ENUM == role::Sylavin) || (ENUM == role::Wing)
            || (ENUM == role::Whelp) || (ENUM == role::Bat));
    }

    bool role::WillInitiallyFly(const role::Enum ENUM)
    {
        return ((ENUM == role::Wing) || (ENUM == role::Whelp) || (ENUM == role::Bat));
    }

    combat::BlockingPosType::Enum role::BlockingPosType(const role::Enum ENUM)
    {
        switch (ENUM)
        {
            case role::Mountain:
            case role::Spider:
            case role::Beetle:
            case role::Boar:
            case role::Lion:
            case role::Ramonaut:
            case role::Serpent:
            case role::Whelp:
            case role::Pod:
            case role::Spike:
            case role::Skeleton:
            case role::Thug:
            case role::Mugger:
            case role::Drunk:
            case role::Grunt:
            case role::FourArmed:
            case role::Tendrilus:
            case role::TwoHeaded:
            case role::Giant:
            case role::Smasher:
            case role::Strangler:
            case role::Soldier:
            case role::Brute:
            case role::Berserker:
            case role::Knight:
            case role::Wolfen:
            case role::Firebrand:
            case role::Ranger:
            case role::Water:
            case role::Blacksmith:
            case role::Cat:
            case role::Wolf:
            case role::Sylavin: { return combat::BlockingPosType::Front;
            }

            case role::Ghost:
            case role::Bat:
            case role::Beastmaster:
            case role::Bard:
            case role::Archer:
            case role::Wing: { return combat::BlockingPosType::Support;
            }

            case role::Captain:
            case role::Warlord:
            case role::Chieftain: { return combat::BlockingPosType::Commanders;
            }

            case role::Thief: { return combat::BlockingPosType::Reluctant;
            }

            case role::Elder:
            case role::Shaman:
            case role::Sorcerer:
            case role::Cleric: { return combat::BlockingPosType::Casters;
            }

            case role::Trader: { return combat::BlockingPosType::Last;
            }

            case role::Count:
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(ENUM) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return combat::BlockingPosType::Count;
            }
        }
    }

    std::vector<role::Enum>
        role::RolesOfBlockingPosType(const combat::BlockingPosType::Enum BLOCKING_POS_ENUM)
    {
        std::vector<role::Enum> rolesVec;
        for (EnumUnderlying_t i(0); i < role::Count; ++i)
        {
            const auto ROLE { static_cast<role::Enum>(i) };
            if (BlockingPosType(ROLE) == BLOCKING_POS_ENUM)
            {
                rolesVec.emplace_back(ROLE);
            }
        }

        if (rolesVec.empty())
        {
            M_HP_LOG_ERR(
                "role::RolesOfBlockingPosType(" << BLOCKING_POS_ENUM << ") produced no roles.");
        }

        return rolesVec;
    }

} // namespace creature
} // namespace heroespath
