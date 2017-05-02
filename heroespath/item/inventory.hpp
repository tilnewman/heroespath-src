#ifndef APPBASE_HEROESPATH_ITEM_INVENTORY_INCLUDED
#define APPBASE_HEROESPATH_ITEM_INVENTORY_INCLUDED
//
// inventory.hpp
//  A class that encapsolates a collection of Items.
//
#include "utilz/boost-serialize-includes.hpp"

#include "heroespath/item/types.hpp"
#include "heroespath/item/item-type-enum.hpp"

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
    using ItemSPtr_t = std::shared_ptr<Item>;
    using ItemSVec_t = std::vector<ItemSPtr_t>;


    //A class that encapsolates a collection of Items.
    class Inventory
    {
    public:
        Inventory(const Coin_t      COINS              = 0,
                  const Meteor_t    METEOR_SHARDS      = 0,
                  const Gem_t       GEMS               = 0,
                  const ItemSVec_t ITEMS_SVEC          = ItemSVec_t(),
                  const ItemSVec_t EQUIPPED_ITEMS_SVEC = ItemSVec_t());

        virtual ~Inventory();

        inline Coin_t Coins() const             { return coins_; }
        inline Meteor_t MeteorShards() const    { return meteorShards_; }
        inline Gem_t Gems() const               { return gems_; }

        //these functions return false if attempt to reduce below zero
        bool CoinsAdj(const Coin_t A);
        bool MeteorShardsAdj(const Meteor_t A);
        bool GemsAdj(const Gem_t A);

        inline const ItemSVec_t Items() const          { return itemsSVec_; }
        inline const ItemSVec_t ItemsEquipped() const   { return equippedItemsSVec_; }

        void ItemAdd(const ItemSPtr_t ITEM_SPTR);
        void ItemRemove(const ItemSPtr_t ITEM_SPTR);

        //moves the item from itemsSVec_ to equippedItemsSVec_
        void ItemEquip(const ItemSPtr_t ITEM_SPTR);

        //moves the item from equippedItemsSVec_ to itemsSVec_
        void ItemUnEquip(const ItemSPtr_t ITEM_SPTR);

        bool ContainsItem(const ItemSPtr_t ITEM_SPTR) const;

        Weight_t Weight() const;

        std::size_t Count() const;

        std::size_t CountItemOfArmorType(const item::armor_type::Enum ARMOR_TYPE) const;
        std::size_t CountItemOfArmorTypeHeld(const item::armor_type::Enum ARMOR_TYPE) const;
        std::size_t CountItemOfArmorTypeEquipped(const item::armor_type::Enum ARMOR_TYPE) const;

        std::size_t CountItemOfWeaponType(const item::armor_type::Enum WEAPON_TYPE) const;
        std::size_t CountItemOfWeaponTypeHeld(const item::armor_type::Enum WEAPON_TYPE) const;
        std::size_t CountItemOfWeaponTypeEquipped(const item::armor_type::Enum WEAPON_TYPE) const;

        std::size_t CountItemOfCategory(const item::category::Enum ITEM_CATEGORY) const;
        std::size_t CountItemOfCategoryHeld(const item::category::Enum ITEM_CATEGORY) const;
        std::size_t CountItemOfCategoryEquipped(const item::category::Enum ITEM_CATEGORY) const;

        std::size_t CountItemOfMiscType(const item::misc_type::Enum MISC_TYPE) const;
        std::size_t CountItemOfMiscTypeHeld(const item::misc_type::Enum MISC_TYPE) const;
        std::size_t CountItemOfMiscTypeEquipped(const item::misc_type::Enum MISC_TYPE) const;

        stats::Armor_t ArmorRating() const;

        virtual const std::string ToString() const;

        friend bool operator==(const Inventory & L, const Inventory & R);

    private:
        Coin_t     coins_;
        Meteor_t   meteorShards_;
        Gem_t      gems_;
        ItemSVec_t itemsSVec_;
        ItemSVec_t equippedItemsSVec_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & coins_;
            ar & meteorShards_;
            ar & gems_;
            ar & itemsSVec_;
            ar & equippedItemsSVec_;
        }
    };


    bool operator==(const Inventory & L, const Inventory & R);

    bool operator!=(const Inventory & L, const Inventory & R);

}
}
#endif //APPBASE_HEROESPATH_ITEM_INVENTORY_INCLUDED
