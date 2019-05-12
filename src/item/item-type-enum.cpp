// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-type-enum.cpp
//
#include "item-type-enum.hpp"

#include "misc/boost-string-includes.hpp"
#include "misc/config-file.hpp"
#include "misc/enum-util.hpp"
#include "misc/log-macros.hpp"

#include <utility>

namespace heroespath
{
namespace item
{

    const std::string category::Name(const Enum ENUM, const EnumStringHow & HOW)
    {
        std::string str = EnumUtil<category>::ToString(ENUM, HOW);
        boost::algorithm::ireplace_all(str, NAMEOF_ENUM(OneHanded), "one-handed");
        boost::algorithm::ireplace_all(str, NAMEOF_ENUM(TwoHanded), "two-handed");
        boost::algorithm::ireplace_all(str, NAMEOF_ENUM(ConsumedOnUse), "consumed upon use");
        boost::algorithm::ireplace_all(str, NAMEOF_ENUM(ShowsEnemyInfo), "shows enemy info");
        return str;
    }

    const std::string
        element_type::Name(const Enum ELEMENT_TYPE, const EnumStringHow &, const bool INCLUDE_OF)
    {
        std::string str;
        str.reserve(64);

        auto appendIfBitIsSet { [&](const element_type::Enum BIT) {
            if (ELEMENT_TYPE & BIT)
            {
                if (str.empty())
                {
                    if (INCLUDE_OF)
                    {
                        str += "of ";
                    }
                }
                else
                {
                    str += " and ";
                }

                str += NAMEOF_ENUM(BIT);
            }
        } };

        appendIfBitIsSet(element_type::Fire);
        appendIfBitIsSet(element_type::Frost);
        appendIfBitIsSet(element_type::Honor);
        appendIfBitIsSet(element_type::Shadow);

        return str;
    }

    const ElementEnumVec_t
        misc_type::ElementTypes(const Enum MISC_TYPE, const bool WILL_INCLUDE_NONE)
    {
        ElementEnumVec_t elementTypes;

        if ((MISC_TYPE == CapeCommanders) || (MISC_TYPE == CapeGenerals) || (MISC_TYPE == CapeKings)
            || (MISC_TYPE == SaintCameoPin))
        {
            elementTypes = { element_type::Honor };
        }
        else if (MISC_TYPE == JeweledPrincessVeil)
        {
            elementTypes = { element_type::Honor, element_type::Shadow };
        }
        else if ((MISC_TYPE == JeweledArmband) || (MISC_TYPE == ManaAmulet))
        {
            elementTypes = {
                element_type::Fire, element_type::Frost, element_type::Honor, element_type::Shadow
            };
        }

        if (WILL_INCLUDE_NONE)
        {
            elementTypes.emplace_back(element_type::None);
        }

        return elementTypes;
    }

    const ElementEnumVec_t
        named_type::ElementTypes(const named_type::Enum NAMED_TYPE, const bool WILL_INCLUDE_NONE)
    {
        ElementEnumVec_t elementTypes;

        if (NAMED_TYPE == named_type::Wicked)
        {
            elementTypes = { element_type::Fire, element_type::Shadow };
        }
        else if (NAMED_TYPE == named_type::Infernal)
        {
            elementTypes
                = element_type::ValidCombinations(element_type::Fire | element_type::Shadow);
        }
        else if (
            (NAMED_TYPE == named_type::Light) || (NAMED_TYPE == named_type::Dawn)
            || (NAMED_TYPE == named_type::Sun))
        {
            elementTypes
                = element_type::ValidCombinations(element_type::Fire | element_type::Honor);
        }
        else if (NAMED_TYPE == named_type::Dancing)
        {
            elementTypes = { element_type::Fire, element_type::Frost };
        }
        else if (NAMED_TYPE == named_type::Princes)
        {
            elementTypes = { element_type::Honor, element_type::Shadow };
        }
        else if (
            (NAMED_TYPE == named_type::Searing) || (NAMED_TYPE == named_type::Burning)
            || (NAMED_TYPE == named_type::Fiery) || (NAMED_TYPE == named_type::Charred))
        {
            elementTypes = { element_type::Fire };
        }
        else if (
            (NAMED_TYPE == named_type::Icy) || (NAMED_TYPE == named_type::Winter)
            || (NAMED_TYPE == named_type::Frigid) || (NAMED_TYPE == named_type::Chill)
            || (NAMED_TYPE == named_type::Frozen) || (NAMED_TYPE == named_type::Arctic))
        {
            elementTypes = { element_type::Frost };
        }
        else if (
            (NAMED_TYPE == named_type::Honest) || (NAMED_TYPE == named_type::Noble)
            || (NAMED_TYPE == named_type::Daring) || (NAMED_TYPE == named_type::Elite)
            || (NAMED_TYPE == named_type::Valiant) || (NAMED_TYPE == named_type::Heroes)
            || (NAMED_TYPE == named_type::Gladiator) || (NAMED_TYPE == named_type::Proud)
            || (NAMED_TYPE == named_type::Glory) || (NAMED_TYPE == named_type::Pure)
            || (NAMED_TYPE == named_type::Samurai) || (NAMED_TYPE == named_type::Thors))
        {
            elementTypes = { element_type::Honor };
        }
        else if (
            (NAMED_TYPE == named_type::Diabolic) || (NAMED_TYPE == named_type::Fiendish)
            || (NAMED_TYPE == named_type::Nile) || (NAMED_TYPE == named_type::Gloom)
            || (NAMED_TYPE == named_type::Twilight) || (NAMED_TYPE == named_type::Dusk)
            || (NAMED_TYPE == named_type::Sorrow) || (NAMED_TYPE == named_type::Woe)
            || (NAMED_TYPE == named_type::Misery) || (NAMED_TYPE == named_type::Moon)
            || (NAMED_TYPE == named_type::Dark) || (NAMED_TYPE == named_type::Betrayer)
            || (NAMED_TYPE == named_type::Burglar) || (NAMED_TYPE == named_type::Mountebank)
            || (NAMED_TYPE == named_type::Charlatans))
        {
            elementTypes = { element_type::Shadow };
        }

        if (WILL_INCLUDE_NONE)
        {
            elementTypes.emplace_back(element_type::None);
        }

        return elementTypes;
    }

    Armor_t material::ArmorRatingBonusSec(const material::Enum MATERIAL_SEC)
    {
        const auto SEC_MATERIAL_ARMOR_ADJ_RATIO {
            misc::ConfigFile::Instance()->ValueOrDefault<float>(
                "item-secondary-material-armor-adj-ratio")
        };

        return Armor_t::Make(
            ArmorRatingBonusPri(MATERIAL_SEC).GetAs<float>() * SEC_MATERIAL_ARMOR_ADJ_RATIO);
    }

    const std::string
        weapon_type::Name(const weapon_type::Enum WEAPON_TYPE, const EnumStringHow & HOW)
    {
        std::string str(EnumUtil<weapon_type>::ToString(WEAPON_TYPE, HOW));

        if (WEAPON_TYPE & weapon_type::BladedStaff)
        {
            boost::algorithm::replace_all(str, "BladedStaff", "Bladed Staff");
        }

        return str;
    }

} // namespace item
} // namespace heroespath
