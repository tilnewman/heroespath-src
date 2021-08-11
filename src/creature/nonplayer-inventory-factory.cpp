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
#include "nonplayer-inventory-factory.hpp"

#include "creature/creature.hpp"
#include "creature/nonplayer-inventory-chances.hpp"
#include "item/item-factory.hpp"
#include "item/item.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/log-macros.hpp"
#include "misc/random.hpp"

#include <algorithm>
#include <exception>
#include <iterator>
#include <sstream>
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
                    if (NEXT_ITEM_PTR->Category() & item::category::Equipable)
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

            if (item::material::SkinMaterial(CHARACTER_PTR->Race()).first
                != item::material::Nothing)
            {
                itemsPtrVecPair.first.emplace_back(
                    itemFactory_.Make(item::body_part::Skin, CHARACTER_PTR));
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
                            ((ITEM_PTR->WeaponInfo().Type() & item::weapon_type::Knife) == 0)
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
                if (NEXT_ITEM_PTR->ArmorType() == item::armor_type::Boots)
                {
                    RemoveArmorTypeFromVecAndFree(item::armor_type::Boots, itemsPtrVecPair.first);
                }
                else if (NEXT_ITEM_PTR->ArmorType() == item::armor_type::Pants)
                {
                    RemoveArmorTypeFromVecAndFree(item::armor_type::Pants, itemsPtrVecPair.first);
                }
                else if (NEXT_ITEM_PTR->ArmorType() == item::armor_type::Shirt)
                {
                    RemoveArmorTypeFromVecAndFree(item::armor_type::Shirt, itemsPtrVecPair.first);
                }
                else if (NEXT_ITEM_PTR->ArmorType() == item::armor_type::Gauntlets)
                {
                    RemoveArmorTypeFromVecAndFree(
                        item::armor_type::Gauntlets, itemsPtrVecPair.first);
                }
            }

            // if pixie then only pixie valid armor
            if (CHARACTER_PTR->IsPixie())
            {
                // helm/shield/aventail/plate
                auto hasNoPixieVersion { [](const auto ITEM_PTR) {
                    return (
                        ITEM_PTR->ArmorInfo().IsShield() || ITEM_PTR->ArmorInfo().IsHelm()
                        || ITEM_PTR->ArmorInfo().IsAventail()
                        || (ITEM_PTR->ArmorInfo().BaseType() == item::armor::base_type::Plate));
                } };

                RemoveItemsAndFree(armorItemsPVecPair.first, hasNoPixieVersion);
            }

            // no vests on beasts
            auto isVestOnBeast { [CHARACTER_PTR](const auto ITEM_PTR) {
                return (
                    (ITEM_PTR->ArmorInfo().CoverType() == item::armor::cover_type::Vest)
                    && ((CHARACTER_PTR->Race() == race::Dragon)
                        || (CHARACTER_PTR->Race() == race::Wolfen)));
            } };

            RemoveItemsAndFree(armorItemsPVecPair.first, isVestOnBeast);

            // no gauntlets without fingers
            auto isGauntletWithoutFingers { [CHARACTER_PTR](const auto ITEM_PTR) {
                return (
                    (ITEM_PTR->ArmorType() == item::armor_type::Gauntlets)
                    && (CHARACTER_PTR->Body().HasFingers() == false));
            } };

            RemoveItemsAndFree(armorItemsPVecPair.first, isGauntletWithoutFingers);

            // no shields without fingers
            auto isShieldWithoutFingers { [CHARACTER_PTR](const auto ITEM_PTR) {
                return (
                    (ITEM_PTR->ArmorType() == item::armor_type::Shield)
                    && (CHARACTER_PTR->Body().HasFingers() == false));
            } };

            RemoveItemsAndFree(armorItemsPVecPair.first, isShieldWithoutFingers);

            // no helms with horns
            auto isHeldWithHorns { [CHARACTER_PTR](const auto ITEM_PTR) {
                return (
                    (ITEM_PTR->ArmorType() == item::armor_type::Helm)
                    && CHARACTER_PTR->Body().HasHorns());
            } };

            RemoveItemsAndFree(armorItemsPVecPair.first, isHeldWithHorns);

            // no pants without legs
            auto isPantsWithoutLegs { [CHARACTER_PTR](const auto ITEM_PTR) {
                return (
                    (ITEM_PTR->ArmorType() == item::armor_type::Pants)
                    && (CHARACTER_PTR->Body().HasLegs() == false));
            } };

            RemoveItemsAndFree(armorItemsPVecPair.first, isPantsWithoutLegs);

            // no boots without feet
            auto isBootsWithoutFeet { [CHARACTER_PTR](const auto ITEM_PTR) {
                return (
                    (ITEM_PTR->ArmorType() == item::armor_type::Boots)
                    && (CHARACTER_PTR->Body().HasLegs() == false));
            } };

            RemoveItemsAndFree(armorItemsPVecPair.first, isBootsWithoutFeet);

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
                                               return (PTR->ArmorType() == item::armor_type::Helm);
                                           })
                                       != itemsPtrVecPair.first.end() };

            if (CONTAINS_HELM == false)
            {
                const auto WILL_ADD_AVENTAIL {
                    std::find_if(
                        itemsPtrVecPair.first.begin(),
                        itemsPtrVecPair.first.end(),
                        [](const item::ItemPtr_t PTR) {
                            return (PTR->ArmorType() == item::armor_type::Aventail);
                        })
                    != itemsPtrVecPair.first.end()
                };

                if (WILL_ADD_AVENTAIL)
                {
                    RemoveArmorTypeFromVecAndFree(
                        item::armor_type::Aventail, itemsPtrVecPair.first);
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
                item::ItemProfile profile;

                profile.SetBoots(
                    item::armor::base_type::Plain,
                    CHANCES.boots.RandomMaterialPri(),
                    CHANCES.boots.RandomMaterialSec(),
                    item::named_type::Not,
                    item::set_type::Not,
                    item::element_type::None,
                    IS_PIXIE);

                itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
            }

            if (CHANCES.gloves.IsOwned())
            {
                item::ItemProfile profile;

                profile.SetGauntlets(
                    item::armor::base_type::Plain,
                    CHANCES.gloves.RandomMaterialPri(),
                    CHANCES.gloves.RandomMaterialSec(),
                    item::named_type::Not,
                    item::set_type::Not,
                    item::element_type::None,
                    IS_PIXIE);

                itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
            }

            if (CHANCES.pants.IsOwned())
            {
                item::ItemProfile profile;

                profile.SetPants(
                    item::armor::base_type::Plain,
                    CHANCES.gloves.RandomMaterialPri(),
                    item::material::Nothing,
                    item::named_type::Not,
                    item::set_type::Not,
                    item::element_type::None,
                    IS_PIXIE);

                itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
            }

            if (CHANCES.shirt.IsOwned())
            {
                item::ItemProfile profile;

                profile.SetShirt(
                    item::armor::base_type::Plain,
                    CHANCES.gloves.RandomMaterialPri(),
                    item::material::Nothing,
                    item::named_type::Not,
                    item::set_type::Not,
                    item::element_type::None,
                    IS_PIXIE);

                itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
            }

            if (CHANCES.vest.IsOwned())
            {
                item::ItemProfile profile;

                profile.SetCover(
                    item::armor::cover_type::Vest,
                    CHANCES.vest.RandomMaterialPri(),
                    CHANCES.vest.RandomMaterialSec(),
                    item::named_type::Not,
                    item::set_type::Not,
                    item::element_type::None,
                    IS_PIXIE);

                itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
            }

            const auto COVER_TYPE { CHANCES.RandomCoverType() };
            if (COVER_TYPE != item::armor::cover_type::Count)
            {
                nonplayer::ItemChances itemChances;
                const auto WAS_FOUND { CHANCES.cover_map.Find(COVER_TYPE, itemChances) };

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    WAS_FOUND,
                    "creature::nonplayer::InventoryFactory::MakeItemSet_Clothing() failed to"
                        << " find \"" << COVER_TYPE << "\".");

                item::ItemProfile profile;

                profile.SetCover(
                    COVER_TYPE,
                    CHANCES.vest.RandomMaterialPri(),
                    CHANCES.vest.RandomMaterialSec(),
                    item::named_type::Not,
                    item::set_type::Not,
                    item::element_type::None,
                    IS_PIXIE);

                itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
            }

            return itemsPtrVecPair;
        }

        const IItemPVecPair_t InventoryFactory::MakeItemSet_Weapons(
            const WeaponChances & WEAPON_CHANCES, const CreaturePtr_t CHARACTER_PTR) const
        {
            IItemPVecPair_t itemsPtrVecPair;

            using KindChancePair_t = std::pair<int, float>;
            misc::VectorMap<item::weapon_type::Enum, KindChancePair_t> typeKindChanceMap;

            // TODO handle multiple item::weapons of the same type/kind

            // TODO handle intentionally unequipped item::weapons

            const auto RANDOM_SELECTED_AXE_PAIR(
                RandomSelectWeapon<item::weapon::axe_type::Enum>(WEAPON_CHANCES.axe_map));

            if ((RANDOM_SELECTED_AXE_PAIR.first != item::weapon::axe_type::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_AXE_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[item::weapon_type::Axe] = std::make_pair(
                    RANDOM_SELECTED_AXE_PAIR.first, RANDOM_SELECTED_AXE_PAIR.second);
            }

            const auto RANDOM_SELECTED_BLADEDSTAFF_PAIR(
                RandomSelectWeapon<item::weapon::bladedstaff_type::Enum>(
                    WEAPON_CHANCES.bladedstaff_map));

            if ((RANDOM_SELECTED_BLADEDSTAFF_PAIR.first != item::weapon::bladedstaff_type::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_BLADEDSTAFF_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[item::weapon_type::BladedStaff] = std::make_pair(
                    RANDOM_SELECTED_BLADEDSTAFF_PAIR.first,
                    RANDOM_SELECTED_BLADEDSTAFF_PAIR.second);
            }

            const auto RANDOM_SELECTED_CLUB_PAIR(
                RandomSelectWeapon<item::weapon::club_type::Enum>(WEAPON_CHANCES.club_map));

            if ((RANDOM_SELECTED_CLUB_PAIR.first != item::weapon::club_type::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_CLUB_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[item::weapon_type::Club] = std::make_pair(
                    RANDOM_SELECTED_CLUB_PAIR.first, RANDOM_SELECTED_CLUB_PAIR.second);
            }

            const auto RANDOM_SELECTED_PROJECTILE_PAIR(
                RandomSelectWeapon<item::weapon::projectile_type::Enum>(
                    WEAPON_CHANCES.projectile_map));

            if ((RANDOM_SELECTED_PROJECTILE_PAIR.first != item::weapon::projectile_type::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_PROJECTILE_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[item::weapon_type::Projectile] = std::make_pair(
                    RANDOM_SELECTED_PROJECTILE_PAIR.first, RANDOM_SELECTED_PROJECTILE_PAIR.second);
            }

            const auto RANDOM_SELECTED_SWORD_PAIR(
                RandomSelectWeapon<item::weapon::sword_type::Enum>(WEAPON_CHANCES.sword_map));

            if ((RANDOM_SELECTED_SWORD_PAIR.first != item::weapon::sword_type::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_SWORD_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[item::weapon_type::Sword] = std::make_pair(
                    RANDOM_SELECTED_SWORD_PAIR.first, RANDOM_SELECTED_SWORD_PAIR.second);
            }

            const auto RANDOM_SELECTED_WHIP_PAIR(
                RandomSelectWeapon<item::weapon::whip_type::Enum>(WEAPON_CHANCES.whip_map));

            if ((RANDOM_SELECTED_WHIP_PAIR.first != item::weapon::whip_type::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_WHIP_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[item::weapon_type::Whip] = std::make_pair(
                    RANDOM_SELECTED_WHIP_PAIR.first, RANDOM_SELECTED_WHIP_PAIR.second);
            }

            // knife/dagger
            {
                auto countChance { 0.0f };
                if (WEAPON_CHANCES.knife.num_owned_map.Find(1, countChance))
                {
                    typeKindChanceMap[item::weapon_type::Knife] = std::make_pair(0, countChance);
                }
            }

            // staff/quarterstaff
            {
                auto countChance { 0.0f };
                if (WEAPON_CHANCES.staff.num_owned_map.Find(1, countChance))
                {
                    typeKindChanceMap[item::weapon_type::Staff] = std::make_pair(0, countChance);
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
                case item::weapon_type::Knife:
                {
                    const auto IS_DAGGER { misc::Random(1.0f) < WEAPON_CHANCES.knife.is_dagger };

                    item::ItemProfile profile;

                    // prevent adding a knife/dagger if invalid
                    if (WEAPON_CHANCES.knife.mat_map_pri.Empty())
                    {
                        break;
                    }

                    if (IS_DAGGER)
                    {
                        profile.SetDagger(
                            WEAPON_CHANCES.knife.RandomMaterialPri(),
                            WEAPON_CHANCES.knife.RandomMaterialSec(),
                            item::named_type::Not,
                            item::set_type::Not,
                            item::element_type::None,
                            CHARACTER_PTR->IsPixie());
                    }
                    else
                    {
                        profile.SetKnife(
                            WEAPON_CHANCES.knife.RandomMaterialPri(),
                            WEAPON_CHANCES.knife.RandomMaterialSec(),
                            item::named_type::Not,
                            item::set_type::Not,
                            item::element_type::None,
                            CHARACTER_PTR->IsPixie());
                    }

                    itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
                    break;
                }
                case item::weapon_type::Staff:
                {
                    item::ItemProfile profile;

                    if (misc::Random(1.0f) < WEAPON_CHANCES.staff.is_quarterstaff)
                    {
                        profile.SetQuarterStaff(
                            WEAPON_CHANCES.staff.RandomMaterialPri(),
                            WEAPON_CHANCES.staff.RandomMaterialSec(),
                            item::named_type::Not,
                            item::set_type::Not,
                            item::element_type::None);
                    }
                    else
                    {
                        profile.SetStaff(
                            WEAPON_CHANCES.staff.RandomMaterialPri(),
                            WEAPON_CHANCES.staff.RandomMaterialSec(),
                            item::named_type::Not,
                            item::set_type::Not,
                            item::element_type::None,
                            item::misc_type::Not,
                            CHARACTER_PTR->IsPixie());
                    }

                    itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
                    break;
                }
                case item::weapon_type::Axe:
                {
                    const auto AXE_TYPE { static_cast<item::weapon::axe_type::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    nonplayer::ItemChances axeChances;
                    const auto WAS_AXE_FOUND { WEAPON_CHANCES.axe_map.Find(AXE_TYPE, axeChances) };

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        WAS_AXE_FOUND,
                        "creature::nonplayer::InventoryFactory::MakeItemSet_Weapons"
                            << "(creature=\"" << CHARACTER_PTR->ToString()
                            << "\") randomly selected item::weapon type=\""
                            << item::weapon_type::ToString(randomSelectedWeaponType)
                            << "\" and kind=\"" << item::weapon::axe_type::ToString(AXE_TYPE)
                            << "\" -but that item::weapon was not found in the original "
                               "WEAPON_CHANCES "
                               "object.");

                    // if the primary material is wood, make sure there is a valid secondary
                    // material
                    const auto MATERIAL_PRI { axeChances.RandomMaterialPri() };
                    auto materialSec { axeChances.RandomMaterialSec() };
                    if ((MATERIAL_PRI == item::material::Wood)
                        && (item::material::Nothing == materialSec))
                    {
                        materialSec = item::material::Steel;
                    }

                    item::ItemProfile profile;

                    profile.SetAxe(AXE_TYPE, MATERIAL_PRI, materialSec);

                    itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
                    break;
                }
                case item::weapon_type::BladedStaff:
                {
                    const auto BLADEDSTAFF_TYPE { static_cast<item::weapon::bladedstaff_type::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    nonplayer::ItemChances bstaffChances;

                    const auto WAS_BSTAFF_FOUND { WEAPON_CHANCES.bladedstaff_map.Find(
                        BLADEDSTAFF_TYPE, bstaffChances) };

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        WAS_BSTAFF_FOUND,
                        "creature::nonplayer::InventoryFactory::MakeItemSet_Weapons"
                            << "(creature=\"" << CHARACTER_PTR->ToString()
                            << "\") randomly selected item::weapon type=\""
                            << item::weapon_type::ToString(randomSelectedWeaponType)
                            << "\" and kind=\""
                            << item::weapon::bladedstaff_type::ToString(BLADEDSTAFF_TYPE)
                            << "\" -but that item::weapon was not found in the original "
                               "WEAPON_CHANCES "
                               "object.");

                    // if the primary material is wood, make sure there is a valid secondary
                    // material
                    const auto MATERIAL_PRI { bstaffChances.RandomMaterialPri() };
                    auto materialSec { bstaffChances.RandomMaterialSec() };

                    if ((MATERIAL_PRI == item::material::Wood)
                        && (item::material::Nothing == materialSec))
                    {
                        materialSec = item::material::Steel;
                    }

                    item::ItemProfile profile;
                    profile.SetBladedStaff(BLADEDSTAFF_TYPE, MATERIAL_PRI, materialSec);
                    itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
                    break;
                }
                case item::weapon_type::Club:
                {
                    const auto CLUB_TYPE { static_cast<item::weapon::club_type::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    nonplayer::ItemChances clubChances;

                    const auto WAS_CLUB_FOUND { WEAPON_CHANCES.club_map.Find(
                        CLUB_TYPE, clubChances) };

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        WAS_CLUB_FOUND,
                        "creature::nonplayer::InventoryFactory::MakeItemSet_Weapons("
                            << "creature=\"" << CHARACTER_PTR->ToString()
                            << "\") randomly selected item::weapon type=\""
                            << item::weapon_type::ToString(randomSelectedWeaponType)
                            << "\" and kind=\"" << item::weapon::club_type::ToString(CLUB_TYPE)
                            << "\" -but that item::weapon was not found in the original "
                               "WEAPON_CHANCES "
                               "object.");

                    // if the primary material is wood, make sure there is a valid secondary
                    // material
                    const auto MATERIAL_PRI { clubChances.RandomMaterialPri() };
                    auto materialSec { clubChances.RandomMaterialSec() };

                    if ((MATERIAL_PRI == item::material::Wood)
                        && (item::material::Nothing == materialSec))
                    {
                        materialSec = item::material::Steel;
                    }

                    item::ItemProfile profile;
                    profile.SetClub(CLUB_TYPE, MATERIAL_PRI, materialSec);
                    itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
                    break;
                }
                case item::weapon_type::Projectile:
                {
                    const auto PROJECTILE_TYPE { static_cast<item::weapon::projectile_type::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    nonplayer::ItemChances projChances;

                    const auto WAS_PROJ_FOUND { WEAPON_CHANCES.projectile_map.Find(
                        PROJECTILE_TYPE, projChances) };

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        WAS_PROJ_FOUND,
                        "creature::nonplayer::InventoryFactory::MakeItemSet_Weapons("
                            << "creature=\"" << CHARACTER_PTR->ToString()
                            << "\") randomly selected item::weapon type=\""
                            << item::weapon_type::ToString(randomSelectedWeaponType)
                            << "\" and kind=\""
                            << item::weapon::projectile_type::ToString(PROJECTILE_TYPE)
                            << "\" -but that item::weapon was not found in the original "
                               "WEAPON_CHANCES "
                               "object.");

                    item::ItemProfile profile;

                    profile.SetProjectile(
                        PROJECTILE_TYPE,
                        projChances.RandomMaterialPri(),
                        projChances.RandomMaterialSec());

                    itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
                    break;
                }
                case item::weapon_type::Sword:
                {
                    const auto SWORD_TYPE { static_cast<item::weapon::sword_type::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    nonplayer::ItemChances swordChances;

                    const auto WAS_SWORD_FOUND { WEAPON_CHANCES.sword_map.Find(
                        SWORD_TYPE, swordChances) };

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        WAS_SWORD_FOUND,
                        "creature::nonplayer::InventoryFactory::MakeItemSet_Weapons("
                            << "creature=\"" << CHARACTER_PTR->ToString()
                            << "\") randomly selected item::weapon type=\""
                            << item::weapon_type::ToString(randomSelectedWeaponType)
                            << "\" and kind=\"" << item::weapon::sword_type::ToString(SWORD_TYPE)
                            << "\" -but that item::weapon was not found in the original "
                               "WEAPON_CHANCES "
                               "object.");

                    item::ItemProfile profile;

                    profile.SetSword(
                        SWORD_TYPE,
                        swordChances.RandomMaterialPri(),
                        swordChances.RandomMaterialSec());

                    itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
                    break;
                }
                case item::weapon_type::Whip:
                {
                    const auto WHIP_TYPE { static_cast<item::weapon::whip_type::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    nonplayer::ItemChances whipChances;

                    const auto WAS_WHIP_FOUND { WEAPON_CHANCES.whip_map.Find(
                        WHIP_TYPE, whipChances) };

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        WAS_WHIP_FOUND,
                        "creature::nonplayer::InventoryFactory::MakeItemSet_Weapons("
                            << "creature=\"" << CHARACTER_PTR->ToString()
                            << "\") randomly selected item::weapon type=\""
                            << item::weapon_type::ToString(randomSelectedWeaponType)
                            << "\" and kind=\"" << item::weapon::whip_type::ToString(WHIP_TYPE)
                            << "\" -but that item::weapon was not found in the original "
                               "WEAPON_CHANCES "
                               "object.");

                    item::ItemProfile profile;

                    profile.SetWhip(
                        WHIP_TYPE,
                        whipChances.RandomMaterialPri(),
                        whipChances.RandomMaterialSec());

                    itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
                    break;
                }
                case item::weapon_type::Bladed:
                case item::weapon_type::Melee:
                case item::weapon_type::None:
                case item::weapon_type::Pointed:
                case item::weapon_type::BodyPart:
                default:
                {
                    std::ostringstream ss;
                    ss << "creature::nonplayer::InventoryFactory::MakeItemSet_Weapons("
                       << "creature=\"" << CHARACTER_PTR->ToString()
                       << "\") failed to find a valid random selected item::weapon.  "
                          "(item::weapon_type="
                       << item::weapon_type::ToString(randomSelectedWeaponType) << "\")";

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
                    item::ItemProfile profile;

                    profile.SetAventail(
                        CHANCES.aventail.RandomArmorBaseType(),
                        CHANCES.aventail.RandomMaterialPri(),
                        CHANCES.aventail.RandomMaterialSec());

                    itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
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
                    item::ItemProfile profile;

                    profile.SetBoots(
                        CHANCES.boots.RandomArmorBaseType(),
                        CHANCES.boots.RandomMaterialPri(),
                        CHANCES.boots.RandomMaterialSec(),
                        item::named_type::Not,
                        item::set_type::Not,
                        item::element_type::None,
                        CHARACTER_PTR->IsPixie());

                    itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
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
                    item::ItemProfile profile;

                    profile.SetBracer(
                        CHANCES.bracers.RandomArmorBaseType(),
                        CHANCES.bracers.RandomMaterialPri(),
                        CHANCES.bracers.RandomMaterialSec(),
                        item::named_type::Not,
                        item::set_type::Not,
                        item::element_type::None,
                        CHARACTER_PTR->IsPixie());

                    itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
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
                    item::ItemProfile profile;

                    profile.SetGauntlets(
                        CHANCES.gauntlets.RandomArmorBaseType(),
                        CHANCES.gauntlets.RandomMaterialPri(),
                        CHANCES.gauntlets.RandomMaterialSec(),
                        item::named_type::Not,
                        item::set_type::Not,
                        item::element_type::None,
                        CHARACTER_PTR->IsPixie());

                    itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
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
                    item::ItemProfile profile;

                    profile.SetPants(
                        CHANCES.pants.RandomArmorBaseType(),
                        CHANCES.pants.RandomMaterialPri(),
                        CHANCES.pants.RandomMaterialSec(),
                        item::named_type::Not,
                        item::set_type::Not,
                        item::element_type::None,
                        CHARACTER_PTR->IsPixie());

                    itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
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
                    item::ItemProfile profile;

                    profile.SetShirt(
                        CHANCES.shirt.RandomArmorBaseType(),
                        CHANCES.shirt.RandomMaterialPri(),
                        CHANCES.shirt.RandomMaterialSec(),
                        item::named_type::Not,
                        item::set_type::Not,
                        item::element_type::None,
                        CHARACTER_PTR->IsPixie());

                    itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
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
                            << item::armor::cover_type::ToString(COVER_PAIR.first)
                            << "\", but that item was not found in the "
                               "ARMOR_CHANCES.cover_map.");

                    item::ItemProfile profile;

                    profile.SetCover(
                        COVER_PAIR.first,
                        coverChances.RandomMaterialPri(),
                        coverChances.RandomMaterialSec(),
                        item::named_type::Not,
                        item::set_type::Not,
                        item::element_type::None,
                        CHARACTER_PTR->IsPixie());

                    itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
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
                            << item::armor::helm_type::ToString(HELM_PAIR.first)
                            << "\", but that item was not found in the "
                               "ARMOR_CHANCES.helm_map.");

                    item::ItemProfile profile;

                    profile.SetHelm(
                        HELM_PAIR.first,
                        helmChances.RandomMaterialPri(),
                        helmChances.RandomMaterialSec());

                    itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
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
                            << item::armor::shield_type::ToString(SHIELD_PAIR.first)
                            << "\", but that item was not found in the "
                               "ARMOR_CHANCES.shield_map.");

                    item::ItemProfile profile;

                    profile.SetShield(
                        SHIELD_PAIR.first,
                        shieldChances.RandomMaterialPri(),
                        shieldChances.RandomMaterialSec());

                    if (HAS_TWO_HANDED_WEAPON)
                    {
                        itemsPtrVecPair.second.emplace_back(itemFactory_.Make(profile));
                    }
                    else
                    {
                        itemsPtrVecPair.first.emplace_back(itemFactory_.Make(profile));
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
            item::ItemPVec_t bodyWeaponsSVec;

            if (CHANCES.has_bite)
            {
                bodyWeaponsSVec.emplace_back(
                    itemFactory_.Make(item::body_part::Bite, CHARACTER_PTR));
            }

            if (CHANCES.has_claws && (HAS_TWO_HANDED_WEAPON_EQUIPPED == false))
            {
                bodyWeaponsSVec.emplace_back(
                    itemFactory_.Make(item::body_part::Claws, CHARACTER_PTR));
            }

            if (CHANCES.has_fists && (HAS_TWO_HANDED_WEAPON_EQUIPPED == false)
                && (CHANCES.has_claws == false))
            {
                bodyWeaponsSVec.emplace_back(
                    itemFactory_.Make(item::body_part::Fists, CHARACTER_PTR));
            }

            if (CHANCES.has_tentacles && (HAS_TWO_HANDED_WEAPON_EQUIPPED == false))
            {
                bodyWeaponsSVec.emplace_back(
                    itemFactory_.Make(item::body_part::Tentacles, CHARACTER_PTR));
            }

            if (CHANCES.has_breath
                && ((CHARACTER_PTR->Role() == role::Sylavin)
                    || (CHARACTER_PTR->Role() == role::Firebrand)))
            {
                bodyWeaponsSVec.emplace_back(
                    itemFactory_.Make(item::body_part::Breath, CHARACTER_PTR));
            }

            return bodyWeaponsSVec;
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
            const item::armor_type::Enum ENUM, item::ItemPVec_t & vec) const
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
