// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ALGORITHMS_HPP_INCLUDED
#define HEROESPATH_ITEM_ALGORITHMS_HPP_INCLUDED
//
// algorithms.cpp
//  A set of functions for parsing vectors of ItemPPtr_ts in useful ways.
//  There are too many variables to code a complete set of functions, so instead,
//  functions will be added here as needed during development.
//
#include "item/armor-enum.hpp"
#include "item/category-enum.hpp"
#include "item/misc-enum.hpp"
#include "item/weapon-enum.hpp"
#include "misc/not-null.hpp"
#include "misc/strings.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace item
{

    // forward declarations
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
    using ItemPVec_t = std::vector<ItemPtr_t>;

    struct Algorithms
    {
        static const std::string
            Names(const ItemPVec_t &, const misc::JoinHow & JOIN_HOW = misc::JoinHow());

        enum class MatchOpt
        {
            Equal,
            NotEqual
        };

        static const ItemPVec_t FindByCategory(
            const ItemPVec_t & itemSVec,
            const Category::Enum CATEGORY_TYPE,
            const MatchOpt MATCH_OPTION = MatchOpt::Equal);

        static const ItemPVec_t FindByWeaponType(
            const ItemPVec_t & itemSVec,
            const Weapon::Enum WEAPON_TYPE,
            const MatchOpt MATCH_OPTION = MatchOpt::Equal);

        static const ItemPVec_t FindByArmorType(
            const ItemPVec_t & itemSVec,
            const Armor::Enum ARMOR_TYPE,
            const MatchOpt MATCH_OPTION = MatchOpt::Equal);

        static const ItemPVec_t FindByMiscType(
            const ItemPVec_t & itemSVec,
            const Misc::Enum MISC_TYPE,
            const MatchOpt MATCH_OPTION = MatchOpt::Equal);

        static const ItemPVec_t
            FindByPixie(const ItemPVec_t & itemSVec, const MatchOpt MATCH_OPTION = MatchOpt::Equal);

        enum class BrokenOpt
        {
            Keep,
            Discard
        };

        static const ItemPVec_t
            FindByBroken(const ItemPVec_t & ITEM_SVEC, const BrokenOpt KEEP_BROKEN_OPTION);
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ALGORITHMS_HPP_INCLUDED
