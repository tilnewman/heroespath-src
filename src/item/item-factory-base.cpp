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
// item-factory-base.cpp
//
#include "item-factory-base.hpp"

#include "creature/creature.hpp"
#include "item/item.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/random.hpp"

#include <sstream>

namespace heroespath
{
namespace item
{

    const std::string ItemFactoryBase::MakeWeaponBodyPartName(
        const creature::CreaturePtr_t CREATURE_PTR, const std::string & READABLE_NAME)
    {
        return CREATURE_PTR->RaceName() + " " + READABLE_NAME;
    }

    const std::string ItemFactoryBase::MakeWeaponBodyPartDescription(
        const std::string & BASE_DESCRIPTION, const creature::CreaturePtr_t CREATURE_PTR)
    {
        return BASE_DESCRIPTION + " " + creature::race::Name(CREATURE_PTR->Race());
    }

    const std::string ItemFactoryBase::MakeArmorBodyPartName(
        const MaterialPair_t & MATERIALS_PAIR, const creature::CreaturePtr_t CREATURE_PTR)
    {
        std::ostringstream ss;
        ss << CREATURE_PTR->RaceName() << "'s ";

        if (MATERIALS_PAIR.second != material::Nothing)
        {
            ss << material::Name(MATERIALS_PAIR.second) << " covered ";
        }

        ss << material::Name(MATERIALS_PAIR.first);

        return ss.str();
    }

    const std::string
        ItemFactoryBase::MakeArmorBodyPartDescription(const MaterialPair_t & MATERIALS_PAIR)
    {
        std::ostringstream ss;
        ss << "tough skin made of " << material::Name(MATERIALS_PAIR.first);

        if (MATERIALS_PAIR.second != material::Nothing)
        {
            ss << " and " << material::Name(MATERIALS_PAIR.second);
        }

        return ss.str();
    }

    const std::string ItemFactoryBase::MakeNonBodyPartName(const ItemProfile & PROFILE)
    {
        std::ostringstream ss;

        AppendBlessedOrCursedIfNeeded(PROFILE, ss);
        AppendPixiePhraseIfNeeded(PROFILE, PhraseType::Name, ss);

        auto const ELEMENT_TYPE_STR{ [&]() -> std::string {
            if (PROFILE.IsElemental())
            {
                return " " + element_type::Name(PROFILE.ElementType());
            }
            else
            {
                return "";
            }
        }() };

        if (PROFILE.IsSet())
        {
            ss << AppendSpaceIfNeeded(ss) << set_type::Name(PROFILE.SetType()) << " "
               << PROFILE.ReadableName();
        }
        else if (PROFILE.IsUnique())
        {
            ss << AppendSpaceIfNeeded(ss) << unique_type::Name(PROFILE.UniqueType());
        }
        else
        {
            auto const NAME_TO_USE{ [&]() {
                if (PROFILE.IsNamed())
                {
                    return named_type::Name(PROFILE.NamedType()) + " "
                        + ReadableNameWithoutArmorBaseType(PROFILE);
                }
                else
                {
                    return ReadableNameWithoutArmorBaseType(PROFILE);
                }
            }() };

            ss << AppendSpaceIfNeeded(ss) << material::Name(PROFILE.MaterialPrimary())
               << ArmorBaseTypeNamePrefix(PROFILE);

            auto const SECONDARY_MATERIAL_PHRASE{ SeccondaryMaterialPhrase(PROFILE) };

            if (SECONDARY_MATERIAL_PHRASE.empty() && material::IsSolid(PROFILE.MaterialSecondary()))
            {
                ss << " and " << material::Name(PROFILE.MaterialSecondary());
            }

            ss << " " << NAME_TO_USE << ELEMENT_TYPE_STR << SECONDARY_MATERIAL_PHRASE;
        }

        // there is a chance for doubled spaces in the logic above
        auto result{ ss.str() };

        namespace ba = boost::algorithm;

        if (ba::contains(result, "  "))
        {
            M_HP_LOG_ERR(
                "The following name had double spaces: \"" << result << "\".  profile={"
                                                           << PROFILE.ToString() << "}");
        }

        return result;
    }

