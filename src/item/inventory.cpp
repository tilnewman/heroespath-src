// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// inventory.cpp
//
#include "inventory.hpp"

#include "item/item-warehouse.hpp"
#include "item/item.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/vectors.hpp"

#include <sstream>
#include <tuple>

namespace heroespath
{
namespace item
{

    Inventory::Inventory(
        const Coin_t & COINS,
        const MeteorShard_t & METEOR_SHARDS,
        const Gem_t & GEMS,
        const ItemPVec_t & ITEMS_SVEC,
        const ItemPVec_t & EQUIPPED_ITEMS_SVEC)
        : coins_(COINS)
        , meteorShards_(METEOR_SHARDS)
        , gems_(GEMS)
        , itemsPVec_(ITEMS_SVEC)
        , equippedItemsPVec_(EQUIPPED_ITEMS_SVEC)
    {}

    Inventory::~Inventory() { FreeAllItemsFromWarehouse(); }

    bool Inventory::CoinsAdj(const Coin_t & COINS)
    {
        coins_ += COINS;

        if (coins_ < 0_coin)
        {
            coins_ = 0_coin;
            return false;
        }
        else
        {
            return true;
        }
    }

    bool Inventory::MeteorShardsAdj(const MeteorShard_t & METEOR_SHARDS)
    {
        meteorShards_ += METEOR_SHARDS;

        if (meteorShards_ < 0_mshard)
        {
            meteorShards_ = 0_mshard;
            return false;
        }
        else
        {
            return true;
        }
    }

    bool Inventory::GemsAdj(const Gem_t & GEMS)
    {
        gems_ += GEMS;

        if (gems_ < 0_gem)
        {
            gems_ = 0_gem;
            return false;
        }
        else
        {
            return true;
        }
    }

    Weight_t Inventory::Weight() const
    {
        Weight_t totalWeight{ 0_weight };

        for (auto const & NEXT_ITEM_PTR : itemsPVec_)
        {
            totalWeight += NEXT_ITEM_PTR->Weight();
        }

        for (auto const & NEXT_EQUIPPED_ITEM_PTR : equippedItemsPVec_)
        {
            totalWeight += NEXT_EQUIPPED_ITEM_PTR->Weight();
        }

        return totalWeight;
    }

    std::size_t Inventory::Count() const { return itemsPVec_.size() + equippedItemsPVec_.size(); }

    std::size_t Inventory::CountItemOfArmorType(const item::armor_type::Enum ARMOR_TYPE) const
    {
        return CountItemOfArmorTypeHeld(ARMOR_TYPE) + CountItemOfArmorTypeEquipped(ARMOR_TYPE);
    }

    std::size_t Inventory::CountItemOfArmorTypeHeld(const item::armor_type::Enum ARMOR_TYPE) const
    {
        std::size_t count(0);
        for (auto const & NEXT_ITEM_PTR : itemsPVec_)
        {
            if (NEXT_ITEM_PTR->ArmorType() & ARMOR_TYPE)
            {
                ++count;
            }
        }

        return count;
    }

    std::size_t
        Inventory::CountItemOfArmorTypeEquipped(const item::armor_type::Enum ARMOR_TYPE) const
    {
        std::size_t count(0);
        for (auto const & NEXT_ITEM_PTR : equippedItemsPVec_)
        {
            if (NEXT_ITEM_PTR->ArmorType() & ARMOR_TYPE)
            {
                ++count;
            }
        }

        return count;
    }

    std::size_t Inventory::CountItemOfWeaponType(const item::armor_type::Enum WEAPON_TYPE) const
    {
        return CountItemOfWeaponTypeHeld(WEAPON_TYPE) + CountItemOfWeaponTypeEquipped(WEAPON_TYPE);
    }

    std::size_t Inventory::CountItemOfWeaponTypeHeld(const item::armor_type::Enum WEAPON_TYPE) const
    {
        std::size_t count(0);
        for (auto const & NEXT_ITEM_PTR : itemsPVec_)
        {
            if (NEXT_ITEM_PTR->WeaponType() & WEAPON_TYPE)
            {
                ++count;
            }
        }

        return count;
    }

