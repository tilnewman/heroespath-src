// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-name-factory.cpp
//
#include "item-name-factory.hpp"

#include "creature/creature.hpp"
#include "item/item-profile.hpp"
#include "item/item.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/random.hpp"
#include "misc/strings.hpp"

namespace heroespath
{
namespace item
{

    const std::string ItemNameFactory::MakeWeaponBodyPartName(
        const creature::CreaturePtr_t CREATURE_PTR, const std::string & READABLE_NAME) const
    {
        return CREATURE_PTR->RaceName() + " " + READABLE_NAME;
    }

    const std::string ItemNameFactory::MakeWeaponBodyPartDescription(
        const std::string & BASE_DESCRIPTION, const creature::CreaturePtr_t CREATURE_PTR) const
    {
        return BASE_DESCRIPTION + " " + creature::race::Name(CREATURE_PTR->Race());
    }

    const std::string ItemNameFactory::MakeArmorBodyPartName(
        const MaterialPair_t & MATERIALS_PAIR, const creature::CreaturePtr_t CREATURE_PTR) const
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
        ItemNameFactory::MakeArmorBodyPartDescription(const MaterialPair_t & MATERIALS_PAIR) const
    {
        std::ostringstream ss;
        ss << "tough skin made of " << material::Name(MATERIALS_PAIR.first);

        if (MATERIALS_PAIR.second != material::Nothing)
        {
            ss << " and " << material::Name(MATERIALS_PAIR.second);
        }

        return ss.str();
    }

    const std::string ItemNameFactory::MakeNonBodyPartName(const ItemProfile & PROFILE) const
    {
        std::ostringstream ss;

        AppendBlessedOrCursedIfNeeded(PROFILE, ss);

        if (PROFILE.IsSet() == false)
        {
            AppendPixiePhraseIfNeeded(PROFILE, PhraseType::Name, ss);
        }

        auto const ELEMENT_TYPE_STR { [&]() -> std::string {
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
            ss << AppendSpaceIfNeeded(ss) << set_type::Name(PROFILE.SetType()) << " ";

            if (PROFILE.IsPixie())
            {
                ss << "Pixie ";
            }

            ss << PROFILE.ReadableName();
        }
        else if (PROFILE.IsUnique())
        {
            ss << AppendSpaceIfNeeded(ss) << misc_type::Name(PROFILE.MiscType());
        }
        else
        {
            auto const NAME_TO_USE { [&]() {
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

            auto const IS_COVER_ARMOR_MADE_OF_CLOTH {
                (PROFILE.ArmorInfo().IsCover() && (PROFILE.MaterialSecondary() == material::Cloth))
            };

            if (IS_COVER_ARMOR_MADE_OF_CLOTH == false)
            {
                ss << AppendSpaceIfNeeded(ss) << material::Name(PROFILE.MaterialPrimary())
                   << ArmorBaseTypeNamePrefix(PROFILE) << HandledNamePrefix(PROFILE);
            }

            auto const SECONDARY_MATERIAL_PHRASE { SeccondaryMaterialPhrase(
                PhraseType::Name, PROFILE) };

            if (SECONDARY_MATERIAL_PHRASE.empty() && material::IsSolid(PROFILE.MaterialSecondary())
                && (IS_COVER_ARMOR_MADE_OF_CLOTH == false))
            {
                ss << AppendSpaceIfNeeded(ss) << "and "
                   << material::Name(PROFILE.MaterialSecondary());
            }

            ss << AppendSpaceIfNeeded(ss) << NAME_TO_USE << ELEMENT_TYPE_STR
               << SECONDARY_MATERIAL_PHRASE;
        }

        return ss.str();
    }

    const std::string ItemNameFactory::MakeNonBodyPartDescription(
        const ItemProfile & PROFILE, const std::string & BASE_DESC) const
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
            ss << ", from " << set_type::Name(PROFILE.SetType()) << " Set for "
               << creature::role::Name(set_type::Role(PROFILE.SetType())) << "s";
        }
        else if (PROFILE.IsNamed())
        {
            ss << ", known as the " << named_type::Name(PROFILE.NamedType()) << " "
               << ReadableNameWithoutArmorBaseType(PROFILE);
        }
        else if (PROFILE.IsUnique())
        {
            ss << ", known as a " << misc_type::Name(PROFILE.MiscType());
        }

        ss << ", made of " << FirstLetterLowercaseCopy(material::Name(PROFILE.MaterialPrimary()))
           << ArmorBaseTypeNamePrefix(PROFILE);

        auto const SECONDARY_MATERIAL_PHRASE { SeccondaryMaterialPhrase(
            PhraseType::Desc, PROFILE) };

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
            ss << ", and pulsing with " << element_type::Name(PROFILE.ElementType(), false)
               << " magic";
        }

        if (PROFILE.SummonInfo().CanSummon())
        {
            ss << ", used to summon a " << creature::race::Name(PROFILE.SummonInfo().Race()) << " "
               << creature::role::Name(PROFILE.SummonInfo().Role());
        }

        return ss.str();
    }

