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
#ifndef HEROESPATH_ITEM_INVENTORY_HPP_INCLUDED
#define HEROESPATH_ITEM_INVENTORY_HPP_INCLUDED
//
// inventory.hpp
//  A class that encapsolates a collection of Items.
//
#include "misc/boost-serialize-includes.hpp"

#include "misc/types.hpp"
#include "stats/trait.hpp"
#include "item/item-type-enum.hpp"

#include <string>
#include <memory>
#include <vector>
#include <sstream>


namespace heroespath
{
namespace item
{

    //foward declarations
    class Item;
    using ItemPtr_t = Item *;
    using ItemPVec_t = std::vector<ItemPtr_t>;


    //A class that encapsolates a collection of Items.
    class Inventory
    {
    public:
        explicit Inventory(const Coin_t &        COINS               = 0_coin,
                           const MeteorShard_t & METEOR_SHARDS       = 0_mshard,
                           const Gem_t &         GEMS                = 0_gem,
                           const ItemPVec_t &    ITEMS_PVEC          = ItemPVec_t(),
                           const ItemPVec_t &    EQUIPPED_ITEMS_PVEC = ItemPVec_t());

        ~Inventory();

        inline Coin_t Coins() const { return coins_; }
        inline MeteorShard_t MeteorShards() const { return meteorShards_; }
        inline Gem_t Gems() const { return gems_; }

        //these functions return false if attempt to reduce below zero
        bool CoinsAdj(const Coin_t &);
        bool MeteorShardsAdj(const MeteorShard_t &);
        bool GemsAdj(const Gem_t &);

        inline const ItemPVec_t Items() const           { return itemsPVec_; }
        inline const ItemPVec_t ItemsEquipped() const   { return equippedItemsPVec_; }

        void ItemAdd(const ItemPtr_t);
        void ItemRemove(const ItemPtr_t);

        //moves the item from itemsSVec_ to equippedItemsSVec_
        void ItemEquip(const ItemPtr_t);

        //moves the item from equippedItemsSVec_ to itemsSVec_
        void ItemUnEquip(const ItemPtr_t);

        bool ContainsItem(const ItemPtr_t) const;

        Weight_t Weight() const;

        std::size_t Count() const;

        std::size_t CountItemOfArmorType(const item::armor_type::Enum) const;
        std::size_t CountItemOfArmorTypeHeld(const item::armor_type::Enum) const;
        std::size_t CountItemOfArmorTypeEquipped(const item::armor_type::Enum) const;

        std::size_t CountItemOfWeaponType(const item::armor_type::Enum) const;
        std::size_t CountItemOfWeaponTypeHeld(const item::armor_type::Enum) const;
        std::size_t CountItemOfWeaponTypeEquipped(const item::armor_type::Enum) const;

        std::size_t CountItemOfCategory(const item::category::Enum) const;
        std::size_t CountItemOfCategoryHeld(const item::category::Enum) const;
        std::size_t CountItemOfCategoryEquipped(const item::category::Enum) const;

        std::size_t CountItemOfMiscType(const item::misc_type::Enum) const;
        std::size_t CountItemOfMiscTypeHeld(const item::misc_type::Enum) const;
        std::size_t CountItemOfMiscTypeEquipped(const item::misc_type::Enum) const;

        Armor_t ArmorRating() const;

        const std::string ToString() const;

        void StoreItemsInWarehouseAfterLoad();

        friend bool operator==(const Inventory & L, const Inventory & R);

    protected:
        void StoreItemPVecInWarehouse(ItemPVec_t &) const;
        void FreeAllItemsFromWarehouse();

    private:
        Coin_t         coins_;
        MeteorShard_t  meteorShards_;
        Gem_t          gems_;
        ItemPVec_t     itemsPVec_;
        ItemPVec_t     equippedItemsPVec_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & coins_;
            ar & meteorShards_;
            ar & gems_;
            ar & itemsPVec_;
            ar & equippedItemsPVec_;
        }
    };


    bool operator==(const Inventory & L, const Inventory & R);


    inline bool operator!=(const Inventory & L, const Inventory & R)
    {
        return ! (L == R);
    }

}
}

#endif //HEROESPATH_ITEM_INVENTORY_HPP_INCLUDED
