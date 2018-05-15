// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_NONPLAYER_INVENTORYFACTORY_HPP_INCLUDED
#define HEROESPATH_NONPLAYER_INVENTORYFACTORY_HPP_INCLUDED
//
// inventory-factory.hpp
//  Code responsible for creating non-player-characters items. (clothes/weapons/armor/jewelry/etc)
//
#include "item/item-factory.hpp"
#include "item/item-type-enum.hpp"
#include "item/item-warehouse.hpp"
#include "misc/not-null.hpp"
#include "misc/vector-map.hpp"
#include "non-player/ownership-chance-types.hpp"
#include "stats/trait.hpp"

#include <utility>
#include <vector>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
} // namespace creature
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
    using ItemPVec_t = std::vector<ItemPtr_t>;
} // namespace item
namespace non_player
{
    namespace ownership
    {

        // the type this factory returns, first=items to equip, second=items NOT to equip
        using IItemPVecPair_t = std::pair<item::ItemPVec_t, item::ItemPVec_t>;

        // used by InventoryFactory
        using MaterialChanceMap_t = misc::VectorMap<item::material::Enum, float>;

        // Responsible for creating sets of items that will equip non-player creatures.
        class InventoryFactory
        {
        public:
            InventoryFactory(const InventoryFactory &) = delete;
            InventoryFactory(InventoryFactory &&) = delete;
            InventoryFactory & operator=(const InventoryFactory &) = delete;
            InventoryFactory & operator=(InventoryFactory &&) = delete;

            InventoryFactory() = default;

            void SetupCreatureInventory(const creature::CreaturePtr_t) const;

            const IItemPVecPair_t MakeItemSet(
                const chance::InventoryChances & CHANCES,
                const creature::CreaturePtr_t CHARACTER_PTR) const;

        private:
            const IItemPVecPair_t MakeItemSet_Clothing(
                const chance::ClothingChances & CHANCES, const bool IS_PIXIE) const;

            const IItemPVecPair_t MakeItemSet_Weapons(
                const chance::WeaponChances & CHANCES,
                const creature::CreaturePtr_t CHARACTER_PTR) const;

            const IItemPVecPair_t MakeItemSet_Armor(
                const chance::ArmorChances & CHANCES,
                const creature::CreaturePtr_t CHARACTER_PTR,
                const bool HAS_TWO_HANDED_WEAPON_EQUIPPED) const;

            const item::ItemPVec_t MakeItemSet_BodyWeapons(
                const chance::WeaponChances & CHANCES,
                const creature::CreaturePtr_t CHARACTER_PTR,
                const bool HAS_TWO_HANDED_WEAPON_EQUIPPED) const;

            Coin_t Make_Coins(const chance::InventoryChances & CHANCES) const;

            bool ContainsTwoHandedWeapon(const item::ItemPVec_t & WEAPON_VEC) const;

            // Returns the chanceTotal so that the weapon type can be compared to other types based
            // on the total chance instead of the specific chance.  For example, the chance that one
            // particular kind of sword will be chosen among other kinds of swords is selected here,
            // but we return the chance for all swords totaled together so that this selected sword
            // can be selected among other weapon types, such as axes/clubs/whips/etc.
            template <typename T>
            const std::pair<T, float>
                RandomSelectWeapon(const misc::VectorMap<T, chance::ItemChances> & MAP) const
            {
                // TOOD handle multiple weapons

                // TODO handle unequipped items

                // find sum of all chances of this type, and the item with the highest chance
                T highestChanceItem(T::Count);
                auto highestChance{ 0.0f };
                auto chanceTotal{ 0.0f };
                for (auto const & NEXT_CHANCE_PAIR : MAP)
                {
                    auto nextChanceVal{ 0.0f };
                    NEXT_CHANCE_PAIR.second.num_owned_map.Find(1, nextChanceVal);

                    chanceTotal += nextChanceVal;
                    if (nextChanceVal > highestChance)
                    {
                        highestChance = nextChanceVal;
                        highestChanceItem = NEXT_CHANCE_PAIR.first;
                    }
                }

                // if zero, then there was no chance of an item of this type
                if (misc::IsRealClose(chanceTotal, 0.0f))
                {
                    return std::make_pair(T::Count, 0.0f);
                }

                auto const RAND{ misc::random::Float(0.0f, chanceTotal) };

                // random selection of one of the items
                auto chanceCumulative{ 0.0f };
                for (auto const & NEXT_CHANCE_PAIR : MAP)
                {
                    auto nextChanceVal{ 0.0f };
                    NEXT_CHANCE_PAIR.second.num_owned_map.Find(1, nextChanceVal);

                    chanceCumulative += nextChanceVal;
                    if (RAND < chanceCumulative)
                    {
                        return std::make_pair(NEXT_CHANCE_PAIR.first, nextChanceVal);
                    }
                }

                // There is a slim chance that none will be selected,
                // and in that case select the item with the highest chance.
                return std::make_pair(highestChanceItem, highestChance);
            }

            void RemoveArmorTypeFromVecAndFree(
                const item::armor_type::Enum ENUM, item::ItemPVec_t & vec) const;

            template <typename T>
            void RemoveItemsAndFree(item::ItemPVec_t & pVec, T & matchingLambda) const
            {
                item::ItemPVec_t itemsToFreePVec;

                std::copy_if(
                    std::begin(pVec),
                    std::end(pVec),
                    std::back_inserter(itemsToFreePVec),
                    matchingLambda);

                pVec.erase(
                    std::remove_if(std::begin(pVec), std::end(pVec), matchingLambda),
                    std::end(pVec));

                item::ItemWarehouse::Access().Free(itemsToFreePVec);
            }

            item::ItemFactory itemFactory_;
        };

    } // namespace ownership
} // namespace non_player
} // namespace heroespath

#endif // HEROESPATH_NONPLAYER_INVENTORYFACTORY_HPP_INCLUDED
