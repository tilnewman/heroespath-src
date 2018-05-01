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
#ifndef HEROESPATH_ITEM_ITEMFACTORYBASE_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEMFACTORYBASE_HPP_INCLUDED
//
// item-factory-base.hpp
//
#include "item/item-type-enum.hpp"
#include "item/item-type-wrapper.hpp"
#include "item/weapon-types.hpp"
#include "misc/types.hpp"
#include "sfml-util/size-enum.hpp"

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

        static bool DoesCreatureRequireSkinArmor(const creature::CreaturePtr_t);

    protected:
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

        static const std::string RandomCoatedPhrase();
        static const std::string RandomCoatedAdjective();
        static const std::string RandomJeweledAdjective();
        static const std::string RandomAdornedAdjective();
        static const std::string RandomChainNoun();
        static const std::string RandomClaspNoun();

    private:
        static bool IsNonEmptyWithoutTrailingSpace(std::ostringstream &);
        static void AppendSpaceIfNeeded(std::ostringstream &);
        static const std::string AppendSpaceIfNeededInline(std::ostringstream &);

        enum class PhraseType
        {
            Name,
            Desc
        };

        static void
            AppendBlessedOrCursedIfNeeded(const ItemProfile & PROFILE, std::ostringstream &);

        static void AppendPixiePhraseIfNeeded(
            const ItemProfile & PROFILE, const PhraseType PHRASE_TYPE, std::ostringstream &);

        static bool IsArmorItemWithClasp(const ItemProfile &);

        static const std::string
            SeccondaryMaterialPhraseWhenAfter(const ItemProfile &, const PhraseType);

        static const std::string SecondaryMaterialPhraseStandard(
            const ItemProfile &, const std::string & SECONDARY_MATERIAL_NAME);

        static const std::string SecondaryMaterialSpecificNoun(const ItemProfile &);

        static const std::string SecondaryMaterialPhraseForBladedStaffs(
            const ItemProfile &, const std::string & SECONDARY_MATERIAL_NAME);

        static const std::string SecondaryMaterialPhraseForClaspedArmor(
            const ItemProfile &, const std::string & SECONDARY_MATERIAL_NAME);

        static const std::string SecondaryMaterialPhraseWithSpecificNoun(
            const ItemProfile &,
            const std::string & SECONDARY_MATERIAL_NAME,
            const std::string & NOUN);

        static const std::string SecondaryMaterialPhraseForItemsWithNonFleshEyes(
            const ItemProfile &, const std::string & SECONDARY_MATERIAL_NAME);

        static const std::string SecondaryMaterialPhraseForShields(
            const ItemProfile &, const std::string & SECONDARY_MATERIAL_NAME);
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEMFACTORYBASE_HPP_INCLUDED
