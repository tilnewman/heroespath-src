//
// inventory.cpp
//
#include "inventory.hpp"

#include "game/item/item.hpp"
#include "utilz/assertlogandthrow.hpp"

#include <tuple>
#include <sstream>


namespace game
{
namespace item
{

    Inventory::Inventory(const Coin_t     COINS,
                         const Meteor_t   METEOR_SHARDS,
                         const Gem_t      GEMS,
                         const ItemSVec_t ITEMS_SVEC,
                         const ItemSVec_t EQUIPPED_ITEMS_SVEC)
    :
        coins_           (COINS),
        meteorShards_    (METEOR_SHARDS),
        gems_            (GEMS),
        itemsSVec_       (ITEMS_SVEC),
        equippedItemsSVec_(EQUIPPED_ITEMS_SVEC)
    {}


    Inventory::~Inventory()
    {}


    bool Inventory::CoinsAdj(const Coin_t A)
    {
        coins_ += A;

        if (coins_ < 0)
        {
            coins_ = 0;
            return false;
        }
        else
            return true;
    }


    bool Inventory::MeteorShardsAdj(const Meteor_t A)
    {
        meteorShards_ += A;

        if (meteorShards_ < 0)
        {
            meteorShards_ = 0;
            return false;
        }
        else
            return true;
    }


    bool Inventory::GemsAdj(const Gem_t A)
    {
        gems_ += A;

        if (gems_ < 0)
        {
            gems_ = 0;
            return false;
        }
        else
            return true;
    }


    Weight_t Inventory::Weight() const
    {
        Weight_t totalWeight(0);

        for (auto const & NEXT_ITEM_SPTR : itemsSVec_)
            totalWeight += NEXT_ITEM_SPTR->Weight();

        for (auto const & NEXT_EQUIPPED_ITEM_SPTR : equippedItemsSVec_)
            totalWeight += NEXT_EQUIPPED_ITEM_SPTR->Weight();

        return totalWeight;
    }


    std::size_t Inventory::Count() const
    {
        return itemsSVec_.size() + equippedItemsSVec_.size();
    }


    std::size_t Inventory::CountItemOfArmorType(const item::armor_type::Enum ARMOR_TYPE) const
    {
        return CountItemOfArmorTypeHeld(ARMOR_TYPE) + CountItemOfArmorTypeEquipped(ARMOR_TYPE);
    }


    std::size_t Inventory::CountItemOfArmorTypeHeld(const item::armor_type::Enum ARMOR_TYPE) const
    {
        std::size_t count(0);
        for (auto const & NEXT_ITEM_SPTR : itemsSVec_)
            if (NEXT_ITEM_SPTR->ArmorType() & ARMOR_TYPE)
                ++count;

        return count;
    }


    std::size_t Inventory::CountItemOfArmorTypeEquipped(const item::armor_type::Enum ARMOR_TYPE) const
    {
        std::size_t count(0);
        for (auto const & NEXT_ITEM_SPTR : equippedItemsSVec_)
            if (NEXT_ITEM_SPTR->ArmorType() & ARMOR_TYPE)
                ++count;

        return count;
    }


    std::size_t Inventory::CountItemOfWeaponType(const item::armor_type::Enum WEAPON_TYPE) const
    {
        return CountItemOfWeaponTypeHeld(WEAPON_TYPE) + CountItemOfWeaponTypeEquipped(WEAPON_TYPE);
    }


    std::size_t Inventory::CountItemOfWeaponTypeHeld(const item::armor_type::Enum WEAPON_TYPE) const
    {
        std::size_t count(0);
        for (auto const & NEXT_ITEM_SPTR : itemsSVec_)
            if (NEXT_ITEM_SPTR->WeaponType() & WEAPON_TYPE)
                ++count;

        return count;
    }


    std::size_t Inventory::CountItemOfWeaponTypeEquipped(const item::armor_type::Enum WEAPON_TYPE) const
    {
        std::size_t count(0);
        for (auto const & NEXT_ITEM_SPTR : equippedItemsSVec_)
            if (NEXT_ITEM_SPTR->WeaponType() & WEAPON_TYPE)
                ++count;

        return count;
    }


    std::size_t Inventory::CountItemOfCategory(const item::category::Enum ITEM_CATEGORY) const
    {
        return CountItemOfCategoryHeld(ITEM_CATEGORY) + CountItemOfCategoryEquipped(ITEM_CATEGORY);
    }


    std::size_t Inventory::CountItemOfCategoryHeld(const item::category::Enum ITEM_CATEGORY) const
    {
        std::size_t count(0);

        for (auto const & NEXT_ITEM_SPTR : itemsSVec_)
            if (NEXT_ITEM_SPTR->Category() & ITEM_CATEGORY)
                ++count;

        return count;
    }


    std::size_t Inventory::CountItemOfCategoryEquipped(const item::category::Enum ITEM_CATEGORY) const
    {
        std::size_t count(0);

        for (auto const & NEXT_ITEM_SPTR : equippedItemsSVec_)
            if (NEXT_ITEM_SPTR->Category() & ITEM_CATEGORY)
                ++count;

        return count;
    }


