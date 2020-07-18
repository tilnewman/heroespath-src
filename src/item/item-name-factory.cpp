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

#include "item/item-profile.hpp"
#include "misc/random.hpp"
#include "misc/strings.hpp"

namespace heroespath
{
namespace item
{

    const std::string ItemNameFactory::MakeName(
        const ItemProfile & PROFILE, const std::string_view CREATURE_RACE_NAME)
    {
        if (PROFILE.IsWeapon() && PROFILE.IsBodyPart())
        {
            return ItemNameFactory::MakeWeaponBodyPartName(
                PROFILE.ReadableName(), CREATURE_RACE_NAME);
        }

        if (PROFILE.IsArmor() && PROFILE.IsBodyPart())
        {
            const auto MATERIALS_PAIR
                = std::make_pair(PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary());

            return ItemNameFactory::MakeArmorBodyPartName(MATERIALS_PAIR, CREATURE_RACE_NAME);
        }

        return ItemNameFactory::MakeNonBodyPartName(PROFILE);
    }

    const std::string ItemNameFactory::MakeDescription(
        const ItemProfile & PROFILE, const std::string_view CREATURE_RACE_NAME)
    {
        if (PROFILE.IsWeapon() && PROFILE.IsBodyPart())
        {
            return ItemNameFactory::MakeWeaponBodyPartDescription(
                PROFILE.BaseDescription(), CREATURE_RACE_NAME);
        }

        if (PROFILE.IsArmor() && PROFILE.IsBodyPart())
        {
            const auto MATERIALS_PAIR
                = std::make_pair(PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary());

            return ItemNameFactory::MakeArmorBodyPartDescription(MATERIALS_PAIR);
        }

        return ItemNameFactory::MakeNonBodyPartDescription(PROFILE);
    }

    const std::string ItemNameFactory::MakeWeaponBodyPartName(
        const std::string_view READABLE_NAME, const std::string_view CREATURE_RACE_NAME)
    {
        std::string str;
        str.reserve(READABLE_NAME.size() + CREATURE_RACE_NAME.size() + 1);
        str += CREATURE_RACE_NAME;
        str += ' ';
        str += READABLE_NAME;
        return str;
    }

    const std::string ItemNameFactory::MakeWeaponBodyPartDescription(
        const std::string_view BASE_DESCRIPTION, const std::string_view CREATURE_RACE_NAME)
    {
        std::string str;
        str.reserve(BASE_DESCRIPTION.size() + CREATURE_RACE_NAME.size() + 1);
        str += BASE_DESCRIPTION;
        str += ' ';
        str += CREATURE_RACE_NAME;
        return str;
    }

    const std::string ItemNameFactory::MakeArmorBodyPartName(
        const MaterialPair_t & MATERIALS_PAIR, const std::string_view CREATURE_RACE_NAME)
    {
        std::string name;
        name.reserve(64);

        name += CREATURE_RACE_NAME;
        name += "'s ";

        if (MATERIALS_PAIR.second < Material::Count)
        {
            name += Material::Name(MATERIALS_PAIR.second);
            name += " covered ";
        }

        name += Material::Name(MATERIALS_PAIR.first);

        return name;
    }

    const std::string
        ItemNameFactory::MakeArmorBodyPartDescription(const MaterialPair_t & MATERIALS_PAIR)
    {
        std::string desc;
        desc.reserve(64);

        desc += "tough skin made of ";
        desc += Material::Name(MATERIALS_PAIR.first);

        if (MATERIALS_PAIR.second < Material::Count)
        {
            desc += " and ";
            desc += Material::Name(MATERIALS_PAIR.second);
        }

        return desc;
    }

