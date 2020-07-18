// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ITEM_NAME_FACTORY_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEM_NAME_FACTORY_HPP_INCLUDED
//
// item-name-factory.hpp
//
#include "item/armor-enum.hpp"
#include "item/material-enum.hpp"
#include "item/misc-enum.hpp"

#include <string>
#include <string_view>

namespace heroespath
{
namespace item
{

    class ItemProfile;

    // A base class for common operations of all item factories.
    struct ItemNameFactory
    {
        ItemNameFactory() = delete;

        static const std::string
            MakeName(const ItemProfile & PROFILE, const std::string_view CREATURE_RACE_NAME);

        static const std::string
            MakeDescription(const ItemProfile & PROFILE, const std::string_view CREATURE_RACE_NAME);

    private:
        static const std::string MakeWeaponBodyPartName(
            const std::string_view READABLE_NAME, const std::string_view CREATURE_RACE_NAME);

        static const std::string MakeWeaponBodyPartDescription(
            const std::string_view BASE_DESCRIPTION, const std::string_view CREATURE_RACE_NAME);

        static const std::string MakeArmorBodyPartName(
            const MaterialPair_t & MATERIALS_PAIR, const std::string_view CREATURE_RACE_NAME);

        static const std::string
            MakeArmorBodyPartDescription(const MaterialPair_t & MATERIALS_PAIR);

        static const std::string MakeNonBodyPartName(const ItemProfile & PROFILE);

        static const std::string MakeNonBodyPartDescription(const ItemProfile & PROFILE);

        static const std::string ReadableNameWithoutArmorForm(const ItemProfile &);
        static const std::string_view PrefixAOrAn(const Material::Enum);
        static const std::string_view RandomCoatedAdjective();

        static inline const std::string_view JeweledAdjective() { return "jeweled"; }
        static inline const std::string_view AdornedAdjective() { return "adorned"; }

        static inline bool IsNonEmptyWithoutTrailingSpace(const std::string_view STR)
        {
            return (!STR.empty() && (STR.back() != ' '));
        }

        static inline const std::string SpaceIfNeeded(const std::string_view STR)
        {
            if (IsNonEmptyWithoutTrailingSpace(STR))
            {
                return " ";
            }
            else
            {
                return "";
            }
        }

        enum class PhraseType
        {
            Name,
            Desc
        };

        static const std::string ArmorFormNamePrefix(const Forms::Enum FORM);
        static const std::string_view BlessedOrCursed(const Misc::Enum MISC);
        static const std::string SeccondaryMaterialPhrase(const PhraseType, const ItemProfile &);

        static const std::string SecondaryMaterialPhraseDecoration(
            const Material::Enum MAT_SEC, const std::string_view MAT_SEC_NAME);

        static const std::string SecondaryMaterialPhraseHandle(
            const Material::Enum MAT_SEC, const std::string_view MAT_SEC_NAME);

        static const std::string SecondaryMaterialPhraseReinforced(
            const Material::Enum MAT_SEC, const std::string_view MAT_SEC_NAME);

        static const std::string SecondaryMaterialPhraseTipped(
            const Material::Enum MAT_SEC, const std::string_view MAT_SEC_NAME);

        static const std::string SecondaryMaterialPhraseClasped(
            const Material::Enum MAT_SEC, const std::string_view MAT_SEC_NAME);

        static const std::string SecondaryMaterialPhraseMisc(
            const Material::Enum MAT_SEC,
            const std::string_view MAT_SEC_NAME,
            const Misc::Enum MISC);

        static const std::string FirstLetterLowercaseCopy(const std::string_view);

        static const std::string SeccondaryMaterialPhraseForSolid(
            const MaterialNameStyle::Enum MAT_NAME_STYLE,
            const Material::Enum MAT_SEC,
            const std::string_view MAT_SEC_NAME,
            const Misc::Enum MISC);
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEM_NAME_FACTORY_HPP_INCLUDED