    std::size_t Inventory::CountItemOfMiscType(const item::misc_type::Enum MISC_TYPE) const
    {
        return CountItemOfMiscTypeHeld(MISC_TYPE) + CountItemOfMiscTypeEquipped(MISC_TYPE);
    }


    std::size_t Inventory::CountItemOfMiscTypeHeld(const item::misc_type::Enum MISC_TYPE) const
    {
        std::size_t count(0);
        for (auto const & NEXT_ITEM_SPTR : itemsSVec_)
            if (NEXT_ITEM_SPTR->MiscType() & MISC_TYPE)
                ++count;

        return count;
    }


    std::size_t Inventory::CountItemOfMiscTypeEquipped(const item::misc_type::Enum MISC_TYPE) const
    {
        std::size_t count(0);
        for (auto const & NEXT_ITEM_SPTR : equippedItemsSVec_)
            if (NEXT_ITEM_SPTR->MiscType() & MISC_TYPE)
                ++count;

        return count;
    }

    void Inventory::ItemAdd(const ItemSPtr_t ITEM_SPTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((ITEM_SPTR.get() != nullptr), "Inventory::ItemAdd(nullptr) was given a null ITEM_SPTR.");
        itemsSVec_.push_back(ITEM_SPTR);
    }


    void Inventory::ItemRemove(const ItemSPtr_t ITEM_SPTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((ITEM_SPTR.get() != nullptr), "Inventory::ItemRemove(nullptr) was given a null ITEM_SPTR.");

        const ItemSVecCIter_t FOUND_CITER( std::find(itemsSVec_.begin(), itemsSVec_.end(), ITEM_SPTR) );
        M_ASSERT_OR_LOGANDTHROW_SS((FOUND_CITER != itemsSVec_.end()), "Inventory::ItemRemove(\"" << ITEM_SPTR->Name() << "\") did not find the item in the unequipped inventory.");

        itemsSVec_.erase(FOUND_CITER);
    }


    void Inventory::ItemEquip(const ItemSPtr_t ITEM_SPTR)
    {
        ItemRemove(ITEM_SPTR);
        equippedItemsSVec_.push_back(ITEM_SPTR);
    }


    void Inventory::ItemUnEquip(const ItemSPtr_t ITEM_SPTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((ITEM_SPTR.get() != nullptr), "Inventory::ItemUnequip(nullptr) was given a null ITEM_SPTR.");

        const ItemSVecCIter_t FOUND_CITER(std::find(equippedItemsSVec_.begin(), equippedItemsSVec_.end(), ITEM_SPTR));
        M_ASSERT_OR_LOGANDTHROW_SS((FOUND_CITER != equippedItemsSVec_.end()), "Inventory::ItemUnEquip(\"" << ITEM_SPTR->Name() << "\") did not find the item in the equipped inventory.");

        equippedItemsSVec_.erase(FOUND_CITER);
        ItemAdd(ITEM_SPTR);
    }


    bool Inventory::ContainsItem(const ItemSPtr_t ITEM_SPTR) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((ITEM_SPTR.get() != nullptr), "Inventory::ContainsItem(nullptr) was given a null ITEM_SPTR.");

        const ItemSVecCIter_t FOUND_CITER(std::find(itemsSVec_.begin(), itemsSVec_.end(), ITEM_SPTR));
        if (FOUND_CITER != itemsSVec_.end())
            return true;

        const ItemSVecCIter_t FOUND_EQUIPPED_CITER(std::find(equippedItemsSVec_.begin(), equippedItemsSVec_.end(), ITEM_SPTR));
        if (FOUND_EQUIPPED_CITER != equippedItemsSVec_.end())
            return true;

        return false;
    }


    stats::Armor_t Inventory::ArmorRating() const
    {
        stats::Armor_t armorRating(0);

        for (auto const & NEXT_EQUIPPED_ITEM_SPTR : equippedItemsSVec_)
            armorRating += NEXT_EQUIPPED_ITEM_SPTR->ArmorRating();

        return armorRating;
    }


    const std::string Inventory::ToString() const
    {
        std::ostringstream ss;

        ss << "[coins=" << coins_
            << ", shards=" << meteorShards_
            << ", gems=" << gems_
            << ", itms_held=";

        for (auto const & NEXT_ITEM_SPTR : itemsSVec_)
            ss << NEXT_ITEM_SPTR->Name() << ",";

        ss << ", items_equipped=";

        for (auto const & NEXT_EQ_ITEM_SPTR : equippedItemsSVec_)
            ss << NEXT_EQ_ITEM_SPTR->Name() << ",";

        ss << "]";

        return ss.str();
    }


    bool operator==(const Inventory & L, const Inventory & R)
    {
        return std::tie(L.coins_,
                        L.meteorShards_,
                        L.gems_,
                        L.itemsSVec_,
                        L.equippedItemsSVec_)
               ==
               std::tie(R.coins_,
                        R.meteorShards_,
                        R.gems_,
                        R.itemsSVec_,
                        R.equippedItemsSVec_);
    }


    bool operator!=(const Inventory & L, const Inventory & R)
    {
        return ! (L == R);
    }

}
}