    const std::string ItemNameFactory::MakeNonBodyPartName(const ItemProfile & PROFILE)
    {
        std::string name;
        name.reserve(64);

        name += BlessedOrCursed(PROFILE.MiscType());

        if (!PROFILE.IsSet())
        {
            if (PROFILE.IsPixie())
            {
                name += SpaceIfNeeded(name);
                name += "Pixie";
            }
        }

        if (PROFILE.IsSet())
        {
            name += SpaceIfNeeded(name);
            name += Set::Name(PROFILE.SetType());
            name += ' ';

            if (PROFILE.IsPixie())
            {
                name += "Pixie ";
            }

            name += PROFILE.ReadableName();
        }
        else if (PROFILE.IsUnique())
        {
            name += SpaceIfNeeded(name);
            name += Misc::Name(PROFILE.MiscType());
        }
        else
        {
            const auto IS_COVER_ARMOR_MADE_OF_CLOTH {
                (PROFILE.ArmorInfo().IsCover() && (PROFILE.MaterialSecondary() == Material::Cloth))
            };

            if (!IS_COVER_ARMOR_MADE_OF_CLOTH)
            {
                name += SpaceIfNeeded(name);
                name += Material::Name(PROFILE.MaterialPrimary());
                name += ArmorFormNamePrefix(PROFILE.ArmorInfo().Form());

                // prefix with the word 'handled' if it is a handled weapon
                if (PROFILE.WeaponInfo().IsMinor<Whips>(Whips::Bullwhip)
                    || PROFILE.WeaponInfo().IsMinor<Projectiles>(Projectiles::Sling))
                {
                    name += "handled ";
                }
            }

            const auto MAT_SEC_PHRASE { SeccondaryMaterialPhrase(PhraseType::Name, PROFILE) };

            if (MAT_SEC_PHRASE.empty() && Material::IsSolid(PROFILE.MaterialSecondary())
                && !IS_COVER_ARMOR_MADE_OF_CLOTH)
            {
                name += SpaceIfNeeded(name);
                name += "and ";
                name += Material::Name(PROFILE.MaterialSecondary());
            }

            name += SpaceIfNeeded(name);

            if (PROFILE.IsNamed())
            {
                name += Named::Name(PROFILE.NamedType());
                name += ' ';
            }

            name += ReadableNameWithoutArmorForm(PROFILE);

            if (PROFILE.IsElemental())
            {
                name += ' ';
                name += Element::Name(PROFILE.ElementType());
            }

            name += MAT_SEC_PHRASE;
        }

        return name;
    }

    const std::string ItemNameFactory::MakeNonBodyPartDescription(const ItemProfile & PROFILE)
    {
        std::string desc;
        desc.reserve(128);

        // If non-BodyPart and non-Misc, then BASE_DESC comes either from
        // WeaponDetails.description or ArmorDetails.description, which needs an "A " put in
        // front of it.  However, this is handled by Item::Description() so that it can prepend
        // "magical" if needed.  Only Item class knows if an item is magical because only it has
        // information on Enchantments, which are not in ItemProfile.  This Item::Description()
        // function is also responsible for putting a period at the end.

        desc += BlessedOrCursed(PROFILE.MiscType());

        if (PROFILE.IsPixie())
        {
            desc += SpaceIfNeeded(desc) + "pixie sized";
        }

        desc += SpaceIfNeeded(desc);
        desc += PROFILE.ReadableName();

        if (PROFILE.IsSet())
        {
            desc += ", from ";
            desc += Set::Name(PROFILE.SetType());
            desc += " Set for ";
            desc += creature::role::Name(Set::RoleRestriction(PROFILE.SetType()));
            desc += 's';
        }
        else if (PROFILE.IsNamed())
        {
            desc += ", known as the ";
            desc += Named::Name(PROFILE.NamedType());
            desc += ' ';
            desc += ReadableNameWithoutArmorForm(PROFILE);
        }
        else if (PROFILE.IsUnique())
        {
            desc += ", known as a ";
            desc += Misc::Name(PROFILE.MiscType());
        }

        desc += ", made of ";
        desc += FirstLetterLowercaseCopy(Material::Name(PROFILE.MaterialPrimary()));
        desc += ArmorFormNamePrefix(PROFILE.ArmorInfo().Form());

        const auto MAT_SEC_PHRASE { SeccondaryMaterialPhrase(PhraseType::Desc, PROFILE) };

        if (MAT_SEC_PHRASE.empty() == false)
        {
            desc += MAT_SEC_PHRASE;
        }
        else if (Material::IsSolid(PROFILE.MaterialSecondary()))
        {
            desc += " and ";
            desc += Material::Name(PROFILE.MaterialSecondary());
        }

        if (PROFILE.IsElemental())
        {
            desc += ", and pulsing with ";
            desc += Element::Name(PROFILE.ElementType(), heroespath::EnumStringHow(), false);
            desc += " magic";
        }

        const auto & SUMMON_INFO { PROFILE.SummoningInfo() };

        if (SUMMON_INFO.IsValid())
        {
            desc += ", used to summon a ";
            desc += creature::race::Name(SUMMON_INFO.Race());
            desc += ' ';
            desc += creature::role::Name(SUMMON_INFO.Role());
        }

        return desc;
    }

