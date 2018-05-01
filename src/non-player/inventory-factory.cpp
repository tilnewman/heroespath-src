// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// inventory-factory.cpp
//
#include "inventory-factory.hpp"

#include "creature/creature.hpp"
#include "item/item-factory.hpp"
#include "item/item.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/random.hpp"
#include "non-player/chance-factory.hpp"

#include <algorithm>
#include <exception>
#include <iterator>
#include <sstream>
#include <string>

namespace heroespath
{
namespace non_player
{
    namespace ownership
    {

        void InventoryFactory::SetupCreatureInventory(const creature::CreaturePtr_t CREATURE_PTR)
        {
            auto const INVENTORY_CHANCES{ ChanceFactory::Make(CREATURE_PTR) };
            CREATURE_PTR->CoinsAdj(Make_Coins(INVENTORY_CHANCES));

            //.first is for equipped items and .second is for unequipped items
            auto const ITEM_PVEC_PAIR{ MakeItemSet(INVENTORY_CHANCES, CREATURE_PTR) };
            for (auto const & NEXT_ITEM_PTR : ITEM_PVEC_PAIR.first)
            {
                auto const ITEM_ADD_RESULT(CREATURE_PTR->ItemAdd(NEXT_ITEM_PTR));
                if (ITEM_ADD_RESULT.empty() == false)
                {
                    M_HP_LOG_ERR(
                        "non-player::ownership::InventoryFactory::SetupCreatureInventory"
                        << "[to equip - add step](creature=\n{" << CREATURE_PTR->ToString()
                        << "}) unable to add the item=\n{" << NEXT_ITEM_PTR->ToString()
                        << "} with reported error=\n\"" << ITEM_ADD_RESULT << "\".  Proceeding...");

                    item::ItemWarehouse::Access().Free(NEXT_ITEM_PTR);
                }
                else
                {
                    if (NEXT_ITEM_PTR->Category() & item::category::Equippable)
                    {
                        auto const ITEM_EQUIP_RESULT(CREATURE_PTR->ItemEquip(NEXT_ITEM_PTR));
                        if (ITEM_EQUIP_RESULT.empty() == false)
                        {
                            M_HP_LOG_ERR(
                                "non-player::ownership::InventoryFactory::"
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

            for (auto const & NEXT_ITEM_PTR : ITEM_PVEC_PAIR.second)
            {
                const std::string ITEM_ADD_RESULT(CREATURE_PTR->ItemAdd(NEXT_ITEM_PTR));
                if (ITEM_ADD_RESULT.empty() == false)
                {
                    M_HP_LOG_ERR(
                        "non-player::ownership::InventoryFactory::"
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
            const chance::InventoryChances & CHANCES, const creature::CreaturePtr_t CHARACTER_PTR)
        {
            IItemPVecPair_t itemsPtrVecPair;

            if (item::ItemFactory::DoesCreatureRequireSkinArmor(CHARACTER_PTR))
            {
                itemsPtrVecPair.first.emplace_back(
                    item::ItemFactory::Make(item::body_part::Skin, CHARACTER_PTR));
            }

            {
                auto const CLOTHING_ITEMS_PVEC_PAIR{ MakeItemSet_Clothing(
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

            bool hasTwoHandedWeapons{ false };

            {
                auto const WEAPON_ITEMS_PVEC_PAIR{ MakeItemSet_Weapons(
                    CHANCES.weapon, CHARACTER_PTR) };

                std::copy(
                    WEAPON_ITEMS_PVEC_PAIR.first.begin(),
                    WEAPON_ITEMS_PVEC_PAIR.first.end(),
                    back_inserter(itemsPtrVecPair.first));

                std::copy(
                    WEAPON_ITEMS_PVEC_PAIR.second.begin(),
                    WEAPON_ITEMS_PVEC_PAIR.second.end(),
                    back_inserter(itemsPtrVecPair.second));

                hasTwoHandedWeapons = ContainsTwoHandedWeapon(WEAPON_ITEMS_PVEC_PAIR.first);
            }

            {
                auto const BODY_WEAPON_ITEMS_PVEC{ MakeItemSet_BodyWeapons(
                    CHANCES.weapon, CHARACTER_PTR, hasTwoHandedWeapons) };

                std::copy(
                    BODY_WEAPON_ITEMS_PVEC.begin(),
                    BODY_WEAPON_ITEMS_PVEC.end(),
                    back_inserter(itemsPtrVecPair.first));
            }

            auto armorItemsPVecPair{ MakeItemSet_Armor(
                CHANCES.armor, CHARACTER_PTR, hasTwoHandedWeapons) };

            // remove clothing items that might conflict with armor equipping
            for (auto const & NEXT_ITEM_PTR : armorItemsPVecPair.first)
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

            // no vests on beasts
            auto isVestOnBeast{ [CHARACTER_PTR](auto const ITEM_PTR) {
                return (
                    (ITEM_PTR->ArmorInfo().CoverType() == item::armor::cover_type::Vest)
                    && ((CHARACTER_PTR->Race() == creature::race::Dragon)
                        || (CHARACTER_PTR->Race() == creature::race::Wolfen)));
            } };

            RemoveItemsAndFree(armorItemsPVecPair.first, isVestOnBeast);

            // no gauntlets without fingers
            auto isGauntletWithoutFingers{ [CHARACTER_PTR](auto const ITEM_PTR) {
                return (
                    (ITEM_PTR->ArmorType() == item::armor_type::Gauntlets)
                    && (CHARACTER_PTR->Body().HasFingers() == false));
            } };

            RemoveItemsAndFree(armorItemsPVecPair.first, isGauntletWithoutFingers);

            // no shields without fingers
            auto isShieldWithoutFingers{ [CHARACTER_PTR](auto const ITEM_PTR) {
                return (
                    (ITEM_PTR->ArmorType() == item::armor_type::Shield)
                    && (CHARACTER_PTR->Body().HasFingers() == false));
            } };

            RemoveItemsAndFree(armorItemsPVecPair.first, isShieldWithoutFingers);

            // no helms with horns
            auto isHeldWithHorns{ [CHARACTER_PTR](auto const ITEM_PTR) {
                return (
                    (ITEM_PTR->ArmorType() == item::armor_type::Helm)
                    && CHARACTER_PTR->Body().HasHorns());
            } };

            RemoveItemsAndFree(armorItemsPVecPair.first, isHeldWithHorns);

            // no pants without legs
            auto isPantsWithoutLegs{ [CHARACTER_PTR](auto const ITEM_PTR) {
                return (
                    (ITEM_PTR->ArmorType() == item::armor_type::Pants)
                    && (CHARACTER_PTR->Body().HasLegs() == false));
            } };

            RemoveItemsAndFree(armorItemsPVecPair.first, isPantsWithoutLegs);

            // no boots without feet
            auto isBootsWithoutFeet{ [CHARACTER_PTR](auto const ITEM_PTR) {
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
            auto isArmorOnBeast{ [CHARACTER_PTR](auto const ITEM_PTR) {
                return (ITEM_PTR->IsArmor() && CHARACTER_PTR->IsBeast());
            } };

            RemoveItemsAndFree(itemsPtrVecPair.first, isArmorOnBeast);
            RemoveItemsAndFree(itemsPtrVecPair.second, isArmorOnBeast);

            // prevent adding an aventail if there is no helm
            auto const CONTAINS_HELM{ std::find_if(
                                          itemsPtrVecPair.first.begin(),
                                          itemsPtrVecPair.first.end(),
                                          [](const item::ItemPtr_t PTR) {
                                              return (PTR->ArmorType() == item::armor_type::Helm);
                                          })
                                      != itemsPtrVecPair.first.end() };

            if (CONTAINS_HELM == false)
            {
                auto const WILL_ADD_AVENTAIL{
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
                [](auto const & ITEM_A_PTR, auto const &) {
                    return ITEM_A_PTR->ArmorInfo().IsHelm();
                });

            return itemsPtrVecPair;
        } // namespace ownership

        const IItemPVecPair_t InventoryFactory::MakeItemSet_Clothing(
            const chance::ClothingChances & CHANCES, const bool IS_PIXIE)
        {
            using namespace item;
            // Assume there is only one of each article of clothing.

            // TODO enchantments

            IItemPVecPair_t itemsPtrVecPair;

            if (CHANCES.boots.IsOwned())
            {
                ItemProfile profile;

                profile.SetBoots(
                    armor::base_type::Plain,
                    CHANCES.boots.RandomMaterialPri(),
                    CHANCES.boots.RandomMaterialSec(),
                    named_type::NotNamed,
                    set_type::NotASet,
                    element_type::None,
                    IS_PIXIE);

                itemsPtrVecPair.first.emplace_back(ItemFactory::Make(profile));
            }

            if (CHANCES.gloves.IsOwned())
            {
                ItemProfile profile;

                profile.SetGauntlets(
                    armor::base_type::Plain,
                    CHANCES.gloves.RandomMaterialPri(),
                    CHANCES.gloves.RandomMaterialSec(),
                    named_type::NotNamed,
                    set_type::NotASet,
                    element_type::None,
                    IS_PIXIE);

                itemsPtrVecPair.first.emplace_back(ItemFactory::Make(profile));
            }

            if (CHANCES.pants.IsOwned())
            {
                ItemProfile profile;

                profile.SetPants(
                    armor::base_type::Plain,
                    CHANCES.gloves.RandomMaterialPri(),
                    material::Nothing,
                    named_type::NotNamed,
                    set_type::NotASet,
                    element_type::None,
                    IS_PIXIE);

                itemsPtrVecPair.first.emplace_back(ItemFactory::Make(profile));
            }

            if (CHANCES.shirt.IsOwned())
            {
                ItemProfile profile;

                profile.SetShirt(
                    armor::base_type::Plain,
                    CHANCES.gloves.RandomMaterialPri(),
                    material::Nothing,
                    named_type::NotNamed,
                    set_type::NotASet,
                    element_type::None,
                    IS_PIXIE);

                itemsPtrVecPair.first.emplace_back(ItemFactory::Make(profile));
            }

            if (CHANCES.vest.IsOwned())
            {
                ItemProfile profile;

                profile.SetCover(
                    armor::cover_type::Vest,
                    CHANCES.vest.RandomMaterialPri(),
                    CHANCES.vest.RandomMaterialSec(),
                    named_type::NotNamed,
                    set_type::NotASet,
                    element_type::None,
                    IS_PIXIE);

                itemsPtrVecPair.first.emplace_back(ItemFactory::Make(profile));
            }

            auto const COVER_TYPE{ CHANCES.RandomCoverType() };
            if (COVER_TYPE != item::armor::cover_type::Count)
            {
                non_player::ownership::chance::ItemChances itemChances;
                auto const WAS_FOUND{ CHANCES.cover_map.Find(COVER_TYPE, itemChances) };

                M_ASSERT_OR_LOGANDTHROW_SS(
                    WAS_FOUND,
                    "non_player::ownership::InventoryFactory::MakeItemSet_Clothing() failed to"
                        << " find \"" << COVER_TYPE << "\".");

                ItemProfile profile;

                profile.SetCover(
                    COVER_TYPE,
                    CHANCES.vest.RandomMaterialPri(),
                    CHANCES.vest.RandomMaterialSec(),
                    named_type::NotNamed,
                    set_type::NotASet,
                    element_type::None,
                    IS_PIXIE);

                itemsPtrVecPair.first.emplace_back(ItemFactory::Make(profile));
            }

            return itemsPtrVecPair;
        }

        const IItemPVecPair_t InventoryFactory::MakeItemSet_Weapons(
            const chance::WeaponChances & WEAPON_CHANCES,
            const creature::CreaturePtr_t CHARACTER_PTR)
        {
            using namespace item;

            IItemPVecPair_t itemsPtrVecPair;

            using KindChancePair_t = std::pair<int, float>;
            misc::VectorMap<weapon_type::Enum, KindChancePair_t> typeKindChanceMap;

            // TODO handle multiple weapons of the same type/kind

            // TODO handle intentionally unequipped weapons

            auto const RANDOM_SELECTED_AXE_PAIR(
                RandomSelectWeapon<weapon::axe_type::Enum>(WEAPON_CHANCES.axe_map));

            if ((RANDOM_SELECTED_AXE_PAIR.first != weapon::axe_type::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_AXE_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[weapon_type::Axe] = std::make_pair(
                    RANDOM_SELECTED_AXE_PAIR.first, RANDOM_SELECTED_AXE_PAIR.second);
            }

            auto const RANDOM_SELECTED_BLADEDSTAFF_PAIR(
                RandomSelectWeapon<weapon::bladedstaff_type::Enum>(WEAPON_CHANCES.bladedstaff_map));

            if ((RANDOM_SELECTED_BLADEDSTAFF_PAIR.first != weapon::bladedstaff_type::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_BLADEDSTAFF_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[weapon_type::BladedStaff] = std::make_pair(
                    RANDOM_SELECTED_BLADEDSTAFF_PAIR.first,
                    RANDOM_SELECTED_BLADEDSTAFF_PAIR.second);
            }

            auto const RANDOM_SELECTED_CLUB_PAIR(
                RandomSelectWeapon<weapon::club_type::Enum>(WEAPON_CHANCES.club_map));

            if ((RANDOM_SELECTED_CLUB_PAIR.first != weapon::club_type::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_CLUB_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[weapon_type::Club] = std::make_pair(
                    RANDOM_SELECTED_CLUB_PAIR.first, RANDOM_SELECTED_CLUB_PAIR.second);
            }

            auto const RANDOM_SELECTED_PROJECTILE_PAIR(
                RandomSelectWeapon<weapon::projectile_type::Enum>(WEAPON_CHANCES.projectile_map));

            if ((RANDOM_SELECTED_PROJECTILE_PAIR.first != weapon::projectile_type::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_PROJECTILE_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[weapon_type::Projectile] = std::make_pair(
                    RANDOM_SELECTED_PROJECTILE_PAIR.first, RANDOM_SELECTED_PROJECTILE_PAIR.second);
            }

            auto const RANDOM_SELECTED_SWORD_PAIR(
                RandomSelectWeapon<weapon::sword_type::Enum>(WEAPON_CHANCES.sword_map));

            if ((RANDOM_SELECTED_SWORD_PAIR.first != weapon::sword_type::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_SWORD_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[weapon_type::Sword] = std::make_pair(
                    RANDOM_SELECTED_SWORD_PAIR.first, RANDOM_SELECTED_SWORD_PAIR.second);
            }

            auto const RANDOM_SELECTED_WHIP_PAIR(
                RandomSelectWeapon<weapon::whip_type::Enum>(WEAPON_CHANCES.whip_map));

            if ((RANDOM_SELECTED_WHIP_PAIR.first != weapon::whip_type::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_WHIP_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[weapon_type::Whip] = std::make_pair(
                    RANDOM_SELECTED_WHIP_PAIR.first, RANDOM_SELECTED_WHIP_PAIR.second);
            }

            // knife/dagger
            {
                auto countChance{ 0.0f };
                if (WEAPON_CHANCES.knife.num_owned_map.Find(1, countChance))
                {
                    typeKindChanceMap[weapon_type::Knife] = std::make_pair(0, countChance);
                }
            }

            // staff/quarterstaff
            {
                auto countChance{ 0.0f };
                if (WEAPON_CHANCES.staff.num_owned_map.Find(1, countChance))
                {
                    typeKindChanceMap[weapon_type::Staff] = std::make_pair(0, countChance);
                }
            }

            // check for no-weapon case
            if (typeKindChanceMap.Empty())
            {
                return itemsPtrVecPair;
            }

            // random select weapon
            auto randomSelectedWeaponType{ typeKindChanceMap.begin()->first };
            auto chanceCumulative{ 0.0f };
            auto const RAND_WEAPON_TYPE_CHANCE{ misc::random::Float() };
            for (auto const & NEXT_TYPEKIND_PAIR : typeKindChanceMap)
            {
                chanceCumulative += NEXT_TYPEKIND_PAIR.second.second;
                if (RAND_WEAPON_TYPE_CHANCE < chanceCumulative)
                {
                    randomSelectedWeaponType = NEXT_TYPEKIND_PAIR.first;
                    break;
                }
            }

            // create weapon
            switch (randomSelectedWeaponType)
            {
                case weapon_type::Knife:
                {
                    // determine which size the knife/dagger will be
                    auto const RAND{ misc::random::Float() };
                    auto knifeSize{ sfml_util::Size::Large };

                    auto chanceOfSmall{ 0.0f };
                    auto const WAS_SMALL_FOUND{ WEAPON_CHANCES.knife.size_map.Find(
                        sfml_util::Size::Small, chanceOfSmall) };

                    if (WAS_SMALL_FOUND && (RAND < chanceOfSmall))
                    {
                        knifeSize = sfml_util::Size::Small;
                    }
                    else
                    {
                        auto chanceOfMedium{ 0.0f };
                        if (WEAPON_CHANCES.knife.size_map.Find(
                                sfml_util::Size::Medium, chanceOfMedium))
                        {
                            if (WAS_SMALL_FOUND)
                            {
                                chanceOfMedium += chanceOfSmall;
                            }

                            if (RAND < chanceOfMedium)
                            {
                                knifeSize = sfml_util::Size::Medium;
                            }
                        }
                    }

                    auto const IS_DAGGER{ misc::random::Float() < WEAPON_CHANCES.knife.is_dagger };

                    // prevent adding a dagger if invalid
                    if (WEAPON_CHANCES.knife.size_map.Empty()
                        || WEAPON_CHANCES.knife.mat_map_pri.Empty())
                    {
                        break;
                    }

                    ItemProfile profile;

                    if (IS_DAGGER)
                    {
                        profile.SetDagger(
                            knifeSize,
                            WEAPON_CHANCES.knife.RandomMaterialPri(),
                            WEAPON_CHANCES.knife.RandomMaterialSec(),
                            named_type::NotNamed,
                            set_type::NotASet,
                            element_type::None,
                            CHARACTER_PTR->IsPixie());
                    }
                    else
                    {
                        profile.SetKnife(
                            knifeSize,
                            WEAPON_CHANCES.knife.RandomMaterialPri(),
                            WEAPON_CHANCES.knife.RandomMaterialSec(),
                            named_type::NotNamed,
                            set_type::NotASet,
                            element_type::None,
                            CHARACTER_PTR->IsPixie());
                    }

                    itemsPtrVecPair.first.emplace_back(ItemFactory::Make(profile));
                    break;
                }
                case weapon_type::Staff:
                {
                    ItemProfile profile;

                    if (misc::random::Float() < WEAPON_CHANCES.staff.is_quarterstaff)
                    {
                        profile.SetQuarterStaff(
                            WEAPON_CHANCES.staff.RandomMaterialPri(),
                            WEAPON_CHANCES.staff.RandomMaterialSec(),
                            named_type::NotNamed,
                            set_type::NotASet,
                            element_type::None);
                    }
                    else
                    {
                        profile.SetStaff(
                            WEAPON_CHANCES.staff.RandomMaterialPri(),
                            WEAPON_CHANCES.staff.RandomMaterialSec(),
                            named_type::NotNamed,
                            set_type::NotASet,
                            element_type::None);
                    }

                    itemsPtrVecPair.first.emplace_back(item::ItemFactory::Make(profile));
                    break;
                }
                case weapon_type::Axe:
                {
                    auto const AXE_TYPE{ static_cast<weapon::axe_type::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    non_player::ownership::chance::ItemChances axeChances;
                    auto const WAS_AXE_FOUND{ WEAPON_CHANCES.axe_map.Find(AXE_TYPE, axeChances) };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        WAS_AXE_FOUND,
                        "non_player::ownership::InventoryFactory::MakeItemSet_Weapons"
                            << "(creature=\"" << CHARACTER_PTR->ToString()
                            << "\") randomly selected weapon type=\""
                            << weapon_type::ToString(randomSelectedWeaponType, false)
                            << "\" and kind=\"" << weapon::axe_type::ToString(AXE_TYPE)
                            << "\" -but that weapon was not found in the original "
                               "WEAPON_CHANCES "
                               "object.");

                    // if the primary material is wood, make sure there is a valid secondary
                    // material
                    auto const MATERIAL_PRI{ axeChances.RandomMaterialPri() };
                    auto materialSec{ axeChances.RandomMaterialSec() };
                    if ((MATERIAL_PRI == material::Wood) && (material::Nothing == materialSec))
                    {
                        materialSec = material::Steel;
                    }

                    ItemProfile profile;

                    profile.SetAxe(AXE_TYPE, MATERIAL_PRI, materialSec);

                    itemsPtrVecPair.first.emplace_back(item::ItemFactory::Make(profile));
                    break;
                }
                case weapon_type::BladedStaff:
                {
                    auto const BLADEDSTAFF_TYPE{ static_cast<weapon::bladedstaff_type::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    non_player::ownership::chance::ItemChances bstaffChances;

                    auto const WAS_BSTAFF_FOUND{ WEAPON_CHANCES.bladedstaff_map.Find(
                        BLADEDSTAFF_TYPE, bstaffChances) };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        WAS_BSTAFF_FOUND,
                        "non_player::ownership::InventoryFactory::MakeItemSet_Weapons"
                            << "(creature=\"" << CHARACTER_PTR->ToString()
                            << "\") randomly selected weapon type=\""
                            << weapon_type::ToString(randomSelectedWeaponType, false)
                            << "\" and kind=\""
                            << weapon::bladedstaff_type::ToString(BLADEDSTAFF_TYPE)
                            << "\" -but that weapon was not found in the original "
                               "WEAPON_CHANCES "
                               "object.");

                    // if the primary material is wood, make sure there is a valid secondary
                    // material
                    auto const MATERIAL_PRI{ bstaffChances.RandomMaterialPri() };
                    auto materialSec{ bstaffChances.RandomMaterialSec() };

                    if ((MATERIAL_PRI == material::Wood) && (material::Nothing == materialSec))
                    {
                        materialSec = material::Steel;
                    }

                    ItemProfile profile;
                    profile.SetBladedStaff(BLADEDSTAFF_TYPE, MATERIAL_PRI, materialSec);
                    itemsPtrVecPair.first.emplace_back(item::ItemFactory::Make(profile));
                    break;
                }
                case weapon_type::Club:
                {
                    auto const CLUB_TYPE{ static_cast<weapon::club_type::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    non_player::ownership::chance::ItemChances clubChances;

                    auto const WAS_CLUB_FOUND{ WEAPON_CHANCES.club_map.Find(
                        CLUB_TYPE, clubChances) };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        WAS_CLUB_FOUND,
                        "non_player::ownership::InventoryFactory::MakeItemSet_Weapons("
                            << "creature=\"" << CHARACTER_PTR->ToString()
                            << "\") randomly selected weapon type=\""
                            << weapon_type::ToString(randomSelectedWeaponType, false)
                            << "\" and kind=\"" << weapon::club_type::ToString(CLUB_TYPE)
                            << "\" -but that weapon was not found in the original "
                               "WEAPON_CHANCES "
                               "object.");

                    // if the primary material is wood, make sure there is a valid secondary
                    // material
                    auto const MATERIAL_PRI{ clubChances.RandomMaterialPri() };
                    auto materialSec{ clubChances.RandomMaterialSec() };

                    if ((MATERIAL_PRI == material::Wood) && (material::Nothing == materialSec))
                    {
                        materialSec = material::Steel;
                    }

                    ItemProfile profile;
                    profile.SetClub(CLUB_TYPE, MATERIAL_PRI, materialSec);
                    itemsPtrVecPair.first.emplace_back(item::ItemFactory::Make(profile));
                    break;
                }
                case weapon_type::Projectile:
                {
                    auto const PROJECTILE_TYPE{ static_cast<weapon::projectile_type::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    non_player::ownership::chance::ItemChances projChances;

                    auto const WAS_PROJ_FOUND{ WEAPON_CHANCES.projectile_map.Find(
                        PROJECTILE_TYPE, projChances) };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        WAS_PROJ_FOUND,
                        "non_player::ownership::InventoryFactory::MakeItemSet_Weapons("
                            << "creature=\"" << CHARACTER_PTR->ToString()
                            << "\") randomly selected weapon type=\""
                            << weapon_type::ToString(randomSelectedWeaponType, false)
                            << "\" and kind=\""
                            << weapon::projectile_type::ToString(PROJECTILE_TYPE)
                            << "\" -but that weapon was not found in the original "
                               "WEAPON_CHANCES "
                               "object.");

                    ItemProfile profile;

                    profile.SetProjectile(
                        PROJECTILE_TYPE,
                        projChances.RandomMaterialPri(),
                        projChances.RandomMaterialSec());

                    itemsPtrVecPair.first.emplace_back(item::ItemFactory::Make(profile));
                    break;
                }
                case weapon_type::Sword:
                {
                    auto const SWORD_TYPE{ static_cast<weapon::sword_type::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    non_player::ownership::chance::ItemChances swordChances;

                    auto const WAS_SWORD_FOUND{ WEAPON_CHANCES.sword_map.Find(
                        SWORD_TYPE, swordChances) };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        WAS_SWORD_FOUND,
                        "non_player::ownership::InventoryFactory::MakeItemSet_Weapons("
                            << "creature=\"" << CHARACTER_PTR->ToString()
                            << "\") randomly selected weapon type=\""
                            << weapon_type::ToString(randomSelectedWeaponType, false)
                            << "\" and kind=\"" << weapon::sword_type::ToString(SWORD_TYPE)
                            << "\" -but that weapon was not found in the original "
                               "WEAPON_CHANCES "
                               "object.");

                    ItemProfile profile;

                    profile.SetSword(
                        SWORD_TYPE,
                        swordChances.RandomMaterialPri(),
                        swordChances.RandomMaterialSec());

                    itemsPtrVecPair.first.emplace_back(item::ItemFactory::Make(profile));
                    break;
                }
                case weapon_type::Whip:
                {
                    auto const WHIP_TYPE{ static_cast<weapon::whip_type::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    non_player::ownership::chance::ItemChances whipChances;

                    auto const WAS_WHIP_FOUND{ WEAPON_CHANCES.whip_map.Find(
                        WHIP_TYPE, whipChances) };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        WAS_WHIP_FOUND,
                        "non_player::ownership::InventoryFactory::MakeItemSet_Weapons("
                            << "creature=\"" << CHARACTER_PTR->ToString()
                            << "\") randomly selected weapon type=\""
                            << weapon_type::ToString(randomSelectedWeaponType, false)
                            << "\" and kind=\"" << weapon::whip_type::ToString(WHIP_TYPE)
                            << "\" -but that weapon was not found in the original "
                               "WEAPON_CHANCES "
                               "object.");

                    ItemProfile profile;

                    profile.SetWhip(
                        WHIP_TYPE,
                        whipChances.RandomMaterialPri(),
                        whipChances.RandomMaterialSec());

                    itemsPtrVecPair.first.emplace_back(item::ItemFactory::Make(profile));
                    break;
                }
                case weapon_type::Bite:
                case weapon_type::Breath:
                case weapon_type::Bladed:
                case weapon_type::Claws:
                case weapon_type::Fists:
                case weapon_type::Melee:
                case weapon_type::NotAWeapon:
                case weapon_type::Pointed:
                case weapon_type::Tendrils:
                case weapon_type::Crossbow:
                case weapon_type::Blowpipe:
                case weapon_type::Bow:
                case weapon_type::Spear:
                case weapon_type::Sling:
                default:
                {
                    std::ostringstream ss;
                    ss << "non_player::ownership::InventoryFactory::MakeItemSet_Weapons("
                       << "creature=\"" << CHARACTER_PTR->ToString()
                       << "\") failed to find a valid random selected weapon.  (weapon_type="
                       << weapon_type::ToString(randomSelectedWeaponType, false) << "\")";

                    throw std::runtime_error(ss.str());
                }
            }

            return itemsPtrVecPair;
        }

        const IItemPVecPair_t InventoryFactory::MakeItemSet_Armor(
            const chance::ArmorChances & CHANCES,
            const creature::CreaturePtr_t CHARACTER_PTR,
            const bool HAS_TWO_HANDED_WEAPON)
        {
            using namespace item;
            using namespace item::armor;

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
                    ItemProfile profile;

                    profile.SetAventail(
                        CHANCES.aventail.RandomArmorBaseType(),
                        CHANCES.aventail.RandomMaterialPri(),
                        CHANCES.aventail.RandomMaterialSec());

                    itemsPtrVecPair.first.emplace_back(item::ItemFactory::Make(profile));
                }
            }
            catch (...)
            {
                M_HP_LOG_ERR(
                    "non_player::ownership::MakeItemSet_Armor(creature={"
                    << CHARACTER_PTR->ToString() << "}, has_two_handed_weapon=" << std::boolalpha
                    << HAS_TWO_HANDED_WEAPON << ")  exception thrown while checking aventail.");

                throw;
            }

            try
            {
                if (CHANCES.boots.IsOwned())
                {
                    ItemProfile profile;

                    profile.SetBoots(
                        CHANCES.boots.RandomArmorBaseType(),
                        CHANCES.boots.RandomMaterialPri(),
                        CHANCES.boots.RandomMaterialSec(),
                        named_type::NotNamed,
                        set_type::NotASet,
                        element_type::None,
                        CHARACTER_PTR->IsPixie());

                    itemsPtrVecPair.first.emplace_back(item::ItemFactory::Make(profile));
                }
            }
            catch (...)
            {
                M_HP_LOG_ERR(
                    "non_player::ownership::MakeItemSet_Armor(creature={"
                    << CHARACTER_PTR->ToString() << "}, has_two_handed_weapon=" << std::boolalpha
                    << HAS_TWO_HANDED_WEAPON << ")  exception thrown while checking boots.");

                throw;
            }

            try
            {
                if (CHANCES.bracers.IsOwned())
                {
                    ItemProfile profile;

                    profile.SetBracer(
                        CHANCES.bracers.RandomArmorBaseType(),
                        CHANCES.bracers.RandomMaterialPri(),
                        CHANCES.bracers.RandomMaterialSec(),
                        named_type::NotNamed,
                        set_type::NotASet,
                        element_type::None,
                        CHARACTER_PTR->IsPixie());

                    itemsPtrVecPair.first.emplace_back(item::ItemFactory::Make(profile));
                }
            }
            catch (...)
            {
                M_HP_LOG_ERR(
                    "non_player::ownership::MakeItemSet_Armor(creature={"
                    << CHARACTER_PTR->ToString() << "}, has_two_handed_weapon=" << std::boolalpha
                    << HAS_TWO_HANDED_WEAPON << ")  exception thrown while checking bracers.");

                throw;
            }

            try
            {
                if (CHANCES.gauntlets.IsOwned())
                {
                    ItemProfile profile;

                    profile.SetGauntlets(
                        CHANCES.gauntlets.RandomArmorBaseType(),
                        CHANCES.gauntlets.RandomMaterialPri(),
                        CHANCES.gauntlets.RandomMaterialSec(),
                        named_type::NotNamed,
                        set_type::NotASet,
                        element_type::None,
                        CHARACTER_PTR->IsPixie());

                    itemsPtrVecPair.first.emplace_back(item::ItemFactory::Make(profile));
                }
            }
            catch (...)
            {
                M_HP_LOG_ERR(
                    "non_player::ownership::MakeItemSet_Armor(creature={"
                    << CHARACTER_PTR->ToString() << "}, has_two_handed_weapon=" << std::boolalpha
                    << HAS_TWO_HANDED_WEAPON << ")  exception thrown while checking gauntlets.");

                throw;
            }

            try
            {
                if (CHANCES.pants.IsOwned())
                {
                    ItemProfile profile;

                    profile.SetPants(
                        CHANCES.pants.RandomArmorBaseType(),
                        CHANCES.pants.RandomMaterialPri(),
                        CHANCES.pants.RandomMaterialSec(),
                        named_type::NotNamed,
                        set_type::NotASet,
                        element_type::None,
                        CHARACTER_PTR->IsPixie());

                    itemsPtrVecPair.first.emplace_back(item::ItemFactory::Make(profile));
                }
            }
            catch (...)
            {
                M_HP_LOG_ERR(
                    "non_player::ownership::MakeItemSet_Armor(creature={"
                    << CHARACTER_PTR->ToString() << "}, has_two_handed_weapon=" << std::boolalpha
                    << HAS_TWO_HANDED_WEAPON << ")  exception thrown while checking pants.");

                throw;
            }

            try
            {
                if (CHANCES.shirt.IsOwned())
                {
                    ItemProfile profile;

                    profile.SetShirt(
                        CHANCES.shirt.RandomArmorBaseType(),
                        CHANCES.shirt.RandomMaterialPri(),
                        CHANCES.shirt.RandomMaterialSec(),
                        named_type::NotNamed,
                        set_type::NotASet,
                        element_type::None,
                        CHARACTER_PTR->IsPixie());

                    itemsPtrVecPair.first.emplace_back(item::ItemFactory::Make(profile));
                }
            }
            catch (...)
            {
                M_HP_LOG_ERR(
                    "non_player::ownership::MakeItemSet_Armor(creature={"
                    << CHARACTER_PTR->ToString() << "}, has_two_handed_weapon=" << std::boolalpha
                    << HAS_TWO_HANDED_WEAPON << ")  exception thrown while checking shirt.");

                throw;
            }

            // covers (vest/robe/cloak/cape)
            try
            {
                auto const COVER_PAIR{ CHANCES.RandomCover() };
                if (COVER_PAIR.second > 0)
                {
                    non_player::ownership::chance::ItemChances coverChances;

                    auto const WAS_COVER_FOUND{ CHANCES.cover_map.Find(
                        COVER_PAIR.first, coverChances) };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        WAS_COVER_FOUND,
                        "non_player::ownership::InventoryFactory::MakeItemSet_Armor(creature=\""
                            << CHARACTER_PTR->ToString()
                            << "\") ARMOR_CHANCES.RandomCover() returned \""
                            << cover_type::ToString(COVER_PAIR.first)
                            << "\", but that item was not found in the "
                               "ARMOR_CHANCES.cover_map.");

                    ItemProfile profile;

                    profile.SetCover(
                        COVER_PAIR.first,
                        coverChances.RandomMaterialPri(),
                        coverChances.RandomMaterialSec(),
                        named_type::NotNamed,
                        set_type::NotASet,
                        element_type::None,
                        CHARACTER_PTR->IsPixie());

                    itemsPtrVecPair.first.emplace_back(item::ItemFactory::Make(profile));
                }
            }
            catch (...)
            {
                M_HP_LOG_ERR(
                    "non_player::ownership::MakeItemSet_Armor(creature={"
                    << CHARACTER_PTR->ToString() << "}, has_two_handed_weapon=" << std::boolalpha
                    << HAS_TWO_HANDED_WEAPON << ")  exception thrown while checking covers.");

                throw;
            }

            // helms
            try
            {
                auto const HELM_PAIR{ CHANCES.RandomHelm() };
                if (HELM_PAIR.second > 0)
                {
                    non_player::ownership::chance::ItemChances helmChances;

                    auto const WAS_HELM_FOUND{ CHANCES.helm_map.Find(
                        HELM_PAIR.first, helmChances) };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        WAS_HELM_FOUND,
                        "non_player::ownership::InventoryFactory::MakeItemSet_Armor(creature=\""
                            << CHARACTER_PTR->ToString()
                            << "\") ARMOR_CHANCES.RandomHelm() returned \""
                            << helm_type::ToString(HELM_PAIR.first)
                            << "\", but that item was not found in the "
                               "ARMOR_CHANCES.helm_map.");

                    ItemProfile profile;

                    profile.SetHelm(
                        HELM_PAIR.first,
                        helmChances.RandomMaterialPri(),
                        helmChances.RandomMaterialSec());

                    itemsPtrVecPair.first.emplace_back(item::ItemFactory::Make(profile));
                }
            }
            catch (...)
            {
                M_HP_LOG_ERR(
                    "non_player::ownership::MakeItemSet_Armor(creature={"
                    << CHARACTER_PTR->ToString() << "}, has_two_handed_weapon=" << std::boolalpha
                    << HAS_TWO_HANDED_WEAPON << ")  exception thrown while checking helms.");

                throw;
            }

            // shields
            try
            {
                auto const SHIELD_PAIR{ CHANCES.RandomShield() };
                if (SHIELD_PAIR.second > 0)
                {
                    ownership::chance::ItemChances shieldChances;

                    auto const WAS_SHIELD_FOUND{ CHANCES.shield_map.Find(
                        SHIELD_PAIR.first, shieldChances) };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        WAS_SHIELD_FOUND,
                        "non_player::ownership::InventoryFactory::MakeItemSet_Armor(creature=\""
                            << CHARACTER_PTR->ToString()
                            << "\") ARMOR_CHANCES.RandomShield() returned \""
                            << shield_type::ToString(SHIELD_PAIR.first)
                            << "\", but that item was not found in the "
                               "ARMOR_CHANCES.shield_map.");

                    ItemProfile profile;

                    profile.SetShield(
                        SHIELD_PAIR.first,
                        shieldChances.RandomMaterialPri(),
                        shieldChances.RandomMaterialSec());

                    if (HAS_TWO_HANDED_WEAPON)
                    {
                        itemsPtrVecPair.second.emplace_back(item::ItemFactory::Make(profile));
                    }
                    else
                    {
                        itemsPtrVecPair.first.emplace_back(item::ItemFactory::Make(profile));
                    }
                }
            }
            catch (...)
            {
                M_HP_LOG_ERR(
                    "non_player::ownership::MakeItemSet_Armor(creature={"
                    << CHARACTER_PTR->ToString() << "}, has_two_handed_weapon=" << std::boolalpha
                    << HAS_TWO_HANDED_WEAPON << ")  exception thrown while checking shields.");

                throw;
            }

            return itemsPtrVecPair;
        }

        const item::ItemPVec_t InventoryFactory::MakeItemSet_BodyWeapons(
            const chance::WeaponChances & CHANCES,
            const creature::CreaturePtr_t CHARACTER_PTR,
            const bool HAS_TWO_HANDED_WEAPON_EQUIPPED)
        {
            item::ItemPVec_t bodyWeaponsSVec;

            if (CHANCES.has_bite)
            {
                bodyWeaponsSVec.emplace_back(
                    item::ItemFactory::Make(item::body_part::Bite, CHARACTER_PTR));
            }

            if (CHANCES.has_claws && (HAS_TWO_HANDED_WEAPON_EQUIPPED == false))
            {
                bodyWeaponsSVec.emplace_back(
                    item::ItemFactory::Make(item::body_part::Claws, CHARACTER_PTR));
            }

            if (CHANCES.has_fists && (HAS_TWO_HANDED_WEAPON_EQUIPPED == false)
                && (CHANCES.has_claws == false))
            {
                bodyWeaponsSVec.emplace_back(
                    item::ItemFactory::Make(item::body_part::Fists, CHARACTER_PTR));
            }

            if (CHANCES.has_tendrils && (HAS_TWO_HANDED_WEAPON_EQUIPPED == false))
            {
                bodyWeaponsSVec.emplace_back(
                    item::ItemFactory::Make(item::body_part::Tendrils, CHARACTER_PTR));
            }

            if (CHANCES.has_breath
                && ((CHARACTER_PTR->Role() == creature::role::Sylavin)
                    || (CHARACTER_PTR->Role() == creature::role::Firebrand)))
            {
                bodyWeaponsSVec.emplace_back(
                    item::ItemFactory::Make(item::body_part::Breath, CHARACTER_PTR));
            }

            return bodyWeaponsSVec;
        }

        Coin_t InventoryFactory::Make_Coins(const chance::InventoryChances & CHANCES)
        {
            return CHANCES.RandomCoins();
        }

        bool InventoryFactory::ContainsTwoHandedWeapon(const item::ItemPVec_t & WEAPON_PVEC)
        {
            for (auto const & NEXT_WEAPON_PTR : WEAPON_PVEC)
            {
                if (NEXT_WEAPON_PTR->IsTwoHanded())
                {
                    return true;
                }
            }

            return false;
        }

        void InventoryFactory::RemoveArmorTypeFromVecAndFree(
            const item::armor_type::Enum ENUM, item::ItemPVec_t & vec)
        {
            item::ItemPVec_t itemsToRemovePVec;

            auto ArmorTypeMatchLambda{ [ENUM](auto const ITEM_PTR) {
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

    } // namespace ownership
} // namespace non_player
} // namespace heroespath