    const std::string ItemFactoryBase::MakeNonBodyPartDescription(
        const ItemProfile & PROFILE, const std::string & BASE_DESC)
    {
        std::ostringstream ss;

        // If non-body_part and non-misc_type, then BASE_DESC comes either from
        // WeaponDetails.description or ArmorDetails.description, which needs an "A " put in front
        // of it.  However, this is handled by Item::Description() so that it can prepend "magical"
        // if needed.  Only Item class knows if an item is magical beccause only it has information
        // on Enchantments, which are not in ItemProfile.  This Item::Description() function is also
        // responsible for putting a period at the end.

        AppendBlessedOrCursedIfNeeded(PROFILE, ss);
        AppendPixiePhraseIfNeeded(PROFILE, PhraseType::Desc, ss);

        ss << AppendSpaceIfNeeded(ss) << BASE_DESC;

        if (PROFILE.IsSet())
        {
            ss << ", an enchanted " << PROFILE.ReadableName() << " from "
               << set_type::Name(PROFILE.SetType()) << " Set";
        }
        else if (PROFILE.IsNamed())
        {
            ss << ", known as the " << named_type::Name(PROFILE.NamedType()) << " "
               << ReadableNameWithoutArmorBaseType(PROFILE);
        }
        else if (PROFILE.IsUnique())
        {
            ss << ", known as a " << unique_type::Name(PROFILE.UniqueType());
        }

        ss << ", made of " << material::Name(PROFILE.MaterialPrimary())
           << ArmorBaseTypeNamePrefix(PROFILE);

        auto const SECONDARY_MATERIAL_PHRASE{ SeccondaryMaterialPhrase(PROFILE) };

        if (SECONDARY_MATERIAL_PHRASE.empty() == false)
        {
            ss << SECONDARY_MATERIAL_PHRASE;
        }
        else if (material::IsSolid(PROFILE.MaterialSecondary()))
        {
            ss << " and " << material::Name(PROFILE.MaterialSecondary());
        }

        if (PROFILE.IsElemental())
        {
            ss << ", and pulsing with the magic " << element_type::Name(PROFILE.ElementType());
        }

        if (PROFILE.SummonInfo().CanSummon())
        {
            ss << ", used to summon a " << creature::race::Name(PROFILE.SummonInfo().Race()) << " "
               << creature::role::Name(PROFILE.SummonInfo().Role());
        }

        // there is a chance for doubled "and"s and spaces in the logic above
        auto result{ ss.str() };

        namespace ba = boost::algorithm;

        if (ba::contains(result, "  "))
        {
            M_HP_LOG_ERR(
                "The following name had double spaces: \"" << result << "\".  profile={"
                                                           << PROFILE.ToString() << "}");
        }

        if (ba::contains(result, "and and"))
        {
            M_HP_LOG_ERR(
                "The following name had a double 'and': \"" << result << "\".  profile={"
                                                            << PROFILE.ToString() << "}");
        }

        return result;
    }

    Coin_t
        ItemFactoryBase::CalculatePrice(const ItemProfile & PROFILE, const Coin_t BASE_PRICE_PARAM)
    {
        auto const BASE_PRICE_FINAL{ (
            (BASE_PRICE_PARAM > 0_coin) ? BASE_PRICE_PARAM
                                        : TreasureScoreToCoins(PROFILE.TreasureScore())) };

        Coin_t price{ BASE_PRICE_FINAL
                      + material::PriceAdj(
                            PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary()) };

        if (PROFILE.IsPixie())
        {
            price = Coin_t(static_cast<Coin_t::type>(price.As<float>() * 1.5f));
        }

        return price;
    }

    Weight_t ItemFactoryBase::CalculateWeight(
        const ItemProfile & PROFILE, const Weight_t BASE_WEIGHT_PARAM)
    {
        auto weight{ (
            (BASE_WEIGHT_PARAM > 0_weight) ? BASE_WEIGHT_PARAM
                                           : misc_type::Weight(PROFILE.MiscType())) };

        weight = Weight_t::Make(
            weight.As<float>()
            * material::WeightMult(PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary()));

        if (PROFILE.IsPixie())
        {
            weight /= 250_weight;
        }

        if (weight < 1_weight)
        {
            weight = 1_weight;
        }

        return weight;
    }

    Armor_t ItemFactoryBase::CalculateArmorRating(
        const ItemProfile & PROFILE, const Armor_t & BASE_ARMOR_RATING)
    {
        // armor items with a clasp will not consider the secondary material in terms of armor
        // rating because that is just the material of the clasp

        return BASE_ARMOR_RATING
            + material::ArmorRatingBonus(
                   PROFILE.MaterialPrimary(),
                   (((PROFILE.NameMaterialType() == name_material_type::Claspped)
                     || (PROFILE.NameMaterialType() == name_material_type::ClasppedWithBase))
                        ? material::Nothing
                        : PROFILE.MaterialSecondary()));
    }

    Coin_t ItemFactoryBase::TreasureScoreToCoins(const Score_t & TREASURE_SCORE)
    {
        // For now Treasure Score equals the price in coins
        return Coin_t(TREASURE_SCORE.As<int>());
    }