    const std::string ItemNameFactory::ReadableNameWithoutArmorForm(const ItemProfile & PROFILE)
    {
        if (PROFILE.IsArmor())
        {
            return PROFILE.ArmorInfo().ReadableNameWithoutForm();
        }
        else
        {
            return PROFILE.ReadableName();
        }
    }

    const std::string_view ItemNameFactory::PrefixAOrAn(const Material::Enum MATERIAL)
    {
        if (Material::RequiresAnPrefix(MATERIAL))
        {
            return "an";
        }
        else
        {
            return "a";
        }
    }

    const std::string_view ItemNameFactory::RandomCoatedAdjective()
    {
        switch (misc::Random(3))
        {
            case 0: return "covered";
            case 1: return "soaked";
            case 2: return "coated";
            case 3:
            default: return "drenched";
        }
    }

    const std::string ItemNameFactory::ArmorFormNamePrefix(const Forms::Enum FORM)
    {
        std::string str;

        if ((FORM == Forms::Mail) || (FORM == Forms::Plate))
        {
            str = " ";
            str += NAMEOF_ENUM(FORM);
        }

        return str;
    }

    const std::string_view ItemNameFactory::BlessedOrCursed(const Misc::Enum MISC)
    {
        if (!Misc::IsUnique(MISC))
        {
            if (Misc::IsBlessed(MISC))
            {
                return "Blessed";
            }
            else if (Misc::IsCursed(MISC))
            {
                return "Cursed";
            }
        }

        return "";
    }

    const std::string ItemNameFactory::SeccondaryMaterialPhrase(
        const PhraseType PHRASE_TYPE, const ItemProfile & PROFILE)
    {
        std::string str;

        const auto MAT_SEC { PROFILE.MaterialSecondary() };

        if ((MAT_SEC >= Material::Count) || Material::IsGas(MAT_SEC))
        {
            return str;
        }

        str.reserve(32);

        const auto MAT_SEC_NAME { [&]() {
            // make lowercase if a Description and not a Name
            if (PHRASE_TYPE == PhraseType::Desc)
            {
                return FirstLetterLowercaseCopy(Material::Name(MAT_SEC));
            }
            else
            {
                return std::string(Material::Name(MAT_SEC));
            }
        }() };

        if (Material::IsLiquid(MAT_SEC))
        {
            str += ' ';
            str += RandomCoatedAdjective();
            str += " with ";
            str += MAT_SEC_NAME;
        }
        else if (Material::IsSpirit(MAT_SEC))
        {
            str += " with a ghostly glow";
        }
        else
        {
            // at this point the material must be solid
            str += SeccondaryMaterialPhraseForSolid(
                PROFILE.GetMaterialNameStyle(), MAT_SEC, MAT_SEC_NAME, PROFILE.MiscType());
        }

        return str;
    }

    const std::string ItemNameFactory::SeccondaryMaterialPhraseForSolid(
        const MaterialNameStyle::Enum MAT_NAME_STYLE,
        const Material::Enum MAT_SEC,
        const std::string_view MAT_SEC_NAME,
        const Misc::Enum MISC)
    {
        switch (MAT_NAME_STYLE)
        {
            case MaterialNameStyle::Decoration:
                return SecondaryMaterialPhraseDecoration(MAT_SEC, MAT_SEC_NAME);

            case MaterialNameStyle::Handle:
                return SecondaryMaterialPhraseHandle(MAT_SEC, MAT_SEC_NAME);

            case MaterialNameStyle::Reinforced:
            case MaterialNameStyle::ReinforcedWithBase:
                return SecondaryMaterialPhraseReinforced(MAT_SEC, MAT_SEC_NAME);

            case MaterialNameStyle::Tipped:
                return SecondaryMaterialPhraseTipped(MAT_SEC, MAT_SEC_NAME);

            case MaterialNameStyle::Clasped:
            case MaterialNameStyle::ClaspedWithBase:
                return SecondaryMaterialPhraseClasped(MAT_SEC, MAT_SEC_NAME);

            case MaterialNameStyle::Misc:
                return SecondaryMaterialPhraseMisc(MAT_SEC, MAT_SEC_NAME, MISC);

            case MaterialNameStyle::BodyPart:
            case MaterialNameStyle::Count:
            default: return "";
        }
    }

