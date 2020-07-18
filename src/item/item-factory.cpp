// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-factory.cpp
//
#include "item-factory.hpp"

#include "creature/creature.hpp"
#include "item/inventory.hpp"
#include "item/item-name-factory.hpp"
#include "item/item-profile.hpp"
#include "item/item-warehouse.hpp"
#include "item/item.hpp"

#include <memory>

namespace heroespath
{
namespace item
{

    const ItemPtr_t
        ItemFactory::Make(const ItemProfile & PROFILE, const std::string_view CREATURE_RACE_NAME)
    {
        return ItemWarehouse::Access().Store(std::make_unique<Item>(
            ItemNameFactory::MakeName(PROFILE, CREATURE_RACE_NAME),
            ItemNameFactory::MakeDescription(PROFILE, CREATURE_RACE_NAME),
            PROFILE));
    }

    const ItemPtr_t ItemFactory::Make(
        const Armor::Enum ARMOR_TYPE,
        const Forms::Enum FORM_TYPE,
        const bool IS_PIXIE,
        const Material::Enum MAT_PRI,
        const Material::Enum MAT_SEC)
    {
        return Make(ItemProfileFactory::Make(ARMOR_TYPE, FORM_TYPE, IS_PIXIE, MAT_PRI, MAT_SEC));
    }

    bool ItemFactory::EquipSkinArmor(
        const creature::CreaturePtr_t CREATURE_PTR, std::string & errorMessage)
    {
        const auto ITEM_PROFILE = ItemProfileFactory::MakeSkinArmor(CREATURE_PTR);

        if (!ITEM_PROFILE.IsValid())
        {
            errorMessage = CREATURE_PTR->RaceName();
            errorMessage += " do not have any skin armor to equip.";
            return true;
        }

        return EquipBodyPart(ITEM_PROFILE, CREATURE_PTR, errorMessage);
    }

    bool ItemFactory::EquipBodyPartWeapon(
        const creature::CreaturePtr_t CREATURE_PTR, std::string & errorMessage)
    {
        const auto ITEM_PROFILE = ItemProfileFactory::MakeBodyPartWeapon(CREATURE_PTR);

        if (!ITEM_PROFILE.IsValid())
        {
            errorMessage = CREATURE_PTR->RaceName();
            errorMessage += " do not have any body part weapons to equip.";
            return true;
        }

        return EquipBodyPart(ITEM_PROFILE, CREATURE_PTR, errorMessage);
    }

    bool ItemFactory::EquipBodyPart(
        const ItemProfile & ITEM_PROFILE,
        const creature::CreaturePtr_t CREATURE_PTR,
        std::string & errorMessage)
    {
        const auto ITEM_PTR = Make(ITEM_PROFILE, CREATURE_PTR->RaceName());

        const auto ADD_STR { CREATURE_PTR->ItemAdd(ITEM_PTR) };

        if (ADD_STR != creature::Creature::ITEM_ACTION_SUCCESS_STR_)
        {
            errorMessage = ADD_STR;
            return false;
        }

        const auto EQUIP_STR { CREATURE_PTR->ItemEquip(ITEM_PTR) };

        if (EQUIP_STR != creature::Creature::ITEM_ACTION_SUCCESS_STR_)
        {
            errorMessage = EQUIP_STR;
            return false;
        }

        errorMessage.clear();
        return true;
    }

} // namespace item
} // namespace heroespath
