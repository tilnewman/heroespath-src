// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ITEMFACTORY_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEMFACTORY_HPP_INCLUDED
//
// item-factory.hpp
//
#include "item/item-profile-factory.hpp"
#include "misc/not-null.hpp"

#include <string_view>

namespace heroespath
{

namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
} // namespace creature

namespace item
{

    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;

    // Responsible for making new (and properly warehousing) item objects from ItemProfiles.
    struct ItemFactory
    {
        ItemFactory() = delete;

        // only non-misc bodypart items need a creature race name
        static const ItemPtr_t
            Make(const ItemProfile &, const std::string_view CREATURE_RACE_NAME = {});

        // make with weapon or armor sub-type enum
        template <typename EquipmentSubEnumWrapper_t>
        static const ItemPtr_t Make(
            const EquipmentSubEnumWrapper_t EQUIPMENT_SUB_TYPE_ENUM,
            const bool IS_PIXIE,
            const Material::Enum MAT_PRI,
            const Material::Enum MAT_SEC = Material::Count)
        {
            return Make(
                ItemProfileFactory::Make(EQUIPMENT_SUB_TYPE_ENUM, IS_PIXIE, MAT_PRI, MAT_SEC));
        }

        // make armor requiring a form (Aventail/Bracer/Boot/Pant/Shirt/Gauntlet)
        static const ItemPtr_t Make(
            const Armor::Enum ARMOR_TYPE,
            const Forms::Enum FORM_TYPE,
            const bool IS_PIXIE,
            const Material::Enum MAT_PRI,
            const Material::Enum MAT_SEC = Material::Count);

        static bool
            EquipSkinArmor(const creature::CreaturePtr_t CREATURE_PTR, std::string & errorMessage);

        static bool EquipBodyPartWeapon(
            const creature::CreaturePtr_t CREATURE_PTR, std::string & errorMessage);

    private:
        static bool EquipBodyPart(
            const ItemProfile & ITEM_PROFILE,
            const creature::CreaturePtr_t CREATURE_PTR,
            std::string & errorMessage);
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEMFACTORY_HPP_INCLUDED
