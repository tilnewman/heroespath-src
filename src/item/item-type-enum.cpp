// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// item-type-enum.cpp
//
#include "item-type-enum.hpp"

#include "game/game-data-file.hpp"

#include <algorithm>
#include <exception>
#include <sstream>

namespace heroespath
{
namespace item
{

    const std::string
        category::ToString(const item::category::Enum CATEGORY_TYPE, const bool WILL_WRAP)
    {
        if (CATEGORY_TYPE == category::None)
        {
            return "";
        }

        std::ostringstream ss;

        auto appendNameIfCategoryBitIsSet{ [&](const category::Enum CATEGORY_BIT,
                                               const std::string & NAME) {
            if (CATEGORY_TYPE & CATEGORY_BIT)
            {
                if (ss.str().empty() == false)
                {
                    ss << ", ";
                }

                ss << NAME;
            }
        } };

        appendNameIfCategoryBitIsSet(category::Broken, "broken");
        appendNameIfCategoryBitIsSet(category::Useable, "useable");
        appendNameIfCategoryBitIsSet(category::BodyPart, "bodypart");
        appendNameIfCategoryBitIsSet(category::Equippable, "equippable");
        appendNameIfCategoryBitIsSet(category::Wearable, "wearable");
        appendNameIfCategoryBitIsSet(category::OneHanded, "one-handed");
        appendNameIfCategoryBitIsSet(category::TwoHanded, "two-handed");
        appendNameIfCategoryBitIsSet(category::ConsumedOnUse, "consumed upon use");
        appendNameIfCategoryBitIsSet(category::ShowsEnemyInfo, "shows enemy info");

        if (ss.str().empty())
        {
            std::ostringstream ssErr;
            ssErr << "item::category::ToString(" << CATEGORY_TYPE << ")_InvalidValueError";
            throw std::range_error(ssErr.str());
        }

        if (WILL_WRAP)
        {
            return "(" + ss.str() + ")";
        }
        else
        {
            return ss.str();
        }
    }

    const std::string element_type::ToString(
        const element_type::Enum ELEMENT_TYPE, const bool WILL_WRAP, const std::string & SEPARATOR)
    {
        if (ELEMENT_TYPE == element_type::None)
        {
            return "";
        }

        std::ostringstream ss;

        auto appendNameIfElementBitIsSet{ [&](const element_type::Enum ELEMENT_BIT,
                                              const std::string & NAME) {
            if (ELEMENT_TYPE & ELEMENT_BIT)
            {
                if (ss.str().empty() == false)
                {
                    ss << SEPARATOR;
                }

                ss << NAME;
            }
        } };

        appendNameIfElementBitIsSet(element_type::Fire, "Fire");
        appendNameIfElementBitIsSet(element_type::Frost, "Frost");
        appendNameIfElementBitIsSet(element_type::Honor, "Honor");
        appendNameIfElementBitIsSet(element_type::Shadow, "Shadow");

        if (ss.str().empty())
        {
            std::ostringstream ssErr;
            ssErr << "item::element_type::ToString(" << ELEMENT_TYPE << ")_InvalidValueError";
            throw std::range_error(ssErr.str());
        }

        if (WILL_WRAP)
        {
            return "(" + ss.str() + ")";
        }
        else
        {
            return ss.str();
        }
    }

    const std::string
        element_type::Name(const element_type::Enum ELEMENT_TYPE, const bool WILL_WRAP)
    {
        if (ELEMENT_TYPE == element_type::None)
        {
            if (WILL_WRAP)
            {
                return "()";
            }
            else
            {
                return "";
            }
        }

        std::vector<std::string> names;

        auto appendNameIfElementBitIsSet{ [&](const element_type::Enum ELEMENT_BIT) {
            if (ELEMENT_TYPE & ELEMENT_BIT)
            {
                names.emplace_back(element_type::ToString(ELEMENT_BIT, false));
            }
        } };

        appendNameIfElementBitIsSet(element_type::Fire);
        appendNameIfElementBitIsSet(element_type::Frost);
        appendNameIfElementBitIsSet(element_type::Honor);
        appendNameIfElementBitIsSet(element_type::Shadow);

        std::ostringstream ss;

        if (names.empty())
        {
            std::ostringstream ssErr;
            ssErr << "item::element_type::Name(" << ELEMENT_TYPE << ")_InvalidValueError";
            throw std::range_error(ssErr.str());
        }

        ss << "of ";

        if (names.size() == 1)
        {
            ss << names.at(0);
        }
        else if (names.size() == 2)
        {
            ss << names.at(0) << " and " << names.at(1);
        }
        else if (names.size() == 3)
        {
            ss << names.at(0) << ", " << names.at(1) << " and " << names.at(2);
        }
        else
        {
            ss << names.at(0) << ", " << names.at(1) << ", " << names.at(2) << " and "
               << names.at(3);
        }

        if (WILL_WRAP)
        {
            return "(" + ss.str() + ")";
        }
        else
        {
            return ss.str();
        }
    }

    int element_type::ValidTypeCounter(const Enum ELEMENT_TYPE)
    {
        if (ELEMENT_TYPE == element_type::None)
        {
            return 0;
        }
        else if (
            ((ELEMENT_TYPE & Fire) && (ELEMENT_TYPE & Honor))
            || ((ELEMENT_TYPE & Fire) && (ELEMENT_TYPE & Shadow))
            || ((ELEMENT_TYPE & Frost) && (ELEMENT_TYPE & Honor))
            || ((ELEMENT_TYPE & Frost) && (ELEMENT_TYPE & Shadow)))
        {
            return 2;
        }
        else
        {
            return 1;
        }
    }

