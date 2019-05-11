// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// inventory.cpp
//
#include "inventory.hpp"

#include "item/item-warehouse.hpp"
#include "item/item.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/serialize-helpers.hpp"
#include "misc/vectors.hpp"

#include <sstream>
#include <tuple>

namespace heroespath
{
namespace item
{

    Inventory::Inventory(
        const Coin_t & COINS,
        const Shard_t & METEOR_SHARDS,
        const Gem_t & GEMS,
        const ItemPVec_t & ITEMS_SVEC,
        const ItemPVec_t & EQUIPPED_ITEMS_SVEC)
        : coins_(COINS)
        , meteorShards_(METEOR_SHARDS)
        , gems_(GEMS)
        , itemsPVec_(ITEMS_SVEC)
        , equippedItemsPVec_(EQUIPPED_ITEMS_SVEC)
    {}

    Inventory::~Inventory()
    {
        ItemWarehouse::Access().Free(itemsPVec_);
        ItemWarehouse::Access().Free(equippedItemsPVec_);
    }

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

    bool Inventory::ShardsAdj(const Shard_t & METEOR_SHARDS)
    {
        meteorShards_ += METEOR_SHARDS;

        if (meteorShards_ < 0_shard)
        {
            meteorShards_ = 0_shard;
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
        Weight_t totalWeight { 0_weight };

        for (const auto & NEXT_ITEM_PTR : itemsPVec_)
        {
            totalWeight += NEXT_ITEM_PTR->Weight();
        }

        for (const auto & NEXT_EQUIPPED_ITEM_PTR : equippedItemsPVec_)
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
        for (const auto & NEXT_ITEM_PTR : itemsPVec_)
        {
            if (NEXT_ITEM_PTR->ArmorType() == ARMOR_TYPE)
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
        for (const auto & NEXT_ITEM_PTR : equippedItemsPVec_)
        {
            if (NEXT_ITEM_PTR->ArmorType() == ARMOR_TYPE)
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
        for (const auto & NEXT_ITEM_PTR : itemsPVec_)
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
        for (const auto & NEXT_ITEM_PTR : equippedItemsPVec_)
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

        for (const auto & NEXT_ITEM_PTR : itemsPVec_)
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

        for (const auto & NEXT_ITEM_PTR : equippedItemsPVec_)
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
        for (const auto & NEXT_ITEM_PTR : itemsPVec_)
        {
            if (NEXT_ITEM_PTR->MiscType() == MISC_TYPE)
            {
                ++count;
            }
        }

        return count;
    }

    std::size_t Inventory::CountItemOfMiscTypeEquipped(const item::misc_type::Enum MISC_TYPE) const
    {
        std::size_t count(0);
        for (const auto & NEXT_ITEM_PTR : equippedItemsPVec_)
        {
            if (NEXT_ITEM_PTR->MiscType() == MISC_TYPE)
            {
                ++count;
            }
        }

        return count;
    }

    bool Inventory::HasCoverTypeEquipped(const armor::cover_type::Enum COVER_TYPE) const
    {
        return std::find_if(
                   std::begin(equippedItemsPVec_),
                   std::end(equippedItemsPVec_),
                   [COVER_TYPE](const auto & ITEM_PTR) {
                       return (ITEM_PTR->ArmorInfo().CoverType() == COVER_TYPE);
                   })
            != std::end(equippedItemsPVec_);
    }

    bool Inventory::HasMusicalInstrumentEquipped() const
    {
        return (
            std::find_if(
                std::begin(equippedItemsPVec_),
                std::end(equippedItemsPVec_),
                [](const auto & ITEM_PTR) {
                    return misc_type::IsMusicalInstrument(ITEM_PTR->MiscType());
                })
            != std::end(equippedItemsPVec_));
    }

    bool Inventory::HasCastingItemEquipped() const
    {
        return (
            std::find_if(
                std::begin(equippedItemsPVec_),
                std::end(equippedItemsPVec_),
                [](const auto & ITEM_PTR) {
                    return misc_type::AllowsCasting(ITEM_PTR->MiscType());
                })
            != std::end(equippedItemsPVec_));
    }

    void Inventory::ItemAdd(const ItemPtr_t ITEM_PTR) { itemsPVec_.emplace_back(ITEM_PTR); }

    void Inventory::ItemRemove(const ItemPtr_t ITEM_PTR)
    {
        const auto FOUND_CITER(std::find(itemsPVec_.begin(), itemsPVec_.end(), ITEM_PTR));

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
        const auto FOUND_CITER(
            std::find(equippedItemsPVec_.begin(), equippedItemsPVec_.end(), ITEM_PTR));

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (FOUND_CITER != equippedItemsPVec_.end()),
            "Inventory::ItemUnEquip(\"" << ITEM_PTR->Name()
                                        << "\") did not find the item in the equipped inventory.");

        equippedItemsPVec_.erase(FOUND_CITER);
        ItemAdd(ITEM_PTR);
    }

    bool Inventory::ContainsItem(const ItemPtr_t ITEM_PTR) const
    {
        const auto FOUND_CITER(std::find(itemsPVec_.begin(), itemsPVec_.end(), ITEM_PTR));

        if (FOUND_CITER != itemsPVec_.end())
        {
            return true;
        }

        const auto FOUND_EQUIPPED_CITER(
            std::find(equippedItemsPVec_.begin(), equippedItemsPVec_.end(), ITEM_PTR));

        if (FOUND_EQUIPPED_CITER != equippedItemsPVec_.end())
        {
            return true;
        }

        return false;
    }

    Armor_t Inventory::ArmorRating() const
    {
        Armor_t armorRating { 0_armor };

        for (const auto & NEXT_EQUIPPED_ITEM_PTR : equippedItemsPVec_)
        {
            armorRating += NEXT_EQUIPPED_ITEM_PTR->ArmorRating();
        }

        return armorRating;
    }

    const std::string Inventory::ToString() const
    {
        std::string str;
        str.reserve(32 + (itemsPVec_.size() * 32) + (equippedItemsPVec_.size() * 32));

        str += "[coins=";
        str += coins_.ToString();
        str += ", shards=";
        str += meteorShards_.ToString();
        str += ", gems=";
        str += gems_.ToString();
        str += ", items_held=";

        for (const auto & NEXT_ITEM_PTR : itemsPVec_)
        {
            str += NEXT_ITEM_PTR->Name();
            str += ',';
        }

        str += ", items_equipped=";

        for (const auto & NEXT_EQ_ITEM_PTR : equippedItemsPVec_)
        {
            str += NEXT_EQ_ITEM_PTR->Name();
            str += ',';
        }

        str += ']';

        return str;
    }

    void Inventory::BeforeSerialize()
    {
        for (const auto & ITEM_PTR : itemsPVec_)
        {
            ITEM_PTR->BeforeSerialize();
        }

        misc::SerializeHelp::BeforeSerialize(itemsPVec_, itemsPVecToSerialize_);

        for (const auto & ITEM_PTR : equippedItemsPVec_)
        {
            ITEM_PTR->BeforeSerialize();
        }

        misc::SerializeHelp::BeforeSerialize(equippedItemsPVec_, equippedItemsPVecToSerialize_);
    }

    void Inventory::AfterSerialize()
    {
        for (const auto & ITEM_PTR : itemsPVec_)
        {
            ITEM_PTR->AfterSerialize();
        }

        misc::SerializeHelp::AfterSerialize(itemsPVecToSerialize_, ItemWarehouse::Access());

        for (const auto & ITEM_PTR : equippedItemsPVec_)
        {
            ITEM_PTR->AfterSerialize();
        }

        misc::SerializeHelp::AfterSerialize(equippedItemsPVecToSerialize_, ItemWarehouse::Access());
    }

    void Inventory::AfterDeserialize()
    {
        for (const auto ITEM_RAW_PTR : itemsPVecToSerialize_)
        {
            ITEM_RAW_PTR->AfterDeserialize();
        }

        misc::SerializeHelp::AfterDeserialize(
            itemsPVec_, itemsPVecToSerialize_, ItemWarehouse::Access());

        for (const auto ITEM_RAW_PTR : equippedItemsPVecToSerialize_)
        {
            ITEM_RAW_PTR->AfterDeserialize();
        }

        misc::SerializeHelp::AfterDeserialize(
            equippedItemsPVec_, equippedItemsPVecToSerialize_, ItemWarehouse::Access());
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
