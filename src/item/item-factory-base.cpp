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

    bool ItemFactoryBase::DoesCreatureRequireSkinArmor(const creature::CreaturePtr_t CREATURE_PTR)
    {
        auto const SKIN_MATERIAL{ material::SkinMaterial(CREATURE_PTR->Race()) };
        return ((SKIN_MATERIAL != material::Count) && (SKIN_MATERIAL != material::Nothing));
    }

    const std::string ItemFactoryBase::MakeNonBodyPartName(const ItemProfile & PROFILE)
    {
        std::ostringstream ss;

        AppendBlessedOrCursedIfNeeded(PROFILE, ss);
        AppendPixiePhraseIfNeeded(PROFILE, PhraseType::Name, ss);

        if (PROFILE.IsUnique())
        {
            ss << AppendSpaceIfNeededInline(ss) << unique_type::Name(PROFILE.UniqueType());
        }
        else
        {
            auto const NAME_TO_USE{ [&]() {
                if (PROFILE.IsSet())
                {
                    return set_type::Name(PROFILE.SetType()) + " " + PROFILE.ReadableName();
                }
                else if (PROFILE.IsNamed())
                {
                    return named_type::Name(PROFILE.NamedType()) + " " + PROFILE.ReadableName();
                }
                else
                {
                    return PROFILE.ReadableName();
                }
            }() };

            ss << AppendSpaceIfNeededInline(ss) << material::Name(PROFILE.MaterialPrimary());

            auto const SECONDARY_MATERIAL_PHRASE_WHEN_AFTER{ SeccondaryMaterialPhraseWhenAfter(
                PROFILE, PhraseType::Name) };

            if (SECONDARY_MATERIAL_PHRASE_WHEN_AFTER.empty())
            {
                if (PROFILE.MaterialSecondary() != material::Nothing)
                {
                    ss << " and " << material::Name(PROFILE.MaterialSecondary());
                }

                ss << " " << NAME_TO_USE;
            }
            else
            {
                ss << " " << NAME_TO_USE << " " << SECONDARY_MATERIAL_PHRASE_WHEN_AFTER;
            }
        }

        if (PROFILE.IsElemental())
        {
            ss << " " << element_type::Name(PROFILE.ElementType());
        }

        // there is a chance for doubled spaces in the logic above
        auto result{ ss.str() };

        namespace ba = boost::algorithm;

        if (ba::contains(result, "  "))
        {
            ba::replace_all(result, "  ", " ");
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

        AppendSpaceIfNeeded(ss);
        ss << AppendSpaceIfNeededInline(ss) << BASE_DESC;

        if (PROFILE.IsSet())
        {
            ss << ", an enchanted " << PROFILE.ReadableName() << " from the "
               << set_type::Name(PROFILE.SetType()) << " Set";
        }
        else if (PROFILE.IsNamed())
        {
            ss << ", known as the " << named_type::Name(PROFILE.NamedType()) << " "
               << PROFILE.ReadableName();
        }
        else if (PROFILE.IsUnique())
        {
            ss << ", known as a " << unique_type::Name(PROFILE.UniqueType());
        }

        ss << ", made of " << material::Name(PROFILE.MaterialPrimary());

        auto const SECONDARY_MATERIAL_PHRASE{ SeccondaryMaterialPhraseWhenAfter(
            PROFILE, PhraseType::Desc) };

        if (SECONDARY_MATERIAL_PHRASE.empty())
        {
            if (PROFILE.MaterialSecondary() != material::Nothing)
            {
                ss << " and " << material::Name(PROFILE.MaterialSecondary());
            }
        }
        else
        {
            ss << SECONDARY_MATERIAL_PHRASE;
        }

        if (PROFILE.IsElemental())
        {
            ss << " and pulsing with the magic " << element_type::Name(PROFILE.ElementType());
        }

        if (PROFILE.SummonInfo().CanSummon())
        {
            ss << ", used to summon " << creature::race::Name(PROFILE.SummonInfo().Race()) << " "
               << creature::role::Name(PROFILE.SummonInfo().Role());
        }

        // there is a chance for doubled "and"s and spaces in the logic above
        auto result{ ss.str() };

        namespace ba = boost::algorithm;

        if (ba::contains(result, "and and"))
        {
            ba::replace_all(result, "and and", "and");
        }

        if (ba::contains(result, "  "))
        {
            ba::replace_all(result, "  ", " ");
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
                   ((IsArmorItemWithClasp(PROFILE)) ? material::Nothing
                                                    : PROFILE.MaterialSecondary()));
    }

    Coin_t ItemFactoryBase::TreasureScoreToCoins(const Score_t & TREASURE_SCORE)
    {
        // For now Treasure Score equals the price in coins
        return Coin_t(TREASURE_SCORE.As<int>());
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

    const std::string ItemFactoryBase::RandomJeweledAdjective()
    {
        if (misc::random::Bool())
        {
            return "jeweled";
        }
        else
        {
            return "gemmed";
        }
    }

    const std::string ItemFactoryBase::RandomAdornedAdjective()
    {
        auto const RAND{ misc::random::Int(2) };
        if (RAND == 0)
        {
            return "adorned";
        }
        else if (RAND == 1)
        {
            return "decorated";
        }
        else
        {
            return "trimmed";
        }
    }

    const std::string ItemFactoryBase::RandomChainNoun()
    {
        switch (misc::random::Int(3))
        {
            case 1:
            {
                return "tie";
            }
            case 2:
            {
                return "tether";
            }
            case 3:
            {
                return "fastener";
            }
            case 0:
            default:
            {
                return "chain";
            }
        }
    }

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

    void ItemFactoryBase::AppendSpaceIfNeeded(std::ostringstream & ss)
    {
        if (IsNonEmptyWithoutTrailingSpace(ss))
        {
            ss << " ";
        }
    }

    const std::string ItemFactoryBase::AppendSpaceIfNeededInline(std::ostringstream & ss)
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
            AppendSpaceIfNeeded(ss);

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
                AppendSpaceIfNeeded(ss);
                ss << BLESSED_OR_CURSED;
            }
        }
    }

    bool ItemFactoryBase::IsArmorItemWithClasp(const ItemProfile & PROFILE)
    {
        return (
            (PROFILE.ArmorType() == armor_type::Covering) || PROFILE.ArmorInfo().IsPants()
            || PROFILE.ArmorInfo().IsShirt() || PROFILE.ArmorInfo().IsAventail());
    }

    const std::string ItemFactoryBase::SeccondaryMaterialPhraseWhenAfter(
        const ItemProfile & PROFILE, const PhraseType)
    {
        std::ostringstream ss;

        if (PROFILE.MaterialSecondary() != material::Nothing)
        {
            auto const SECONDARY_MATERIAL_NAME{ material::Name(PROFILE.MaterialSecondary()) };

            auto const SECONDARY_MATERIAL_SPECIFIC_NOUN{ SecondaryMaterialSpecificNoun(PROFILE) };

            if (SECONDARY_MATERIAL_SPECIFIC_NOUN.empty() == false)
            {
                ss << SecondaryMaterialPhraseWithSpecificNoun(
                    PROFILE, SECONDARY_MATERIAL_NAME, SECONDARY_MATERIAL_SPECIFIC_NOUN);
            }
            else if (PROFILE.WeaponInfo().IsBladedStaff())
            {
                ss << SecondaryMaterialPhraseForBladedStaffs(PROFILE, SECONDARY_MATERIAL_NAME);
            }
            else if (IsArmorItemWithClasp(PROFILE))
            {
                ss << SecondaryMaterialPhraseForClaspedArmor(PROFILE, SECONDARY_MATERIAL_NAME);
            }
            else if (misc_type::HasNonFleshEyes(PROFILE.MiscType()))
            {
                ss << SecondaryMaterialPhraseForItemsWithNonFleshEyes(
                    PROFILE, SECONDARY_MATERIAL_NAME);
            }
            else if (PROFILE.ArmorInfo().IsShield())
            {
                ss << SecondaryMaterialPhraseForShields(PROFILE, SECONDARY_MATERIAL_NAME);
            }
            else
            {
                ss << SecondaryMaterialPhraseStandard(PROFILE, SECONDARY_MATERIAL_NAME);
            }
        }

        return ss.str();
    }

    const std::string ItemFactoryBase::SecondaryMaterialPhraseStandard(
        const ItemProfile & PROFILE, const std::string & SECONDARY_MATERIAL_NAME)
    {
        std::ostringstream ss;

        if (material::IsJewel(PROFILE.MaterialSecondary()))
        {
            ss << " " << RandomJeweledAdjective() << " with " << SECONDARY_MATERIAL_NAME;
        }
        else if (material::IsPrecious(PROFILE.MaterialSecondary()))
        {
            // at this point we know MATERIAL_SEC is either
            // silver/gold/platinum/pearl/obsidian, one of the "adorned" materials
            ss << " " << RandomAdornedAdjective() << " with " << SECONDARY_MATERIAL_NAME;
        }
        else if (material::IsLiquid(PROFILE.MaterialSecondary()))
        {
            ss << " " << RandomCoatedAdjective() << " with " << SECONDARY_MATERIAL_NAME;
        }
        else if (material::IsSpirit(PROFILE.MaterialSecondary()))
        {
            ss << " with a ghostly glow";
        }

        return ss.str();
    }

    const std::string ItemFactoryBase::SecondaryMaterialSpecificNoun(const ItemProfile & PROFILE)
    {
        if (PROFILE.WeaponInfo().IsWhip())
        {
            return "handle";
        }
        else if (PROFILE.WeaponInfo().IsProjectile())
        {
            return "grip";
        }
        else
        {
            return "";
        }
    }

    const std::string ItemFactoryBase::SecondaryMaterialPhraseForBladedStaffs(
        const ItemProfile & PROFILE, const std::string & SECONDARY_MATERIAL_NAME)
    {
        std::ostringstream ss;
        ss << " ";

        if (material::IsJewel(PROFILE.MaterialSecondary()))
        {
            ss << RandomJeweledAdjective() << " with";
        }
        else if (PROFILE.MaterialSecondary() == material::Pearl)
        {
            ss << RandomAdornedAdjective() << " with";
        }
        else if (material::IsRigid(PROFILE.MaterialSecondary()))
        {
            if (PROFILE.WeaponType() & weapon_type::Spear)
            {
                ss << "and tipped with";
            }
            else
            {
                ss << "and bladed with";
            }
        }
        else if (material::IsLiquid(PROFILE.MaterialSecondary()))
        {
            ss << "and coated in";
        }
        else
        {
            ss << "and";
        }

        ss << " " << SECONDARY_MATERIAL_NAME;

        return ss.str();
    }

    const std::string ItemFactoryBase::SecondaryMaterialPhraseForClaspedArmor(
        const ItemProfile & PROFILE, const std::string & SECONDARY_MATERIAL_NAME)
    {
        std::ostringstream ss;

        if (material::IsJewel(PROFILE.MaterialSecondary()))
        {
            ss << " "
               << "with a " << SECONDARY_MATERIAL_NAME << " " << RandomJeweledAdjective() << " "
               << RandomClaspNoun();
        }
        else if (material::IsMetal(PROFILE.MaterialSecondary()))
        {
            ss << " with a " << SECONDARY_MATERIAL_NAME << " " << RandomChainNoun();
        }
        else if (material::IsLiquid(PROFILE.MaterialSecondary()))
        {
            ss << " " << RandomCoatedPhrase() << " " << SECONDARY_MATERIAL_NAME;
        }
        else if (material::IsSolid(PROFILE.MaterialSecondary()))
        {
            ss << " with a " << SECONDARY_MATERIAL_NAME << " " << RandomClaspNoun();
        }
        else if (material::IsSpirit(PROFILE.MaterialSecondary()))
        {
            ss << " with a " << RandomClaspNoun() << " that has a ghostly glow";
        }

        return ss.str();
    }

    const std::string ItemFactoryBase::SecondaryMaterialPhraseWithSpecificNoun(
        const ItemProfile & PROFILE,
        const std::string & SECONDARY_MATERIAL_NAME,
        const std::string & NOUN)
    {
        std::ostringstream ss;

        ss << " with a ";

        if (material::IsRigid(PROFILE.MaterialSecondary()))
        {
            ss << SECONDARY_MATERIAL_NAME;

            if (material::IsJewel(PROFILE.MaterialSecondary()))
            {
                ss << " " << RandomJeweledAdjective();
            }
            else if (material::IsPrecious(PROFILE.MaterialSecondary()))
            {
                // at this point we know MATERIAL_SEC is either
                // silver/gold/platinum/pearl/obsidian, one of the "adorned" materials
                ss << " " << RandomAdornedAdjective();
            }

            ss << " " << NOUN;
        }
        else
        {
            ss << SECONDARY_MATERIAL_NAME << " coated " << NOUN;
        }

        return ss.str();
    }

    const std::string ItemFactoryBase::SecondaryMaterialPhraseForItemsWithNonFleshEyes(
        const ItemProfile & PROFILE, const std::string & SECONDARY_MATERIAL_NAME)
    {
        if (material::IsSolid(PROFILE.MaterialSecondary())
            && (PROFILE.MaterialSecondary() != material::Flesh))
        {
            return " with " + SECONDARY_MATERIAL_NAME + " eyes";
        }
        else
        {
            return "";
        }
    }

    const std::string ItemFactoryBase::SecondaryMaterialPhraseForShields(
        const ItemProfile & PROFILE, const std::string & SECONDARY_MATERIAL_NAME)
    {
        std::ostringstream ss;

        if (material::IsJewel(PROFILE.MaterialSecondary()))
        {
            ss << " " << RandomJeweledAdjective() << " with " << SECONDARY_MATERIAL_NAME;
        }
        else if (material::IsRigid(PROFILE.MaterialSecondary()))
        {
            ss << " reinforced with " << SECONDARY_MATERIAL_NAME;
        }
        else if (material::IsLiquid(PROFILE.MaterialSecondary()))
        {
            ss << " " << RandomCoatedAdjective() << " with " << SECONDARY_MATERIAL_NAME;
        }

        return ss.str();
    }

} // namespace item
} // namespace heroespath
