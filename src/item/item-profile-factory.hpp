// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ITEM_PROFILE_FACTORY_HPP_INCLUDE
#define HEROESPATH_ITEM_ITEM_PROFILE_FACTORY_HPP_INCLUDE
//
// item-profile-factory.hpp
//
#include "game/strong-types.hpp"
#include "item/item-profile.hpp"
#include "item/item-template-factory.hpp"
#include "misc/not-null.hpp"

#include <string>
#include <tuple>
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

    class ItemProfileWarehouse;

    // everything in here could be made constexpr/noexcept after Enchantments are overhauled
    struct ItemProfileFactory
    {
        ItemProfileFactory() = delete;

        // make with weapon or armor sub-type enum
        template <typename EquipmentSubEnumWrapper_t>
        static const ItemProfile Make(
            const EquipmentSubEnumWrapper_t EQUIPMENT_SUB_TYPE_ENUM,
            const bool IS_PIXIE,
            const Material::Enum MAT_PRI,
            const Material::Enum MAT_SEC = Material::Count)
        {
            return Make(
                ItemTemplateFactory::Instance()->MakeFromSubType(EQUIPMENT_SUB_TYPE_ENUM),
                IS_PIXIE,
                MAT_PRI,
                MAT_SEC,
                Named::Count,
                Set::Count,
                Element::None,
                Misc::Count);
        }

        // make armor requiring a form (Aventail/Bracer/Boot/Pant/Shirt/Gauntlet)
        static const ItemProfile Make(
            const Armor::Enum ARMOR_TYPE,
            const Forms::Enum FORM_TYPE,
            const bool IS_PIXIE,
            const Material::Enum MAT_PRI,
            const Material::Enum MAT_SEC = Material::Count);

        static const ItemProfile MakeSkinArmor(const creature::CreaturePtr_t CREATURE_PTR);
        static const ItemProfile MakeBodyPartWeapon(const creature::CreaturePtr_t CREATURE_PTR);

    private:
        friend ItemProfileWarehouse;

        // most specific/complete verion of Make() that is called by all others
        static const ItemProfile Make(
            const ItemTemplate & TEMPLATE,
            const bool IS_PIXIE,
            const Material::Enum MAT_PRI,
            const Material::Enum MAT_SEC,
            const Named::Enum NAMED,
            const Set::Enum SET,
            const Element::Enum ELEMENT,
            const Misc::Enum MISC,
            const creature::SummonInfo & SUMMON_INFO = creature::SummonInfoEmpty);

        static Score_t CalcScore(
            const ItemTemplate & TEMPLATE,
            const item::Material::Enum MAT_PRI,
            const item::Material::Enum MAT_SEC,
            const item::Named::Enum NAMED,
            const item::Set::Enum SET,
            const item::Element::Enum ELEMENT,
            const Misc::Enum MISC,
            const creature::SummonInfo & SUMMON_INFO);

        static bool CalcIsMagical(
            const Category::Enum CATEGORY,
            const Named::Enum NAMED,
            const Set::Enum SET,
            const Element::Enum ELEMENT,
            const Misc::Enum MISC,
            const creature::SummonInfo & SUMMON_INFO);

        static Category::Enum CalcCategory(const ItemTemplate & TEMPLATE, const Misc::Enum MISC);

        static Armor_t CalcArmorRating(
            const bool IS_ARMROR,
            const EquipmentDetails & EQUIPMENT_DETAILS,
            const Material::Enum MAT_PRI,
            const Material::Enum MAT_SEC);

        static Coin_t CalculatePrice(
            const ItemTemplate & TEMPLATE,
            const bool IS_PIXIE,
            const Material::Enum MAT_PRI,
            const Material::Enum MAT_SEC,
            const Score_t SCORE);

        static Weight_t CalculateWeight(
            const EquipmentDetails & EQUIPMENT_DETAILS,
            const bool IS_PIXIE,
            const Material::Enum MAT_PRI,
            const Material::Enum MAT_SEC,
            const Misc::Enum MISC);

        // misc items that are also equipment must use the equipment Template
        static const ItemTemplate SwitchToEquipmentTemplateIfNeeded(const ItemTemplate & TEMPLATE);
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEM_PROFILE_FACTORY_HPP_INCLUDE