    bool element_type::IsValid(const element_type::Enum E)
    {
        if (((E & element_type::Fire) && (E & element_type::Frost))
            || ((E & element_type::Honor) && (E & element_type::Shadow)))
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    const std::string misc_type::ToString(const misc_type::Enum E)
    {
        switch (E)
        {
            case NotMisc:
            {
                return "NotMisc";
            }
            case Amulet:
            {
                return "Amulet";
            }
            case Ankh_Necklace:
            {
                return "Ankh_Necklace";
            }
            case Armband:
            {
                return "Armband";
            }
            case Bag:
            {
                return "Bag";
            }
            case Beard:
            {
                return "Beard";
            }
            case Bell:
            {
                return "Bell";
            }
            case Bird_Claw:
            {
                return "Bird_Claw";
            }
            case Bone:
            {
                return "Bone";
            }
            case Bone_Whistle:
            {
                return "Bone_Whistle";
            }
            case Book:
            {
                return "Book";
            }
            case Bracelet_Crown:
            {
                return "Bracelet_Crown";
            }
            case Bracelet_Feather:
            {
                return "Bracelet_Feather";
            }
            case Bracelet_Fist:
            {
                return "Bracelet_Fist";
            }
            case Bracelet_Hourglass:
            {
                return "Bracelet_Hourglass";
            }
            case Bracelet_Key:
            {
                return "Bracelet_Key";
            }
            case Bracelet_Mask:
            {
                return "Bracelet_Mask";
            }
            case Angel_Braid:
            {
                return "Angel_Braid";
            }
            case Brooch_Crown:
            {
                return "Brooch_Crown";
            }
            case Brooch_Feather:
            {
                return "Brooch_Feather";
            }
            case Brooch_Fist:
            {
                return "Brooch_Fist";
            }
            case Brooch_Hourglass:
            {
                return "Brooch_Hourglass";
            }
            case Brooch_Key:
            {
                return "Brooch_Key";
            }
            case Brooch_Mask:
            {
                return "Brooch_Mask";
            }
            case Bust:
            {
                return "Bust";
            }
            case CameoPin:
            {
                return "CameoPin";
            }
            case Cape:
            {
                return "Cape";
            }
            case Cat:
            {
                return "Cat";
            }
            case Chains:
            {
                return "Chains";
            }
            case Charm_Crown:
            {
                return "Charm_Crown";
            }
            case Charm_Feather:
            {
                return "Charm_Feather";
            }
            case Charm_Fist:
            {
                return "Charm_Fist";
            }
            case Charm_Hourglass:
            {
                return "Charm_Hourglass";
            }
            case Charm_Key:
            {
                return "Charm_Key";
            }
            case Charm_Mask:
            {
                return "Charm_Mask";
            }
            case Chimes:
            {
                return "Chimes";
            }
            case Cloak:
            {
                return "Cloak";
            }
            case Conch:
            {
                return "Conch";
            }
            case Crumhorn:
            {
                return "Crumhorn";
            }
            case Devil_Horn:
            {
                return "Devil_Horn";
            }
            case Doll:
            {
                return "Doll";
            }
            case Doll_Blessed:
            {
                return "Doll_Blessed";
            }
            case Doll_Cursed:
            {
                return "Doll_Cursed";
            }
            case Dried_Head:
            {
                return "Dried_Head";
            }
            case DrumLute:
            {
                return "DrumLute";
            }
            case Egg:
            {
                return "Egg";
            }
            case Embryo:
            {
                return "Embryo";
            }
            case Eye:
            {
                return "Eye";
            }
            case Feather:
            {
                return "Feather";
            }
            case Figurine_Blessed:
            {
                return "Figurine_Blessed";
            }
            case Figurine_Cursed:
            {
                return "Figurine_Cursed";
            }
            case Golem_Finger:
            {
                return "Golem_Finger";
            }
            case Fingerclaw:
            {
                return "Fingerclaw";
            }
            case Flag:
            {
                return "Flag";
            }
            case Frog:
            {
                return "Frog";
            }
            case Gecko:
            {
                return "Gecko";
            }
            case Ghost_Sheet:
            {
                return "Ghost_Sheet";
            }
            case Gizzard:
            {
                return "Gizzard";
            }
            case Goblet:
            {
                return "Goblet";
            }
            case Gong:
            {
                return "Gong";
            }
            case Grave_Ornament:
            {
                return "Grave_Ornament";
            }
            case Handbag:
            {
                return "Handbag";
            }
            case Hanky:
            {
                return "Hanky";
            }
            case Headdress:
            {
                return "Headdress";
            }
            case Hide:
            {
                return "Hide";
            }
            case Horn:
            {
                return "Horn";
            }
            case Horseshoe:
            {
                return "Horseshoe";
            }
            case Hurdy_Gurdy:
            {
                return "Hurdy_Gurdy";
            }
            case Icicle:
            {
                return "Icicle";
            }
            case Iguana:
            {
                return "Iguana";
            }
            case Imp_Tail:
            {
                return "Imp_Tail";
            }
            case Key:
            {
                return "Key";
            }
            case Leaf:
            {
                return "Leaf";
            }
            case Legtie:
            {
                return "Legtie";
            }
            case Litch_Hand:
            {
                return "Litch_Hand";
            }
            case Lizard:
            {
                return "Lizard";
            }
            case LockPicks:
            {
                return "LockPicks";
            }
            case Lyre:
            {
                return "Lyre";
            }
            case Magnifying_Glass:
            {
                return "Magnifying_Glass";
            }
            case Mask:
            {
                return "Mask";
            }
            case Mirror:
            {
                return "Mirror";
            }
            case Mummy_Hand:
            {
                return "Mummy_Hand";
            }
            case Necklace:
            {
                return "Necklace";
            }
            case Noose:
            {
                return "Noose";
            }
            case Nose:
            {
                return "Nose";
            }
            case Orb:
            {
                return "Orb";
            }
            case Paw:
            {
                return "Paw";
            }
            case Pendant:
            {
                return "Pendant";
            }
            case Petrified_Snake:
            {
                return "Petrified_Snake";
            }
            case Pin_Crown:
            {
                return "Pin_Crown";
            }
            case Pin_Feather:
            {
                return "Pin_Feather";
            }
            case Pin_Fist:
            {
                return "Pin_Fist";
            }
            case Pin_Hourglass:
            {
                return "Pin_Hourglass";
            }
            case Pin_Key:
            {
                return "Pin_Key";
            }
            case Pin_Mask:
            {
                return "Pin_Mask";
            }
            case Pipe_And_Tabor:
            {
                return "Pipe_And_Tabor";
            }
            case Potion:
            {
                return "Potion";
            }
            case Puppet_Blessed:
            {
                return "Puppet_Blessed";
            }
            case Puppet_Cursed:
            {
                return "Puppet_Cursed";
            }
            case Rabbit_Foot:
            {
                return "Rabbit_Foot";
            }
            case Rainmaker:
            {
                return "Rainmaker";
            }
            case Rat_Juju:
            {
                return "Rat_Juju";
            }
            case Rattlesnake_Tail:
            {
                return "Rattlesnake_Tail";
            }
            case Recorder:
            {
                return "Recorder";
            }
            case Relic:
            {
                return "Relic";
            }
            case Ring:
            {
                return "Ring";
            }
            case Robe:
            {
                return "Robe";
            }
            case Salamander:
            {
                return "Salamander";
            }
            case Scales:
            {
                return "Scales";
            }
            case Scepter:
            {
                return "Scepter";
            }
            case Scroll:
            {
                return "Scroll";
            }
            case Scythe:
            {
                return "Scythe";
            }
            case Seeds:
            {
                return "Seeds";
            }
            case Shard:
            {
                return "Shard";
            }
            case Shark_Tooth_Necklace:
            {
                return "Shark_Tooth_Necklace";
            }
            case Shroud:
            {
                return "Shroud";
            }
            case Signet_Crown:
            {
                return "Signet_Crown";
            }
            case Signet_Feather:
            {
                return "Signet_Feather";
            }
            case Signet_Fist:
            {
                return "Signet_Fist";
            }
            case Signet_Hourglass:
            {
                return "Signet_Hourglass";
            }
            case Signet_Key:
            {
                return "Signet_Key";
            }
            case Signet_Mask:
            {
                return "Signet_Mask";
            }
            case Skink:
            {
                return "Skink";
            }
            case Spider_Eggs:
            {
                return "Spider_Eggs";
            }
            case Spyglass:
            {
                return "Spyglass";
            }
            case Staff:
            {
                return "Staff";
            }
            case Summoning_Statue:
            {
                return "Summoning_Statue";
            }
            case Talisman:
            {
                return "Talisman";
            }
            case Tome:
            {
                return "Tome";
            }
            case Tongue:
            {
                return "Tongue";
            }
            case Tooth:
            {
                return "Tooth";
            }
            case Tooth_Necklace:
            {
                return "Tooth_Necklace";
            }
            case Troll_Figure:
            {
                return "Troll_Figure";
            }
            case Trophy:
            {
                return "Trophy";
            }
            case Tuning_Fork:
            {
                return "Tuning_Fork";
            }
            case Turtle_Shell:
            {
                return "Turtle_Shell";
            }
            case Unicorn_Horn:
            {
                return "Unicorn_Horn";
            }
            case Veil:
            {
                return "Veil";
            }
            case Viol:
            {
                return "Viol";
            }
            case Wand:
            {
                return "Wand";
            }
            case Warhorse_Marionette:
            {
                return "Warhorse_Marionette";
            }
            case Weasel_Totem:
            {
                return "Weasel_Totem";
            }
            case Wolfen_Fur:
            {
                return "Wolfen_Fur";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "item::misc_type::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    const std::string misc_type::Name(const misc_type::Enum E)
    {
        switch (E)
        {
            case NotMisc:
            case Angel_Braid:
            {
                return "Angel Braid";
            }
            case Ankh_Necklace:
            {
                return "Ankh Necklace";
            }
            case Bird_Claw:
            {
                return "Raven Claw";
            }
            case Bone_Whistle:
            {
                return "Dragon Bone Whistle";
            }
            case Bracelet_Crown:
            {
                return "Crown Emblem Bracelet";
            }
            case Bracelet_Feather:
            {
                return "Feather Emblem Bracelet";
            }
            case Bracelet_Fist:
            {
                return "Fist Emblem Bracelet";
            }
            case Bracelet_Hourglass:
            {
                return "Hourglass Emblem Bracelet";
            }
            case Bracelet_Key:
            {
                return "Key Emblem Bracelet";
            }
            case Bracelet_Mask:
            {
                return "Mask Emblem Bracelet";
            }
            case Brooch_Crown:
            {
                return "Crown Emblem Brooch";
            }
            case Brooch_Feather:
            {
                return "Feather Emblem Brooch";
            }
            case Brooch_Fist:
            {
                return "Fist Emblem Brooch";
            }
            case Brooch_Hourglass:
            {
                return "Hourglass Emblem Brooch";
            }
            case Brooch_Key:
            {
                return "Key Emblem Brooch";
            }
            case Brooch_Mask:
            {
                return "Mask Emblem Brooch";
            }
            case CameoPin:
            {
                return "Cameo Pin";
            }
            case Charm_Crown:
            {
                return "Crown Charm";
            }
            case Charm_Feather:
            {
                return "Feather Charm";
            }
            case Charm_Fist:
            {
                return "Fist Charm";
            }
            case Charm_Hourglass:
            {
                return "Hourglass Charm";
            }
            case Charm_Key:
            {
                return "Key Charm";
            }
            case Charm_Mask:
            {
                return "Mask Charm";
            }
            case Devil_Horn:
            {
                return "Devil Horn";
            }
            case Doll_Blessed:
            {
                return "Doll";
            }
            case Doll_Cursed:
            {
                return "Doll";
            }
            case Dried_Head:
            {
                return "Dried Head";
            }
            case Figurine_Blessed:
            {
                return "Figurine";
            }
            case Figurine_Cursed:
            {
                return "Figurine";
            }
            case Ghost_Sheet:
            {
                return "Ghost Sheet";
            }
            case Golem_Finger:
            {
                return "Golem Finger";
            }
            case Grave_Ornament:
            {
                return "Grave Ornament";
            }
            case Hurdy_Gurdy:
            {
                return "Hurdy-Gurdy";
            }
            case Imp_Tail:
            {
                return "Imp Tail";
            }
            case Litch_Hand:
            {
                return "Litch Hand";
            }
            case Magnifying_Glass:
            {
                return "Magnifying Glass";
            }
            case Petrified_Snake:
            {
                return "Petrified Snake";
            }
            case Pin_Crown:
            {
                return "Crown Emblem Pin";
            }
            case Pin_Feather:
            {
                return "Feather Emblem Pin";
            }
            case Pin_Fist:
            {
                return "Fist Emblem Pin";
            }
            case Pin_Hourglass:
            {
                return "Hourglass Emblem Pin";
            }
            case Pin_Key:
            {
                return "Key Emblem Pin";
            }
            case Pin_Mask:
            {
                return "Mask Emblem Pin";
            }
            case Pipe_And_Tabor:
            {
                return "Pipe and Tabor";
            }
            case Puppet_Blessed:
            {
                return "Puppet";
            }
            case Puppet_Cursed:
            {
                return "Puppet";
            }
            case Rabbit_Foot:
            {
                return "Rabbit Foot";
            }
            case Rat_Juju:
            {
                return "Rat Juju";
            }
            case Rattlesnake_Tail:
            {
                return "Rattlesnake Tail";
            }
            case Shark_Tooth_Necklace:
            {
                return "Shark Tooth Necklace";
            }
            case Signet_Crown:
            {
                return "Crown Signet";
            }
            case Signet_Feather:
            {
                return "Feather Signet";
            }
            case Signet_Fist:
            {
                return "Fist Signet";
            }
            case Signet_Hourglass:
            {
                return "Hourglass Signet";
            }
            case Signet_Key:
            {
                return "Key Signet";
            }
            case Signet_Mask:
            {
                return "Mask Signet";
            }
            case Spider_Eggs:
            {
                return "Spider Eggs";
            }
            case Summoning_Statue:
            {
                return "Summoning Statue";
            }
            case Tooth_Necklace:
            {
                return "Vampire Tooth Necklace";
            }
            case Troll_Figure:
            {
                return "Troll Figure";
            }
            case Tuning_Fork:
            {
                return "Tuning Fork";
            }
            case Turtle_Shell:
            {
                return "Turtle Shell";
            }
            case Unicorn_Horn:
            {
                return "Unicorn Horn";
            }
            case Warhorse_Marionette:
            {
                return "Warhorse Marionette";
            }
            case Weasel_Totem:
            {
                return "Weasel Totem";
            }
            case Wolfen_Fur:
            {
                return "Wolfen Fur";
            }
            case Amulet:
            case Armband:
            case Bag:
            case Beard:
            case Bell:
            case Bone:
            case Book:
            case Bust:
            case Cape:
            case Cat:
            case Chains:
            case Chimes:
            case Cloak:
            case Conch:
            case Crumhorn:
            case Doll:
            case DrumLute:
            case Embryo:
            case Egg:
            case Eye:
            case Feather:
            case Fingerclaw:
            case Flag:
            case Frog:
            case Gecko:
            case Gizzard:
            case Goblet:
            case Gong:
            case Handbag:
            case Hanky:
            case Headdress:
            case Hide:
            case Horn:
            case Horseshoe:
            case Icicle:
            case Iguana:
            case Key:
            case Leaf:
            case Legtie:
            case Lizard:
            case LockPicks:
            case Lyre:
            case Mask:
            case Mirror:
            case Mummy_Hand:
            case Necklace:
            case Noose:
            case Nose:
            case Orb:
            case Paw:
            case Pendant:
            case Potion:
            case Rainmaker:
            case Recorder:
            case Relic:
            case Ring:
            case Robe:
            case Salamander:
            case Scales:
            case Scepter:
            case Scroll:
            case Scythe:
            case Seeds:
            case Skink:
            case Shard:
            case Shroud:
            case Spyglass:
            case Staff:
            case Talisman:
            case Tome:
            case Tongue:
            case Tooth:
            case Trophy:
            case Veil:
            case Viol:
            case Wand:
            {
                return misc_type::ToString(E);
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "item::misc_type::Name(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    bool misc_type::IsMusicalInstrument(const Enum MISC_TYPE)
    {
        return (
            (MISC_TYPE == DrumLute) || (MISC_TYPE == Crumhorn) || (MISC_TYPE == Hurdy_Gurdy)
            || (MISC_TYPE == Lyre) || (MISC_TYPE == Pipe_And_Tabor) || (MISC_TYPE == Recorder)
            || (MISC_TYPE == Viol));
    }

    bool misc_type::IsSummoning(const Enum MISC_TYPE)
    {
        return (
            (MISC_TYPE == Egg) || (MISC_TYPE == Embryo) || (MISC_TYPE == Seeds)
            || (MISC_TYPE == Summoning_Statue) || ((MISC_TYPE == Spider_Eggs)));
    }

    bool misc_type::IsStandalone(const misc_type::Enum MISC_TYPE)
    {
        // keep in sync with misc_type::Materials() and ItemProfile::SetMisc()
        return (
            IsSummoning(MISC_TYPE) || IsBlessed(MISC_TYPE) || IsCursed(MISC_TYPE)
            || IsOrdinary(MISC_TYPE));
    }

    bool misc_type::IsOrdinary(const Enum MISC_TYPE)
    {
        // keep in sync with misc_type::Materials() and ItemProfile::SetMisc()
        return (
            (MISC_TYPE == Bust) || (MISC_TYPE == Doll) || (MISC_TYPE == DrumLute)
            || (MISC_TYPE == Goblet) || (MISC_TYPE == Key) || (MISC_TYPE == LockPicks)
            || (MISC_TYPE == Mirror) || (MISC_TYPE == Orb) || (MISC_TYPE == Pendant)
            || (MISC_TYPE == Ring) || (MISC_TYPE == Scepter) || (MISC_TYPE == Shard)
            || (MISC_TYPE == Wand));
    }

    bool misc_type::HasPixieVersion(const Enum MISC_TYPE)
    {
        return ((MISC_TYPE == Bell) || IsMusicalInstrument(MISC_TYPE));
    }

    bool misc_type::HasOnlyPixieVersion(const misc_type::Enum MISC_TYPE)
    {
        return (MISC_TYPE == Bell);
    }

    float misc_type::ReligiousRatio(const misc_type::Enum MISC_TYPE)
    {
        if (MISC_TYPE == Figurine_Blessed)
        {
            return 0.9f;
        }
        else if (MISC_TYPE == Figurine_Cursed)
        {
            return 0.9f;
        }
        else if (MISC_TYPE == Doll_Blessed)
        {
            return 0.9f;
        }
        else if (MISC_TYPE == Doll_Cursed)
        {
            return 0.9f;
        }
        else if (MISC_TYPE == Bust)
        {
            return 0.6f;
        }
        else if (MISC_TYPE == Dried_Head)
        {
            return 0.75f;
        }
        else
        {
            return 0.0f;
        }
    }

    bool misc_type::HasNonFleshEyes(const misc_type::Enum MISC_TYPE)
    {
        return (
            (MISC_TYPE == Bust) || (MISC_TYPE == Doll) || (MISC_TYPE == Doll_Blessed)
            || (MISC_TYPE == Doll_Cursed) || (MISC_TYPE == Figurine_Blessed)
            || (MISC_TYPE == Figurine_Cursed) || (MISC_TYPE == Puppet_Blessed)
            || (MISC_TYPE == Puppet_Cursed) || (MISC_TYPE == Rat_Juju)
            || (MISC_TYPE == Summoning_Statue) || (MISC_TYPE == Talisman)
            || (MISC_TYPE == Troll_Figure) || (MISC_TYPE == Weasel_Totem)
            || (MISC_TYPE == Dried_Head)); // yeah it's flesh but what the hell having special
                                           // eyes is cool (oh and it's not edible so it's okay)
    }

    bool misc_type::IsBlessed(const misc_type::Enum E)
    {
        return (
            (E == Doll_Blessed) || (E == Figurine_Blessed) || (E == Puppet_Blessed) || (E == Bust));
    }

    bool misc_type::IsCursed(const misc_type::Enum E)
    {
        return (
            (E == Doll_Cursed) || (E == Figurine_Cursed) || (E == Puppet_Cursed)
            || (E == Dried_Head));
    }

    Weight_t misc_type::Weight(const misc_type::Enum MISC_TYPE)
    {
        // keep in sync with misc_type::IsStandalone()

        switch (MISC_TYPE)
        {
            case Amulet:
            {
                return 30_weight;
            }
            case Ankh_Necklace:
            {
                return 24_weight;
            }
            case Armband:
            {
                return 40_weight;
            }
            case Bag:
            {
                return 20_weight;
            }
            case Beard:
            {
                return 7_weight;
            }
            case Bell:
            {
                return 35_weight;
            }
            case Bird_Claw:
            {
                return 4_weight;
            }
            case Bone:
            {
                return 32_weight;
            }
            case Bone_Whistle:
            {
                return 8_weight;
            }
            case Book:
            {
                return 60_weight;
            }
            case Bracelet_Crown:
            case Bracelet_Feather:
            case Bracelet_Fist:
            case Bracelet_Hourglass:
            case Bracelet_Key:
            case Bracelet_Mask:
            {
                return 33_weight;
            }
            case Angel_Braid:
            {
                return 8_weight;
            }
            case Brooch_Crown:
            case Brooch_Feather:
            case Brooch_Fist:
            case Brooch_Hourglass:
            case Brooch_Key:
            case Brooch_Mask:
            {
                return 29_weight;
            }
            case Bust:
            {
                return 200_weight;
            }
            case CameoPin:
            {
                return 21_weight;
            }
            case Cape:
            {
                return 57_weight;
            }
            case Cat:
            {
                return 236_weight;
            }
            case Chains:
            {
                return 300_weight;
            }
            case Charm_Crown:
            case Charm_Feather:
            case Charm_Fist:
            case Charm_Hourglass:
            case Charm_Key:
            case Charm_Mask:
            {
                return 31_weight;
            }
            case Chimes:
            {
                return 58_weight;
            }
            case Cloak:
            {
                return 111_weight;
            }
            case Conch:
            {
                return 49_weight;
            }
            case Crumhorn:
            {
                return 47_weight;
            }
            case Devil_Horn:
            {
                return 85_weight;
            }
            case Doll:
            case Doll_Blessed:
            case Doll_Cursed:
            {
                return 28_weight;
            }
            case Dried_Head:
            {
                return 65_weight;
            }
            case DrumLute:
            {
                return 364_weight;
            }
            case Egg:
            {
                return 153_weight;
            }
            case Embryo:
            {
                return 41_weight;
            }
            case Eye:
            {
                return 31_weight;
            }
            case Feather:
            {
                return 1_weight;
            }
            case Figurine_Blessed:
            case Figurine_Cursed:
            {
                return 38_weight;
            }
            case Golem_Finger:
            {
                return 51_weight;
            }
            case Fingerclaw:
            {
                return 19_weight;
            }
            case Flag:
            {
                return 56_weight;
            }
            case Frog:
            {
                return 47_weight;
            }
            case Gecko:
            {
                return 46_weight;
            }
            case Ghost_Sheet:
            {
                return 56_weight;
            }
            case Gizzard:
            {
                return 53_weight;
            }
            case Goblet:
            {
                return 50_weight;
            }
            case Gong:
            {
                return 255_weight;
            }
            case Grave_Ornament:
            {
                return 77_weight;
            }
            case Handbag:
            {
                return 27_weight;
            }
            case Hanky:
            {
                return 13_weight;
            }
            case Headdress:
            {
                return 68_weight;
            }
            case Hide:
            {
                return 101_weight;
            }
            case Horn:
            {
                return 48_weight;
            }
            case Horseshoe:
            {
                return 47_weight;
            }
            case Hurdy_Gurdy:
            {
                return 446_weight;
            }
            case Icicle:
            {
                return 26_weight;
            }
            case Iguana:
            {
                return 44_weight;
            }
            case Imp_Tail:
            {
                return 52_weight;
            }
            case Key:
            {
                return 36_weight;
            }
            case Leaf:
            {
                return 1_weight;
            }
            case Legtie:
            {
                return 11_weight;
            }
            case Litch_Hand:
            {
                return 37_weight;
            }
            case Lizard:
            {
                return 43_weight;
            }
            case LockPicks:
            {
                return 33_weight;
            }
            case Lyre:
            {
                return 54_weight;
            }
            case Magnifying_Glass:
            {
                return 48_weight;
            }
            case Mask:
            {
                return 57_weight;
            }
            case Mirror:
            {
                return 47_weight;
            }
            case Mummy_Hand:
            {
                return 38_weight;
            }
            case Necklace:
            {
                return 21_weight;
            }
            case Noose:
            {
                return 29_weight;
            }
            case Nose:
            {
                return 13_weight;
            }
            case Orb:
            {
                return 87_weight;
            }
            case Paw:
            {
                return 10_weight;
            }
            case Pendant:
            {
                return 26_weight;
            }
            case Petrified_Snake:
            {
                return 32_weight;
            }
            case Pin_Crown:
            case Pin_Feather:
            case Pin_Fist:
            case Pin_Hourglass:
            case Pin_Key:
            case Pin_Mask:
            {
                return 16_weight;
            }
            case Pipe_And_Tabor:
            {
                return 127_weight;
            }
            case Potion:
            {
                return 95_weight;
            }
            case Puppet_Blessed:
            case Puppet_Cursed:
            {
                return 38_weight;
            }
            case Rabbit_Foot:
            {
                return 22_weight;
            }
            case Rainmaker:
            {
                return 43_weight;
            }
            case Rat_Juju:
            {
                return 25_weight;
            }
            case Rattlesnake_Tail:
            {
                return 11_weight;
            }
            case Recorder:
            {
                return 36_weight;
            }
            case Relic:
            {
                return 57_weight;
            }
            case Ring:
            {
                return 12_weight;
            }
            case Robe:
            {
                return 159_weight;
            }
            case Salamander:
            {
                return 48_weight;
            }
            case Scales:
            {
                return 122_weight;
            }
            case Scepter:
            {
                return 48_weight;
            }
            case Scroll:
            {
                return 33_weight;
            }
            case Scythe:
            {
                return 413_weight;
            }
            case Seeds:
            {
                return 12_weight;
            }
            case Shard:
            {
                return 49_weight;
            }
            case Shark_Tooth_Necklace:
            {
                return 22_weight;
            }
            case Shroud:
            {
                return 166_weight;
            }
            case Signet_Crown:
            case Signet_Feather:
            case Signet_Fist:
            case Signet_Hourglass:
            case Signet_Key:
            case Signet_Mask:
            {
                return 35_weight;
            }
            case Skink:
            {
                return 45_weight;
            }
            case Spider_Eggs:
            {
                return 12_weight;
            }
            case Spyglass:
            {
                return 62_weight;
            }
            case Staff:
            {
                return 277_weight;
            }
            case Summoning_Statue:
            {
                return 38_weight;
            }
            case Talisman:
            {
                return 32_weight;
            }
            case Tome:
            {
                return 70_weight;
            }
            case Tongue:
            {
                return 34_weight;
            }
            case Tooth:
            {
                return 5_weight;
            }
            case Tooth_Necklace:
            {
                return 22_weight;
            }
            case Troll_Figure:
            {
                return 39_weight;
            }
            case Trophy:
            {
                return 137_weight;
            }
            case Tuning_Fork:
            {
                return 18_weight;
            }
            case Turtle_Shell:
            {
                return 59_weight;
            }
            case Unicorn_Horn:
            {
                return 28_weight;
            }
            case Veil:
            {
                return 39_weight;
            }
            case Viol:
            {
                return 233_weight;
            }
            case Wand:
            {
                return 15_weight;
            }
            case Warhorse_Marionette:
            {
                return 433_weight;
            }
            case Weasel_Totem:
            {
                return 43_weight;
            }
            case Wolfen_Fur:
            {
                return 91_weight;
            }
            case NotMisc:
            case Count:
            default:
            {
                return 0_weight;
            }
        }
    }

    bool misc_type::IsUseable(const misc_type::Enum MISC_TYPE)
    {
        return (
            IsSummoning(MISC_TYPE) || (MISC_TYPE == Bell) || (MISC_TYPE == Bone_Whistle)
            || (MISC_TYPE == Chimes) || (MISC_TYPE == Gong) || (MISC_TYPE == Key)
            || (MISC_TYPE == Magnifying_Glass) || (MISC_TYPE == Potion) || (MISC_TYPE == Rainmaker)
            || (MISC_TYPE == Chains));
    }

    bool misc_type::IsWearable(const Enum MISC_TYPE)
    {
        switch (MISC_TYPE)
        {
            case Amulet:
            case Ankh_Necklace:
            case Armband:
            case Bracelet_Crown:
            case Bracelet_Feather:
            case Bracelet_Fist:
            case Bracelet_Hourglass:
            case Bracelet_Key:
            case Bracelet_Mask:
            case Brooch_Crown:
            case Brooch_Feather:
            case Brooch_Fist:
            case Brooch_Hourglass:
            case Brooch_Key:
            case Brooch_Mask:
            case CameoPin:
            case Cape:
            case Charm_Crown:
            case Charm_Feather:
            case Charm_Fist:
            case Charm_Hourglass:
            case Charm_Key:
            case Charm_Mask:
            case Cloak:
            case Fingerclaw:
            case Ghost_Sheet:
            case Headdress:
            case Legtie:
            case Mask:
            case Necklace:
            case Pendant:
            case Pin_Crown:
            case Pin_Feather:
            case Pin_Fist:
            case Pin_Hourglass:
            case Pin_Key:
            case Pin_Mask:
            case Ring:
            case Robe:
            case Shark_Tooth_Necklace:
            case Shroud:
            case Signet_Crown:
            case Signet_Feather:
            case Signet_Fist:
            case Signet_Hourglass:
            case Signet_Key:
            case Signet_Mask:
            case Tooth_Necklace:
            case Veil:

            {
                return true;
            }
            case Viol:
            case Wand:
            case Wolfen_Fur:
            case Troll_Figure:
            case Unicorn_Horn:
            case Staff:
            case Scythe:
            case Shard:
            case Scepter:
            case Pipe_And_Tabor:
            case Rabbit_Foot:
            case Recorder:
            case Hide:
            case Litch_Hand:
            case LockPicks:
            case Lyre:
            case Mummy_Hand:
            case Hurdy_Gurdy:
            case Icicle:
            case Conch:
            case Crumhorn:
            case Devil_Horn:
            case DrumLute:
            case Golem_Finger:
            case Angel_Braid:
            case Bag:
            case Beard:
            case Bell:
            case Bird_Claw:
            case Bone:
            case Bone_Whistle:
            case Book:
            case Bust:
            case Cat:
            case Chains:
            case Chimes:
            case Doll:
            case Doll_Blessed:
            case Doll_Cursed:
            case Dried_Head:
            case Embryo:
            case Egg:
            case Eye:
            case Feather:
            case Figurine_Blessed:
            case Figurine_Cursed:
            case Flag:
            case Frog:
            case Gecko:
            case Gizzard:
            case Goblet:
            case Gong:
            case Grave_Ornament:
            case Handbag:
            case Hanky:
            case Horn:
            case Horseshoe:
            case Iguana:
            case Imp_Tail:
            case Key:
            case Leaf:
            case Lizard:
            case Magnifying_Glass:
            case Mirror:
            case Noose:
            case Nose:
            case Orb:
            case Paw:
            case Petrified_Snake:
            case Potion:
            case Puppet_Blessed:
            case Puppet_Cursed:
            case Rainmaker:
            case Rat_Juju:
            case Rattlesnake_Tail:
            case Relic:
            case Salamander:
            case Scales:
            case Scroll:
            case Seeds:
            case Skink:
            case Spider_Eggs:
            case Spyglass:
            case Summoning_Statue:
            case Talisman:
            case Tome:
            case Tongue:
            case Tooth:
            case Trophy:
            case Tuning_Fork:
            case Turtle_Shell:
            case Warhorse_Marionette:
            case Weasel_Totem:
            case NotMisc:
            case Count:
            default:
            {
                return false;
            }
        }
    }

    bool misc_type::IsEquippable(const Enum MISC_TYPE)
    {
        if (IsMusicalInstrument(MISC_TYPE) || IsWearable(MISC_TYPE))
        {
            return true;
        }
        else
        {
            return (
                (MISC_TYPE == Angel_Braid) || (MISC_TYPE == Devil_Horn)
                || (MISC_TYPE == Ghost_Sheet) || (MISC_TYPE == Icicle) || (MISC_TYPE == Mummy_Hand)
                || (MISC_TYPE == Litch_Hand) || (MISC_TYPE == Scepter) || (MISC_TYPE == Scythe)
                || (MISC_TYPE == Shard) || (MISC_TYPE == Staff) || (MISC_TYPE == Unicorn_Horn)
                || (MISC_TYPE == Wand));
        }
    }

    bool misc_type::IsQuestItem(const Enum MISC_TYPE)
    {
        if (IsMusicalInstrument(MISC_TYPE) && (MISC_TYPE != DrumLute))
        {
            return true;
        }
        else
        {
            return (
                (MISC_TYPE == Petrified_Snake) || (MISC_TYPE == Mummy_Hand) || (MISC_TYPE == Icicle)
                || (MISC_TYPE == Unicorn_Horn) || (MISC_TYPE == Devil_Horn)
                || (MISC_TYPE == Angel_Braid));
        }
    }

    bool misc_type::AllowsCasting(const Enum MISC_TYPE)
    {
        return (
            IsBlessed(MISC_TYPE) || IsCursed(MISC_TYPE) || (MISC_TYPE == Petrified_Snake)
            || (MISC_TYPE == Mummy_Hand) || (MISC_TYPE == Wand) || (MISC_TYPE == Shard)
            || (MISC_TYPE == Icicle) || (MISC_TYPE == Devil_Horn) || (MISC_TYPE == Orb)
            || (MISC_TYPE == Scepter) || (MISC_TYPE == Unicorn_Horn) || (MISC_TYPE == Litch_Hand));
    }

    bool misc_type::IsWeapon(const Enum MISC_TYPE)
    {
        return (MISC_TYPE == Scythe) || (MISC_TYPE == Staff);
    }

    bool misc_type::IsArmor(const Enum MISC_TYPE)
    {
        return (MISC_TYPE == Cape) || (MISC_TYPE == Cloak) || (MISC_TYPE == Ghost_Sheet)
            || (MISC_TYPE == Robe) || (MISC_TYPE == Shroud);
    }

    const ElementEnumVec_t misc_type::ElementTypes(const Enum, const bool WILL_INCLUDE_NONE)
    {
        ElementEnumVec_t elementTypes;

        // currently there are no elemental misc types but that will change when unique_type is
        // eliminated and it's values are brought into misc_type

        if (WILL_INCLUDE_NONE)
        {
            elementTypes.emplace_back(element_type::None);
        }

        return elementTypes;
    }

    const std::string set_type::ToString(const set_type::Enum E)
    {
        switch (E)
        {
            case NotASet:
            {
                return "NotASet";
            }
            case TheAssassins:
            {
                return "TheAssassins";
            }
            case TheNeverwinter:
            {
                return "TheNeverwinter";
            }
            case TheTickler:
            {
                return "TheTickler";
            }
            case TheMagus:
            {
                return "TheMagus";
            }
            case TheNecromancers:
            {
                return "TheNecromancers";
            }
            case TheWarlocks:
            {
                return "TheWarlocks";
            }
            case TheLichKings:
            {
                return "TheLichKings";
            }
            case TheSages:
            {
                return "TheSages";
            }
            case TheShamans:
            {
                return "TheShamans";
            }
            case TheOracles:
            {
                return "TheOracles";
            }
            case TheAngelic:
            {
                return "TheAngelic";
            }
            case TheBalladeers:
            {
                return "TheBalladeers";
            }
            case TheTroubadours:
            {
                return "TheTroubadours";
            }
            case TheMuses:
            {
                return "TheMuses";
            }
            case TheCavaliers:
            {
                return "TheCavaliers";
            }
            case TheChampions:
            {
                return "TheChampions";
            }
            case ThePaladins:
            {
                return "ThePaladins";
            }
            case TheBerserkers:
            {
                return "TheBerserkers";
            }
            case TheRosewood:
            {
                return "TheRosewood";
            }
            case TheDragonslayers:
            {
                return "TheDragonslayers";
            }
            case TheEventideRider:
            {
                return "TheEventideRider";
            }
            case TheHunters:
            {
                return "TheHunters";
            }
            case TheSureShot:
            {
                return "TheSureShot";
            }
            case TheMarksmans:
            {
                return "TheMarksmans";
            }
            case TheDeadeye:
            {
                return "TheDeadeye";
            }
            case TheDuskRanger:
            {
                return "TheDuskRanger";
            }
            case TheVenomBow:
            {
                return "TheVenomBow";
            }
            case TheCritterChannelers:
            {
                return "TheCritterChannelers";
            }
            case TheMammalianHead:
            {
                return "TheMammalianHead";
            }
            case TheSavageTaskmasters:
            {
                return "TheSavageTaskmasters";
            }
            case TheMonsterOverseers:
            {
                return "TheMonsterOverseers";
            }
            case TheBeastRulers:
            {
                return "TheBeastRulers";
            }
            case Count:
            default:
            {
                std::ostringstream ssErr;
                ssErr << "item::set_type::ToString(" << E << ")_InvalidValueError";
                throw std::range_error(ssErr.str());
            }
        }
    }

    const std::string set_type::Name(const set_type::Enum E)
    {
        switch (E)
        {
            case NotASet:
            {
                return "NotASet";
            }
            case TheAssassins:
            {
                return "The Assassin's";
            }
            case TheNeverwinter:
            {
                return "The Neverwinter";
            }
            case TheTickler:
            {
                return "The Tickler's";
            }
            case TheMagus:
            {
                return "The Magus'";
            }
            case TheNecromancers:
            {
                return "The Necromancer's";
            }
            case TheWarlocks:
            {
                return "The Warlock's";
            }
            case TheLichKings:
            {
                return "The Lich King's";
            }
            case TheSages:
            {
                return "The Sage's";
            }
            case TheShamans:
            {
                return "The Shaman's";
            }
            case TheOracles:
            {
                return "The Oracle's";
            }
            case TheAngelic:
            {
                return "The Angelic";
            }
            case TheBalladeers:
            {
                return "The Balladeer's";
            }
            case TheTroubadours:
            {
                return "The Troubadour's";
            }
            case TheMuses:
            {
                return "The Muses'";
            }
            case TheCavaliers:
            {
                return "The Cavalier's";
            }
            case TheChampions:
            {
                return "The Champion's";
            }
            case ThePaladins:
            {
                return "The Paladin's";
            }
            case TheBerserkers:
            {
                return "The Berserker's";
            }
            case TheRosewood:
            {
                return "The Rosewood";
            }
            case TheDragonslayers:
            {
                return "The Dragonslayer's";
            }
            case TheEventideRider:
            {
                return "The Eventide Rider's";
            }
            case TheHunters:
            {
                return "The Hunter's";
            }
            case TheSureShot:
            {
                return "The Sure Shot's";
            }
            case TheMarksmans:
            {
                return "The Marksman's";
            }
            case TheDeadeye:
            {
                return "The Deadeye's";
            }
            case TheDuskRanger:
            {
                return "The Dusk Ranger's";
            }
            case TheVenomBow:
            {
                return "The Venom Bow";
            }
            case TheCritterChannelers:
            {
                return "The Critter Channeler's";
            }
            case TheMammalianHead:
            {
                return "The Mammalian Head's";
            }
            case TheSavageTaskmasters:
            {
                return "The Savage Taskmaster's";
            }
            case TheMonsterOverseers:
            {
                return "The Monster Overseer's";
            }
            case TheBeastRulers:
            {
                return "The Beast Ruler's";
            }
            case Count:
            default:
            {
                std::ostringstream ssErr;
                ssErr << "item::set_type::Name(" << E << ")_InvalidValueError";
                throw std::range_error(ssErr.str());
            }
        }
    }

    creature::role::Enum set_type::Role(const set_type::Enum E)
    {
        switch (E)
        {
            case NotASet:
            {
                return creature::role::Count;
            }
            case TheAssassins:
            case TheTickler:
            case TheNeverwinter:
            {
                return creature::role::Thief;
            }
            case TheMagus:
            case TheNecromancers:
            case TheWarlocks:
            case TheLichKings:
            {
                return creature::role::Sorcerer;
            }
            case TheSages:
            case TheShamans:
            case TheOracles:
            case TheAngelic:
            {
                return creature::role::Cleric;
            }
            case TheBalladeers:
            case TheTroubadours:
            case TheMuses:
            {
                return creature::role::Bard;
            }
            case TheCavaliers:
            case TheChampions:
            case ThePaladins:
            case TheBerserkers:
            case TheRosewood:
            case TheDragonslayers:
            case TheEventideRider:
            {
                return creature::role::Knight;
            }
            case TheHunters:
            case TheSureShot:
            case TheMarksmans:
            case TheDeadeye:
            case TheDuskRanger:
            case TheVenomBow:
            {
                return creature::role::Archer;
            }
            case TheCritterChannelers:
            case TheMammalianHead:
            case TheSavageTaskmasters:
            case TheMonsterOverseers:
            case TheBeastRulers:
            {
                return creature::role::Beastmaster;
            }
            case Count:
            default:
            {
                std::ostringstream ssErr;
                ssErr << "item::set_type::Role(" << E << ")_InvalidValueError";
                throw std::range_error(ssErr.str());
            }
        }
    }

    const std::string unique_type::ToString(const Enum E)
    {
        switch (E)
        {
            case NotUnique:
            {
                return "NotUnqiue";
            }
            case BasiliskTonge:
            {
                return "BasiliskTonge";
            }
            case BerserkersBeard:
            {
                return "BerserkersBeard";
            }
            case BishopsHanky:
            {
                return "BishopsHanky";
            }
            case BleedingTrophy:
            {
                return "BleedingTrophy";
            }
            case BloodyDragonScale:
            {
                return "BloodyDragonScale";
            }
            case BottleOfBansheeScreams:
            {
                return "BottleOfBansheeScreams";
            }
            case BraceletCrown:
            {
                return "BraceletCrown";
            }
            case BraceletFeather:
            {
                return "BraceletFeather";
            }
            case BraceletFist:
            {
                return "BraceletFist";
            }
            case BraceletHourglass:
            {
                return "BraceletHourglass";
            }
            case BraceletKey:
            {
                return "BraceletKey";
            }
            case BraceletMask:
            {
                return "BraceletMask";
            }
            case BroochCrown:
            {
                return "BroochCrown";
            }
            case BroochFeather:
            {
                return "BroochFeather";
            }
            case BroochFist:
            {
                return "BroochFist";
            }
            case BroochHourglass:
            {
                return "BroochHourglass";
            }
            case BroochKey:
            {
                return "BroochKey";
            }
            case BroochMask:
            {
                return "BroochMask";
            }
            case BurialShroud:
            {
                return "BurialShroud";
            }
            case CharmCrown:
            {
                return "CharmCrown";
            }
            case CharmFeather:
            {
                return "CharmFeather";
            }
            case CharmFist:
            {
                return "CharmFist";
            }
            case CharmHourglass:
            {
                return "CharmHourglass";
            }
            case CharmKey:
            {
                return "CharmKey";
            }
            case CharmMask:
            {
                return "CharmMask";
            }
            case ChimeraBone:
            {
                return "ChimeraBone";
            }
            case CobraTooth:
            {
                return "CobraTooth";
            }
            case CommandersCape:
            {
                return "CommandersCape";
            }
            case BronzeTroll:
            {
                return "BronzeTroll";
            }
            case GlassCat:
            {
                return "GlassCat";
            }
            case CrystalChimes:
            {
                return "CrystalChimes";
            }
            case CyclopsEye:
            {
                return "CyclopsEye";
            }
            case DemonDiary:
            {
                return "DemonDiary";
            }
            case DoveBloodVial:
            {
                return "DoveBloodVial";
            }
            case DragonToothWhistle:
            {
                return "DragonToothWhistle";
            }
            case DriedFrog:
            {
                return "DriedFrog";
            }
            case DriedGecko:
            {
                return "DriedGecko";
            }
            case DriedIguana:
            {
                return "DriedIguana";
            }
            case DriedLizard:
            {
                return "DriedLizard";
            }
            case DriedSalamander:
            {
                return "DriedSalamander";
            }
            case DriedSkink:
            {
                return "DriedSkink";
            }
            case DriedToad:
            {
                return "DriedToad";
            }
            case DriedTurtle:
            {
                return "DriedTurtle";
            }
            case DruidLeaf:
            {
                return "DruidLeaf";
            }
            case EvilRabbitsFoot:
            {
                return "EvilRabbitsFoot";
            }
            case ExoticGoldenGong:
            {
                return "ExoticGoldenGong";
            }
            case FanaticsFlag:
            {
                return "FanaticsFlag";
            }
            case FriarsChronicle:
            {
                return "FriarsChronicle";
            }
            case FuneralRecord:
            {
                return "FuneralRecord";
            }
            case GeneralsCape:
            {
                return "GeneralsCape";
            }
            case GhostSheet:
            {
                return "GhostSheet";
            }
            case GiantOwlEye:
            {
                return "GiantOwlEye";
            }
            case GriffinFeather:
            {
                return "GriffinFeather";
            }
            case HangmansNoose:
            {
                return "HangmansNoose";
            }
            case HawkEye:
            {
                return "HawkEye";
            }
            case HobgoblinNose:
            {
                return "HobgoblinNose";
            }
            case HoboRing:
            {
                return "HoboRing";
            }
            case HolyEpic:
            {
                return "HolyEpic";
            }
            case HornOfTheHorde:
            {
                return "HornOfTheHorde";
            }
            case ImpTail:
            {
                return "ImpTail";
            }
            case IslanderHeaddress:
            {
                return "IslanderHeaddress";
            }
            case JeweledAnkhNecklace:
            {
                return "JeweledAnkhNecklace";
            }
            case JeweledArmband:
            {
                return "JeweledArmband";
            }
            case JeweledHandbag:
            {
                return "JeweledHandbag";
            }
            case JeweledPrincessVeil:
            {
                return "JeweledPrincessVeil";
            }
            case KingsCape:
            {
                return "KingsCape";
            }
            case LastRitesScroll:
            {
                return "LastRitesScroll";
            }
            case MacabreManuscript:
            {
                return "MacabreManuscript";
            }
            case MadRatJuju:
            {
                return "MadRatJuju";
            }
            case MagicHorseshoe:
            {
                return "MagicHorseshoe";
            }
            case MagnifyingGlass:
            {
                return "MagnifyingGlass";
            }
            case ManaAmulet:
            {
                return "ManaAmulet";
            }
            case MendicantRing:
            {
                return "MendicantRing";
            }
            case MinotaurHide:
            {
                return "MinotaurHide";
            }
            case MonkRing:
            {
                return "MonkRing";
            }
            case MortuaryOrnament:
            {
                return "MortuaryOrnament";
            }
            case MournersMask:
            {
                return "MournersMask";
            }
            case PantherPaw:
            {
                return "PantherPaw";
            }
            case PinCrown:
            {
                return "PinCrown";
            }
            case PinFeather:
            {
                return "PinFeather";
            }
            case PinFist:
            {
                return "PinFist";
            }
            case PinHourglass:
            {
                return "PinHourglass";
            }
            case PinKey:
            {
                return "PinKey";
            }
            case PinMask:
            {
                return "PinMask";
            }
            case PixieBell:
            {
                return "PixieBell";
            }
            case PriestRing:
            {
                return "PriestRing";
            }
            case RascalMask:
            {
                return "RascalMask";
            }
            case RattlesnakeTail:
            {
                return "RattlesnakeTail";
            }
            case RavenClaw:
            {
                return "RavenClaw";
            }
            case RazorFingerclaw:
            {
                return "RazorFingerclaw";
            }
            case ReaperScythe:
            {
                return "ReaperScythe";
            }
            case RegalCaptainsFlag:
            {
                return "RegalCaptainsFlag";
            }
            case RequiemRegister:
            {
                return "RequiemRegister";
            }
            case RoyalScoutSpyglass:
            {
                return "RoyalScoutSpyglass";
            }
            case SaintCameoPin:
            {
                return "SaintCameoPin";
            }
            case SaintsJournal:
            {
                return "SaintsJournal";
            }
            case SanguineRelic:
            {
                return "SanguineRelic";
            }
            case ScorpionStingerFingerclaw:
            {
                return "ScorpionStingerFingerclaw";
            }
            case ScoundrelSack:
            {
                return "ScoundrelSack";
            }
            case SepultureDecoration:
            {
                return "SepultureDecoration";
            }
            case ShadeCloak:
            {
                return "ShadeCloak";
            }
            case ShamanRainmaker:
            {
                return "ShamanRainmaker";
            }
            case SharkToothNecklace:
            {
                return "SharkToothNecklace";
            }
            case SignetCrown:
            {
                return "SignetCrown";
            }
            case SignetFeather:
            {
                return "SignetFeather";
            }
            case SignetFist:
            {
                return "SignetFist";
            }
            case SignetHourglass:
            {
                return "SignetHourglass";
            }
            case SignetKey:
            {
                return "SignetKey";
            }
            case SignetMask:
            {
                return "SignetMask";
            }
            case SirenConch:
            {
                return "SirenConch";
            }
            case SpecterChains:
            {
                return "SpecterChains";
            }
            case SpecterRobe:
            {
                return "SpecterRobe";
            }
            case SprintersLegtie:
            {
                return "SprintersLegtie";
            }
            case SwindlersBag:
            {
                return "SwindlersBag";
            }
            case TribalFlag:
            {
                return "TribalFlag";
            }
            case TricksterPouch:
            {
                return "TricksterPouch";
            }
            case TuningFork:
            {
                return "TuningFork";
            }
            case TurtleShell:
            {
                return "TurtleShell";
            }
            case VampiresToothNecklace:
            {
                return "VampiresToothNecklace";
            }
            case ViperFangFingerclaw:
            {
                return "ViperFangFingerclaw";
            }
            case VultureGizzard:
            {
                return "VultureGizzard";
            }
            case WarhorseMarionette:
            {
                return "WarhorseMarionette";
            }
            case WarTrumpet:
            {
                return "WarTrumpet";
            }
            case WeaselTotem:
            {
                return "WeaselTotem";
            }
            case WolfenFur:
            {
                return "WolfenFur";
            }
            case WraithTalisman:
            {
                return "WraithTalisman";
            }
            case Count:
            default:
            {
                std::ostringstream ssErr;
                ssErr << "item::unique_type::ToString(" << E << ")_InvalidValueError";
                throw std::range_error(ssErr.str());
            }
        }
    }

    const std::string unique_type::Name(const Enum E)
    {
        switch (E)
        {
            case NotUnique:
            {
                return "NotUnqiue";
            }
            case BasiliskTonge:
            {
                return "Basilisk Tonge";
            }
            case BerserkersBeard:
            {
                return "Berserker's Beard";
            }
            case BishopsHanky:
            {
                return "Bishop's Hanky";
            }
            case BleedingTrophy:
            {
                return "Bleeding Trophy";
            }
            case BloodyDragonScale:
            {
                return "Bloody Dragon Scale";
            }
            case BottleOfBansheeScreams:
            {
                return "Bottle Of Banshee Screams";
            }
            case BraceletCrown:
            {
                return "Crown Emblem Bracelet";
            }
            case BraceletFeather:
            {
                return "Feather Emblem Bracelet";
            }
            case BraceletFist:
            {
                return "Fist Emblem Bracelet";
            }
            case BraceletHourglass:
            {
                return "Hourglass Emblem Bracelet";
            }
            case BraceletKey:
            {
                return "Key Emblem Bracelet";
            }
            case BraceletMask:
            {
                return "Mask Emblem Bracelet";
            }
            case BroochCrown:
            {
                return "Crown Emblem Brooch";
            }
            case BroochFeather:
            {
                return "Feather Emblem Brooch";
            }
            case BroochFist:
            {
                return "Fist Emblem Brooch";
            }
            case BroochHourglass:
            {
                return "Hourglass Emblem Brooch";
            }
            case BroochKey:
            {
                return "Key Emblem Brooch";
            }
            case BroochMask:
            {
                return "Mask Emblem Brooch";
            }
            case BurialShroud:
            {
                return "Burial Shroud";
            }
            case CharmCrown:
            {
                return "Crown Charm";
            }
            case CharmFeather:
            {
                return "Feather Charm";
            }
            case CharmFist:
            {
                return "Fist Charm";
            }
            case CharmHourglass:
            {
                return "Hourglass Charm";
            }
            case CharmKey:
            {
                return "Key Charm";
            }
            case CharmMask:
            {
                return "Mask Charm";
            }
            case ChimeraBone:
            {
                return "Chimera Bone";
            }
            case CobraTooth:
            {
                return "Cobra Tooth";
            }
            case CommandersCape:
            {
                return "Commander's Cape";
            }
            case BronzeTroll:
            {
                return "Bronze Troll";
            }
            case GlassCat:
            {
                return "Crystal Cat";
            }
            case CrystalChimes:
            {
                return "Crystal Chimes";
            }
            case CyclopsEye:
            {
                return "Cyclops Eye";
            }
            case DemonDiary:
            {
                return "Demon Diary";
            }
            case DoveBloodVial:
            {
                return "Dove Blood Vial";
            }
            case DragonToothWhistle:
            {
                return "Dragon Tooth Whistle";
            }
            case DriedFrog:
            {
                return "Dried Frog";
            }
            case DriedGecko:
            {
                return "Dried Gecko";
            }
            case DriedIguana:
            {
                return "Dried Iguana";
            }
            case DriedLizard:
            {
                return "Dried Lizard";
            }
            case DriedSalamander:
            {
                return "Dried Salamander";
            }
            case DriedSkink:
            {
                return "Dried Skink";
            }
            case DriedToad:
            {
                return "Dried Toad";
            }
            case DriedTurtle:
            {
                return "Dried Turtle";
            }
            case DruidLeaf:
            {
                return "Druid Leaf";
            }
            case EvilRabbitsFoot:
            {
                return "Evil Rabbit's Foot";
            }
            case ExoticGoldenGong:
            {
                return "Exotic Golden Gong";
            }
            case FanaticsFlag:
            {
                return "Fanatic's Flag";
            }
            case FriarsChronicle:
            {
                return "Friar's Chronicle";
            }
            case FuneralRecord:
            {
                return "Funeral Record";
            }
            case GeneralsCape:
            {
                return "General's Cape";
            }
            case GhostSheet:
            {
                return "Ghost Sheet";
            }
            case GiantOwlEye:
            {
                return "Giant Owl Eye";
            }
            case GriffinFeather:
            {
                return "Griffin Feather";
            }
            case HangmansNoose:
            {
                return "Hangman's Noose";
            }
            case HawkEye:
            {
                return "Hawk Eye";
            }
            case HobgoblinNose:
            {
                return "Hobgoblin Nose";
            }
            case HoboRing:
            {
                return "Hobo Ring";
            }
            case HolyEpic:
            {
                return "Holy Epic";
            }
            case HornOfTheHorde:
            {
                return "Horn Of The Horde";
            }
            case ImpTail:
            {
                return "Imp Tail";
            }
            case IslanderHeaddress:
            {
                return "Islander Headdress";
            }
            case JeweledAnkhNecklace:
            {
                return "Jeweled Ankh Necklace";
            }
            case JeweledArmband:
            {
                return "Jeweled Armband";
            }
            case JeweledHandbag:
            {
                return "Jeweled Handbag";
            }
            case JeweledPrincessVeil:
            {
                return "Jeweled Princess Veil";
            }
            case KingsCape:
            {
                return "King's Cape";
            }
            case LastRitesScroll:
            {
                return "Last Rites Scroll";
            }
            case MacabreManuscript:
            {
                return "Macabre Manuscript";
            }
            case MadRatJuju:
            {
                return "Mad Rat Juju";
            }
            case MagicHorseshoe:
            {
                return "Magic Horseshoe";
            }
            case MagnifyingGlass:
            {
                return "Magnifying Glass";
            }
            case ManaAmulet:
            {
                return "Mana Amulet";
            }
            case MendicantRing:
            {
                return "Mendicant Ring";
            }
            case MinotaurHide:
            {
                return "Minotaur Hide";
            }
            case MonkRing:
            {
                return "Monk Ring";
            }
            case MortuaryOrnament:
            {
                return "Mortuary Ornament";
            }
            case MournersMask:
            {
                return "Mourner's Mask";
            }
            case PantherPaw:
            {
                return "Panther Paw";
            }
            case PinCrown:
            {
                return "Crown Emblem Pin";
            }
            case PinFeather:
            {
                return "Feather Emblem Pin";
            }
            case PinFist:
            {
                return "Fist Emblem Pin";
            }
            case PinHourglass:
            {
                return "Hourglass Emblem Pin";
            }
            case PinKey:
            {
                return "Key Emblem Pin";
            }
            case PinMask:
            {
                return "Mask Emblem Pin";
            }
            case PixieBell:
            {
                return "Pixie Bell";
            }
            case PriestRing:
            {
                return "Priest Ring";
            }
            case RascalMask:
            {
                return "Rascal Mask";
            }
            case RattlesnakeTail:
            {
                return "Rattlesnake Tail";
            }
            case RavenClaw:
            {
                return "Raven Claw";
            }
            case RazorFingerclaw:
            {
                return "Razor Fingerclaw";
            }
            case ReaperScythe:
            {
                return "Reaper Scythe";
            }
            case RegalCaptainsFlag:
            {
                return "Regal Captain's Flag";
            }
            case RequiemRegister:
            {
                return "Requiem Register";
            }
            case RoyalScoutSpyglass:
            {
                return "Royal Scout Spyglass";
            }
            case SaintCameoPin:
            {
                return "Saint Cameo Pin";
            }
            case SaintsJournal:
            {
                return "Saint's Journal";
            }
            case SanguineRelic:
            {
                return "Sanguine Relic";
            }
            case ScorpionStingerFingerclaw:
            {
                return "Scorpion Stinger Fingerclaw";
            }
            case ScoundrelSack:
            {
                return "Scoundrel Sack";
            }
            case SepultureDecoration:
            {
                return "Sepulture Decoration";
            }
            case ShadeCloak:
            {
                return "Shade Cloak";
            }
            case ShamanRainmaker:
            {
                return "Shaman Rainmaker";
            }
            case SharkToothNecklace:
            {
                return "Shark Tooth Necklace";
            }
            case SignetCrown:
            {
                return "Crown Signet";
            }
            case SignetFeather:
            {
                return "Feather Signet";
            }
            case SignetFist:
            {
                return "Fist Signet";
            }
            case SignetHourglass:
            {
                return "Hourglass Signet";
            }
            case SignetKey:
            {
                return "Key Signet";
            }
            case SignetMask:
            {
                return "Mask Signet";
            }
            case SirenConch:
            {
                return "Siren Conch";
            }
            case SpecterChains:
            {
                return "Specter Chains";
            }
            case SpecterRobe:
            {
                return "Specter Robe";
            }
            case SprintersLegtie:
            {
                return "Sprinter's Legtie";
            }
            case SwindlersBag:
            {
                return "Swindler's Bag";
            }
            case TribalFlag:
            {
                return "Tribal Flag";
            }
            case TricksterPouch:
            {
                return "Trickster Pouch";
            }
            case TuningFork:
            {
                return "Tuning Fork";
            }
            case TurtleShell:
            {
                return "Turtle Shell";
            }
            case VampiresToothNecklace:
            {
                return "Vampire's Tooth Necklace";
            }
            case ViperFangFingerclaw:
            {
                return "Viper Fang Fingerclaw";
            }
            case VultureGizzard:
            {
                return "Vulture Gizzard";
            }
            case WarhorseMarionette:
            {
                return "Warhorse Marionette";
            }
            case WarTrumpet:
            {
                return "War Trumpet";
            }
            case WeaselTotem:
            {
                return "Weasel Totem";
            }
            case WolfenFur:
            {
                return "Wolfen Fur";
            }
            case WraithTalisman:
            {
                return "Wraith Talisman";
            }
            case Count:
            default:
            {
                std::ostringstream ssErr;
                ssErr << "item::unique_type::Name(" << E << ")_InvalidValueError";
                throw std::range_error(ssErr.str());
            }
        }
    }

    misc_type::Enum unique_type::MiscType(const Enum E)
    {
        switch (E)
        {
            case NotUnique:
            {
                return misc_type::NotMisc;
            }
            case BasiliskTonge:
            {
                return misc_type::Tongue;
            }
            case BerserkersBeard:
            {
                return misc_type::Beard;
            }
            case BishopsHanky:
            {
                return misc_type::Hanky;
            }
            case BleedingTrophy:
            {
                return misc_type::Trophy;
            }
            case BloodyDragonScale:
            {
                return misc_type::Scales;
            }
            case BottleOfBansheeScreams:
            {
                return misc_type::Potion;
            }
            case BraceletCrown:
            {
                return misc_type::Bracelet_Crown;
            }
            case BraceletFeather:
            {
                return misc_type::Bracelet_Feather;
            }
            case BraceletFist:
            {
                return misc_type::Bracelet_Fist;
            }
            case BraceletHourglass:
            {
                return misc_type::Bracelet_Hourglass;
            }
            case BraceletKey:
            {
                return misc_type::Bracelet_Key;
            }
            case BraceletMask:
            {
                return misc_type::Bracelet_Mask;
            }
            case BroochCrown:
            {
                return misc_type::Brooch_Crown;
            }
            case BroochFeather:
            {
                return misc_type::Brooch_Feather;
            }
            case BroochFist:
            {
                return misc_type::Brooch_Fist;
            }
            case BroochHourglass:
            {
                return misc_type::Brooch_Hourglass;
            }
            case BroochKey:
            {
                return misc_type::Brooch_Key;
            }
            case BroochMask:
            {
                return misc_type::Brooch_Mask;
            }
            case BurialShroud:
            {
                return misc_type::Shroud;
            }
            case CharmCrown:
            {
                return misc_type::Charm_Crown;
            }
            case CharmFeather:
            {
                return misc_type::Charm_Feather;
            }
            case CharmFist:
            {
                return misc_type::Charm_Fist;
            }
            case CharmHourglass:
            {
                return misc_type::Charm_Hourglass;
            }
            case CharmKey:
            {
                return misc_type::Charm_Key;
            }
            case CharmMask:
            {
                return misc_type::Charm_Mask;
            }
            case ChimeraBone:
            {
                return misc_type::Bone;
            }
            case CobraTooth:
            {
                return misc_type::Tooth;
            }
            case CommandersCape:
            {
                return misc_type::Cape;
            }
            case BronzeTroll:
            {
                return misc_type::Troll_Figure;
            }
            case GlassCat:
            {
                return misc_type::Cat;
            }
            case CrystalChimes:
            {
                return misc_type::Chimes;
            }
            case CyclopsEye:
            {
                return misc_type::Eye;
            }
            case DemonDiary:
            {
                return misc_type::Tome;
            }
            case DoveBloodVial:
            {
                return misc_type::Potion;
            }
            case DragonToothWhistle:
            {
                return misc_type::Bone_Whistle;
            }
            case DriedFrog:
            {
                return misc_type::Frog;
            }
            case DriedGecko:
            {
                return misc_type::Gecko;
            }
            case DriedIguana:
            {
                return misc_type::Iguana;
            }
            case DriedLizard:
            {
                return misc_type::Lizard;
            }
            case DriedSalamander:
            {
                return misc_type::Salamander;
            }
            case DriedSkink:
            {
                return misc_type::Skink;
            }
            case DriedToad:
            {
                return misc_type::Frog;
            }
            case DriedTurtle:
            {
                return misc_type::Turtle_Shell;
            }
            case DruidLeaf:
            {
                return misc_type::Leaf;
            }
            case EvilRabbitsFoot:
            {
                return misc_type::Rabbit_Foot;
            }
            case ExoticGoldenGong:
            {
                return misc_type::Gong;
            }
            case FanaticsFlag:
            {
                return misc_type::Flag;
            }
            case FriarsChronicle:
            {
                return misc_type::Book;
            }
            case FuneralRecord:
            {
                return misc_type::Scroll;
            }
            case GeneralsCape:
            {
                return misc_type::Cape;
            }
            case GhostSheet:
            {
                return misc_type::Ghost_Sheet;
            }
            case GiantOwlEye:
            {
                return misc_type::Eye;
            }
            case GriffinFeather:
            {
                return misc_type::Feather;
            }
            case HangmansNoose:
            {
                return misc_type::Noose;
            }
            case HawkEye:
            {
                return misc_type::Eye;
            }
            case HobgoblinNose:
            {
                return misc_type::Nose;
            }
            case HoboRing:
            {
                return misc_type::Ring;
            }
            case HolyEpic:
            {
                return misc_type::Book;
            }
            case HornOfTheHorde:
            {
                return misc_type::Horn;
            }
            case ImpTail:
            {
                return misc_type::Imp_Tail;
            }
            case IslanderHeaddress:
            {
                return misc_type::Headdress;
            }
            case JeweledAnkhNecklace:
            {
                return misc_type::Ankh_Necklace;
            }
            case JeweledArmband:
            {
                return misc_type::Armband;
            }
            case JeweledHandbag:
            {
                return misc_type::Handbag;
            }
            case JeweledPrincessVeil:
            {
                return misc_type::Veil;
            }
            case KingsCape:
            {
                return misc_type::Cape;
            }
            case LastRitesScroll:
            {
                return misc_type::Scroll;
            }
            case MacabreManuscript:
            {
                return misc_type::Book;
            }
            case MadRatJuju:
            {
                return misc_type::Rat_Juju;
            }
            case MagicHorseshoe:
            {
                return misc_type::Horseshoe;
            }
            case MagnifyingGlass:
            {
                return misc_type::Magnifying_Glass;
            }
            case ManaAmulet:
            {
                return misc_type::Amulet;
            }
            case MendicantRing:
            {
                return misc_type::Ring;
            }
            case MinotaurHide:
            {
                return misc_type::Hide;
            }
            case MonkRing:
            {
                return misc_type::Ring;
            }
            case MortuaryOrnament:
            {
                return misc_type::Grave_Ornament;
            }
            case MournersMask:
            {
                return misc_type::Mask;
            }
            case PantherPaw:
            {
                return misc_type::Paw;
            }
            case PinCrown:
            {
                return misc_type::Pin_Crown;
            }
            case PinFeather:
            {
                return misc_type::Pin_Feather;
            }
            case PinFist:
            {
                return misc_type::Pin_Fist;
            }
            case PinHourglass:
            {
                return misc_type::Pin_Hourglass;
            }
            case PinKey:
            {
                return misc_type::Pin_Key;
            }
            case PinMask:
            {
                return misc_type::Pin_Mask;
            }
            case PixieBell:
            {
                return misc_type::Bell;
            }
            case PriestRing:
            {
                return misc_type::Ring;
            }
            case RascalMask:
            {
                return misc_type::Mask;
            }
            case RattlesnakeTail:
            {
                return misc_type::Rattlesnake_Tail;
            }
            case RavenClaw:
            {
                return misc_type::Bird_Claw;
            }
            case RazorFingerclaw:
            {
                return misc_type::Fingerclaw;
            }
            case ReaperScythe:
            {
                return misc_type::Scythe;
            }
            case RegalCaptainsFlag:
            {
                return misc_type::Flag;
            }
            case RequiemRegister:
            {
                return misc_type::Book;
            }
            case RoyalScoutSpyglass:
            {
                return misc_type::Spyglass;
            }
            case SaintCameoPin:
            {
                return misc_type::CameoPin;
            }
            case SaintsJournal:
            {
                return misc_type::Book;
            }
            case SanguineRelic:
            {
                return misc_type::Relic;
            }
            case ScorpionStingerFingerclaw:
            {
                return misc_type::Fingerclaw;
            }
            case ScoundrelSack:
            {
                return misc_type::Bag;
            }
            case SepultureDecoration:
            {
                return misc_type::Grave_Ornament;
            }
            case ShadeCloak:
            {
                return misc_type::Cloak;
            }
            case ShamanRainmaker:
            {
                return misc_type::Rainmaker;
            }
            case SharkToothNecklace:
            {
                return misc_type::Shark_Tooth_Necklace;
            }
            case SignetCrown:
            {
                return misc_type::Signet_Crown;
            }
            case SignetFeather:
            {
                return misc_type::Signet_Feather;
            }
            case SignetFist:
            {
                return misc_type::Signet_Fist;
            }
            case SignetHourglass:
            {
                return misc_type::Signet_Hourglass;
            }
            case SignetKey:
            {
                return misc_type::Signet_Key;
            }
            case SignetMask:
            {
                return misc_type::Signet_Mask;
            }
            case SirenConch:
            {
                return misc_type::Conch;
            }
            case SpecterChains:
            {
                return misc_type::Chains;
            }
            case SpecterRobe:
            {
                return misc_type::Robe;
            }
            case SprintersLegtie:
            {
                return misc_type::Legtie;
            }
            case SwindlersBag:
            {
                return misc_type::Bag;
            }
            case TribalFlag:
            {
                return misc_type::Flag;
            }
            case TricksterPouch:
            {
                return misc_type::Bag;
            }
            case TuningFork:
            {
                return misc_type::Tuning_Fork;
            }
            case TurtleShell:
            {
                return misc_type::Turtle_Shell;
            }
            case VampiresToothNecklace:
            {
                return misc_type::Tooth_Necklace;
            }
            case ViperFangFingerclaw:
            {
                return misc_type::Fingerclaw;
            }
            case VultureGizzard:
            {
                return misc_type::Gizzard;
            }
            case WarhorseMarionette:
            {
                return misc_type::Warhorse_Marionette;
            }
            case WarTrumpet:
            {
                return misc_type::Horn;
            }
            case WeaselTotem:
            {
                return misc_type::Weasel_Totem;
            }
            case WolfenFur:
            {
                return misc_type::Wolfen_Fur;
            }
            case WraithTalisman:
            {
                return misc_type::Talisman;
            }
            case Count:
            default:
            {
                std::ostringstream ssErr;
                ssErr << "item::unique_type::MiscType(" << E << ")_InvalidValueError";
                throw std::range_error(ssErr.str());
            }
        }
    }

    const ElementEnumVec_t
        unique_type::ElementTypes(const unique_type::Enum UNIQUE_TYPE, const bool WILL_INCLUDE_NONE)
    {
        ElementEnumVec_t elementTypes;

        if ((UNIQUE_TYPE == CommandersCape) || (UNIQUE_TYPE == GeneralsCape)
            || (UNIQUE_TYPE == KingsCape) || (UNIQUE_TYPE == SaintCameoPin))
        {
            elementTypes = { element_type::Honor };
        }
        else if (UNIQUE_TYPE == JeweledPrincessVeil)
        {
            elementTypes = { element_type::Honor, element_type::Shadow };
        }
        else if (
            (UNIQUE_TYPE == unique_type::JeweledArmband)
            || (UNIQUE_TYPE == unique_type::ManaAmulet))
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

    bool unique_type::IsUseable(const unique_type::Enum E)
    {
        switch (E)
        {
            case CrystalChimes:
            case DoveBloodVial:
            case DragonToothWhistle:
            case DriedFrog:
            case DriedGecko:
            case DriedIguana:
            case DriedLizard:
            case DriedSalamander:
            case DriedSkink:
            case DriedToad:
            case DriedTurtle:
            case ExoticGoldenGong:
            case MagnifyingGlass:
            case PixieBell:
            case ShamanRainmaker:
            case SpecterChains:
            case VultureGizzard:
            case WarTrumpet:
            case WraithTalisman:
            {
                return true;
            }
            case NotUnique:
            case BasiliskTonge:
            case BerserkersBeard:
            case BishopsHanky:
            case BleedingTrophy:
            case BloodyDragonScale:
            case BottleOfBansheeScreams:
            case BraceletCrown:
            case BraceletFeather:
            case BraceletFist:
            case BraceletHourglass:
            case BraceletKey:
            case BraceletMask:
            case BroochCrown:
            case BroochFeather:
            case BroochFist:
            case BroochHourglass:
            case BroochKey:
            case BroochMask:
            case BurialShroud:
            case CharmCrown:
            case CharmFeather:
            case CharmFist:
            case CharmHourglass:
            case CharmKey:
            case CharmMask:
            case ChimeraBone:
            case CobraTooth:
            case CommandersCape:
            case BronzeTroll:
            case GlassCat:
            case CyclopsEye:
            case DemonDiary:
            case DruidLeaf:
            case EvilRabbitsFoot:
            case FanaticsFlag:
            case FriarsChronicle:
            case FuneralRecord:
            case GeneralsCape:
            case GhostSheet:
            case GiantOwlEye:
            case GriffinFeather:
            case HangmansNoose:
            case HawkEye:
            case HobgoblinNose:
            case HoboRing:
            case HolyEpic:
            case HornOfTheHorde:
            case ImpTail:
            case IslanderHeaddress:
            case JeweledAnkhNecklace:
            case JeweledArmband:
            case JeweledHandbag:
            case JeweledPrincessVeil:
            case KingsCape:
            case LastRitesScroll:
            case MacabreManuscript:
            case MadRatJuju:
            case MagicHorseshoe:
            case ManaAmulet:
            case MendicantRing:
            case MinotaurHide:
            case MonkRing:
            case MortuaryOrnament:
            case MournersMask:
            case PantherPaw:
            case PinCrown:
            case PinFeather:
            case PinFist:
            case PinHourglass:
            case PinKey:
            case PinMask:
            case PriestRing:
            case RascalMask:
            case RattlesnakeTail:
            case RavenClaw:
            case RazorFingerclaw:
            case ReaperScythe:
            case RegalCaptainsFlag:
            case RequiemRegister:
            case RoyalScoutSpyglass:
            case SaintCameoPin:
            case SaintsJournal:
            case SanguineRelic:
            case ScorpionStingerFingerclaw:
            case ScoundrelSack:
            case SepultureDecoration:
            case ShadeCloak:
            case SharkToothNecklace:
            case SignetCrown:
            case SignetFeather:
            case SignetFist:
            case SignetHourglass:
            case SignetKey:
            case SignetMask:
            case SirenConch:
            case SpecterRobe:
            case SprintersLegtie:
            case SwindlersBag:
            case TribalFlag:
            case TricksterPouch:
            case TuningFork:
            case TurtleShell:
            case VampiresToothNecklace:
            case ViperFangFingerclaw:
            case WarhorseMarionette:
            case WeaselTotem:
            case WolfenFur:
            case Count:
            default:
            {
                return false;
            }
        }
    }

    float unique_type::ReligiousRatio(const unique_type::Enum E)
    {
        switch (E)
        {
            case NotUnique:
            {
                return 0.0f;
            }
            case CrystalChimes:
            {
                return 0.0f;
            }
            case DoveBloodVial:
            {
                return 0.1f;
            }
            case DragonToothWhistle:
            {
                return 0.0f;
            }
            case DriedFrog:
            {
                return 0.0f;
            }
            case DriedGecko:
            {
                return 0.0f;
            }
            case DriedIguana:
            {
                return 0.0f;
            }
            case DriedLizard:
            {
                return 0.0f;
            }
            case DriedSalamander:
            {
                return 0.0f;
            }
            case DriedSkink:
            {
                return 0.0f;
            }
            case DriedToad:
            {
                return 0.0f;
            }
            case DriedTurtle:
            {
                return 0.0f;
            }
            case ExoticGoldenGong:
            {
                return 0.0f;
            }
            case MagnifyingGlass:
            {
                return 0.0f;
            }
            case PixieBell:
            {
                return 0.0f;
            }
            case ShamanRainmaker:
            {
                return 0.9f;
            }
            case SpecterChains:
            {
                return 0.1f;
            }
            case VultureGizzard:
            {
                return 0.0f;
            }
            case WarTrumpet:
            {
                return 0.0f;
            }
            case WraithTalisman:
            {
                return 0.5f;
            }
            case BasiliskTonge:
            {
                return 0.0f;
            }
            case BerserkersBeard:
            {
                return 0.0f;
            }
            case BishopsHanky:
            {
                return 0.99f;
            }
            case BleedingTrophy:
            {
                return 0.5f;
            }
            case BloodyDragonScale:
            {
                return 0.0f;
            }
            case BottleOfBansheeScreams:
            {
                return 0.0f;
            }
            case BraceletCrown:
            {
                return 0.0f;
            }
            case BraceletFeather:
            {
                return 0.0f;
            }
            case BraceletFist:
            {
                return 0.0f;
            }
            case BraceletHourglass:
            {
                return 0.0f;
            }
            case BraceletKey:
            {
                return 0.0f;
            }
            case BraceletMask:
            {
                return 0.0f;
            }
            case BroochCrown:
            {
                return 0.0f;
            }
            case BroochFeather:
            {
                return 0.0f;
            }
            case BroochFist:
            {
                return 0.0f;
            }
            case BroochHourglass:
            {
                return 0.0f;
            }
            case BroochKey:
            {
                return 0.0f;
            }
            case BroochMask:
            {
                return 0.0f;
            }
            case BurialShroud:
            {
                return 0.8f;
            }
            case CharmCrown:
            {
                return 0.0f;
            }
            case CharmFeather:
            {
                return 0.0f;
            }
            case CharmFist:
            {
                return 0.0f;
            }
            case CharmHourglass:
            {
                return 0.0f;
            }
            case CharmKey:
            {
                return 0.0f;
            }
            case CharmMask:
            {
                return 0.0f;
            }
            case ChimeraBone:
            {
                return 0.0f;
            }
            case CobraTooth:
            {
                return 0.0f;
            }
            case CommandersCape:
            {
                return 0.0f;
            }
            case BronzeTroll:
            {
                return 0.0f;
            }
            case GlassCat:
            {
                return 0.0f;
            }
            case CyclopsEye:
            {
                return 0.0f;
            }
            case DemonDiary:
            {
                return 0.75f;
            }
            case DruidLeaf:
            {
                return 0.9f;
            }
            case EvilRabbitsFoot:
            {
                return 0.5f;
            }
            case FanaticsFlag:
            {
                return 0.0f;
            }
            case FriarsChronicle:
            {
                return 0.8f;
            }
            case FuneralRecord:
            {
                return 0.8f;
            }
            case GeneralsCape:
            {
                return 0.0f;
            }
            case GhostSheet:
            {
                return 0.8f;
            }
            case GiantOwlEye:
            {
                return 0.0f;
            }
            case GriffinFeather:
            {
                return 0.0f;
            }
            case HangmansNoose:
            {
                return 0.0f;
            }
            case HawkEye:
            {
                return 0.0f;
            }
            case HobgoblinNose:
            {
                return 0.0f;
            }
            case HoboRing:
            {
                return 0.0f;
            }
            case HolyEpic:
            {
                return 0.9f;
            }
            case HornOfTheHorde:
            {
                return 0.0f;
            }
            case ImpTail:
            {
                return 0.0f;
            }
            case IslanderHeaddress:
            {
                return 0.0f;
            }
            case JeweledAnkhNecklace:
            {
                return 0.0f;
            }
            case JeweledArmband:
            {
                return 0.0f;
            }
            case JeweledHandbag:
            {
                return 0.0f;
            }
            case JeweledPrincessVeil:
            {
                return 0.0f;
            }
            case KingsCape:
            {
                return 0.0f;
            }
            case LastRitesScroll:
            {
                return 0.9f;
            }
            case MacabreManuscript:
            {
                return 0.75f;
            }
            case MadRatJuju:
            {
                return 0.0f;
            }
            case MagicHorseshoe:
            {
                return 0.0f;
            }
            case ManaAmulet:
            {
                return 0.2f;
            }
            case MendicantRing:
            {
                return 0.75f;
            }
            case MinotaurHide:
            {
                return 0.0f;
            }
            case MonkRing:
            {
                return 0.9f;
            }
            case MortuaryOrnament:
            {
                return 0.8f;
            }
            case MournersMask:
            {
                return 0.0f;
            }
            case PantherPaw:
            {
                return 0.0f;
            }
            case PinCrown:
            {
                return 0.0f;
            }
            case PinFeather:
            {
                return 0.0f;
            }
            case PinFist:
            {
                return 0.0f;
            }
            case PinHourglass:
            {
                return 0.0f;
            }
            case PinKey:
            {
                return 0.0f;
            }
            case PinMask:
            {
                return 0.0f;
            }
            case PriestRing:
            {
                return 0.99f;
            }
            case RascalMask:
            {
                return 0.0f;
            }
            case RattlesnakeTail:
            {
                return 0.0f;
            }
            case RavenClaw:
            {
                return 0.0f;
            }
            case RazorFingerclaw:
            {
                return 0.0f;
            }
            case ReaperScythe:
            {
                return 0.0f;
            }
            case RegalCaptainsFlag:
            {
                return 0.0f;
            }
            case RequiemRegister:
            {
                return 0.9f;
            }
            case RoyalScoutSpyglass:
            {
                return 0.0f;
            }
            case SaintCameoPin:
            {
                return 0.9f;
            }
            case SaintsJournal:
            {
                return 0.9f;
            }
            case SanguineRelic:
            {
                return 0.5f;
            }
            case ScorpionStingerFingerclaw:
            {
                return 0.0f;
            }
            case ScoundrelSack:
            {
                return 0.0f;
            }
            case SepultureDecoration:
            {
                return 0.65f;
            }
            case ShadeCloak:
            {
                return 0.5f;
            }
            case SharkToothNecklace:
            {
                return 0.0f;
            }
            case SignetCrown:
            {
                return 0.0f;
            }
            case SignetFeather:
            {
                return 0.0f;
            }
            case SignetFist:
            {
                return 0.0f;
            }
            case SignetHourglass:
            {
                return 0.0f;
            }
            case SignetKey:
            {
                return 0.0f;
            }
            case SignetMask:
            {
                return 0.0f;
            }
            case SirenConch:
            {
                return 0.0f;
            }
            case SpecterRobe:
            {
                return 0.2f;
            }
            case SprintersLegtie:
            {
                return 0.0f;
            }
            case SwindlersBag:
            {
                return 0.0f;
            }
            case TribalFlag:
            {
                return 0.0f;
            }
            case TricksterPouch:
            {
                return 0.0f;
            }
            case TuningFork:
            {
                return 0.0f;
            }
            case TurtleShell:
            {
                return 0.0f;
            }
            case VampiresToothNecklace:
            {
                return 0.0f;
            }
            case ViperFangFingerclaw:
            {
                return 0.0f;
            }
            case WarhorseMarionette:
            {
                return 0.0f;
            }
            case WeaselTotem:
            {
                return 0.0f;
            }
            case WolfenFur:
            {
                return 0.0f;
            }
            case Count:
            {
                return 0.0f;
            }
            default:
            {
                return 0.0f;
            }
        }
    }

    const std::string named_type::ToString(const named_type::Enum E)
    {
        switch (E)
        {
            case NotNamed:
            {
                return "NotNamed";
            }
            case Arctic:
            {
                return "Arctic";
            }
            case Army:
            {
                return "Army";
            }
            case Betrayer:
            {
                return "Betrayer";
            }
            case Burglar:
            {
                return "Burglar";
            }
            case Burning:
            {
                return "Burning";
            }
            case Charlatans:
            {
                return "Charlatans";
            }
            case Charred:
            {
                return "Charred";
            }
            case Chill:
            {
                return "Chill";
            }
            case Dancing:
            {
                return "Dancing";
            }
            case Daring:
            {
                return "Daring";
            }
            case Dark:
            {
                return "Dark";
            }
            case Dawn:
            {
                return "Dawn";
            }
            case Diabolic:
            {
                return "Diabolic";
            }
            case Dusk:
            {
                return "Dusk";
            }
            case Elite:
            {
                return "Elite";
            }
            case Fiendish:
            {
                return "Fiendish";
            }
            case Fiery:
            {
                return "Fiery";
            }
            case Focus:
            {
                return "Focus";
            }
            case Frigid:
            {
                return "Frigid";
            }
            case Frozen:
            {
                return "Frozen";
            }
            case Gladiator:
            {
                return "Gladiator";
            }
            case Gloom:
            {
                return "Gloom";
            }
            case Glory:
            {
                return "Glory";
            }
            case Heros:
            {
                return "Heros";
            }
            case Honest:
            {
                return "Honest";
            }
            case Icy:
            {
                return "Icy";
            }
            case Imposters:
            {
                return "Imposters";
            }
            case Infernal:
            {
                return "Infernal";
            }
            case Knaves:
            {
                return "Knaves";
            }
            case Light:
            {
                return "Light";
            }
            case Marauder:
            {
                return "Marauder";
            }
            case Misery:
            {
                return "Misery";
            }
            case Moon:
            {
                return "Moon";
            }
            case Mountebank:
            {
                return "Mountebank";
            }
            case Muggers:
            {
                return "Muggers";
            }
            case Nile:
            {
                return "Nile";
            }
            case Noble:
            {
                return "Noble";
            }
            case Pickpocket:
            {
                return "Pickpocket";
            }
            case Pirate:
            {
                return "Pirate";
            }
            case Princes:
            {
                return "Princes";
            }
            case Proud:
            {
                return "Proud";
            }
            case Pure:
            {
                return "Pure";
            }
            case Raging:
            {
                return "Raging";
            }
            case Ranger:
            {
                return "Ranger";
            }
            case Robbers:
            {
                return "Robbers";
            }
            case Samurai:
            {
                return "Samurai";
            }
            case Searing:
            {
                return "Searing";
            }
            case Soldiers:
            {
                return "Soldiers";
            }
            case Sorrow:
            {
                return "Sorrow";
            }
            case Sun:
            {
                return "Sun";
            }
            case Thief:
            {
                return "Theif";
            }
            case Thors:
            {
                return "Thors";
            }
            case Thugs:
            {
                return "Thugs";
            }
            case Twilight:
            {
                return "Twilight";
            }
            case Valiant:
            {
                return "Valiant";
            }
            case Wardens:
            {
                return "Wardens";
            }
            case Wicked:
            {
                return "Wicked";
            }
            case Winter:
            {
                return "Winter";
            }
            case Woe:
            {
                return "Woe";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "item::named_type::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    const std::string named_type::Name(const named_type::Enum E)
    {
        switch (E)
        {
            case Charlatans:
            {
                return "Charlatan's";
            }
            case Heros:
            {
                return "Hero's";
            }
            case Imposters:
            {
                return "Imposter's";
            }
            case Knaves:
            {
                return "Knave's";
            }
            case Muggers:
            {
                return "Mugger's";
            }
            case Princes:
            {
                return "Prince's";
            }
            case Robbers:
            {
                return "Robber's";
            }
            case Soldiers:
            {
                return "Soldier's";
            }
            case Thors:
            {
                return "Thor's";
            }
            case Thugs:
            {
                return "Thug's";
            }
            case Wardens:
            {
                return "Warden's";
            }
            case NotNamed:
            case Arctic:
            case Army:
            case Betrayer:
            case Burglar:
            case Burning:
            case Charred:
            case Chill:
            case Dancing:
            case Daring:
            case Dark:
            case Dawn:
            case Diabolic:
            case Dusk:
            case Elite:
            case Fiendish:
            case Fiery:
            case Focus:
            case Frigid:
            case Frozen:
            case Gladiator:
            case Gloom:
            case Glory:
            case Honest:
            case Icy:
            case Infernal:
            case Light:
            case Marauder:
            case Misery:
            case Moon:
            case Mountebank:
            case Nile:
            case Noble:
            case Pickpocket:
            case Pirate:
            case Proud:
            case Pure:
            case Searing:
            case Raging:
            case Ranger:
            case Samurai:
            case Sorrow:
            case Sun:
            case Thief:
            case Twilight:
            case Valiant:
            case Wicked:
            case Winter:
            case Woe:
            {
                return named_type::ToString(E);
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "item::named_type::Name(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
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
            elementTypes = element_type::AllCombinations(element_type::Fire | element_type::Shadow);
        }
        else if (
            (NAMED_TYPE == named_type::Light) || (NAMED_TYPE == named_type::Dawn)
            || (NAMED_TYPE == named_type::Sun))
        {
            elementTypes = element_type::AllCombinations(element_type::Fire | element_type::Honor);
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
            || (NAMED_TYPE == named_type::Valiant) || (NAMED_TYPE == named_type::Heros)
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

    const std::string material::ToString(const item::material::Enum E)
    {
        switch (E)
        {
            case Nothing:
            {
                return "Nothing";
            }
            case Wood:
            {
                return "Wood";
            }
            case Leather:
            {
                return "Leather";
            }
            case Silk:
            {
                return "Silk";
            }
            case Bone:
            {
                return "Bone";
            }
            case Water:
            {
                return "Water";
            }
            case Dirt:
            {
                return "Dirt";
            }
            case Acid:
            {
                return "Acid";
            }
            case Stone:
            {
                return "Stone";
            }
            case Plant:
            {
                return "Plant";
            }
            case DriedFlesh:
            {
                return "DriedFlesh";
            }
            case Hide:
            {
                return "Hide";
            }
            case Feather:
            {
                return "Feather";
            }
            case Fur:
            {
                return "Fur";
            }
            case Hair:
            {
                return "Hair";
            }
            case Scales:
            {
                return "Scales";
            }
            case Obsidian:
            {
                return "Obsidian";
            }
            case Jade:
            {
                return "Jade";
            }
            case Amethyst:
            {
                return "Amethyst";
            }
            case Emerald:
            {
                return "Emerald";
            }
            case Pearl:
            {
                return "Pearl";
            }
            case Ruby:
            {
                return "Ruby";
            }
            case Lazuli:
            {
                return "Lazuli";
            }
            case Sapphire:
            {
                return "Sapphire";
            }
            case Diamond:
            {
                return "Diamond";
            }
            case Glass:
            {
                return "Glass";
            }
            case Gas:
            {
                return "Gas";
            }
            case Blood:
            {
                return "Blood";
            }
            case Paper:
            {
                return "Paper";
            }
            case Rope:
            {
                return "Rope";
            }
            case Cloth:
            {
                return "Cloth";
            }
            case Claw:
            {
                return "Claw";
            }
            case Horn:
            {
                return "Horn";
            }
            case Tooth:
            {
                return "Tooth";
            }
            case Spirit:
            {
                return "Spirit";
            }
            case Tin:
            {
                return "Tin";
            }
            case Gold:
            {
                return "Gold";
            }
            case Iron:
            {
                return "Iron";
            }
            case Bronze:
            {
                return "Bronze";
            }
            case Silver:
            {
                return "Silver";
            }
            case Steel:
            {
                return "Steel";
            }
            case Platinum:
            {
                return "Platinum";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "item::material::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    const std::string material::Name(const item::material::Enum MATERIAL_TYPE)
    {
        if (MATERIAL_TYPE == material::DriedFlesh)
        {
            return "Dried Flesh";
        }
        else
        {
            return ToString(MATERIAL_TYPE);
        }
    }

    Armor_t material::ArmorRatingBonus(
        const material::Enum MATERIAL_PRI, const material::Enum MATERIAL_SEC)
    {
        return ArmorRatingBonusPri(MATERIAL_PRI) + ArmorRatingBonusSec(MATERIAL_SEC);
    }

    Armor_t material::ArmorRatingBonusPri(const material::Enum MATERIAL_PRI)
    {
        switch (MATERIAL_PRI)
        {
            case Nothing:
            case Spirit:
            case Gas:
            case Water:
            case Blood:
            case Acid:
            case Dirt:
            case Paper:
            case Glass:
            case Feather:
            case Hair:
            case Rope:
            {
                return 0_armor;
            }
            case Cloth:
            {
                return 1_armor;
            }
            case Silk:
            {
                return 2_armor;
            }
            case Hide:
            {
                return 4_armor;
            }
            case DriedFlesh:
            {
                return 5_armor;
            }
            case Leather:
            {
                return 7_armor;
            }
            case Fur:
            {
                return 9_armor;
            }
            case Plant:
            {
                return 11_armor;
            }
            case Claw:
            case Horn:
            {
                return 12_armor;
            }
            case Bone:
            case Tooth:
            {
                return 13_armor;
            }
            case Wood:
            {
                return 15_armor;
            }
            case Scales:
            {
                return 18_armor;
            }
            case Stone:
            {
                return 20_armor;
            }
            case Obsidian:
            {
                return 21_armor;
            }
            case Tin:
            {
                return 25_armor;
            }
            case Bronze:
            {
                return 26_armor;
            }
            case Iron:
            {
                return 27_armor;
            }
            case Steel:
            {
                return 28_armor;
            }
            case Jade:
            {
                return 29_armor;
            }
            case Amethyst:
            {
                return 30_armor;
            }
            case Emerald:
            {
                return 31_armor;
            }
            case Pearl:
            {
                return 32_armor;
            }
            case Ruby:
            {
                return 33_armor;
            }
            case Lazuli:
            {
                return 34_armor;
            }
            case Silver:
            {
                return 35_armor;
            }
            case Sapphire:
            {
                return 36_armor;
            }
            case Gold:
            {
                return 40_armor;
            }
            case Platinum:
            {
                return 45_armor;
            }
            case Diamond:
            {
                return 50_armor;
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "item::material::ArmorRatingBonusPri(" << MATERIAL_PRI
                   << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }

    Armor_t material::ArmorRatingBonusSec(const material::Enum MATERIAL_SEC)
    {
        auto const SEC_MATERIAL_ARMOR_ADJ_RATIO{ game::GameDataFile::Instance()->GetCopyFloat(
            "heroespath-item-secondary-material-armor-adj-ratio") };

        return Armor_t(static_cast<Armor_t::type>(
            ArmorRatingBonusPri(MATERIAL_SEC).As<float>() * SEC_MATERIAL_ARMOR_ADJ_RATIO));
    }

    Coin_t material::PriceAdj(const material::Enum MATERIAL_PRI, const material::Enum MATERIAL_SEC)
    {
        return PriceAdjPri(MATERIAL_PRI) + PriceAdjSec(MATERIAL_SEC);
    }

    Coin_t material::PriceAdjPri(const material::Enum MATERIAL_PRI)
    {
        switch (MATERIAL_PRI)
        {
            case Nothing:
            {
                return 0_coin;
            }
            case Spirit:
            {
                return 0_coin;
            }
            case Gas:
            {
                return 0_coin;
            }
            case Water:
            {
                return 0_coin;
            }
            case Blood:
            {
                return 0_coin;
            }
            case Acid:
            {
                return 100_coin;
            }
            case Dirt:
            {
                return 0_coin;
            }
            case Paper:
            {
                return 0_coin;
            }
            case Glass:
            {
                return 10_coin;
            }
            case Feather:
            {
                return 0_coin;
            }
            case Fur:
            {
                return 8_coin;
            }
            case Hair:
            {
                return 0_coin;
            }
            case DriedFlesh:
            {
                return 4_coin;
            }
            case Rope:
            {
                return 0_coin;
            }
            case Cloth:
            {
                return 0_coin;
            }
            case Silk:
            {
                return 16_coin;
            }
            case Hide:
            {
                return 7_coin;
            }
            case Leather:
            {
                return 10_coin;
            }
            case Plant:
            {
                return 0_coin;
            }
            case Claw:
            {
                return 4_coin;
            }
            case Scales:
            {
                return 1000_coin;
            }
            case Horn:
            {
                return 6_coin;
            }
            case Bone:
            {
                return 12_coin;
            }
            case Tooth:
            {
                return 5_coin;
            }
            case Wood:
            {
                return 0_coin;
            }
            case Stone:
            {
                return 0_coin;
            }
            case Obsidian:
            {
                return 20_coin;
            }
            case Tin:
            {
                return 30_coin;
            }
            case Bronze:
            {
                return 32_coin;
            }
            case Iron:
            {
                return 36_coin;
            }
            case Steel:
            {
                return 40_coin;
            }
            case Jade:
            {
                return 100_coin;
            }
            case Amethyst:
            {
                return 200_coin;
            }
            case Emerald:
            {
                return 300_coin;
            }
            case Pearl:
            {
                return 500_coin;
            }
            case Ruby:
            {
                return 600_coin;
            }
            case Lazuli:
            {
                return 700_coin;
            }
            case Silver:
            {
                return 1000_coin;
            }
            case Sapphire:
            {
                return 2000_coin;
            }
            case Gold:
            {
                return 5000_coin;
            }
            case Platinum:
            {
                return 7500_coin;
            }
            case Diamond:
            {
                return 10000_coin;
            }
            case Count:
            default:
            {
                std::ostringstream ss;

                ss << "item::material::PriceAdjPri(" << MATERIAL_PRI << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }

    Coin_t material::PriceAdjSec(const material::Enum E) { return PriceAdjPri(E) / 10_coin; }

    float material::WeightMult(const material::Enum MATERIAL_PRI, const material::Enum MATERIAL_SEC)
    {
        return WeightMultPri(MATERIAL_PRI) + WeightMultSec(MATERIAL_SEC);
    }

    float material::WeightMultPri(const material::Enum MATERIAL_PRI)
    {
        switch (MATERIAL_PRI)
        {
            case Nothing:
            {
                return 0.0f;
            }
            case Spirit:
            {
                return 0.0f;
            }
            case Gas:
            {
                return 0.001f;
            }
            case Plant:
            {
                return 0.9f;
            }
            case Cloth:
            {
                return 0.9f;
            }
            case Silk:
            {
                return 0.8f;
            }
            case Leather:
            {
                return 1.05f;
            }
            case Rope:
            {
                return 1.1f;
            }
            case Bone:
            {
                return 1.15f;
            }
            case Paper:
            {
                return 1.2f;
            }
            case Claw:
            {
                return 1.2f;
            }
            case Horn:
            {
                return 1.2f;
            }
            case Tooth:
            {
                return 1.2f;
            }
            case Water:
            {
                return 1.5f;
            }
            case Acid:
            {
                return 1.5f;
            }
            case Blood:
            {
                return 1.55f;
            }
            case DriedFlesh:
            {
                return 1.6f;
            }
            case Hide:
            {
                return 1.35f;
            }
            case Feather:
            {
                return 0.5f;
            }
            case Fur:
            {
                return 0.75f;
            }
            case Hair:
            {
                return 0.5f;
            }
            case Scales:
            {
                return 0.9f;
            }
            case Glass:
            {
                return 1.6f;
            }
            case Wood:
            {
                return 1.85f;
            }
            case Dirt:
            {
                return 1.9f;
            }
            case Obsidian:
            {
                return 2.2f;
            }
            case Jade:
            {
                return 2.1f;
            }
            case Amethyst:
            {
                return 2.1f;
            }
            case Emerald:
            {
                return 2.15f;
            }
            case Pearl:
            {
                return 1.2f;
            }
            case Ruby:
            {
                return 2.2f;
            }
            case Lazuli:
            {
                return 2.5f;
            }
            case Sapphire:
            {
                return 2.1f;
            }
            case Diamond:
            {
                return 2.6f;
            }
            case Stone:
            {
                return 3.0f;
            }
            case Tin:
            {
                return 4.0f;
            } // 7.3 - density
            case Bronze:
            {
                return 4.2f;
            } // 7.5
            case Iron:
            {
                return 4.6f;
            } // 7.8
            case Steel:
            {
                return 4.8f;
            } // 8.0
            case Silver:
            {
                return 7.3f;
            } // 10.5
            case Gold:
            {
                return 16.6f;
            } // 19.3
            case Platinum:
            {
                return 18.8f;
            } // 21.5
            case Count:
            default:
            {
                std::ostringstream ss;

                ss << "item::material::WeightMultPri(" << MATERIAL_PRI << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }

    float material::WeightMultSec(const material::Enum MATERIAL_SEC)
    {
        return WeightMultPri(MATERIAL_SEC) / 10.0f;
    }

    int material::EnchantmentBonus(
        const material::Enum MATERIAL_PRIMARY, const material::Enum MATERIAL_SECONDARY)
    {
        // the enchantment factory has been setup to work with values that range from [0,20]
        auto const RANGE{ 20.0f };

        // these two ratios should add up to 1.0f
        auto const PRIMARY_RATIO{ 0.75f };
        auto const SECONDARY_RATIO{ 0.25f };

        if (MATERIAL_SECONDARY == material::Nothing)
        {
            return static_cast<int>(EnchantmentBonusRatio(MATERIAL_PRIMARY) * RANGE);
        }
        else
        {
            return static_cast<int>(
                ((EnchantmentBonusRatio(MATERIAL_PRIMARY) * PRIMARY_RATIO)
                 + (EnchantmentBonusRatio(MATERIAL_SECONDARY) * SECONDARY_RATIO))
                * RANGE);
        }
    }

    float material::EnchantmentBonusRatio(const material::Enum MATERIAL)
    {
        // clang-format off
        switch (MATERIAL)
        {
            // this is the standard set that runs the full range of values
            case Glass:     { return 0.1f;  }
            case DriedFlesh:{ return 0.5f;  }
            case Blood:     { return 0.85f; }
            case Spirit:    { return 1.25f; }//extra bonus for spirit is intentional

            // this is the "tribal" set that should range from about 5% to 50%
            case Leather:   { return 0.05f;  }
            case Stone:     { return 0.1f;   }
            case Fur:       { return 0.136f; }
            case Horn:      { return 0.172f; }
            case Hide:      { return 0.23f;  }
            case Silk:      { return 0.262f; }
            case Claw:      { return 0.28f;  }
            case Tooth:     { return 0.33f;  }
            case Scales:    { return 0.38f;  }
            case Bone:      { return 0.45f;  }
            case Obsidian:  { return 0.5f;   }

            // the jewel set should start where tribal left off (50%) and reach 100% with diamond
            case Jade:      { return 0.5f;  }
            case Amethyst:  { return 0.55f; }
            case Emerald:   { return 0.6f;  }
            case Lazuli:    { return 0.68f; }
            case Pearl:     { return 0.76f; }
            case Ruby:      { return 0.84f; }
            case Sapphire:  { return 0.92f; }
            case Diamond:   { return 1.0f;  }

            // the metal set varies wildly, but should stop short of diamond's value
            case Tin:       { return 0.05f;  }
            case Iron:      { return 0.09f;  }
            case Steel:     { return 0.11f;  }
            case Bronze:    { return 0.333f; }
            case Silver:    { return 0.666f; }
            case Gold:      { return 0.8f;   }
            case Platinum:  { return 0.85f;   }

            // all others are 0%
            case Nothing:
            case Feather:
            case Gas:
            case Water:
            case Dirt:
            case Acid:
            case Paper:
            case Hair:
            case Rope:
            case Cloth:
            case Plant:
            case Wood:
            case Count:
            default:
            {
                return 0.0f;
            }
        }
        // clang-format on
    }

    // TODO move to MaterialFactory
    const MaterialPair_t material::SkinMaterial(const creature::race::Enum RACE)
    {
        // keep in sync with ItemImageMachine::GetSkinImageFilename()

        if ((RACE == creature::race::Wolfen) || (RACE == creature::race::Troll)
            || (RACE == creature::race::Boar) || (RACE == creature::race::LionBoar)
            || (RACE == creature::race::Ramonaut) || (RACE == creature::race::Wereboar))
        {
            return std::make_pair(material::Hide, material::Nothing);
        }
        else if (
            (RACE == creature::race::ThreeHeadedHound) || (RACE == creature::race::Minotaur)
            || (RACE == creature::race::Lion) || (RACE == creature::race::Werebear)
            || (RACE == creature::race::Werewolf))
        {
            return std::make_pair(material::Hide, material::Fur);
        }
        else if (
            (RACE == creature::race::Dragon) || (RACE == creature::race::Hydra)
            || (RACE == creature::race::LizardWalker) || (RACE == creature::race::Naga)
            || (RACE == creature::race::Serpent) || (RACE == creature::race::Cobra)
            || (RACE == creature::race::Wyvern))
        {
            return std::make_pair(material::Scales, material::Nothing);
        }
        else if (RACE == creature::race::Plant)
        {
            return std::make_pair(material::Plant, material::Nothing);
        }
        else
        {
            return std::make_pair(material::Nothing, material::Nothing);
        }
    }

    bool material::IsSolid(const material::Enum MATERIAL)
    {
        return (
            (MATERIAL != material::Nothing) && (IsLiquid(MATERIAL) == false)
            && (IsGas(MATERIAL) == false) && (IsSpirit(MATERIAL) == false));
    }

    bool material::IsLiquid(const material::Enum MATERIAL)
    {
        return (
            (MATERIAL == material::Acid) || (MATERIAL == material::Blood)
            || (MATERIAL == material::Water));
    }

    bool material::IsGas(const material::Enum MATERIAL) { return (MATERIAL == material::Gas); }

    bool material::IsSpirit(const material::Enum MATERIAL)
    {
        return (MATERIAL == material::Spirit);
    }

    bool material::IsBendy(const material::Enum MATERIAL)
    {
        return (
            (MATERIAL == material::Leather) || (MATERIAL == material::Silk)
            || (MATERIAL == material::Dirt) || (MATERIAL == material::Plant)
            || (MATERIAL == material::DriedFlesh) || (MATERIAL == material::Hide)
            || (MATERIAL == material::Feather) || (MATERIAL == material::Fur)
            || (MATERIAL == material::Hair) || (MATERIAL == material::Paper)
            || (MATERIAL == material::Rope) || (MATERIAL == material::Cloth));
    }

    bool material::IsRigid(const material::Enum MATERIAL)
    {
        return (IsSolid(MATERIAL) && (IsBendy(MATERIAL) == false));
    }

    bool material::IsBloody(const material::Enum PRI, const material::Enum SEC)
    {
        return ((PRI == material::Blood) || (SEC == material::Blood));
    }

    float material::FireDamageRatio(const material::Enum PRI, const material::Enum SEC)
    {
        float fireDamageRatio(0.0f);
        switch (PRI)
        {
            case material::Nothing:
            {
                break;
            }
            case material::Wood:
            {
                fireDamageRatio = 0.5f;
                break;
            }
            case material::Leather:
            {
                fireDamageRatio = 0.6f;
                break;
            }
            case material::Silk:
            {
                fireDamageRatio = 0.9f;
                break;
            }
            case material::Bone:
            {
                fireDamageRatio = 0.2f;
                break;
            }
            case material::Water:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Dirt:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Acid:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Stone:
            {
                fireDamageRatio = 0.001f;
                break;
            }
            case material::Plant:
            {
                fireDamageRatio = 0.85f;
                break;
            }
            case material::DriedFlesh:
            {
                fireDamageRatio = 0.75f;
                break;
            }
            case material::Hide:
            {
                fireDamageRatio = 0.87f;
                break;
            }
            case material::Feather:
            {
                fireDamageRatio = 1.0f;
                break;
            }
            case material::Fur:
            {
                fireDamageRatio = 1.0f;
                break;
            }
            case material::Hair:
            {
                fireDamageRatio = 2.0f;
                break;
            }
            case material::Scales:
            {
                fireDamageRatio = 0.1f;
                break;
            }
            case material::Obsidian:
            {
                fireDamageRatio = 0.0f;
                break;
            }
            case material::Jade:
            {
                fireDamageRatio = 0.0f;
                break;
            }
            case material::Amethyst:
            {
                fireDamageRatio = 0.0f;
                break;
            }
            case material::Emerald:
            {
                fireDamageRatio = 0.0f;
                break;
            }
            case material::Pearl:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Ruby:
            {
                fireDamageRatio = 0.0f;
                break;
            }
            case material::Lazuli:
            {
                fireDamageRatio = 0.0f;
                break;
            }
            case material::Sapphire:
            {
                fireDamageRatio = 0.0f;
                break;
            }
            case material::Diamond:
            {
                fireDamageRatio = 0.0f;
                break;
            }
            case material::Glass:
            {
                fireDamageRatio = 0.9f;
                break;
            }
            case material::Gas:
            {
                fireDamageRatio = 0.0f;
                break;
            }
            case material::Blood:
            {
                fireDamageRatio = 0.2f;
                break;
            }
            case material::Paper:
            {
                fireDamageRatio = 0.99f;
                break;
            }
            case material::Rope:
            {
                fireDamageRatio = 0.85f;
                break;
            }
            case material::Cloth:
            {
                fireDamageRatio = 0.85f;
                break;
            }
            case material::Claw:
            {
                fireDamageRatio = 0.1f;
                break;
            }
            case material::Horn:
            {
                fireDamageRatio = 0.1f;
                break;
            }
            case material::Tooth:
            {
                fireDamageRatio = 0.05f;
                break;
            }
            case material::Spirit:
            {
                fireDamageRatio = 0.0f;
                break;
            }
            case material::Gold:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Iron:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Tin:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Bronze:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Silver:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Steel:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Platinum:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Count:
            default:
            {
                std::ostringstream ss;
                ss << "item::FireDamageRatio(material_pri=" << PRI
                   << ", material_sec=...) -but that material_pri type is unknown.";

                throw std::range_error(ss.str());
            }
        }
        switch (SEC)
        {
            case material::Nothing:
            {
                break;
            }
            case material::Wood:
            {
                fireDamageRatio += 0.2f;
                break;
            }
            case material::Leather:
            {
                fireDamageRatio += 0.1f;
                break;
            }
            case material::Silk:
            {
                fireDamageRatio += 0.27f;
                break;
            }
            case material::Bone:
            {
                fireDamageRatio += 0.05f;
                break;
            }
            case material::Water:
            {
                fireDamageRatio -= 0.75f;
                break;
            }
            case material::Dirt:
            {
                fireDamageRatio -= 0.6f;
                break;
            }
            case material::Acid:
            {
                fireDamageRatio -= 0.75f;
                break;
            }
            case material::Stone:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Plant:
            {
                fireDamageRatio += 0.4f;
                break;
            }
            case material::DriedFlesh:
            {
                fireDamageRatio += 0.1f;
                break;
            }
            case material::Hide:
            {
                fireDamageRatio += 0.3f;
                break;
            }
            case material::Feather:
            {
                fireDamageRatio += 0.2f;
                break;
            }
            case material::Fur:
            {
                fireDamageRatio += 0.3f;
                break;
            }
            case material::Hair:
            {
                fireDamageRatio += 0.5f;
                break;
            }
            case material::Scales:
            {
                fireDamageRatio -= 0.75f;
                break;
            }
            case material::Obsidian:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Jade:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Amethyst:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Emerald:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Pearl:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Ruby:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Lazuli:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Sapphire:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Diamond:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Glass:
            {
                break;
            }
            case material::Gas:
            {
                break;
            }
            case material::Blood:
            {
                fireDamageRatio -= 0.75f;
                break;
            }
            case material::Paper:
            {
                fireDamageRatio += 0.5f;
                break;
            }
            case material::Rope:
            {
                fireDamageRatio += 0.25f;
                break;
            }
            case material::Cloth:
            {
                fireDamageRatio += 0.25f;
                break;
            }
            case material::Claw:
            {
                fireDamageRatio -= 0.1f;
                break;
            }
            case material::Horn:
            {
                fireDamageRatio -= 0.1f;
                break;
            }
            case material::Tooth:
            {
                fireDamageRatio -= 0.1f;
                break;
            }
            case material::Spirit:
            {
                break;
            }
            case material::Gold:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Iron:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Tin:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Bronze:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Silver:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Steel:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Platinum:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Count:
            default:
            {
                std::ostringstream ss;
                ss << "item::FireDamageRatio(material_pri=" << material::ToString(PRI)
                   << ", material_sec=" << SEC << " -but that material_sec type is unknown.";

                throw std::range_error(ss.str());
            }
        }

        if (fireDamageRatio < 0.0f)
        {
            fireDamageRatio = 0.0f;
        }

        if (fireDamageRatio > 1.0f)
        {
            fireDamageRatio = 1.0f;
        }

        return fireDamageRatio;
    }

    bool material::IsMetal(const material::Enum MATERIAL)
    {
        return (
            (MATERIAL == material::Tin) || (MATERIAL == material::Gold)
            || (MATERIAL == material::Iron) || (MATERIAL == material::Bronze)
            || (MATERIAL == material::Silver) || (MATERIAL == material::Steel)
            || (MATERIAL == material::Platinum));
    }

    bool material::IsFancyJewel(const material::Enum MATERIAL)
    {
        return (
            (MATERIAL == Amethyst) || (MATERIAL == Emerald) || (MATERIAL == Ruby)
            || (MATERIAL == Sapphire) || (MATERIAL == Diamond));
    }

    bool material::IsFancyMetal(const material::Enum MATERIAL)
    {
        return ((MATERIAL == Silver) || (MATERIAL == Gold));
    }

    bool material::IsFancyOpaque(const material::Enum MATERIAL)
    {
        return (
            (MATERIAL == material::Obsidian) || (MATERIAL == material::Pearl)
            || (MATERIAL == material::Jade) || (MATERIAL == material::Lazuli));
    }

    bool material::IsFancyTribal(const material::Enum MATERIAL)
    {
        return (
            (MATERIAL == Claw) || (MATERIAL == Bone) || (MATERIAL == Tooth)
            || (MATERIAL == Scales));
    }

    bool material::IsFancy(const Enum MATERIAL)
    {
        return (
            IsFancyJewel(MATERIAL) || IsFancyMetal(MATERIAL) || IsFancyOpaque(MATERIAL)
            || IsFancyTribal(MATERIAL));
    }

    bool material::RequiresAnPrefix(const Enum MATERIAL)
    {
        return (
            (MATERIAL == Obsidian) || (MATERIAL == Iron) || (MATERIAL == Amethyst)
            || (MATERIAL == Emerald));
    }

    const std::string weapon_type::ToString(
        const item::weapon_type::Enum WEAPON_TYPE, const bool WILL_WRAP, const bool IS_READBLE)
    {
        std::ostringstream ss;

        if (WEAPON_TYPE == weapon_type::NotAWeapon)
        {
            ss << "NotAWeapon";
        }
        else
        {
            auto appendNameIfBitSet{ [&](const weapon_type::Enum WEAPON_TYPE_BIT,
                                         const std::string & NAME) {
                if (WEAPON_TYPE & WEAPON_TYPE_BIT)
                {
                    if (ss.str().empty() == false)
                    {
                        ss << "/";
                    }

                    ss << NAME;
                }
            } };

            appendNameIfBitSet(weapon_type::BodyPart, "BodyPart");
            appendNameIfBitSet(weapon_type::Sword, "Sword");
            appendNameIfBitSet(weapon_type::Axe, "Axe");
            appendNameIfBitSet(weapon_type::Whip, "Whip");
            appendNameIfBitSet(weapon_type::Knife, "Knife");
            appendNameIfBitSet(weapon_type::Club, "Club");
            appendNameIfBitSet(weapon_type::Staff, "Staff");
            appendNameIfBitSet(
                weapon_type::BladedStaff, ((IS_READBLE) ? "Bladed Staff" : "BladedStaff"));
            appendNameIfBitSet(weapon_type::Melee, "Melee");
            appendNameIfBitSet(weapon_type::Projectile, "Projectile");
            appendNameIfBitSet(weapon_type::Bladed, "Bladed");
            appendNameIfBitSet(weapon_type::Pointed, "Pointed");
        }

        if (ss.str().empty())
        {
            std::ostringstream ssErr;
            ssErr << "item::weapon_type::ToString(" << WEAPON_TYPE
                  << ", will_wrap=" << std::boolalpha << WILL_WRAP << ", is_readable=" << IS_READBLE
                  << ")_InvalidValueError";

            throw std::range_error(ssErr.str());
        }

        if (WILL_WRAP)
        {
            return "(" + ss.str() + ")";
        }
        else
        {
            return ss.str();
        }
    }

    const std::string weapon_type::Name(const weapon_type::Enum WEAPON_TYPE)
    {
        return weapon_type::ToString(WEAPON_TYPE, false, true);
    }

    weapon_type::Enum weapon_type::FromString(const std::string & S)
    {
        weapon_type::Enum type{ NotAWeapon };

        unsigned flag{ 1 };

        while (flag != Last)
        {
            auto const WEAPON_TYPE_ENUM{ static_cast<weapon_type::Enum>(flag) };

            if (boost::algorithm::icontains(S, weapon_type::ToString(WEAPON_TYPE_ENUM)))
            {
                type = static_cast<weapon_type::Enum>(type | flag);
            }

            flag <<= 1;
        }

        return type;
    }

    const std::string armor_type::ToString(const item::armor_type::Enum ARMOR_TYPE)
    {
        switch (ARMOR_TYPE)
        {
            case NotArmor:
            {
                return "NotArmor";
            }
            case Shield:
            {
                return "Shield";
            }
            case Helm:
            {
                return "Helm";
            }
            case Gauntlets:
            {
                return "Gauntlets";
            }
            case Pants:
            {
                return "Pants";
            }
            case Boots:
            {
                return "Boots";
            }
            case Shirt:
            {
                return "Shirt";
            }
            case Bracers:
            {
                return "Bracers";
            }
            case Aventail:
            {
                return "Aventail";
            }
            case Covering:
            {
                return "Covering";
            }
            case Skin:
            {
                return "Skin";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "item::armor_type::ToString(" << ARMOR_TYPE << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    bool armor_type::DoesRequireBaseType(const armor_type::Enum ARMOR_TYPE)
    {
        switch (ARMOR_TYPE)
        {
            case Gauntlets:
            case Pants:
            case Boots:
            case Shirt:
            case Bracers:
            case Aventail:
            {
                return true;
            }
            case NotArmor:
            case Shield:
            case Helm:
            case Covering:
            case Skin:
            case Count:
            default:
            {
                return false;
            }
        }
    }

    const std::string body_part::ToString(const body_part::Enum BODY_PART)
    {
        switch (BODY_PART)
        {
            case Fists:
            {
                return "Fists";
            }
            case Claws:
            {
                return "Claws";
            }
            case Tentacles:
            {
                return "Tentacles";
            }
            case Bite:
            {
                return "Bite";
            }
            case Breath:
            {
                return "Breath";
            }
            case Skin:
            {
                return "Skin";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "item::body_part::ToString(" << BODY_PART << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    const std::string
        name_material_type::ToString(const name_material_type::Enum NAME_MATERIAL_TYPE)
    {
        switch (NAME_MATERIAL_TYPE)
        {
            case name_material_type::Misc:
            {
                return "Misc";
            }
            case name_material_type::BodyPart:
            {
                return "BodyPart";
            }
            case name_material_type::Decoration:
            {
                return "Decoration";
            }
            case name_material_type::Handle:
            {
                return "Handle";
            }
            case name_material_type::Reinforced:
            {
                return "Reinforced";
            }
            case name_material_type::ReinforcedWithBase:
            {
                return "ReinforcedWithBase:";
            }
            case name_material_type::Tipped:
            {
                return "Tipped";
            }
            case name_material_type::Claspped:
            {
                return "Claspped";
            }
            case name_material_type::ClasppedWithBase:
            {
                return "ClasppedWithBase";
            }
            case name_material_type::Count:
            default:
            {
                std::ostringstream ss;
                ss << "item::name_material_type::ToString(" << NAME_MATERIAL_TYPE
                   << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

} // namespace item
} // namespace heroespath