    std::size_t
        Inventory::CountItemOfWeaponTypeEquipped(const item::armor_type::Enum WEAPON_TYPE) const
    {
        std::size_t count(0);
        for (auto const & NEXT_ITEM_PTR : equippedItemsPVec_)
        {
            if (NEXT_ITEM_PTR->WeaponType() & WEAPON_TYPE)
            {
                ++count;
            }
        }

        return count;
    }

    std::size_t Inventory::CountItemOfCategory(const item::category::Enum ITEM_CATEGORY) const
    {
        return CountItemOfCategoryHeld(ITEM_CATEGORY) + CountItemOfCategoryEquipped(ITEM_CATEGORY);
    }

    std::size_t Inventory::CountItemOfCategoryHeld(const item::category::Enum ITEM_CATEGORY) const
    {
        std::size_t count(0);

        for (auto const & NEXT_ITEM_PTR : itemsPVec_)
        {
            if (NEXT_ITEM_PTR->Category() & ITEM_CATEGORY)
            {
                ++count;
            }
        }

        return count;
    }

    std::size_t
        Inventory::CountItemOfCategoryEquipped(const item::category::Enum ITEM_CATEGORY) const
    {
        std::size_t count(0);

        for (auto const & NEXT_ITEM_PTR : equippedItemsPVec_)
        {
            if (NEXT_ITEM_PTR->Category() & ITEM_CATEGORY)
            {
                ++count;
            }
        }

        return count;
    }

    std::size_t Inventory::CountItemOfMiscType(const item::misc_type::Enum MISC_TYPE) const
    {
        return CountItemOfMiscTypeHeld(MISC_TYPE) + CountItemOfMiscTypeEquipped(MISC_TYPE);
    }

    std::size_t Inventory::CountItemOfMiscTypeHeld(const item::misc_type::Enum MISC_TYPE) const
    {
        std::size_t count(0);
        for (auto const & NEXT_ITEM_PTR : itemsPVec_)
        {
            if (NEXT_ITEM_PTR->MiscType() & MISC_TYPE)
            {
                ++count;
            }
        }

        return count;
    }

    std::size_t Inventory::CountItemOfMiscTypeEquipped(const item::misc_type::Enum MISC_TYPE) const
    {
        std::size_t count(0);
        for (auto const & NEXT_ITEM_PTR : equippedItemsPVec_)
        {
            if (NEXT_ITEM_PTR->MiscType() & MISC_TYPE)
            {
                ++count;
            }
        }

        return count;
    }

    void Inventory::ItemAdd(const ItemPtr_t ITEM_PTR) { itemsPVec_.emplace_back(ITEM_PTR); }

    void Inventory::ItemRemove(const ItemPtr_t ITEM_PTR)
    {
        auto const FOUND_CITER(std::find(itemsPVec_.begin(), itemsPVec_.end(), ITEM_PTR));

        if (FOUND_CITER == itemsPVec_.end())
        {
            M_HP_LOG_ERR(
                "Inventory::ItemRemove(\""
                << ITEM_PTR->Name() << "\") did not find the item in the unequipped inventory.");
        }
        else
        {
            itemsPVec_.erase(FOUND_CITER);
        }
    }

    void Inventory::ItemEquip(const ItemPtr_t ITEM_PTR)
    {
        ItemRemove(ITEM_PTR);
        equippedItemsPVec_.emplace_back(ITEM_PTR);
    }

    void Inventory::ItemUnEquip(const ItemPtr_t ITEM_PTR)
    {
        auto const FOUND_CITER(
            std::find(equippedItemsPVec_.begin(), equippedItemsPVec_.end(), ITEM_PTR));

        M_ASSERT_OR_LOGANDTHROW_SS(
            (FOUND_CITER != equippedItemsPVec_.end()),
            "Inventory::ItemUnEquip(\"" << ITEM_PTR->Name()
                                        << "\") did not find the item in the equipped inventory.");

        equippedItemsPVec_.erase(FOUND_CITER);
        ItemAdd(ITEM_PTR);
    }

