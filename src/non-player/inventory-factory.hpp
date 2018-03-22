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
#ifndef HEROESPATH_NONPLAYER_INVENTORYFACTORY_HPP_INCLUDED
#define HEROESPATH_NONPLAYER_INVENTORYFACTORY_HPP_INCLUDED
//
// inventory-factory.hpp
//  Code responsible for creating non-player-characters items. (clothes/weapons/armor/jewelry/etc)
//
#include "item/item-type-enum.hpp"
#include "misc/vector-map.hpp"
#include "non-player/ownership-chance-types.hpp"
#include "stats/trait.hpp"

#include <utility>
#include <vector>

namespace heroespath
{
namespace item
{
    class Item;
    using ItemPtr_t = Item *;
    using ItemPVec_t = std::vector<ItemPtr_t>;
} // namespace item
namespace non_player
{

    class Character;
    using CharacterPtr_t = Character *;

    namespace ownership
    {

        // the type this factory returns, first=items to equip, second=items NOT to equip
        using IItemPVecPair_t = std::pair<item::ItemPVec_t, item::ItemPVec_t>;

        // used by InventoryFactory
        using MaterialChanceMap_t = misc::VectorMap<item::material::Enum, float>;

        // A singleton class that is responsible for creating sets of items
        // that will equip non-player characters.
        class InventoryFactory
        {
        public:
            InventoryFactory(const InventoryFactory &) = delete;
            InventoryFactory(InventoryFactory &&) = delete;
            InventoryFactory & operator=(const InventoryFactory &) = delete;
            InventoryFactory & operator=(InventoryFactory &&) = delete;

            InventoryFactory();
            ~InventoryFactory();

            static InventoryFactory * Instance();
            static void Acquire();
            static void Release();

            static void SetupCreatureInventory(non_player::CharacterPtr_t creaturePtr);

            static const IItemPVecPair_t MakeItemSet(
                const chance::InventoryChances & CHANCES,
                const non_player::CharacterPtr_t CHARACTER_PTR);

        private:
            static const IItemPVecPair_t
                MakeItemSet_Clothing(const chance::ClothingChances & CHANCES);

            static const IItemPVecPair_t MakeItemSet_Weapons(
                const chance::WeaponChances & CHANCES,
                const non_player::CharacterPtr_t CHARACTER_PTR);

            static const IItemPVecPair_t MakeItemSet_Armor(
                const chance::ArmorChances & CHANCES,
                const non_player::CharacterPtr_t CHARACTER_PTR,
                const bool HAS_TWO_HANDED_WEAPON_EQUIPPED);

            static const item::ItemPVec_t MakeItemSet_BodyWeapons(
                const chance::WeaponChances & CHANCES,
                const non_player::CharacterPtr_t CHARACTER_PTR,
                const bool HAS_TWO_HANDED_WEAPON_EQUIPPED);

            static Coin_t Make_Coins(const chance::InventoryChances & CHANCES);

            static bool ContainsTwoHandedWeapon(const item::ItemPVec_t & WEAPON_VEC);

            // Returns the chanceTotal so that the weapon type can be compared to other types based
            // on the total chance instead of the specific chance.  For example, the chance that one
            // particular kind of sword will be chosen among other kinds of swords is selected here,
            // but we return the chance for all swords totaled together so that this selected sword
            // can be selected among other weapon types, such as axes/clubs/whips/etc.
            template <typename T>
            static const std::pair<T, float>
                RandomSelectWeapon(const misc::VectorMap<T, chance::ItemChances> & MAP)
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

            static void
                RemoveArmorTypeFromVec(const item::armor_type::Enum ENUM, item::ItemPVec_t & vec);

        private:
            static std::unique_ptr<InventoryFactory> instanceUPtr_;
        };
    } // namespace ownership
} // namespace non_player
} // namespace heroespath

#endif // HEROESPATH_NONPLAYER_INVENTORYFACTORY_HPP_INCLUDED
