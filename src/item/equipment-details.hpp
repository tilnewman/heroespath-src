// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_EQUIPMENT_DETAILS_HPP_INCLUDED
#define HEROESPATH_ITEM_EQUIPMENT_DETAILS_HPP_INCLUDED
//
// equipment-details.hpp
//
#include "creature/complexity-type.hpp"
#include "game/strong-types.hpp"
#include "item/category-enum.hpp"
#include "item/material-enum.hpp"
#include "misc/boost-serialize-includes.hpp"

#include <string_view>

namespace heroespath
{
namespace item
{

    // This struct is not meant to wrap all the details about an equipemnt item, only those details
    // that are best defined in the enum wrapper structs so the values can be lined up and see
    // together as a group.  That way you can see how the score/price/weight vary between different
    // items more easily, which makes play-balancing adjustments much easier.

    struct EquipmentDetails
    {
        constexpr EquipmentDetails() noexcept
            : base_score(0_score)
            , base_price(0_coin)
            , base_weight(0_weight)
            , base_armor_rating(0_armor)
            , damage_min(0_health)
            , damage_max(0_health)
            , category(Category::None)
            , complexity(creature::nonplayer::complexity_type::Count)
            , material_name_type(MaterialNameStyle::Count)
        {}

        // for weapons
        constexpr EquipmentDetails(
            const Score_t BASE_SCORE,
            const Coin_t BASE_PRICE,
            const Weight_t BASE_WEIGHT,
            const Health_t DAMAGE_MIN,
            const Health_t DAMAGE_MAX,
            const Category::Enum HANDEDNESS,
            const creature::nonplayer::complexity_type::Enum COMPLEXITY,
            const MaterialNameStyle::Enum NAME_TYPE) noexcept
            : base_score(BASE_SCORE)
            , base_price(BASE_PRICE)
            , base_weight(BASE_WEIGHT)
            , base_armor_rating(0_armor)
            , damage_min(DAMAGE_MIN)
            , damage_max(DAMAGE_MAX)
            , category(HANDEDNESS)
            , complexity(COMPLEXITY)
            , material_name_type(NAME_TYPE)
        {}

        // for armor
        constexpr EquipmentDetails(
            const Score_t BASE_SCORE,
            const Coin_t BASE_PRICE,
            const Weight_t BASE_WEIGHT,
            const Armor_t BASE_ARMOR_RATING,
            const Category::Enum CATEGORY,
            const creature::nonplayer::complexity_type::Enum COMPLEXITY,
            const MaterialNameStyle::Enum NAME_TYPE) noexcept
            : base_score(BASE_SCORE)
            , base_price(BASE_PRICE)
            , base_weight(BASE_WEIGHT)
            , base_armor_rating(BASE_ARMOR_RATING)
            , damage_min(0_health)
            , damage_max(0_health)
            , category(CATEGORY)
            , complexity(COMPLEXITY)
            , material_name_type(NAME_TYPE)
        {}

        constexpr EquipmentDetails(const EquipmentDetails &) noexcept = default;
        constexpr EquipmentDetails(EquipmentDetails &&) noexcept = default;
        constexpr EquipmentDetails & operator=(const EquipmentDetails &) noexcept = default;
        constexpr EquipmentDetails & operator=(EquipmentDetails &&) noexcept = default;

        Score_t base_score;
        Coin_t base_price;
        Weight_t base_weight;
        Armor_t base_armor_rating;
        Health_t damage_min;
        Health_t damage_max;
        Category::Enum category;
        creature::nonplayer::complexity_type::Enum complexity;
        MaterialNameStyle::Enum material_name_type;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & base_score;
            ar & base_price;
            ar & base_weight;
            ar & base_armor_rating;
            ar & damage_min;
            ar & damage_max;
            ar & category;
            ar & complexity;
            ar & material_name_type;
        }
    };

    constexpr const EquipmentDetails EquipmentDetailsEmpty;

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_EQUIPMENT_DETAILS_HPP_INCLUDED
