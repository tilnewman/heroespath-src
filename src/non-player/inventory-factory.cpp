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
#include "item/item.hpp"
#include "log/log-macros.hpp"
#include "non-player/chance-factory.hpp"
#include "non-player/character.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/random.hpp"

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

        std::unique_ptr<InventoryFactory> InventoryFactory::instanceUPtr_{ nullptr };

        InventoryFactory::InventoryFactory()
        {
            M_HP_LOG_DBG("Singleton Construction: InventoryFactory");
        }

        InventoryFactory::~InventoryFactory()
        {
            M_HP_LOG_DBG("Singleton Destruction: InventoryFactory");
        }

        InventoryFactory * InventoryFactory::Instance()
        {
            if (instanceUPtr_.get() == nullptr)
            {
                M_HP_LOG_WRN("Singleton Instance() before Acquire(): InventoryFactory");
                Acquire();
            }

            return instanceUPtr_.get();
        }

        void InventoryFactory::Acquire()
        {
            if (instanceUPtr_.get() == nullptr)
            {
                instanceUPtr_ = std::make_unique<InventoryFactory>();
            }
            else
            {
                M_HP_LOG_WRN("Singleton Acquire() after Construction: InventoryFactory");
            }
        }

        void InventoryFactory::Release()
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (instanceUPtr_.get() != nullptr),
                "non_player::ownership::InventoryFactory::Release() found "
                    << "instanceUPtr that was null.");

            instanceUPtr_.reset();
        }

        void InventoryFactory::SetupCreatureInventory(non_player::CharacterPtr_t creaturePtr)
        {
            auto const INVENTORY_CHANCES{ ChanceFactory::Make(creaturePtr) };
            creaturePtr->CoinsAdj(Make_Coins(INVENTORY_CHANCES));

            //.first is for equipped items and .second is for unequipped items
            auto const ITEM_PVEC_PAIR{ MakeItemSet(INVENTORY_CHANCES, creaturePtr) };
            for (auto const NEXT_ITEM_PTR : ITEM_PVEC_PAIR.first)
            {
                auto const ITEM_ADD_RESULT(creaturePtr->ItemAdd(NEXT_ITEM_PTR));
                if (ITEM_ADD_RESULT.empty() == false)
                {
                    M_HP_LOG_ERR(
                        "non-player::ownership::InventoryFactory::PopulateCreatureInventory"
                        << "[to equip - add step](creature=\"" << creaturePtr->ToString()
                        << "\") unable to add the item \"" << NEXT_ITEM_PTR->Name() << "\" \""
                        << NEXT_ITEM_PTR->Desc() << "\" with reported error \"" << ITEM_ADD_RESULT
                        << "\".  Proceeding...");
                }
                else
                {
                    if (NEXT_ITEM_PTR->Category() & item::category::Equippable)
                    {
                        auto const ITEM_EQUIP_RESULT(creaturePtr->ItemEquip(NEXT_ITEM_PTR));
                        if (ITEM_EQUIP_RESULT.empty() == false)
                        {
                            M_HP_LOG_ERR(
                                "non-player::ownership::InventoryFactory::"
                                << "PopulateCreatureInventory[to equip - equip step](creature=\""
                                << creaturePtr->ToString() << "\") unable to add the item \""
                                << NEXT_ITEM_PTR->Name() << "\" \"" << NEXT_ITEM_PTR->Desc()
                                << "\"with reported error \"" << ITEM_EQUIP_RESULT
                                << "\".  Proceeding...");
                        }
                    }
                }
            }

            for (auto const NEXT_ITEM_PTR : ITEM_PVEC_PAIR.second)
            {
                const std::string ITEM_ADD_RESULT(creaturePtr->ItemAdd(NEXT_ITEM_PTR));
                if (ITEM_ADD_RESULT.empty() == false)
                {
                    M_HP_LOG_ERR(
                        "non-player::ownership::InventoryFactory::"
                        << "PopulateCreatureInventory[not to equip](creature=\""
                        << creaturePtr->ToString() << "\") unable to add the item \""
                        << NEXT_ITEM_PTR->Name() << "\" \"" << NEXT_ITEM_PTR->Desc()
                        << "\" with reported error \"" << ITEM_ADD_RESULT << "\".  Proceeding...");
                }
            }

            creaturePtr->SetCurrentWeaponsToBest();
        }

        const IItemPVecPair_t InventoryFactory::MakeItemSet(
            const chance::InventoryChances & CHANCES,
            const non_player::CharacterPtr_t CHARACTER_PTR)
        {
            IItemPVecPair_t itemsPtrVecPair;

            auto const CLOTHING_ITEMS_PVEC_PAIR(MakeItemSet_Clothing(CHANCES.clothes));

            std::copy(
                CLOTHING_ITEMS_PVEC_PAIR.first.begin(),
                CLOTHING_ITEMS_PVEC_PAIR.first.end(),
                back_inserter(itemsPtrVecPair.first));

            std::copy(
                CLOTHING_ITEMS_PVEC_PAIR.second.begin(),
                CLOTHING_ITEMS_PVEC_PAIR.second.end(),
                back_inserter(itemsPtrVecPair.second));

            auto const WEAPON_ITEMS_PVEC_PAIR(MakeItemSet_Weapons(CHANCES.weapon, CHARACTER_PTR));

            std::copy(
                WEAPON_ITEMS_PVEC_PAIR.first.begin(),
                WEAPON_ITEMS_PVEC_PAIR.first.end(),
                back_inserter(itemsPtrVecPair.first));

            std::copy(
                WEAPON_ITEMS_PVEC_PAIR.second.begin(),
                WEAPON_ITEMS_PVEC_PAIR.second.end(),
                back_inserter(itemsPtrVecPair.second));

            auto const HAS_TWOHANDED_WEAPON_EQUIPPED(
                ContainsTwoHandedWeapon(WEAPON_ITEMS_PVEC_PAIR.first));

            auto const BODY_WEAPON_ITEMS_PVEC(MakeItemSet_BodyWeapons(
                CHANCES.weapon, CHARACTER_PTR, HAS_TWOHANDED_WEAPON_EQUIPPED));

            std::copy(
                BODY_WEAPON_ITEMS_PVEC.begin(),
                BODY_WEAPON_ITEMS_PVEC.end(),
                back_inserter(itemsPtrVecPair.first));

            auto armorItemsPVecPair(
                MakeItemSet_Armor(CHANCES.armor, CHARACTER_PTR, HAS_TWOHANDED_WEAPON_EQUIPPED));

            // remove clothing items that might conflict with armor equipping
            for (auto const NEXT_ITEM_PTR : armorItemsPVecPair.first)
            {
                if (NEXT_ITEM_PTR->ArmorType() == item::armor_type::Boots)
                {
                    RemoveArmorTypeFromVec(item::armor_type::Boots, itemsPtrVecPair.first);
                }
                else if (NEXT_ITEM_PTR->ArmorType() == item::armor_type::Pants)
                {
                    RemoveArmorTypeFromVec(item::armor_type::Pants, itemsPtrVecPair.first);
                }
                else if (NEXT_ITEM_PTR->ArmorType() == item::armor_type::Shirt)
                {
                    RemoveArmorTypeFromVec(item::armor_type::Shirt, itemsPtrVecPair.first);
                }
                else if (NEXT_ITEM_PTR->ArmorType() == item::armor_type::Gauntlets)
                {
                    RemoveArmorTypeFromVec(item::armor_type::Gauntlets, itemsPtrVecPair.first);
                }
            }

            // prevent adding an aventail if there is no helm
            auto const WILL_ADD_AVENTAIL{
                std::find_if(
                    armorItemsPVecPair.first.begin(),
                    armorItemsPVecPair.first.end(),
                    [](const item::ItemPtr_t PTR) {
                        return (PTR->ArmorType() == item::armor_type::Aventail);
                    })
                != armorItemsPVecPair.first.end()
            };

            auto const CONTAINS_HELM{ std::find_if(
                                          itemsPtrVecPair.first.begin(),
                                          itemsPtrVecPair.first.end(),
                                          [](const item::ItemPtr_t PTR) {
                                              return (PTR->ArmorType() == item::armor_type::Helm);
                                          })
                                      != itemsPtrVecPair.first.end() };

            if (WILL_ADD_AVENTAIL && (CONTAINS_HELM == false))
            {
                RemoveArmorTypeFromVec(item::armor_type::Aventail, armorItemsPVecPair.first);
            }

            // vests on beasts
            itemsPtrVecPair.first.erase(
                std::remove_if(
                    itemsPtrVecPair.first.begin(),
                    itemsPtrVecPair.first.end(),
                    [&](const item::ItemPtr_t PTR) {
                        return (
                            (PTR->Armor_Info().cover == item::armor::cover_type::Vest)
                            && ((CHARACTER_PTR->Race() == creature::race::Dragon)
                                || (CHARACTER_PTR->Race() == creature::race::Wolfen)));
                    }),
                itemsPtrVecPair.first.end());

            // gauntlets without fingers
            armorItemsPVecPair.first.erase(
                std::remove_if(
                    armorItemsPVecPair.first.begin(),
                    armorItemsPVecPair.first.end(),
                    [&](const item::ItemPtr_t PTR) {
                        return (
                            (PTR->ArmorType() == item::armor_type::Gauntlets)
                            && (CHARACTER_PTR->Body().HasFingers() == false));
                    }),
                armorItemsPVecPair.first.end());

            // shields without fingers
            armorItemsPVecPair.first.erase(
                std::remove_if(
                    armorItemsPVecPair.first.begin(),
                    armorItemsPVecPair.first.end(),
                    [&](const item::ItemPtr_t PTR) {
                        return (
                            (PTR->ArmorType() == item::armor_type::Sheild)
                            && (CHARACTER_PTR->Body().HasFingers() == false));
                    }),
                armorItemsPVecPair.first.end());

            // helms with horns
            armorItemsPVecPair.first.erase(
                std::remove_if(
                    armorItemsPVecPair.first.begin(),
                    armorItemsPVecPair.first.end(),
                    [&](const item::ItemPtr_t PTR) {
                        return (
                            (PTR->ArmorType() == item::armor_type::Helm)
                            && CHARACTER_PTR->Body().HasHorns());
                    }),
                armorItemsPVecPair.first.end());

            // pants without legs
            armorItemsPVecPair.first.erase(
                std::remove_if(
                    armorItemsPVecPair.first.begin(),
                    armorItemsPVecPair.first.end(),
                    [&](const item::ItemPtr_t PTR) {
                        return (
                            (PTR->ArmorType() == item::armor_type::Pants)
                            && (CHARACTER_PTR->Body().HasLegs() == false));
                    }),
                armorItemsPVecPair.first.end());

            // boots without feet
            armorItemsPVecPair.first.erase(
                std::remove_if(
                    armorItemsPVecPair.first.begin(),
                    armorItemsPVecPair.first.end(),
                    [&](const item::ItemPtr_t PTR) {
                        return (
                            (PTR->ArmorType() == item::armor_type::Boots)
                            && (CHARACTER_PTR->Body().HasLegs() == false));
                    }),
                armorItemsPVecPair.first.end());

            // finally, add the armor items
            std::copy(
                armorItemsPVecPair.first.begin(),
                armorItemsPVecPair.first.end(),
                back_inserter(itemsPtrVecPair.first));

            std::copy(
                armorItemsPVecPair.second.begin(),
                armorItemsPVecPair.second.end(),
                back_inserter(itemsPtrVecPair.second));

            return itemsPtrVecPair;
        }

        const IItemPVecPair_t
            InventoryFactory::MakeItemSet_Clothing(const chance::ClothingChances & CHANCES)
        {
            // Assume there is only one of each article of clothing.

            // TODO enchantments

            IItemPVecPair_t itemsPtrVecPair;

            if (CHANCES.boots.IsOwned())
            {
                itemsPtrVecPair.first.emplace_back(item::armor::ArmorFactory::Make_Boots(
                    item::armor::base_type::Plain,
                    CHANCES.boots.RandomMaterialPri(),
                    CHANCES.boots.RandomMaterialSec()));
            }

            if (CHANCES.gloves.IsOwned())
            {
                itemsPtrVecPair.first.emplace_back(item::armor::ArmorFactory::Make_Gauntlets(
                    item::armor::base_type::Plain,
                    CHANCES.gloves.RandomMaterialPri(),
                    CHANCES.gloves.RandomMaterialSec()));
            }

            if (CHANCES.pants.IsOwned())
            {
                itemsPtrVecPair.first.emplace_back(item::armor::ArmorFactory::Make_Pants(
                    item::armor::base_type::Plain, CHANCES.pants.RandomMaterialPri()));
            }

            if (CHANCES.shirt.IsOwned())
            {
                itemsPtrVecPair.first.emplace_back(item::armor::ArmorFactory::Make_Shirt(
                    item::armor::base_type::Plain, CHANCES.shirt.RandomMaterialPri()));
            }

            if (CHANCES.vest.IsOwned())
            {
                itemsPtrVecPair.first.emplace_back(item::armor::ArmorFactory::Make_Cover(
                    item::armor::cover_type::Vest,
                    CHANCES.vest.RandomMaterialPri(),
                    CHANCES.vest.RandomMaterialSec()));
            }

            const item::armor::cover_type::Enum COVER_TYPE(CHANCES.RandomCoverType());
            if (COVER_TYPE != item::armor::cover_type::Count)
            {
                non_player::ownership::chance::ItemChances itemChances;
                auto WAS_FOUND{ CHANCES.cover_map.Find(COVER_TYPE, itemChances) };

                M_ASSERT_OR_LOGANDTHROW_SS(
                    WAS_FOUND,
                    "non_player::ownership::InventoryFactory::MakeItemSet_Clothing() failed to"
                        << " find \"" << COVER_TYPE << "\".");

                itemsPtrVecPair.first.emplace_back(item::armor::ArmorFactory::Make_Cover(
                    COVER_TYPE, itemChances.RandomMaterialPri(), itemChances.RandomMaterialSec()));
            }

            return itemsPtrVecPair;
        }

        const IItemPVecPair_t InventoryFactory::MakeItemSet_Weapons(
            const chance::WeaponChances & WEAPON_CHANCES,
            const non_player::CharacterPtr_t CHARACTER_PTR)
        {
            IItemPVecPair_t itemsPtrVecPair;

            using KindChancePair_t = std::pair<int, float>;
            using TypeKindChanceMap_t = std::map<item::weapon_type::Enum, KindChancePair_t>;
            TypeKindChanceMap_t typeKindChanceMap;

            // TODO handle multiple weapons of the same type/kind

            // TODO handle intentionally unequipped weapons

            auto const RANDOM_SELECTED_AXE_PAIR(
                RandomSelectWeapon<item::weapon::axe_type::Enum>(WEAPON_CHANCES.axe_map));

            if ((RANDOM_SELECTED_AXE_PAIR.first != item::weapon::axe_type::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_AXE_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[item::weapon_type::Axe] = std::make_pair(
                    RANDOM_SELECTED_AXE_PAIR.first, RANDOM_SELECTED_AXE_PAIR.second);
            }

            auto const RANDOM_SELECTED_BLADEDSTAFF_PAIR(
                RandomSelectWeapon<item::weapon::bladedstaff_type::Enum>(
                    WEAPON_CHANCES.bladedstaff_map));

            if ((RANDOM_SELECTED_BLADEDSTAFF_PAIR.first != item::weapon::bladedstaff_type::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_BLADEDSTAFF_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[item::weapon_type::BladedStaff] = std::make_pair(
                    RANDOM_SELECTED_BLADEDSTAFF_PAIR.first,
                    RANDOM_SELECTED_BLADEDSTAFF_PAIR.second);
            }

            auto const RANDOM_SELECTED_CLUB_PAIR(
                RandomSelectWeapon<item::weapon::club_type::Enum>(WEAPON_CHANCES.club_map));

            if ((RANDOM_SELECTED_CLUB_PAIR.first != item::weapon::club_type::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_CLUB_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[item::weapon_type::Club] = std::make_pair(
                    RANDOM_SELECTED_CLUB_PAIR.first, RANDOM_SELECTED_CLUB_PAIR.second);
            }

            auto const RANDOM_SELECTED_PROJECTILE_PAIR(
                RandomSelectWeapon<item::weapon::projectile_type::Enum>(
                    WEAPON_CHANCES.projectile_map));

            if ((RANDOM_SELECTED_PROJECTILE_PAIR.first != item::weapon::projectile_type::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_PROJECTILE_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[item::weapon_type::Projectile] = std::make_pair(
                    RANDOM_SELECTED_PROJECTILE_PAIR.first, RANDOM_SELECTED_PROJECTILE_PAIR.second);
            }

            auto const RANDOM_SELECTED_SWORD_PAIR(
                RandomSelectWeapon<item::weapon::sword_type::Enum>(WEAPON_CHANCES.sword_map));

            if ((RANDOM_SELECTED_SWORD_PAIR.first != item::weapon::sword_type::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_SWORD_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[item::weapon_type::Sword] = std::make_pair(
                    RANDOM_SELECTED_SWORD_PAIR.first, RANDOM_SELECTED_SWORD_PAIR.second);
            }

            auto const RANDOM_SELECTED_WHIP_PAIR(
                RandomSelectWeapon<item::weapon::whip_type::Enum>(WEAPON_CHANCES.whip_map));

            if ((RANDOM_SELECTED_WHIP_PAIR.first != item::weapon::whip_type::Count)
                && (misc::IsRealClose(RANDOM_SELECTED_WHIP_PAIR.second, 0.0f) == false))
            {
                typeKindChanceMap[item::weapon_type::Whip] = std::make_pair(
                    RANDOM_SELECTED_WHIP_PAIR.first, RANDOM_SELECTED_WHIP_PAIR.second);
            }

            // knife/dagger
            {
                auto countChance{ 0.0f };
                if (WEAPON_CHANCES.knife.num_owned_map.Find(1, countChance))
                {
                    typeKindChanceMap[item::weapon_type::Knife] = std::make_pair(0, countChance);
                }
            }

            // staff/quarterstaff
            {
                auto countChance{ 0.0f };
                if (WEAPON_CHANCES.staff.num_owned_map.Find(1, countChance))
                {
                    typeKindChanceMap[item::weapon_type::Staff] = std::make_pair(0, countChance);
                }
            }

            // check for no-weapon case
            if (typeKindChanceMap.empty())
            {
                return itemsPtrVecPair;
            }

            // random select weapon
            item::weapon_type::Enum randomSelectedWeaponType(typeKindChanceMap.begin()->first);
            float chanceCumulative(0.0f);
            const float RAND_WEAPON_TYPE_CHANCE(misc::random::Float());
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
                case item::weapon_type::Knife:
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

                    itemsPtrVecPair.first.emplace_back(
                        item::weapon::WeaponFactory::Instance()->Make_Knife(
                            IS_DAGGER,
                            knifeSize,
                            WEAPON_CHANCES.knife.RandomMaterialPri(),
                            WEAPON_CHANCES.knife.RandomMaterialSec()));

                    break;
                }
                case item::weapon_type::Staff:
                {
                    auto const IS_QUARTERSTAFF{ misc::random::Float()
                                                < WEAPON_CHANCES.staff.is_quarterstaff };

                    itemsPtrVecPair.first.emplace_back(
                        item::weapon::WeaponFactory::Instance()->Make_Staff(
                            IS_QUARTERSTAFF,
                            WEAPON_CHANCES.staff.RandomMaterialPri(),
                            WEAPON_CHANCES.staff.RandomMaterialSec()));
                    break;
                }
                case item::weapon_type::Axe:
                {
                    auto const AXE_TYPE{ static_cast<item::weapon::axe_type::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    non_player::ownership::chance::ItemChances axeChances;
                    auto const WAS_AXE_FOUND{ WEAPON_CHANCES.axe_map.Find(AXE_TYPE, axeChances) };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        WAS_AXE_FOUND,
                        "non_player::ownership::InventoryFactory::MakeItemSet_Weapons"
                            << "(creature_name=\"" << CHARACTER_PTR->Name()
                            << "\") randomly selected weapon type=\""
                            << item::weapon_type::ToString(randomSelectedWeaponType, false)
                            << "\" and kind=\"" << item::weapon::axe_type::ToString(AXE_TYPE)
                            << "\" -but that weapon was not found in the original WEAPON_CHANCES "
                               "object.");

                    // if the primary material is wood, make sure there is a valid secondary
                    // material
                    auto const MATERIAL_PRI{ axeChances.RandomMaterialPri() };
                    auto materialSec{ axeChances.RandomMaterialSec() };
                    if ((MATERIAL_PRI == item::material::Wood)
                        && (item::material::Nothing == materialSec))
                    {
                        materialSec = item::material::Steel;
                    }

                    itemsPtrVecPair.first.emplace_back(
                        item::weapon::WeaponFactory::Instance()->Make_Axe(
                            AXE_TYPE, MATERIAL_PRI, materialSec));

                    break;
                }
                case item::weapon_type::BladedStaff:
                {
                    auto const BLADEDSTAFF_TYPE{ static_cast<item::weapon::bladedstaff_type::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    non_player::ownership::chance::ItemChances bstaffChances;

                    auto const WAS_BSTAFF_FOUND{ WEAPON_CHANCES.bladedstaff_map.Find(
                        BLADEDSTAFF_TYPE, bstaffChances) };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        WAS_BSTAFF_FOUND,
                        "non_player::ownership::InventoryFactory::MakeItemSet_Weapons"
                            << "(creature_name=\"" << CHARACTER_PTR->Name()
                            << "\") randomly selected weapon type=\""
                            << item::weapon_type::ToString(randomSelectedWeaponType, false)
                            << "\" and kind=\""
                            << item::weapon::bladedstaff_type::ToString(BLADEDSTAFF_TYPE)
                            << "\" -but that weapon was not found in the original WEAPON_CHANCES "
                               "object.");

                    // if the primary material is wood, make sure there is a valid secondary
                    // material
                    auto const MATERIAL_PRI{ bstaffChances.RandomMaterialPri() };
                    auto materialSec{ bstaffChances.RandomMaterialSec() };

                    if ((MATERIAL_PRI == item::material::Wood)
                        && (item::material::Nothing == materialSec))
                    {
                        materialSec = item::material::Steel;
                    }

                    itemsPtrVecPair.first.emplace_back(
                        item::weapon::WeaponFactory::Instance()->Make_BladedStaff(
                            BLADEDSTAFF_TYPE, MATERIAL_PRI, materialSec));

                    break;
                }
                case item::weapon_type::Club:
                {
                    auto const CLUB_TYPE{ static_cast<item::weapon::club_type::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    non_player::ownership::chance::ItemChances clubChances;

                    auto const WAS_CLUB_FOUND{ WEAPON_CHANCES.club_map.Find(
                        CLUB_TYPE, clubChances) };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        WAS_CLUB_FOUND,
                        "non_player::ownership::InventoryFactory::MakeItemSet_Weapons("
                            << "creature_name=\"" << CHARACTER_PTR->Name()
                            << "\") randomly selected weapon type=\""
                            << item::weapon_type::ToString(randomSelectedWeaponType, false)
                            << "\" and kind=\"" << item::weapon::club_type::ToString(CLUB_TYPE)
                            << "\" -but that weapon was not found in the original WEAPON_CHANCES "
                               "object.");

                    // if the primary material is wood, make sure there is a valid secondary
                    // material
                    auto const MATERIAL_PRI{ clubChances.RandomMaterialPri() };
                    auto materialSec{ clubChances.RandomMaterialSec() };

                    if ((MATERIAL_PRI == item::material::Wood)
                        && (item::material::Nothing == materialSec))
                    {
                        materialSec = item::material::Steel;
                    }

                    itemsPtrVecPair.first.emplace_back(
                        item::weapon::WeaponFactory::Instance()->Make_Club(
                            CLUB_TYPE, MATERIAL_PRI, materialSec));

                    break;
                }
                case item::weapon_type::Projectile:
                {
                    auto const PROJECTILE_TYPE{ static_cast<item::weapon::projectile_type::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    non_player::ownership::chance::ItemChances projChances;

                    auto const WAS_PROJ_FOUND{ WEAPON_CHANCES.projectile_map.Find(
                        PROJECTILE_TYPE, projChances) };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        WAS_PROJ_FOUND,
                        "non_player::ownership::InventoryFactory::MakeItemSet_Weapons("
                            << "creature_name=\"" << CHARACTER_PTR->Name()
                            << "\") randomly selected weapon type=\""
                            << item::weapon_type::ToString(randomSelectedWeaponType, false)
                            << "\" and kind=\""
                            << item::weapon::projectile_type::ToString(PROJECTILE_TYPE)
                            << "\" -but that weapon was not found in the original WEAPON_CHANCES "
                               "object.");

                    itemsPtrVecPair.first.emplace_back(
                        item::weapon::WeaponFactory::Instance()->Make_Projectile(
                            PROJECTILE_TYPE,
                            projChances.RandomMaterialPri(),
                            projChances.RandomMaterialSec()));

                    break;
                }
                case item::weapon_type::Sword:
                {
                    auto const SWORD_TYPE{ static_cast<item::weapon::sword_type::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    non_player::ownership::chance::ItemChances swordChances;

                    auto const WAS_SWORD_FOUND{ WEAPON_CHANCES.sword_map.Find(
                        SWORD_TYPE, swordChances) };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        WAS_SWORD_FOUND,
                        "non_player::ownership::InventoryFactory::MakeItemSet_Weapons("
                            << "creature_name=\"" << CHARACTER_PTR->Name()
                            << "\") randomly selected weapon type=\""
                            << item::weapon_type::ToString(randomSelectedWeaponType, false)
                            << "\" and kind=\"" << item::weapon::sword_type::ToString(SWORD_TYPE)
                            << "\" -but that weapon was not found in the original WEAPON_CHANCES "
                               "object.");

                    itemsPtrVecPair.first.emplace_back(
                        item::weapon::WeaponFactory::Instance()->Make_Sword(
                            SWORD_TYPE,
                            swordChances.RandomMaterialPri(),
                            swordChances.RandomMaterialSec()));

                    break;
                }
                case item::weapon_type::Whip:
                {
                    auto const WHIP_TYPE{ static_cast<item::weapon::whip_type::Enum>(
                        typeKindChanceMap[randomSelectedWeaponType].first) };

                    non_player::ownership::chance::ItemChances whipChances;

                    auto const WAS_WHIP_FOUND{ WEAPON_CHANCES.whip_map.Find(
                        WHIP_TYPE, whipChances) };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        WAS_WHIP_FOUND,
                        "non_player::ownership::InventoryFactory::MakeItemSet_Weapons("
                            << "creature_name=\"" << CHARACTER_PTR->Name()
                            << "\") randomly selected weapon type=\""
                            << item::weapon_type::ToString(randomSelectedWeaponType, false)
                            << "\" and kind=\"" << item::weapon::whip_type::ToString(WHIP_TYPE)
                            << "\" -but that weapon was not found in the original WEAPON_CHANCES "
                               "object.");

                    itemsPtrVecPair.first.emplace_back(
                        item::weapon::WeaponFactory::Instance()->Make_Whip(
                            WHIP_TYPE,
                            whipChances.RandomMaterialPri(),
                            whipChances.RandomMaterialSec()));

                    break;
                }
                case item::weapon_type::Bite:
                case item::weapon_type::Breath:
                case item::weapon_type::Bladed:
                case item::weapon_type::Claws:
                case item::weapon_type::Fists:
                case item::weapon_type::Melee:
                case item::weapon_type::NotAWeapon:
                case item::weapon_type::Pointed:
                case item::weapon_type::Tendrils:
                case item::weapon_type::Crossbow:
                case item::weapon_type::Blowpipe:
                case item::weapon_type::Bow:
                case item::weapon_type::Spear:
                case item::weapon_type::Sling:
                default:
                {
                    std::ostringstream ss;
                    ss << "non_player::ownership::InventoryFactory::MakeItemSet_Weapons("
                       << "creature_name=\"" << CHARACTER_PTR->Name()
                       << "\") failed to find a valid random selected weapon.  (weapon_type="
                       << item::weapon_type::ToString(randomSelectedWeaponType, false) << "\")";

                    throw std::runtime_error(ss.str());
                }
            }

            return itemsPtrVecPair;
        }

        const IItemPVecPair_t InventoryFactory::MakeItemSet_Armor(
            const chance::ArmorChances & CHANCES,
            const non_player::CharacterPtr_t CHARACTER_PTR,
            const bool HAS_TWO_HANDED_WEAPON)
        {
            using namespace item::armor;

            IItemPVecPair_t itemsPtrVecPair;

            if (CHANCES.aventail.IsOwned())
            {
                itemsPtrVecPair.first.emplace_back(ArmorFactory::Instance()->Make_Aventail(
                    CHANCES.aventail.RandomArmorBaseType(),
                    CHANCES.aventail.RandomMaterialPri(),
                    CHANCES.aventail.RandomMaterialSec()));
            }

            if (CHANCES.boots.IsOwned())
            {
                itemsPtrVecPair.first.emplace_back(ArmorFactory::Instance()->Make_Boots(
                    CHANCES.boots.RandomArmorBaseType(),
                    CHANCES.boots.RandomMaterialPri(),
                    CHANCES.boots.RandomMaterialSec()));
            }

            if (CHANCES.bracers.IsOwned())
            {
                itemsPtrVecPair.first.emplace_back(ArmorFactory::Instance()->Make_Bracer(
                    CHANCES.bracers.RandomArmorBaseType(),
                    CHANCES.bracers.RandomMaterialPri(),
                    CHANCES.bracers.RandomMaterialSec()));
            }

            if (CHANCES.gauntlets.IsOwned())
            {
                itemsPtrVecPair.first.emplace_back(ArmorFactory::Instance()->Make_Gauntlets(
                    CHANCES.gauntlets.RandomArmorBaseType(),
                    CHANCES.gauntlets.RandomMaterialPri(),
                    CHANCES.gauntlets.RandomMaterialSec()));
            }

            if (CHANCES.pants.IsOwned())
            {
                itemsPtrVecPair.first.emplace_back(ArmorFactory::Instance()->Make_Pants(
                    CHANCES.pants.RandomArmorBaseType(), CHANCES.pants.RandomMaterialSec()));
            }

            if (CHANCES.shirt.IsOwned())
            {
                itemsPtrVecPair.first.emplace_back(ArmorFactory::Instance()->Make_Shirt(
                    CHANCES.shirt.RandomArmorBaseType(), CHANCES.shirt.RandomMaterialSec()));
            }

            // covers (vest/robe/cloak/cape)
            auto const COVER_PAIR{ CHANCES.RandomCover() };
            if (COVER_PAIR.second > 0)
            {
                non_player::ownership::chance::ItemChances coverChances;

                auto const WAS_COVER_FOUND{ CHANCES.cover_map.Find(
                    COVER_PAIR.first, coverChances) };

                M_ASSERT_OR_LOGANDTHROW_SS(
                    WAS_COVER_FOUND,
                    "non_player::ownership::InventoryFactory::MakeItemSet_Armor(creature_name=\""
                        << CHARACTER_PTR->Name() << "\") ARMOR_CHANCES.RandomCover() returned \""
                        << cover_type::ToString(COVER_PAIR.first)
                        << "\", but that item was not found in the ARMOR_CHANCES.cover_map.");

                itemsPtrVecPair.first.emplace_back(ArmorFactory::Instance()->Make_Cover(
                    COVER_PAIR.first,
                    coverChances.RandomMaterialPri(),
                    coverChances.RandomMaterialSec()));
            }

            // helms
            auto const HELM_PAIR{ CHANCES.RandomHelm() };
            if (HELM_PAIR.second > 0)
            {
                non_player::ownership::chance::ItemChances helmChances;

                auto const WAS_HELM_FOUND{ CHANCES.helm_map.Find(HELM_PAIR.first, helmChances) };

                M_ASSERT_OR_LOGANDTHROW_SS(
                    WAS_HELM_FOUND,
                    "non_player::ownership::InventoryFactory::MakeItemSet_Armor(creature_name=\""
                        << CHARACTER_PTR->Name() << "\") ARMOR_CHANCES.RandomHelm() returned \""
                        << helm_type::ToString(HELM_PAIR.first)
                        << "\", but that item was not found in the ARMOR_CHANCES.helm_map.");

                itemsPtrVecPair.first.emplace_back(ArmorFactory::Instance()->Make_Helm(
                    HELM_PAIR.first,
                    helmChances.RandomMaterialPri(),
                    helmChances.RandomMaterialSec()));
            }

            // shields
            auto const SHIELD_PAIR{ CHANCES.RandomShield() };
            if (SHIELD_PAIR.second > 0)
            {
                ownership::chance::ItemChances shieldChances;

                auto const WAS_SHIELD_FOUND{ CHANCES.shield_map.Find(
                    SHIELD_PAIR.first, shieldChances) };

                M_ASSERT_OR_LOGANDTHROW_SS(
                    WAS_SHIELD_FOUND,
                    "non_player::ownership::InventoryFactory::MakeItemSet_Armor(creature_name=\""
                        << CHARACTER_PTR->Name() << "\") ARMOR_CHANCES.RandomShield() returned \""
                        << shield_type::ToString(SHIELD_PAIR.first)
                        << "\", but that item was not found in the ARMOR_CHANCES.shield_map.");

                if (HAS_TWO_HANDED_WEAPON)
                {
                    itemsPtrVecPair.second.emplace_back(ArmorFactory::Instance()->Make_Shield(
                        SHIELD_PAIR.first,
                        shieldChances.RandomMaterialPri(),
                        shieldChances.RandomMaterialSec()));
                }
                else
                {
                    itemsPtrVecPair.first.emplace_back(ArmorFactory::Instance()->Make_Shield(
                        SHIELD_PAIR.first,
                        shieldChances.RandomMaterialPri(),
                        shieldChances.RandomMaterialSec()));
                }
            }

            return itemsPtrVecPair;
        }

        const item::ItemPVec_t InventoryFactory::MakeItemSet_BodyWeapons(
            const chance::WeaponChances & CHANCES,
            const non_player::CharacterPtr_t CHARACTER_PTR,
            const bool HAS_TWO_HANDED_WEAPON_EQUIPPED)
        {
            item::ItemPVec_t bodyWeaponsSVec;

            if (CHANCES.has_bite)
            {
                bodyWeaponsSVec.emplace_back(
                    item::weapon::WeaponFactory::Instance()->Make_Bite(CHARACTER_PTR));
            }

            if (CHANCES.has_claws && (HAS_TWO_HANDED_WEAPON_EQUIPPED == false))
            {
                bodyWeaponsSVec.emplace_back(
                    item::weapon::WeaponFactory::Instance()->Make_Claws(CHARACTER_PTR));
            }

            if (CHANCES.has_fists && (HAS_TWO_HANDED_WEAPON_EQUIPPED == false)
                && (CHANCES.has_claws == false))
            {
                bodyWeaponsSVec.emplace_back(item::weapon::WeaponFactory::Instance()->Make_Fists());
            }

            if (CHANCES.has_tendrils && (HAS_TWO_HANDED_WEAPON_EQUIPPED == false))
            {
                bodyWeaponsSVec.emplace_back(
                    item::weapon::WeaponFactory::Instance()->Make_Tendrils(CHARACTER_PTR));
            }

            if (CHANCES.has_breath)
            {
                bodyWeaponsSVec.emplace_back(
                    item::weapon::WeaponFactory::Instance()->Make_Breath(CHARACTER_PTR));
            }

            return bodyWeaponsSVec;
        }

        Coin_t InventoryFactory::Make_Coins(const chance::InventoryChances & CHANCES)
        {
            return CHANCES.RandomCoins();
        }

        bool InventoryFactory::ContainsTwoHandedWeapon(const item::ItemPVec_t & WEAPON_PVEC)
        {
            for (auto const NEXT_WEAPON_SPTR : WEAPON_PVEC)
            {
                if (NEXT_WEAPON_SPTR->IsTwoHanded())
                {
                    return true;
                }
            }

            return false;
        }

        void InventoryFactory::RemoveArmorTypeFromVec(
            const item::armor_type::Enum ENUM, item::ItemPVec_t & vec)
        {
            vec.erase(
                std::remove_if(
                    vec.begin(),
                    vec.end(),
                    [&](const item::ItemPtr_t PTR) { return (PTR->ArmorType() == ENUM); }),
                vec.end());
        }

    } // namespace ownership
} // namespace non_player
} // namespace heroespath