    const std::string ItemFactoryBase::ReadableNameWithoutArmorBaseType(const ItemProfile & PROFILE)
    {
        if (PROFILE.IsArmor())
        {
            return PROFILE.ArmorInfo().ReadableNameWithoutBase();
        }
        else
        {
            return PROFILE.ReadableName();
        }
    }

    const std::string ItemFactoryBase::PrefixAOrAn(const material::Enum MATERIAL)
    {
        if (material::RequiresAnPrefix(MATERIAL))
        {
            return "an";
        }
        else
        {
            return "a";
        }
    }

    const std::string ItemFactoryBase::RandomCoatedPhrase()
    {
        if (misc::random::Bool())
        {
            return "dripping with";
        }
        else
        {
            return RandomCoatedAdjective() + " in";
        }
    }

    const std::string ItemFactoryBase::RandomCoatedAdjective()
    {
        switch (misc::random::Int(3))
        {
            case 0:
            {
                return "covered";
            }
            case 1:
            {
                return "soaked";
            }
            case 2:
            {
                return "coated";
            }
            case 3:
            default:
            {
                return "drenched";
            }
        }
    }

    const std::string ItemFactoryBase::JeweledAdjective() { return "jeweled"; }

    const std::string ItemFactoryBase::AdornedAdjective() { return "adorned"; }

    const std::string ItemFactoryBase::RandomClaspNoun()
    {
        switch (misc::random::Int(2))
        {
            case 1:
            {
                return "clasp";
            }
            case 2:
            {
                return "tether";
            }
            case 0:
            default:
            {
                return "fastener";
            }
        }
    }

    bool ItemFactoryBase::IsNonEmptyWithoutTrailingSpace(std::ostringstream & ss)
    {
        auto const CURRENT_STR{ ss.str() };
        return ((CURRENT_STR.empty() == false) && (CURRENT_STR.at(CURRENT_STR.size() - 1) != ' '));
    }

    const std::string ItemFactoryBase::AppendSpaceIfNeeded(std::ostringstream & ss)
    {
        if (IsNonEmptyWithoutTrailingSpace(ss))
        {
            return " ";
        }
        else
        {
            return "";
        }
    }

    void ItemFactoryBase::AppendPixiePhraseIfNeeded(
        const ItemProfile & PROFILE, const PhraseType PHRASE_TYPE, std::ostringstream & ss)
    {
        if (PROFILE.IsPixie())
        {
            ss << AppendSpaceIfNeeded(ss);

            if (PHRASE_TYPE == PhraseType::Name)
            {
                ss << "Pixie";
            }
            else
            {
                ss << "Pixie sized";
            }
        }
    }

    const std::string ItemFactoryBase::ArmorBaseTypeNamePrefix(const ItemProfile & PROFILE)
    {
        auto const BASE_TYPE{ PROFILE.ArmorInfo().BaseType() };
        if (PROFILE.IsArmor()
            && ((BASE_TYPE == armor::base_type::Mail) || (BASE_TYPE == armor::base_type::Plate)))
        {
            return " " + armor::base_type::ToString(BASE_TYPE);
        }
        else
        {
            return "";
        }
    }

    void ItemFactoryBase::AppendBlessedOrCursedIfNeeded(
        const ItemProfile & PROFILE, std::ostringstream & ss)
    {
        if (PROFILE.IsUnique() == false)
        {
            auto const BLESSED_OR_CURSED{ [&]() -> std::string {
                if (misc_type::IsBlessed(PROFILE.MiscType()))
                {
                    return "Blessed";
                }
                else if (misc_type::IsCursed(PROFILE.MiscType()))
                {
                    return "Cursed";
                }
                else
                {
                    return "";
                }
            }() };

            if (BLESSED_OR_CURSED.empty() == false)
            {
                ss << AppendSpaceIfNeeded(ss) << BLESSED_OR_CURSED;
            }
        }
    }

