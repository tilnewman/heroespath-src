// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ITEMFACTORY_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEMFACTORY_HPP_INCLUDED
//
// item-factory.hpp
//
#include "game/strong-types.hpp"
#include "item/item-name-factory.hpp"
#include "item/item-type-enum.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <string>

namespace heroespath
{

namespace stage
{
    struct IStage;
    using IStagePtr_t = misc::NotNull<IStage *>;
} // namespace stage

namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
} // namespace creature

namespace item
{
    class ItemProfile;

    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;

    // Responsible for making new (and properly warehousing) item objects from ItemProfiles.
    class ItemFactory
    {
    public:
        ItemFactory(const ItemFactory &) = delete;
        ItemFactory(ItemFactory &&) = delete;
        ItemFactory & operator=(const ItemFactory &) = delete;
        ItemFactory & operator=(ItemFactory &&) = delete;

        ItemFactory() = default;

        bool Test(stage::IStagePtr_t iStagePtr) const;

        const ItemPtr_t Make(const ItemProfile &) const;
        const ItemPtr_t Make(const body_part::Enum, const creature::CreaturePtr_t) const;

    private:
        void TestItem(const ItemPtr_t & ITEM_PTR, const ItemProfile & ITEM_PROFILE) const;

        const ItemPtr_t MakeArmor(const ItemProfile &) const;
        const ItemPtr_t MakeArmor(const body_part::Enum, const creature::CreaturePtr_t) const;

        const ItemPtr_t MakeWeapon(const ItemProfile &) const;
        const ItemPtr_t MakeWeapon(const body_part::Enum, const creature::CreaturePtr_t) const;

        const ItemPtr_t MakeMisc(const ItemProfile & PROFILE) const;

        Coin_t CalculatePrice(
            const ItemProfile & PROFILE, const Coin_t BASE_PRICE_PARAM = 0_coin) const;

        Weight_t CalculateWeight(
            const ItemProfile & PROFILE, const Weight_t BASE_WEIGHT_PARAM = 0_weight) const;

        Armor_t CalculateArmorRating(
            const ItemProfile & PROFILE, const Armor_t & BASE_ARMOR_RATING = 0_armor) const;

        Coin_t TreasureScoreToCoins(const Score_t &) const;

        ItemNameFactory nameFactory_;
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEMFACTORY_HPP_INCLUDED