    const std::string ItemNameFactory::HandledNamePrefix(const ItemProfile & PROFILE) const
    {
        if ((PROFILE.WeaponInfo().WhipType() == weapon::whip_type::Bullwhip)
            || (PROFILE.WeaponInfo().ProjectileType() == weapon::projectile_type::Sling))
        {
            return "handled ";
        }
        else
        {
            return "";
        }
    }

    const std::string
        ItemNameFactory::ReadableNameWithoutArmorBaseType(const ItemProfile & PROFILE) const
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

    const std::string ItemNameFactory::PrefixAOrAn(const material::Enum MATERIAL) const
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

    const std::string ItemNameFactory::RandomCoatedPhrase() const
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

    const std::string ItemNameFactory::RandomCoatedAdjective() const
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

    const std::string ItemNameFactory::JeweledAdjective() const { return "jeweled"; }

    const std::string ItemNameFactory::AdornedAdjective() const { return "adorned"; }

    const std::string ItemNameFactory::RandomClaspNoun() const
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

    bool ItemNameFactory::IsNonEmptyWithoutTrailingSpace(std::ostringstream & ss) const
    {
        auto const CURRENT_STR { ss.str() };
        return ((CURRENT_STR.empty() == false) && (CURRENT_STR.at(CURRENT_STR.size() - 1) != ' '));
    }

    const std::string ItemNameFactory::AppendSpaceIfNeeded(std::ostringstream & ss) const
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

    void ItemNameFactory::AppendPixiePhraseIfNeeded(
        const ItemProfile & PROFILE, const PhraseType PHRASE_TYPE, std::ostringstream & ss) const
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
                ss << "pixie sized";
            }
        }
    }

    const std::string ItemNameFactory::ArmorBaseTypeNamePrefix(const ItemProfile & PROFILE) const
    {
        auto const BASE_TYPE { PROFILE.ArmorInfo().BaseType() };
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

    void ItemNameFactory::AppendBlessedOrCursedIfNeeded(
        const ItemProfile & PROFILE, std::ostringstream & ss) const
    {
        if (PROFILE.IsUnique() == false)
        {
            auto const BLESSED_OR_CURSED { [&]() -> std::string {
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

    const std::string ItemNameFactory::SeccondaryMaterialPhrase(
        const PhraseType PHRASE_TYPE, const ItemProfile & PROFILE) const
    {
        auto const SECONDARY_MATERIAL { PROFILE.MaterialSecondary() };

        auto const SECONDARY_MATERIAL_NAME { [&]() {
            // make lowercase if a Description and not a Name
            if (PHRASE_TYPE == PhraseType::Desc)
            {
                return FirstLetterLowercaseCopy(material::Name(SECONDARY_MATERIAL));
            }
            else
            {
                return material::Name(SECONDARY_MATERIAL);
            }
        }() };

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

    const std::string ItemNameFactory::SecondaryMaterialPhraseDecoration(
        const material::Enum SECONDARY_MATERIAL, const std::string & SECONDARY_MATERIAL_NAME) const
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

    const std::string ItemNameFactory::SecondaryMaterialPhraseHandle(
        const material::Enum SECONDARY_MATERIAL, const std::string & SECONDARY_MATERIAL_NAME) const
    {
        return " with " + PrefixAOrAn(SECONDARY_MATERIAL) + " " + SECONDARY_MATERIAL_NAME
            + " handle";
    }

    const std::string ItemNameFactory::SecondaryMaterialPhraseReinforced(
        const material::Enum SECONDARY_MATERIAL, const std::string & SECONDARY_MATERIAL_NAME) const
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

    const std::string ItemNameFactory::SecondaryMaterialPhraseTipped(
        const material::Enum, const std::string & SECONDARY_MATERIAL_NAME) const
    {
        return " tipped with " + SECONDARY_MATERIAL_NAME;
    }

    const std::string ItemNameFactory::SecondaryMaterialPhraseClasped(
        const material::Enum SECONDARY_MATERIAL, const std::string & SECONDARY_MATERIAL_NAME) const
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

    const std::string ItemNameFactory::FirstLetterLowercaseCopy(const std::string & ORIG_STR) const
    {
        std::string finalStr { ORIG_STR };

        if (finalStr.empty() == false)
        {
            misc::ToLower(finalStr.front());
        }

        return finalStr;
    }

} // namespace item
} // namespace heroespath