    bool Inventory::ContainsItem(const ItemPtr_t ITEM_PTR) const
    {
        auto const FOUND_CITER(std::find(itemsPVec_.begin(), itemsPVec_.end(), ITEM_PTR));

        if (FOUND_CITER != itemsPVec_.end())
        {
            return true;
        }

        auto const FOUND_EQUIPPED_CITER(
            std::find(equippedItemsPVec_.begin(), equippedItemsPVec_.end(), ITEM_PTR));

        if (FOUND_EQUIPPED_CITER != equippedItemsPVec_.end())
        {
            return true;
        }

        return false;
    }

    Armor_t Inventory::ArmorRating() const
    {
        Armor_t armorRating{ 0_armor };

        for (auto const & NEXT_EQUIPPED_ITEM_PTR : equippedItemsPVec_)
        {
            armorRating += NEXT_EQUIPPED_ITEM_PTR->ArmorRating();
        }

        return armorRating;
    }

    const std::string Inventory::ToString() const
    {
        std::ostringstream ss;

        ss << "[coins=" << coins_ << ", shards=" << meteorShards_ << ", gems=" << gems_
           << ", itms_held=";

        for (auto const & NEXT_ITEM_PTR : itemsPVec_)
        {
            ss << NEXT_ITEM_PTR->Name() << ",";
        }

        ss << ", items_equipped=";

        for (auto const & NEXT_EQ_ITEM_PTR : equippedItemsPVec_)
        {
            ss << NEXT_EQ_ITEM_PTR->Name() << ",";
        }

        ss << "]";

        return ss.str();
    }

    void Inventory::BeforeSerialize()
    {
        itemsPVecToSerialize_.clear();
        for (auto const & ITEM_PTR : itemsPVec_)
        {
            ITEM_PTR->BeforeSerialize();
            itemsPVecToSerialize_.emplace_back(ITEM_PTR.Ptr());
        }
        // everything in itemsPVec is free'd in the destructor

        equippedItemsPVecToSerialize_.clear();
        for (auto const & ITEM_PTR : equippedItemsPVec_)
        {
            ITEM_PTR->BeforeSerialize();
            equippedItemsPVecToSerialize_.emplace_back(ITEM_PTR.Ptr());
        }
        // everything in equippedItemsPVec_ is free'd in the destructor
    }

    void Inventory::AfterDeserialize()
    {
        itemsPVec_.clear();
        for (auto const ITEM_RAW_PTR : itemsPVecToSerialize_)
        {
            ITEM_RAW_PTR->AfterDeserialize();
            itemsPVec_.emplace_back(ItemWarehouse::Instance()->Store(ITEM_RAW_PTR));
        }
        itemsPVecToSerialize_.clear();

        equippedItemsPVec_.clear();
        for (auto const ITEM_RAW_PTR : equippedItemsPVecToSerialize_)
        {
            ITEM_RAW_PTR->AfterDeserialize();
            equippedItemsPVec_.emplace_back(ItemWarehouse::Instance()->Store(ITEM_RAW_PTR));
        }
        equippedItemsPVecToSerialize_.clear();
    }

    void Inventory::FreeAllItemsFromWarehouse()
    {
        ItemWarehouse::Instance()->Free(itemsPVec_);
        ItemWarehouse::Instance()->Free(equippedItemsPVec_);
    }

    bool operator==(const Inventory & L, const Inventory & R)
    {
        if (std::tie(L.coins_, L.meteorShards_, L.gems_)
            != std::tie(R.coins_, R.meteorShards_, R.gems_))
        {
            return false;
        }

        if (misc::Vector::OrderlessCompareEqual(L.itemsPVec_, R.itemsPVec_) == false)
        {
            return false;
        }

        return misc::Vector::OrderlessCompareEqual(L.equippedItemsPVec_, R.equippedItemsPVec_);
    }

} // namespace item
} // namespace heroespath