    const std::string ItemNameFactory::SecondaryMaterialPhraseMisc(
        const Material::Enum MAT_SEC, const std::string_view MAT_SEC_NAME, const Misc::Enum MISC)
    {
        std::string str;

        if (Misc::HasNonFleshEyes(MISC) && (MAT_SEC != Material::DriedFlesh))
        {
            str.reserve(16);
            str += " with ";
            str += MAT_SEC_NAME;
            str += " eyes";
        }

        return str;
    }

    const std::string ItemNameFactory::SecondaryMaterialPhraseDecoration(
        const Material::Enum MAT_SEC, const std::string_view MAT_SEC_NAME)
    {
        std::string str;
        str.reserve(16);

        if (Material::IsFancyJewel(MAT_SEC))
        {
            str += " ";
            str += JeweledAdjective();
            str += " with ";
            str += MAT_SEC_NAME;
        }
        else if (Material::IsFancyMetal(MAT_SEC))
        {
            str += " with ";
            str += MAT_SEC_NAME;
            str += " filigree";
        }
        else if (Material::IsFancyOpaque(MAT_SEC) || Material::IsFancyTribal(MAT_SEC))
        {
            str += " ";
            str += AdornedAdjective();
            str += " with ";
            str += MAT_SEC_NAME;
        }

        return str;
    }

    const std::string ItemNameFactory::SecondaryMaterialPhraseHandle(
        const Material::Enum MAT_SEC, const std::string_view MAT_SEC_NAME)
    {
        std::string str;
        str.reserve(32);
        str += " with ";
        str += PrefixAOrAn(MAT_SEC);
        str += " ";
        str += MAT_SEC_NAME;
        str += " handle";
        return str;
    }

    const std::string ItemNameFactory::SecondaryMaterialPhraseReinforced(
        const Material::Enum MAT_SEC, const std::string_view MAT_SEC_NAME)
    {
        std::string str;
        str.reserve(32);

        if (Material::IsMetal(MAT_SEC))
        {
            str += " reinforced with ";
            str += MAT_SEC_NAME;
        }
        else
        {
            str += SecondaryMaterialPhraseDecoration(MAT_SEC, MAT_SEC_NAME);
        }

        return str;
    }

    const std::string ItemNameFactory::SecondaryMaterialPhraseTipped(
        const Material::Enum, const std::string_view MAT_SEC_NAME)
    {
        std::string str;
        str.reserve(32);
        str += " tipped with ";
        str += MAT_SEC_NAME;
        return str;
    }

    const std::string ItemNameFactory::SecondaryMaterialPhraseClasped(
        const Material::Enum MAT_SEC, const std::string_view MAT_SEC_NAME)
    {
        std::string str;
        str.reserve(64);

        if (Material::IsFancyJewel(MAT_SEC))
        {
            str += " with ";
            str += PrefixAOrAn(MAT_SEC);
            str += " ";
            str += MAT_SEC_NAME;
            str += " ";
            str += JeweledAdjective();
            str += " clasp";
        }
        else if (Material::IsFancyOpaque(MAT_SEC) || Material::IsFancyTribal(MAT_SEC))
        {
            str += " with ";
            str += PrefixAOrAn(MAT_SEC);
            str += " ";
            str += MAT_SEC_NAME;
            str += " ";
            str += AdornedAdjective();
            str += " clasp";
        }
        else
        {
            str += " with ";
            str += PrefixAOrAn(MAT_SEC);
            str += " ";
            str += MAT_SEC_NAME;
            str += " clasp";
        }

        return str;
    }

    const std::string ItemNameFactory::FirstLetterLowercaseCopy(const std::string_view ORIG_STR)
    {
        std::string finalStr { ORIG_STR };

        if (!finalStr.empty())
        {
            misc::ToLower(finalStr.front());
        }

        return finalStr;
    }

} // namespace item
} // namespace heroespath
