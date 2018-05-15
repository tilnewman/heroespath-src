// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ITEMFACTORYBASE_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEMFACTORYBASE_HPP_INCLUDED
//
// item-factory-base.hpp
//
#include "item/item-type-enum.hpp"
#include "item/item-type-wrapper.hpp"
#include "item/weapon-types.hpp"
#include "misc/types.hpp"

#include <memory>
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

    // A base class for common operations of all item factories.
    class ItemFactoryBase
    {
    public:
        ItemFactoryBase(const ItemFactoryBase &) = delete;
        ItemFactoryBase(ItemFactoryBase &&) = delete;
        ItemFactoryBase & operator=(const ItemFactoryBase &) = delete;
        ItemFactoryBase & operator=(ItemFactoryBase &&) = delete;
        ItemFactoryBase() = delete;

    protected:
        static const std::string MakeWeaponBodyPartName(
            const creature::CreaturePtr_t CREATURE_PTR, const std::string & READABLE_NAME);

        static const std::string MakeWeaponBodyPartDescription(
            const std::string & BASE_DESCRIPTION, const creature::CreaturePtr_t CREATURE_PTR);

        static const std::string MakeArmorBodyPartName(
            const MaterialPair_t & MATERIALS_PAIR, const creature::CreaturePtr_t CREATURE_PTR);

        static const std::string
            MakeArmorBodyPartDescription(const MaterialPair_t & MATERIALS_PAIR);

        static const std::string MakeNonBodyPartName(const ItemProfile & PROFILE);

        static const std::string
            MakeNonBodyPartDescription(const ItemProfile & PROFILE, const std::string & BASE_DESC);

        static Coin_t
            CalculatePrice(const ItemProfile & PROFILE, const Coin_t BASE_PRICE_PARAM = 0_coin);

        static Weight_t CalculateWeight(
            const ItemProfile & PROFILE, const Weight_t BASE_WEIGHT_PARAM = 0_weight);

        static Armor_t CalculateArmorRating(
            const ItemProfile & PROFILE, const Armor_t & BASE_ARMOR_RATING = 0_armor);

        static Coin_t TreasureScoreToCoins(const Score_t &);

    private:
        static const std::string HandledNamePrefix(const ItemProfile &);
        static const std::string ReadableNameWithoutArmorBaseType(const ItemProfile &);
        static const std::string PrefixAOrAn(const material::Enum);
        static const std::string RandomCoatedPhrase();
        static const std::string RandomCoatedAdjective();
        static const std::string JeweledAdjective();
        static const std::string AdornedAdjective();
        static const std::string RandomClaspNoun();

        static bool IsNonEmptyWithoutTrailingSpace(std::ostringstream &);
        static const std::string AppendSpaceIfNeeded(std::ostringstream &);

        enum class PhraseType
        {
            Name,
            Desc
        };

        static const std::string ArmorBaseTypeNamePrefix(const ItemProfile & PROFILE);

        static void
            AppendBlessedOrCursedIfNeeded(const ItemProfile & PROFILE, std::ostringstream &);

        static void AppendPixiePhraseIfNeeded(
            const ItemProfile & PROFILE, const PhraseType PHRASE_TYPE, std::ostringstream &);

        static const std::string SeccondaryMaterialPhrase(const ItemProfile &);

        static const std::string SecondaryMaterialPhraseDecoration(
            const material::Enum SECONDARY_MATERIAL, const std::string & SECONDARY_MATERIAL_NAME);

        static const std::string SecondaryMaterialPhraseHandle(
            const material::Enum SECONDARY_MATERIAL, const std::string & SECONDARY_MATERIAL_NAME);

        static const std::string SecondaryMaterialPhraseReinforced(
            const material::Enum SECONDARY_MATERIAL, const std::string & SECONDARY_MATERIAL_NAME);

        static const std::string SecondaryMaterialPhraseTipped(
            const material::Enum SECONDARY_MATERIAL, const std::string & SECONDARY_MATERIAL_NAME);

        static const std::string SecondaryMaterialPhraseClasped(
            const material::Enum SECONDARY_MATERIAL, const std::string & SECONDARY_MATERIAL_NAME);
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEMFACTORYBASE_HPP_INCLUDED
