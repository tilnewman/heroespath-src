#ifndef GAME_ITEM_ALGORITHMS_HPP_INCLUDED
#define GAME_ITEM_ALGORITHMS_HPP_INCLUDED
//
// algorithms.cpp
//  A set of functions for parsing vectors of ItemSPtr_ts in useful ways.
//  There are too many variables to code a complete set of functions, so instead,
//  functions will be added here as needed during development.
//
#include "game/item/item-type-enum.hpp"

#include <memory>
#include <vector>


namespace game
{
namespace item
{

    //forward declarations
    class Item;
    using ItemSPtr_t = std::shared_ptr<Item>;
    using ItemSVec_t = std::vector<ItemSPtr_t>;


    struct Algorithms
    {
        static const std::string Names(const ItemSVec_t &,
                                       const bool         WILL_WRAP       = false,
                                       const bool         WILL_APPEND_AND = false,
                                       const std::size_t  MAX_COUNT       = 0,
                                       const bool         WILL_ELLIPSIS   = false);

        static const ItemSVec_t FindByCategory(const ItemSVec_t &   itemSVec,
                                               const category::Enum CATEGORY_TYPE,
                                               const bool           IS_MATCH = true);

        static const ItemSVec_t FindByWeaponType(const ItemSVec_t &      itemSVec,
                                                 const weapon_type::Enum WEAPON_TYPE,
                                                 const bool              IS_MATCH = true);

        static const ItemSVec_t FindByArmorType(const ItemSVec_t &     itemSVec,
                                                const armor_type::Enum ARMOR_TYPE,
                                                const bool             IS_MATCH = true);

        static const ItemSVec_t FindByMiscType(const ItemSVec_t &    itemSVec,
                                               const misc_type::Enum MISC_TYPE,
                                               const bool            IS_MATCH = true);

        static const ItemSVec_t FindByPixie(const ItemSVec_t & itemSVec,
                                            const bool         IS_MATCH = true);

        static const ItemSVec_t FindByBroken(const ItemSVec_t & ITEM_SVEC,
                                             const bool         WILL_KEEP_BROKEN);
    };

}
}

#endif //GAME_ITEM_ALGORITHMS_HPP_INCLUDED