    const std::string ItemFactoryBase::SeccondaryMaterialPhrase(const ItemProfile & PROFILE)
    {
        auto const SECONDARY_MATERIAL{ PROFILE.MaterialSecondary() };
        auto const SECONDARY_MATERIAL_NAME{ material::Name(SECONDARY_MATERIAL) };

        if ((SECONDARY_MATERIAL == material::Nothing) || material::IsGas(SECONDARY_MATERIAL))
        {
            return "";
        }
        else if (material::IsLiquid(SECONDARY_MATERIAL))
        {
            return " " + RandomCoatedAdjective() + " with " + SECONDARY_MATERIAL_NAME;
        }
        else if (material::IsSpirit(SECONDARY_MATERIAL))
        {
            return " with a ghostly glow";
        }
        else
        {
            // at this point the material must be solid
            switch (PROFILE.NameMaterialType())
            {
                case name_material_type::Decoration:
                {
                    return SecondaryMaterialPhraseDecoration(
                        SECONDARY_MATERIAL, SECONDARY_MATERIAL_NAME);
                }
                case name_material_type::Handle:
                {
                    return SecondaryMaterialPhraseHandle(
                        SECONDARY_MATERIAL, SECONDARY_MATERIAL_NAME);
                }
                case name_material_type::Reinforced:
                case name_material_type::ReinforcedWithBase:
                {
                    return SecondaryMaterialPhraseReinforced(
                        SECONDARY_MATERIAL, SECONDARY_MATERIAL_NAME);
                }
                case name_material_type::Tipped:
                {
                    return SecondaryMaterialPhraseTipped(
                        SECONDARY_MATERIAL, SECONDARY_MATERIAL_NAME);
                }
                case name_material_type::Claspped:
                case name_material_type::ClasppedWithBase:
                {
                    return SecondaryMaterialPhraseClasped(
                        SECONDARY_MATERIAL, SECONDARY_MATERIAL_NAME);
                }
                case name_material_type::Misc:
                {
                    if (misc_type::HasNonFleshEyes(PROFILE.MiscType())
                        && (SECONDARY_MATERIAL != material::DriedFlesh))
                    {
                        return " with " + SECONDARY_MATERIAL_NAME + " eyes";
                    }
                    else
                    {
                        return "";
                    }
                }

                case name_material_type::BodyPart:
                case name_material_type::Count:
                default:
                {
                    return "";
                }
            }
        }
    }

    const std::string ItemFactoryBase::SecondaryMaterialPhraseDecoration(
        const material::Enum SECONDARY_MATERIAL, const std::string & SECONDARY_MATERIAL_NAME)
    {
        if (material::IsFancyJewel(SECONDARY_MATERIAL))
        {
            return " " + JeweledAdjective() + " with " + SECONDARY_MATERIAL_NAME;
        }
        else if (material::IsFancyMetal(SECONDARY_MATERIAL))
        {
            return " with " + SECONDARY_MATERIAL_NAME + " filigree";
        }
        else if (
            material::IsFancyOpaque(SECONDARY_MATERIAL)
            || material::IsFancyTribal(SECONDARY_MATERIAL))
        {
            return " " + AdornedAdjective() + " with " + SECONDARY_MATERIAL_NAME;
        }
        else
        {
            return "";
        }
    }

    const std::string ItemFactoryBase::SecondaryMaterialPhraseHandle(
        const material::Enum SECONDARY_MATERIAL, const std::string & SECONDARY_MATERIAL_NAME)
    {
        return " with " + PrefixAOrAn(SECONDARY_MATERIAL) + " " + SECONDARY_MATERIAL_NAME
            + " handle";
    }

    const std::string ItemFactoryBase::SecondaryMaterialPhraseReinforced(
        const material::Enum SECONDARY_MATERIAL, const std::string & SECONDARY_MATERIAL_NAME)
    {
        if (material::IsMetal(SECONDARY_MATERIAL))
        {
            return " reinforced with " + SECONDARY_MATERIAL_NAME;
        }
        else
        {
            return SecondaryMaterialPhraseDecoration(SECONDARY_MATERIAL, SECONDARY_MATERIAL_NAME);
        }
    }

    const std::string ItemFactoryBase::SecondaryMaterialPhraseTipped(
        const material::Enum, const std::string & SECONDARY_MATERIAL_NAME)
    {
        return " tipped with " + SECONDARY_MATERIAL_NAME;
    }

    const std::string ItemFactoryBase::SecondaryMaterialPhraseClasped(
        const material::Enum SECONDARY_MATERIAL, const std::string & SECONDARY_MATERIAL_NAME)
    {
        if (material::IsFancyJewel(SECONDARY_MATERIAL))
        {
            return " with " + PrefixAOrAn(SECONDARY_MATERIAL) + " " + SECONDARY_MATERIAL_NAME + " "
                + JeweledAdjective() + " clasp";
        }
        else if (
            material::IsFancyOpaque(SECONDARY_MATERIAL)
            || material::IsFancyTribal(SECONDARY_MATERIAL))
        {
            return " with " + PrefixAOrAn(SECONDARY_MATERIAL) + " " + SECONDARY_MATERIAL_NAME + " "
                + AdornedAdjective() + " clasp";
        }
        else
        {
            return " with " + PrefixAOrAn(SECONDARY_MATERIAL) + " " + SECONDARY_MATERIAL_NAME
                + " clasp";
        }
    }

} // namespace item
} // namespace heroespath
