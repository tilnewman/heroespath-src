// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// nonplayer-inventory-factory.cpp
//
/*
#include "nonplayer-inventory-factory.hpp"

#include "creature/creature.hpp"
#include "creature/nonplayer-inventory-chances.hpp"
#include "item/item-factory.hpp"
#include "item/item.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/log-macros.hpp"
#include "misc/random.hpp"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>

namespace heroespath
{
namespace creature
{
    namespace nonplayer
    {

        void InventoryFactory::SetupCreatureInventory(const CreaturePtr_t CREATURE_PTR) const
        {
            const auto INVENTORY_CHANCES { ChanceFactory::Instance()->Make(CREATURE_PTR) };
            CREATURE_PTR->CoinsAdj(Make_Coins(INVENTORY_CHANCES));

            //.first is for equipped items and .second is for unequipped items
            const auto ITEM_PVEC_PAIR { MakeItemSet(INVENTORY_CHANCES, CREATURE_PTR) };
            for (const auto & NEXT_ITEM_PTR : ITEM_PVEC_PAIR.first)
            {
                const auto ITEM_ADD_RESULT(CREATURE_PTR->ItemAdd(NEXT_ITEM_PTR));
                if (ITEM_ADD_RESULT.empty() == false)
                {
                    M_HP_LOG_ERR(
                        "creature::nonplayer::InventoryFactory::SetupCreatureInventory"
                        << "[to equip - add step](creature=\n{" << CREATURE_PTR->ToString()
                        << "}) unable to add the item=\n{" << NEXT_ITEM_PTR->ToString()
                        << "} with reported error=\n\"" << ITEM_ADD_RESULT << "\".  Proceeding...");

                    item::ItemWarehouse::Access().Free(NEXT_ITEM_PTR);
                }
                else
                {
                    if (NEXT_ITEM_PTR->Category() & item::Category::Equipable)
                    {
                        const auto ITEM_EQUIP_RESULT(CREATURE_PTR->ItemEquip(NEXT_ITEM_PTR));
                        if (ITEM_EQUIP_RESULT.empty() == false)
                        {
                            M_HP_LOG_ERR(
                                "creature::nonplayer::InventoryFactory::"
                                << "SetupCreatureInventory[to equip - equip step](creature=\n{"
                                << CREATURE_PTR->ToString() << "}) unable to add the item=\n{"
                                << NEXT_ITEM_PTR->ToString() << "} with reported error=\n\""
                                << ITEM_EQUIP_RESULT << "\".  Proceeding...");

                            CREATURE_PTR->ItemRemove(NEXT_ITEM_PTR);
                            item::ItemWarehouse::Access().Free(NEXT_ITEM_PTR);
                        }
                    }
                }
            }

            for (const auto & NEXT_ITEM_PTR : ITEM_PVEC_PAIR.second)
            {
                const std::string ITEM_ADD_RESULT(CREATURE_PTR->ItemAdd(NEXT_ITEM_PTR));
                if (ITEM_ADD_RESULT.empty() == false)
                {
                    M_HP_LOG_ERR(
                        "creature::nonplayer::InventoryFactory::"
                        << "SetupCreatureInventory[not to equip]\n(creature={"
                        << CREATURE_PTR->ToString() << "})\nunable to add the item=\n{"
                        << NEXT_ITEM_PTR->ToString() << "} with reported error=\n\""
                        << ITEM_ADD_RESULT << "\".  Proceeding...");

                    item::ItemWarehouse::Access().Free(NEXT_ITEM_PTR);
                }
            }

            CREATURE_PTR->SetHeldWeaponsToBest();
        }

        const IItemPVecPair_t InventoryFactory::MakeItemSet(
            const InventoryChances & CHANCES, const CreaturePtr_t CHARACTER_PTR) const
        {
            IItemPVecPair_t itemsPtrVecPair;

            const auto MAT_PAIR = item::Material::SkinMaterials(CHARACTER_PTR->Race());

            if (MAT_PAIR.first < item::Material::Count)
            {
                itemsPtrVecPair.first.emplace_back(ItemFactory::Make(
                    ItemProfile(
                        Skins::Flesh,
                        CHARACTER_PTR->IsPixie(),
                        MAT_PAIR.first,
                        MAT_PAIR.second),
                    CHARACTER_PTR->RaceName()));
            }

            {
                const auto CLOTHING_ITEMS_PVEC_PAIR { MakeItemSet_Clothing(
                    CHANCES.clothes, CHARACTER_PTR->IsPixie()) };

                std::copy(
                    CLOTHING_ITEMS_PVEC_PAIR.first.begin(),
                    CLOTHING_ITEMS_PVEC_PAIR.first.end(),
                    back_inserter(itemsPtrVecPair.first));

                std::copy(
                    CLOTHING_ITEMS_PVEC_PAIR.second.begin(),
                    CLOTHING_ITEMS_PVEC_PAIR.second.end(),
                    back_inserter(itemsPtrVecPair.second));
            }

            bool hasTwoHandedWeapons { false };

            {
                auto weaponItemsPVecPair { MakeItemSet_Weapons(CHANCES.weapon, CHARACTER_PTR) };

                // if pixie then only pixie valid item::weapons
                if (CHARACTER_PTR->IsPixie())
                {
                    auto hasNoPixieVersion { [](const auto ITEM_PTR) {
                        return (
                            ((ITEM_PTR->WeaponInfo().Type() & item::Weapon::Knife) == 0)
                            && (ITEM_PTR->WeaponInfo().IsStaff() == false));
                    } };

                    RemoveItemsAndFree(weaponItemsPVecPair.first, hasNoPixieVersion);
                }

                std::copy(
                    weaponItemsPVecPair.first.begin(),
                    weaponItemsPVecPair.first.end(),
                    back_inserter(itemsPtrVecPair.first));

                std::copy(
                    weaponItemsPVecPair.second.begin(),
                    weaponItemsPVecPair.second.end(),
                    back_inserter(itemsPtrVecPair.second));

                hasTwoHandedWeapons = ContainsTwoHandedWeapon(weaponItemsPVecPair.first);
            }

            {
                const auto BODY_WEAPON_ITEMS_PVEC { MakeItemSet_BodyWeapons(
                    CHANCES.weapon, CHARACTER_PTR, hasTwoHandedWeapons) };

                std::copy(
                    BODY_WEAPON_ITEMS_PVEC.begin(),
                    BODY_WEAPON_ITEMS_PVEC.end(),
                    back_inserter(itemsPtrVecPair.first));
            }

            auto armorItemsPVecPair { MakeItemSet_Armor(
                CHANCES.armor, CHARACTER_PTR, hasTwoHandedWeapons) };

            // remove clothing items that might conflict with armor equipping
            for (const auto & NEXT_ITEM_PTR : armorItemsPVecPair.first)
            {
                if (NEXT_ITEM_PTR->ArmorType() == item::Armor::Boot)
                {
                    RemoveArmorTypeFromVecAndFree(item::Armor::Boot, itemsPtrVecPair.first);
                }
                else if (NEXT_ITEM_PTR->ArmorType() == item::Armor::Pant)
                {
                    RemoveArmorTypeFromVecAndFree(item::Armor::Pant, itemsPtrVecPair.first);
                }
                else if (NEXT_ITEM_PTR->ArmorType() == item::Armor::Shirt)
                {
                    RemoveArmorTypeFromVecAndFree(item::Armor::Shirt, itemsPtrVecPair.first);
                }
                else if (NEXT_ITEM_PTR->ArmorType() == item::Armor::Gauntlet)
                {
                    RemoveArmorTypeFromVecAndFree(item::Armor::Gauntlet, itemsPtrVecPair.first);
                }
            }

            // if pixie then only pixie valid armor
            if (CHARACTER_PTR->IsPixie())
            {
                // helm/shield/aventail/plate
                auto hasNoPixieVersion { [](const auto ITEM_PTR) {
                    const auto & ARMOR_INFO { ITEM_PTR->ArmorInfo() };

                    return (
                        ARMOR_INFO.IsShield() || ARMOR_INFO.IsHelm() || ARMOR_INFO.IsAventail()
                        || (ARMOR_INFO.Form() == item::Forms::Plate));
                } };

                RemoveItemsAndFree(armorItemsPVecPair.first, hasNoPixieVersion);
            }

            // no vests on beasts
            auto isVestOnBeast { [CHARACTER_PTR](const auto ITEM_PTR) {
                return (
                    ITEM_PTR->ArmorInfo().IsMinor(Covers::Vest)
                    && ((CHARACTER_PTR->Race() == race::Dragon)
                        || (CHARACTER_PTR->Race() == race::Wolfen)));
            } };

            RemoveItemsAndFree(armorItemsPVecPair.first, isVestOnBeast);

            // no gauntlets without fingers
            auto isGauntletWithoutFingers { [CHARACTER_PTR](const auto ITEM_PTR) {
                return (
                    (ITEM_PTR->ArmorType() == item::Armor::Gauntlet)
                    && (CHARACTER_PTR->Body().HasFingers() == false));
            } };

            RemoveItemsAndFree(armorItemsPVecPair.first, isGauntletWithoutFingers);

            // no shields without fingers
            auto isShieldWithoutFingers { [CHARACTER_PTR](const auto ITEM_PTR) {
                return (
                    (ITEM_PTR->ArmorType() == item::Armor::Shield)
                    && (CHARACTER_PTR->Body().HasFingers() == false));
            } };

            RemoveItemsAndFree(armorItemsPVecPair.first, isShieldWithoutFingers);

            // no helms with horns
            auto isHeldWithHorns { [CHARACTER_PTR](const auto ITEM_PTR) {
                return (
                    (ITEM_PTR->ArmorType() == item::Armor::Helm)
                    && CHARACTER_PTR->Body().HasHorns());
            } };

            RemoveItemsAndFree(armorItemsPVecPair.first, isHeldWithHorns);

            // no pants without legs
            auto isPantWithoutLegs { [CHARACTER_PTR](const auto ITEM_PTR) {
                return (
                    (ITEM_PTR->ArmorType() == item::Armor::Pant)
                    && (CHARACTER_PTR->Body().HasLegs() == false));
            } };

            RemoveItemsAndFree(armorItemsPVecPair.first, isPantWithoutLegs);

            // no boots without feet
            auto isBootWithoutFeet { [CHARACTER_PTR](const auto ITEM_PTR) {
                return (
                    (ITEM_PTR->ArmorType() == item::Armor::Boot)
                    && (CHARACTER_PTR->Body().HasLegs() == false));
            } };

            RemoveItemsAndFree(armorItemsPVecPair.first, isBootWithoutFeet);

            // finally, add the armor items
            std::copy(
                armorItemsPVecPair.first.begin(),
                armorItemsPVecPair.first.end(),
                back_inserter(itemsPtrVecPair.first));

            std::copy(
                armorItemsPVecPair.second.begin(),
                armorItemsPVecPair.second.end(),
                back_inserter(itemsPtrVecPair.second));

            // one last check to remove armor from beast inventories
            auto isArmorOnBeast { [CHARACTER_PTR](const auto ITEM_PTR) {
                return (ITEM_PTR->IsArmor() && CHARACTER_PTR->IsBeast());
            } };

            RemoveItemsAndFree(itemsPtrVecPair.first, isArmorOnBeast);
            RemoveItemsAndFree(itemsPtrVecPair.second, isArmorOnBeast);

            // prevent adding an aventail if there is no helm
            const auto CONTAINS_HELM { std::find_if(
                                           itemsPtrVecPair.first.begin(),
                                           itemsPtrVecPair.first.end(),
                                           [](const item::ItemPtr_t PTR) {
                                               return (PTR->ArmorType() == item::Armor::Helm);
                                           })
                                       != itemsPtrVecPair.first.end() };

            if (CONTAINS_HELM == false)
            {
                const auto WILL_ADD_AVENTAIL {
                    std::find_if(
                        itemsPtrVecPair.first.begin(),
                        itemsPtrVecPair.first.end(),
                        [](const item::ItemPtr_t PTR) {
                            return (PTR->ArmorType() == item::Armor::Aventail);
                        })
                    != itemsPtrVecPair.first.end()
                };

                if (WILL_ADD_AVENTAIL)
                {
                    RemoveArmorTypeFromVecAndFree(item::Armor::Aventail, itemsPtrVecPair.first);
                }
            }

            // sort equipped items so that helms appear first, so that we don't get errors trying to
            // add an aventail without a helm...
            std::sort(
                std::begin(itemsPtrVecPair.first),
                std::end(itemsPtrVecPair.first),
                [](const auto & ITEM_A_PTR, const auto &) {
                    return ITEM_A_PTR->ArmorInfo().IsHelm();
                });

            return itemsPtrVecPair;
        }

        const IItemPVecPair_t InventoryFactory::MakeItemSet_Clothing(
            const ClothingChances & CHANCES, const bool IS_PIXIE) const
        {
            // Assume there is only one of each article of clothing.

            // TODO enchantments

            IItemPVecPair_t itemsPtrVecPair;

            if (CHANCES.boots.IsOwned())
            {
                itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                    item::Armor::Boot,
                    item::Forms::Plain,
                    IS_PIXIE,
                    CHANCES.boots.RandomMaterialPri(),
                    CHANCES.boots.RandomMaterialSec())));
            }

            if (CHANCES.gloves.IsOwned())
            {
                itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                    item::Armor::Gauntlet,
                    item::Forms::Plain,
                    IS_PIXIE,
                    CHANCES.gloves.RandomMaterialPri(),
                    CHANCES.gloves.RandomMaterialSec())));
            }

            if (CHANCES.pants.IsOwned())
            {
                itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                    item::Armor::Pant,
                    item::Forms::Plain,
                    IS_PIXIE,
                    CHANCES.pants.RandomMaterialPri(),
                    CHANCES.pants.RandomMaterialSec())));
            }

            if (CHANCES.shirt.IsOwned())
            {
                itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                    item::Armor::Shirt,
                    item::Forms::Plain,
                    IS_PIXIE,
                    CHANCES.shirt.RandomMaterialPri(),
                    CHANCES.shirt.RandomMaterialSec())));
            }

            if (CHANCES.vest.IsOwned())
            {
                itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                    item::Covers::Vest,
                    IS_PIXIE,
                    CHANCES.vest.RandomMaterialPri(),
                    CHANCES.vest.RandomMaterialSec())));
            }

            const auto COVER_TYPE { CHANCES.RandomCoverType() };
            if (COVER_TYPE < item::Covers::Count)
            {
                nonplayer::ItemChances itemChances;
                const auto WAS_FOUND { CHANCES.cover_map.Find(COVER_TYPE, itemChances) };

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    WAS_FOUND,
                    "creature::nonplayer::InventoryFactory::MakeItemSet_Clothing() failed to"
                        << " find \"" << COVER_TYPE << "\".");

                itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                    COVER_TYPE,
                    IS_PIXIE,
                    CHANCES.vest.RandomMaterialPri(),
                    CHANCES.vest.RandomMaterialSec())));
            }

            return itemsPtrVecPair;
        }

        const IItemPVecPair_t InventoryFactory::MakeItemSet_Weapons(
            const WeaponChances & WEAPON_CHANCES, const CreaturePtr_t CHARACTER_PTR) const
        {
            IItemPVecPair_t itemsPtrVecPair;

            using KindChancePair_t = std::pair<int, float>;
            misc::VectorMap<item::Weapon::Enum, KindChancePair_t> typeKindChanceMap;

            // TODO handle multiple item::weapons of the same type/kind

            // TODO handle intentionally unequipped item::weapons

            const auto RANDOM_SELECTED_AXE_PAIR(
                RandomSelectWeapon<item::Axes::Enum>(WEAPON_CHANCES.axe_map));

            if ((RANDOM_SELECTED_AXE_PAIR.first < item::Axes::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_AXE_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[item::Weapon::Axe] = std::make_pair(
                    RANDOM_SELECTED_AXE_PAIR.first, RANDOM_SELECTED_AXE_PAIR.second);
            }

            const auto RANDOM_SELECTED_BLADEDSTAFF_PAIR(
                RandomSelectWeapon<item::Bladedstaffs::Enum>(
                    WEAPON_CHANCES.bladedstaff_map));

            if ((RANDOM_SELECTED_BLADEDSTAFF_PAIR.first < item::Bladedstaffs::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_BLADEDSTAFF_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[item::Weapon::Bladedstaff] = std::make_pair(
                    RANDOM_SELECTED_BLADEDSTAFF_PAIR.first,
                    RANDOM_SELECTED_BLADEDSTAFF_PAIR.second);
            }

            const auto RANDOM_SELECTED_CLUB_PAIR(
                RandomSelectWeapon<item::Clubs::Enum>(WEAPON_CHANCES.club_map));

            if ((RANDOM_SELECTED_CLUB_PAIR.first < item::Clubs::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_CLUB_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[item::Weapon::Club] = std::make_pair(
                    RANDOM_SELECTED_CLUB_PAIR.first, RANDOM_SELECTED_CLUB_PAIR.second);
            }

            const auto RANDOM_SELECTED_PROJECTILE_PAIR(
                RandomSelectWeapon<item::Projectiles::Enum>(WEAPON_CHANCES.projectile_map));

            if ((RANDOM_SELECTED_PROJECTILE_PAIR.first < item::Projectiles::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_PROJECTILE_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[item::Weapon::Projectile] = std::make_pair(
                    RANDOM_SELECTED_PROJECTILE_PAIR.first, RANDOM_SELECTED_PROJECTILE_PAIR.second);
            }

            const auto RANDOM_SELECTED_SWORD_PAIR(
                RandomSelectWeapon<item::Swords::Enum>(WEAPON_CHANCES.sword_map));

            if ((RANDOM_SELECTED_SWORD_PAIR.first < item::Swords::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_SWORD_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[item::Weapon::Sword] = std::make_pair(
                    RANDOM_SELECTED_SWORD_PAIR.first, RANDOM_SELECTED_SWORD_PAIR.second);
            }

            const auto RANDOM_SELECTED_WHIP_PAIR(
                RandomSelectWeapon<item::Whips::Enum>(WEAPON_CHANCES.whip_map));

            if ((RANDOM_SELECTED_WHIP_PAIR.first < item::Whips::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_WHIP_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[item::Weapon::Whip] = std::make_pair(
                    RANDOM_SELECTED_WHIP_PAIR.first, RANDOM_SELECTED_WHIP_PAIR.second);
            }

            // knife/dagger
            {
                auto countChance { 0.0f };
                if (WEAPON_CHANCES.knife.num_owned_map.Find(1, countChance))
                {
                    typeKindChanceMap[item::Weapon::Knife] = std::make_pair(0, countChance);
                }
            }

            // staff/quarterstaff
            {
                auto countChance { 0.0f };
                if (WEAPON_CHANCES.staff.num_owned_map.Find(1, countChance))
                {
                    typeKindChanceMap[item::Weapon::Staff] = std::make_pair(0, countChance);
                }
            }

            // check for no-item::weapon case
            if (typeKindChanceMap.Empty())
            {
                return itemsPtrVecPair;
            }

            // random select item::weapon
            auto randomSelectedWeaponType { typeKindChanceMap.begin()->first };
            auto chanceCumulative { 0.0f };
            const auto RAND_WEAPON_TYPE_CHANCE { misc::Random(1.0f) };
            for (const auto & NEXT_TYPEKIND_PAIR : typeKindChanceMap)
            {
                chanceCumulative += NEXT_TYPEKIND_PAIR.second.second;
                if (RAND_WEAPON_TYPE_CHANCE < chanceCumulative)
                {
                    randomSelectedWeaponType = NEXT_TYPEKIND_PAIR.first;
                    break;
                }
            }

            // create item::weapon
            switch (randomSelectedWeaponType)
            {
                case item::Weapon::Knife:
                {
                    const auto IS_DAGGER { misc::Random(1.0f) < WEAPON_CHANCES.knife.is_dagger };

                    // prevent adding a knife/dagger if invalid
                    if (WEAPON_CHANCES.knife.mat_map_pri.Empty())
                    {
                        break;
                    }

                    if (IS_DAGGER)
                    {
                        itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                            item::Knifes::Dagger,
                            CHARACTER_PTR->IsPixie(),
                            WEAPON_CHANCES.knife.RandomMaterialPri(),
                            WEAPON_CHANCES.knife.RandomMaterialSec())));
                    }
                    else
                    {
                        itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                            item::Knifes::Knife,
                            CHARACTER_PTR->IsPixie(),
                            WEAPON_CHANCES.knife.RandomMaterialPri(),
                            WEAPON_CHANCES.knife.RandomMaterialSec())));
                    }

                    break;
                }
                case item::Weapon::Staff:
                {
                    if (misc::Random(1.0f) < WEAPON_CHANCES.staff.is_quarterstaff)
                    {
                        itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                            item::Staffs::Quarterstaff,
                            false,
                            WEAPON_CHANCES.staff.RandomMaterialPri(),
                            WEAPON_CHANCES.staff.RandomMaterialSec())));
                    }
                    else
                    {
                        itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                            item::Staffs::Staff,
                            CHARACTER_PTR->IsPixie(),
                            WEAPON_CHANCES.staff.RandomMaterialPri(),
                            WEAPON_CHANCES.staff.RandomMaterialSec())));
                    }

                    break;
                }
                case item::Weapon::Axe:
                {
                    const auto AXE_TYPE { static_cast<item::Axes::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    nonplayer::ItemChances axeChances;
                    const auto WAS_AXE_FOUND { WEAPON_CHANCES.axe_map.Find(AXE_TYPE, axeChances) };

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        WAS_AXE_FOUND,
                        "creature::nonplayer::InventoryFactory::MakeItemSet_Weapons"
                            << "(creature=\"" << CHARACTER_PTR->ToString()
                            << "\") randomly selected item::weapon type=\""
                            << EnumUtil<item::Weapon>::ToString(randomSelectedWeaponType)
                            << "\" and kind=\"" << NAMEOF_ENUM(AXE_TYPE)
                            << "\" -but that item::weapon was not found in the original "
                               "WEAPON_CHANCES "
                               "object.");

                    // if the primary material is wood, make sure there is a valid secondary
                    // material
                    const auto MAT_PRI { axeChances.RandomMaterialPri() };
                    auto materialSec { axeChances.RandomMaterialSec() };
                    if ((MAT_PRI == item::Material::Wood) && (materialSec >= item::Material::Count))
                    {
                        materialSec = item::Material::Steel;
                    }

                    itemsPtrVecPair.first.emplace_back(ItemFactory::Make(
                        ItemProfile(AXE_TYPE, CHARACTER_PTR->IsPixie(), MAT_PRI, materialSec)));

                    break;
                }
                case item::Weapon::Bladedstaff:
                {
                    const auto BLADEDSTAFF_TYPE { static_cast<item::Bladedstaffs::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    nonplayer::ItemChances bstaffChances;

                    const auto WAS_BSTAFF_FOUND { WEAPON_CHANCES.bladedstaff_map.Find(
                        BLADEDSTAFF_TYPE, bstaffChances) };

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        WAS_BSTAFF_FOUND,
                        "creature::nonplayer::InventoryFactory::MakeItemSet_Weapons"
                            << "(creature=\"" << CHARACTER_PTR->ToString()
                            << "\") randomly selected item::weapon type=\""
                            << EnumUtil<item::Weapon>::ToString(randomSelectedWeaponType)
                            << "\" and kind=\"" << NAMEOF_ENUM(BLADEDSTAFF_TYPE)
                            << "\" -but that item::weapon was not found in the original "
                               "WEAPON_CHANCES "
                               "object.");

                    // if the primary material is wood, make sure there is a valid secondary
                    // material
                    const auto MAT_PRI { bstaffChances.RandomMaterialPri() };
                    auto materialSec { bstaffChances.RandomMaterialSec() };

                    if ((MAT_PRI == item::Material::Wood) && (materialSec >= item::Material::Count))
                    {
                        materialSec = item::Material::Steel;
                    }

                    itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                        BLADEDSTAFF_TYPE, CHARACTER_PTR->IsPixie(), MAT_PRI, materialSec)));

                    break;
                }
                case item::Weapon::Club:
                {
                    const auto CLUB_TYPE { static_cast<item::Clubs::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    nonplayer::ItemChances clubChances;

                    const auto WAS_CLUB_FOUND { WEAPON_CHANCES.club_map.Find(
                        CLUB_TYPE, clubChances) };

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        WAS_CLUB_FOUND,
                        "creature::nonplayer::InventoryFactory::MakeItemSet_Weapons("
                            << "creature=\"" << CHARACTER_PTR->ToString()
                            << "\") randomly selected item::weapon type=\""
                            << EnumUtil<item::Weapon>::ToString(randomSelectedWeaponType)
                            << "\" and kind=\"" << NAMEOF_ENUM(CLUB_TYPE)
                            << "\" -but that item::weapon was not found in the original "
                               "WEAPON_CHANCES "
                               "object.");

                    // if the primary material is wood, make sure there is a valid secondary
                    // material
                    const auto MAT_PRI { clubChances.RandomMaterialPri() };
                    auto materialSec { clubChances.RandomMaterialSec() };

                    if ((MAT_PRI == item::Material::Wood) && (materialSec >= item::Material::Count))
                    {
                        materialSec = item::Material::Steel;
                    }

                    itemsPtrVecPair.first.emplace_back(ItemFactory::Make(
                        ItemProfile(CLUB_TYPE, CHARACTER_PTR->IsPixie(), MAT_PRI, materialSec)));

                    break;
                }
                case item::Weapon::Projectile:
                {
                    const auto PROJECTILE_TYPE { static_cast<item::Projectiles::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    nonplayer::ItemChances projChances;

                    const auto WAS_PROJ_FOUND { WEAPON_CHANCES.projectile_map.Find(
                        PROJECTILE_TYPE, projChances) };

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        WAS_PROJ_FOUND,
                        "creature::nonplayer::InventoryFactory::MakeItemSet_Weapons("
                            << "creature=\"" << CHARACTER_PTR->ToString()
                            << "\") randomly selected item::weapon type=\""
                            << EnumUtil<item::Weapon>::ToString(randomSelectedWeaponType)
                            << "\" and kind=\"" << NAMEOF_ENUM(PROJECTILE_TYPE)
                            << "\" -but that item::weapon was not found in the original "
                               "WEAPON_CHANCES "
                               "object.");

                    itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                        PROJECTILE_TYPE,
                        CHARACTER_PTR->IsPixie(),
                        projChances.RandomMaterialPri(),
                        projChances.RandomMaterialSec())));

                    break;
                }
                case item::Weapon::Sword:
                {
                    const auto SWORD_TYPE { static_cast<item::Swords::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    nonplayer::ItemChances swordChances;

                    const auto WAS_SWORD_FOUND { WEAPON_CHANCES.sword_map.Find(
                        SWORD_TYPE, swordChances) };

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        WAS_SWORD_FOUND,
                        "creature::nonplayer::InventoryFactory::MakeItemSet_Weapons("
                            << "creature=\"" << CHARACTER_PTR->ToString()
                            << "\") randomly selected item::weapon type=\""
                            << EnumUtil<item::Weapon>::ToString(randomSelectedWeaponType)
                            << "\" and kind=\"" << NAMEOF_ENUM(SWORD_TYPE)
                            << "\" -but that item::weapon was not found in the original "
                               "WEAPON_CHANCES "
                               "object.");

                    itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                        SWORD_TYPE,
                        CHARACTER_PTR->IsPixie(),
                        swordChances.RandomMaterialPri(),
                        swordChances.RandomMaterialSec())));

                    break;
                }
                case item::Weapon::Whip:
                {
                    const auto WHIP_TYPE { static_cast<item::Whips::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    nonplayer::ItemChances whipChances;

                    const auto WAS_WHIP_FOUND { WEAPON_CHANCES.whip_map.Find(
                        WHIP_TYPE, whipChances) };

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        WAS_WHIP_FOUND,
                        "creature::nonplayer::InventoryFactory::MakeItemSet_Weapons("
                            << "creature=\"" << CHARACTER_PTR->ToString()
                            << "\") randomly selected item::weapon type=\""
                            << EnumUtil<item::Weapon>::ToString(randomSelectedWeaponType)
                            << "\" and kind=\"" << NAMEOF_ENUM(WHIP_TYPE)
                            << "\" -but that item::weapon was not found in the original "
                               "WEAPON_CHANCES "
                               "object.");

                    itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                        WHIP_TYPE,
                        CHARACTER_PTR->IsPixie(),
                        whipChances.RandomMaterialPri(),
                        whipChances.RandomMaterialSec())));

                    break;
                }
                case item::Weapon::Bladed:
                case item::Weapon::Melee:
                case item::Weapon::None:
                case item::Weapon::Pointed:
                case item::Weapon::BodyPart:
                default:
                {
                    std::ostringstream ss;
                    ss << "creature::nonplayer::InventoryFactory::MakeItemSet_Weapons("
                       << "creature=\"" << CHARACTER_PTR->ToString()
                       << "\") failed to find a valid random selected item::weapon.  "
                          "(item::Weapon="
                       << EnumUtil<item::Weapon>::ToString(randomSelectedWeaponType) << "\")";

                    throw std::runtime_error(ss.str());
                }
            }

            return itemsPtrVecPair;
        }

        const IItemPVecPair_t InventoryFactory::MakeItemSet_Armor(
            const ArmorChances & CHANCES,
            const CreaturePtr_t CHARACTER_PTR,
            const bool HAS_TWO_HANDED_WEAPON) const
        {
            IItemPVecPair_t itemsPtrVecPair;

            // beast cannot even be given armor, let alone equip it
            if (CHARACTER_PTR->IsBeast())
            {
                return itemsPtrVecPair;
            }

            try
            {
                if (CHANCES.aventail.IsOwned())
                {
                    itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                        item::Armor::Aventail,
                        CHANCES.aventail.RandomArmorForm(),
                        CHARACTER_PTR->IsPixie(),
                        CHANCES.aventail.RandomMaterialPri(),
                        CHANCES.aventail.RandomMaterialSec())));
                }
            }
            catch (...)
            {
                M_HP_LOG_ERR(
                    "creature::nonplayer::MakeItemSet_Armor(creature={"
                    << CHARACTER_PTR->ToString()
                    << "}, has_two_handed_item::weapon=" << std::boolalpha << HAS_TWO_HANDED_WEAPON
                    << ")  exception thrown while checking aventail.");

                throw;
            }

            try
            {
                if (CHANCES.boots.IsOwned())
                {
                    itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                        item::Armor::Boot,
                        CHANCES.boots.RandomArmorForm(),
                        CHARACTER_PTR->IsPixie(),
                        CHANCES.boots.RandomMaterialPri(),
                        CHANCES.boots.RandomMaterialSec())));
                }
            }
            catch (...)
            {
                M_HP_LOG_ERR(
                    "creature::nonplayer::MakeItemSet_Armor(creature={"
                    << CHARACTER_PTR->ToString()
                    << "}, has_two_handed_item::weapon=" << std::boolalpha << HAS_TWO_HANDED_WEAPON
                    << ")  exception thrown while checking boots.");

                throw;
            }

            try
            {
                if (CHANCES.bracers.IsOwned())
                {
                    itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                        item::Armor::Bracer,
                        CHANCES.bracers.RandomArmorForm(),
                        CHARACTER_PTR->IsPixie(),
                        CHANCES.bracers.RandomMaterialPri(),
                        CHANCES.bracers.RandomMaterialSec())));
                }
            }
            catch (...)
            {
                M_HP_LOG_ERR(
                    "creature::nonplayer::MakeItemSet_Armor(creature={"
                    << CHARACTER_PTR->ToString()
                    << "}, has_two_handed_item::weapon=" << std::boolalpha << HAS_TWO_HANDED_WEAPON
                    << ")  exception thrown while checking bracers.");

                throw;
            }

            try
            {
                if (CHANCES.gauntlets.IsOwned())
                {
                    itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                        item::Armor::Gauntlet,
                        CHANCES.gauntlets.RandomArmorForm(),
                        CHARACTER_PTR->IsPixie(),
                        CHANCES.gauntlets.RandomMaterialPri(),
                        CHANCES.gauntlets.RandomMaterialSec())));
                }
            }
            catch (...)
            {
                M_HP_LOG_ERR(
                    "creature::nonplayer::MakeItemSet_Armor(creature={"
                    << CHARACTER_PTR->ToString()
                    << "}, has_two_handed_item::weapon=" << std::boolalpha << HAS_TWO_HANDED_WEAPON
                    << ")  exception thrown while checking gauntlets.");

                throw;
            }

            try
            {
                if (CHANCES.pants.IsOwned())
                {
                    itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                        item::Armor::Pant,
                        CHANCES.pants.RandomArmorForm(),
                        CHARACTER_PTR->IsPixie(),
                        CHANCES.pants.RandomMaterialPri(),
                        CHANCES.pants.RandomMaterialSec())));
                }
            }
            catch (...)
            {
                M_HP_LOG_ERR(
                    "creature::nonplayer::MakeItemSet_Armor(creature={"
                    << CHARACTER_PTR->ToString()
                    << "}, has_two_handed_item::weapon=" << std::boolalpha << HAS_TWO_HANDED_WEAPON
                    << ")  exception thrown while checking pants.");

                throw;
            }

            try
            {
                if (CHANCES.shirt.IsOwned())
                {
                    itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                        item::Armor::Shirt,
                        CHANCES.shirt.RandomArmorForm(),
                        CHARACTER_PTR->IsPixie(),
                        CHANCES.shirt.RandomMaterialPri(),
                        CHANCES.shirt.RandomMaterialSec())));
                }
            }
            catch (...)
            {
                M_HP_LOG_ERR(
                    "creature::nonplayer::MakeItemSet_Armor(creature={"
                    << CHARACTER_PTR->ToString()
                    << "}, has_two_handed_item::weapon=" << std::boolalpha << HAS_TWO_HANDED_WEAPON
                    << ")  exception thrown while checking shirt.");

                throw;
            }

            // covers (vest/robe/cloak/cape)
            try
            {
                const auto COVER_PAIR { CHANCES.RandomCover() };
                if (COVER_PAIR.second > 0)
                {
                    nonplayer::ItemChances coverChances;

                    const auto WAS_COVER_FOUND { CHANCES.cover_map.Find(
                        COVER_PAIR.first, coverChances) };

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        WAS_COVER_FOUND,
                        "creature::nonplayer::InventoryFactory::MakeItemSet_Armor(creature=\""
                            << CHARACTER_PTR->ToString()
                            << "\") ARMOR_CHANCES.RandomCover() returned \""
                            << NAMEOF_ENUM(COVER_PAIR.first)
                            << "\", but that item was not found in the "
                               "ARMOR_CHANCES.cover_map.");

                    itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                        COVER_PAIR.first,
                        CHARACTER_PTR->IsPixie(),
                        coverChances.RandomMaterialPri(),
                        coverChances.RandomMaterialSec())));
                }
            }
            catch (...)
            {
                M_HP_LOG_ERR(
                    "creature::nonplayer::MakeItemSet_Armor(creature={"
                    << CHARACTER_PTR->ToString()
                    << "}, has_two_handed_item::weapon=" << std::boolalpha << HAS_TWO_HANDED_WEAPON
                    << ")  exception thrown while checking covers.");

                throw;
            }

            // helms
            try
            {
                const auto HELM_PAIR { CHANCES.RandomHelm() };
                if (HELM_PAIR.second > 0)
                {
                    nonplayer::ItemChances helmChances;

                    const auto WAS_HELM_FOUND { CHANCES.helm_map.Find(
                        HELM_PAIR.first, helmChances) };

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        WAS_HELM_FOUND,
                        "creature::nonplayer::InventoryFactory::MakeItemSet_Armor(creature=\""
                            << CHARACTER_PTR->ToString()
                            << "\") ARMOR_CHANCES.RandomHelm() returned \""
                            << NAMEOF_ENUM(HELM_PAIR.first)
                            << "\", but that item was not found in the "
                               "ARMOR_CHANCES.helm_map.");

                    itemsPtrVecPair.first.emplace_back(ItemFactory::Make(ItemProfile(
                        HELM_PAIR.first,
                        CHARACTER_PTR->IsPixie(),
                        helmChances.RandomMaterialPri(),
                        helmChances.RandomMaterialSec())));
                }
            }
            catch (...)
            {
                M_HP_LOG_ERR(
                    "creature::nonplayer::MakeItemSet_Armor(creature={"
                    << CHARACTER_PTR->ToString()
                    << "}, has_two_handed_item::weapon=" << std::boolalpha << HAS_TWO_HANDED_WEAPON
                    << ")  exception thrown while checking helms.");

                throw;
            }

            // shields
            try
            {
                const auto SHIELD_PAIR { CHANCES.RandomShield() };
                if (SHIELD_PAIR.second > 0)
                {
                    ItemChances shieldChances;

                    const auto WAS_SHIELD_FOUND { CHANCES.shield_map.Find(
                        SHIELD_PAIR.first, shieldChances) };

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        WAS_SHIELD_FOUND,
                        "creature::nonplayer::InventoryFactory::MakeItemSet_Armor(creature=\""
                            << CHARACTER_PTR->ToString()
                            << "\") ARMOR_CHANCES.RandomShield() returned \""
                            << NAMEOF_ENUM(SHIELD_PAIR.first)
                            << "\", but that item was not found in the "
                               "ARMOR_CHANCES.shield_map.");

                    const ItemPtr_t ITEM_PTR = ItemFactory::Make(ItemProfile(
                        SHIELD_PAIR.first,
                        CHARACTER_PTR->IsPixie(),
                        shieldChances.RandomMaterialPri(),
                        shieldChances.RandomMaterialSec()));

                    if (HAS_TWO_HANDED_WEAPON)
                    {
                        itemsPtrVecPair.second.emplace_back(ITEM_PTR);
                    }
                    else
                    {
                        itemsPtrVecPair.first.emplace_back(ITEM_PTR);
                    }
                }
            }
            catch (...)
            {
                M_HP_LOG_ERR(
                    "creature::nonplayer::MakeItemSet_Armor(creature={"
                    << CHARACTER_PTR->ToString()
                    << "}, has_two_handed_item::weapon=" << std::boolalpha << HAS_TWO_HANDED_WEAPON
                    << ")  exception thrown while checking shields.");

                throw;
            }

            return itemsPtrVecPair;
        }

        const item::ItemPVec_t InventoryFactory::MakeItemSet_BodyWeapons(
            const WeaponChances & CHANCES,
            const CreaturePtr_t CHARACTER_PTR,
            const bool HAS_TWO_HANDED_WEAPON_EQUIPPED) const
        {
            item::ItemPVec_t bodyWeapons;

            const auto MAT_PAIR = item::BodyPartWeapons::MaterialsPair(CHARACTER_PTR->Race());

            if (CHANCES.has_bite)
            {
                bodyWeapons.emplace_back(ItemFactory::Make(
                    ItemProfile(
                        item::BodyPartWeapons::Bite,
                        CHARACTER_PTR->IsPixie(),
                        MAT_PAIR.first,
                        MAT_PAIR.second),
                    CHARACTER_PTR->RaceName()));
            }

            if (CHANCES.has_claws && (HAS_TWO_HANDED_WEAPON_EQUIPPED == false))
            {
                bodyWeapons.emplace_back(ItemFactory::Make(
                    ItemProfile(
                        item::BodyPartWeapons::Claws,
                        CHARACTER_PTR->IsPixie(),
                        MAT_PAIR.first,
                        MAT_PAIR.second),
                    CHARACTER_PTR->RaceName()));
            }

            if (CHANCES.has_fists && (HAS_TWO_HANDED_WEAPON_EQUIPPED == false)
                && (CHANCES.has_claws == false))
            {
                bodyWeapons.emplace_back(ItemFactory::Make(
                    ItemProfile(
                        item::BodyPartWeapons::Fists,
                        CHARACTER_PTR->IsPixie(),
                        MAT_PAIR.first,
                        MAT_PAIR.second),
                    CHARACTER_PTR->RaceName()));
            }

            if (CHANCES.has_tentacles && (HAS_TWO_HANDED_WEAPON_EQUIPPED == false))
            {
                bodyWeapons.emplace_back(ItemFactory::Make(
                    ItemProfile(
                        item::BodyPartWeapons::Tentacles,
                        CHARACTER_PTR->IsPixie(),
                        MAT_PAIR.first,
                        MAT_PAIR.second),
                    CHARACTER_PTR->RaceName()));
            }

            if (CHANCES.has_breath)
            {
                if (CHARACTER_PTR->Role() == role::Sylavin)
                {
                    bodyWeapons.emplace_back(ItemFactory::Make(
                        ItemProfile(
                            item::BodyPartWeapons::BreathOfFrost,
                            CHARACTER_PTR->IsPixie(),
                            MAT_PAIR.first,
                            MAT_PAIR.second),
                        CHARACTER_PTR->RaceName()));
                }
                else if (CHARACTER_PTR->Role() == role::Firebrand)
                {
                    bodyWeapons.emplace_back(ItemFactory::Make(
                        ItemProfile(
                            item::BodyPartWeapons::BreathOfFire,
                            CHARACTER_PTR->IsPixie(),
                            MAT_PAIR.first,
                            MAT_PAIR.second),
                        CHARACTER_PTR->RaceName()));
                }
            }

            return bodyWeapons;
        }

        Coin_t InventoryFactory::Make_Coins(const InventoryChances & CHANCES) const
        {
            return CHANCES.RandomCoins();
        }

        bool InventoryFactory::ContainsTwoHandedWeapon(const item::ItemPVec_t & WEAPON_PVEC) const
        {
            for (const auto & NEXT_WEAPON_PTR : WEAPON_PVEC)
            {
                if (NEXT_WEAPON_PTR->IsTwoHanded())
                {
                    return true;
                }
            }

            return false;
        }

        void InventoryFactory::RemoveArmorTypeFromVecAndFree(
            const item::Armor::Enum ENUM, item::ItemPVec_t & vec) const
        {
            item::ItemPVec_t itemsToRemovePVec;

            auto ArmorTypeMatchLambda { [ENUM](const auto ITEM_PTR) {
                return (ITEM_PTR->ArmorType() == ENUM);
            } };

            std::copy_if(
                vec.begin(),
                vec.end(),
                std::back_inserter(itemsToRemovePVec),
                ArmorTypeMatchLambda);

            vec.erase(std::remove_if(vec.begin(), vec.end(), ArmorTypeMatchLambda), vec.end());

            item::ItemWarehouse::Access().Free(itemsToRemovePVec);
        }

    } // namespace nonplayer
} // namespace creature
} // namespace heroespath
*/
