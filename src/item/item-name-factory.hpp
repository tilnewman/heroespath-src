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
#include "item/item-type-enum.hpp"
#include "misc/not-null.hpp"

#include <string>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
} // namespace creature
namespace item
{

    class ItemProfile;

    // A base class for common operations of all item factories.
    class ItemNameFactory
    {
    public:
        ItemNameFactory(const ItemNameFactory &) = delete;
        ItemNameFactory(ItemNameFactory &&) = delete;
        ItemNameFactory & operator=(const ItemNameFactory &) = delete;
        ItemNameFactory & operator=(ItemNameFactory &&) = delete;

        ItemNameFactory() = default;

        const std::string MakeWeaponBodyPartName(
            const creature::CreaturePtr_t CREATURE_PTR, const std::string & READABLE_NAME) const;

        const std::string MakeWeaponBodyPartDescription(
            const std::string & BASE_DESCRIPTION, const creature::CreaturePtr_t CREATURE_PTR) const;

        const std::string MakeArmorBodyPartName(
            const MaterialPair_t & MATERIALS_PAIR,
            const creature::CreaturePtr_t CREATURE_PTR) const;

        const std::string MakeArmorBodyPartDescription(const MaterialPair_t & MATERIALS_PAIR) const;

        const std::string MakeNonBodyPartName(const ItemProfile & PROFILE) const;

        const std::string MakeNonBodyPartDescription(
            const ItemProfile & PROFILE, const std::string & BASE_DESC) const;

    private:
        const std::string ReadableNameWithoutArmorBaseType(const ItemProfile &) const;
        const std::string PrefixAOrAn(const material::Enum) const;
        const std::string RandomCoatedPhrase() const;
        const std::string RandomCoatedAdjective() const;
        const std::string JeweledAdjective() const;
        const std::string AdornedAdjective() const;
        const std::string RandomClaspNoun() const;

        inline bool IsNonEmptyWithoutTrailingSpace(const std::string & S) const
        {
            return (!S.empty() && (S.back() != ' '));
        }

        inline const std::string SpaceIfNeeded(const std::string & S) const
        {
            if (IsNonEmptyWithoutTrailingSpace(S))
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

        const std::string ArmorBaseTypeNamePrefix(const ItemProfile & PROFILE) const;

        const std::string BlessedOrCursed(const ItemProfile & PROFILE) const;

        const std::string SeccondaryMaterialPhrase(const PhraseType, const ItemProfile &) const;

        const std::string SecondaryMaterialPhraseDecoration(
            const material::Enum SECONDARY_MATERIAL,
            const std::string & SECONDARY_MATERIAL_NAME) const;

        const std::string SecondaryMaterialPhraseHandle(
            const material::Enum SECONDARY_MATERIAL,
            const std::string & SECONDARY_MATERIAL_NAME) const;

        const std::string SecondaryMaterialPhraseReinforced(
            const material::Enum SECONDARY_MATERIAL,
            const std::string & SECONDARY_MATERIAL_NAME) const;

        const std::string SecondaryMaterialPhraseTipped(
            const material::Enum SECONDARY_MATERIAL,
            const std::string & SECONDARY_MATERIAL_NAME) const;

        const std::string SecondaryMaterialPhraseClasped(
            const material::Enum SECONDARY_MATERIAL,
            const std::string & SECONDARY_MATERIAL_NAME) const;

        const std::string FirstLetterLowercaseCopy(const std::string_view) const;
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEM_NAME_FACTORY_HPP_INCLUDED
