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

#include "game/item/item.hpp"
#include "game/non-player/chance-factory.hpp"
#include "game/creature/creature.hpp"
#include "game/log-macros.hpp"
#include "game/non-player/character.hpp"

#include "utilz/random.hpp"

#include <iterator>//for back_inserter
#include <exception>
#include <algorithm>
#include <sstream>
#include <string>
#include <map>


namespace game
{
namespace non_player
{
namespace ownership
{

    InventoryFactorySPtr_t InventoryFactory::instance_(nullptr);


    InventoryFactory::InventoryFactory()
    {}


    InventoryFactory::~InventoryFactory()
    {}


    InventoryFactorySPtr_t InventoryFactory::Instance()
    {
        if (instance_.get() == nullptr)
            instance_.reset( new InventoryFactory );

        return instance_;
    }


    void InventoryFactory::PopulateCreatureInventory(non_player::CharacterSPtr_t & creatureSPtr)
    {
        const chance::InventoryChances INVENTORY_CHANCES( ChanceFactory::Make(creatureSPtr) );
        creatureSPtr->CoinsAdj( Make_Coins(INVENTORY_CHANCES) );

        //.first is for equipped items and .second is for unequipped items
        const IItemSVecPair_t ITEM_SVEC_PAIR( MakeItemSet(INVENTORY_CHANCES, creatureSPtr) );
        for (auto const & NEXT_ITEM_SPTR : ITEM_SVEC_PAIR.first)
        {
            const std::string ITEM_ADD_RESULT( creatureSPtr->ItemAdd(NEXT_ITEM_SPTR) );
            if (ITEM_ADD_RESULT.empty() == false)
            {
                M_HP_LOG("game::non-player::ownership::InventoryFactory::PopulateCreatureInventory[to equip - add step](creature=\"" << creatureSPtr->ToString() << "\") unable to add the item \"" << NEXT_ITEM_SPTR->Name() << "\" \"" << NEXT_ITEM_SPTR->Desc() << "\" with reported error \"" << ITEM_ADD_RESULT << "\".  Proceeding...");
            }
            else
            {
                if (NEXT_ITEM_SPTR->Category() & item::category::Equippable)
                {
                    const std::string ITEM_EQUIP_RESULT(creatureSPtr->ItemEquip(NEXT_ITEM_SPTR));
                    if (ITEM_EQUIP_RESULT.empty() == false)
                        M_HP_LOG("game::non-player::ownership::InventoryFactory::PopulateCreatureInventory[to equip - equip step](creature=\"" << creatureSPtr->ToString() << "\") unable to add the item \"" << NEXT_ITEM_SPTR->Name() << "\" \"" << NEXT_ITEM_SPTR->Desc() << "\"with reported error \"" << ITEM_EQUIP_RESULT << "\".  Proceeding...");
                }
            }
        }

        for (auto const & NEXT_ITEM_SPTR : ITEM_SVEC_PAIR.second)
        {
            const std::string ITEM_ADD_RESULT(creatureSPtr->ItemAdd(NEXT_ITEM_SPTR));
            if (ITEM_ADD_RESULT.empty() == false)
                M_HP_LOG("game::non-player::ownership::InventoryFactory::PopulateCreatureInventory[not to equip](creature=\"" << creatureSPtr->ToString() << "\") unable to add the item \"" << NEXT_ITEM_SPTR->Name() << "\" \"" << NEXT_ITEM_SPTR->Desc() << "\" with reported error \"" << ITEM_ADD_RESULT << "\".  Proceeding...");
        }
    }


    const IItemSVecPair_t InventoryFactory::MakeItemSet(const chance::InventoryChances &    CHANCES,
                                                        const non_player::CharacterSPtr_t & CREATURE_SPTR)
    {
        IItemSVecPair_t itemsSPtrVecPair;

        const IItemSVecPair_t CLOTHING_ITEMS_SVEC_PAIR( MakeItemSet_Clothing(CHANCES.clothes) );
        std::copy(CLOTHING_ITEMS_SVEC_PAIR.first.begin(),  CLOTHING_ITEMS_SVEC_PAIR.first.end(),  back_inserter(itemsSPtrVecPair.first));
        std::copy(CLOTHING_ITEMS_SVEC_PAIR.second.begin(), CLOTHING_ITEMS_SVEC_PAIR.second.end(), back_inserter(itemsSPtrVecPair.second));

        const IItemSVecPair_t WEAPON_ITEMS_SVEC_PAIR( MakeItemSet_Weapons(CHANCES.weapon, CREATURE_SPTR) );
        std::copy(WEAPON_ITEMS_SVEC_PAIR.first.begin(),  WEAPON_ITEMS_SVEC_PAIR.first.end(),  back_inserter(itemsSPtrVecPair.first));
        std::copy(WEAPON_ITEMS_SVEC_PAIR.second.begin(), WEAPON_ITEMS_SVEC_PAIR.second.end(), back_inserter(itemsSPtrVecPair.second));

        const bool HAS_TWOHANDED_WEAPON_EQUIPPED( ContainsTwoHandedWeapon(WEAPON_ITEMS_SVEC_PAIR.first));

        const item::ItemSVec_t BODY_WEAPON_ITEMS_SVEC(MakeItemSet_BodyWeapons(CHANCES.weapon, CREATURE_SPTR, HAS_TWOHANDED_WEAPON_EQUIPPED));
        std::copy(BODY_WEAPON_ITEMS_SVEC.begin(), BODY_WEAPON_ITEMS_SVEC.end(), back_inserter(itemsSPtrVecPair.first));

        const IItemSVecPair_t ARMOR_ITEMS_SVEC_PAIR(MakeItemSet_Armor(CHANCES.armor, CREATURE_SPTR, HAS_TWOHANDED_WEAPON_EQUIPPED));
        std::copy(ARMOR_ITEMS_SVEC_PAIR.first.begin(),  ARMOR_ITEMS_SVEC_PAIR.first.end(),  back_inserter(itemsSPtrVecPair.first));
        std::copy(ARMOR_ITEMS_SVEC_PAIR.second.begin(), ARMOR_ITEMS_SVEC_PAIR.second.end(), back_inserter(itemsSPtrVecPair.second));

        return itemsSPtrVecPair;
    }


    const IItemSVecPair_t InventoryFactory::MakeItemSet_Clothing(const chance::ClothingChances & CHANCES)
    {
        //Assume there is only one of each article of clothing.

        //TODO enchantments

        IItemSVecPair_t itemsSPtrVecPair;

        if (CHANCES.boots.IsOwned())
            itemsSPtrVecPair.first.push_back( item::armor::ArmorFactory::Make_Boots(item::armor::base_type::Plain,
                                                                                    CHANCES.boots.RandomMaterialPri(),
                                                                                    CHANCES.boots.RandomMaterialSec()) );

        if (CHANCES.gloves.IsOwned())
            itemsSPtrVecPair.first.push_back( item::armor::ArmorFactory::Make_Gauntlets(item::armor::base_type::Plain,
                                                                                        CHANCES.boots.RandomMaterialPri(),
                                                                                        CHANCES.boots.RandomMaterialSec()) );

        if (CHANCES.pants.IsOwned())
            itemsSPtrVecPair.first.push_back( item::armor::ArmorFactory::Make_Pants(item::armor::base_type::Plain,
                                                                                    CHANCES.pants.RandomMaterialPri()) );

        if (CHANCES.shirt.IsOwned())
            itemsSPtrVecPair.first.push_back( item::armor::ArmorFactory::Make_Shirt(item::armor::base_type::Plain,
                                                                                    CHANCES.shirt.RandomMaterialPri()) );

        if (CHANCES.vest.IsOwned())
            itemsSPtrVecPair.first.push_back( item::armor::ArmorFactory::Make_Cover(item::armor::cover_type::Vest,
                                                                                    CHANCES.vest.RandomMaterialPri(),
                                                                                    CHANCES.vest.RandomMaterialSec()) );

        const item::armor::cover_type::Enum COVER_TYPE( CHANCES.RandomCoverType() );
        if (COVER_TYPE != item::armor::cover_type::Count)
        {
            chance::CoverChanceMap_t::const_iterator ITER( CHANCES.cover_map.find(COVER_TYPE) );
            M_ASSERT_OR_LOGANDTHROW_SS((ITER != CHANCES.cover_map.end()), "game::non_player::ownership::InventoryFactory::MakeItemSet_Clothing() failed to find \"" << COVER_TYPE << "\".");
            itemsSPtrVecPair.first.push_back( item::armor::ArmorFactory::Make_Cover(COVER_TYPE,
                                                                                    ITER->second.RandomMaterialPri(),
                                                                                    ITER->second.RandomMaterialSec()) );
        }

        return itemsSPtrVecPair;
    }


    const IItemSVecPair_t InventoryFactory::MakeItemSet_Weapons(const chance::WeaponChances &       WEAPON_CHANCES,
                                                                const non_player::CharacterSPtr_t & CREATURE_SPTR)
    {
        IItemSVecPair_t itemsSPtrVecPair;

        using KindChancePair_t = std::pair<int, float>;
        using TypeKindChanceMap_t = std::map<item::weapon_type::Enum, KindChancePair_t>;
        TypeKindChanceMap_t typeKindChanceMap;

        //TODO handle multiple weapons of the same type/kind

        //TODO handle intentionally unequipped weapons

        std::pair<item::weapon::axe_type::Enum, float> RANDOM_SELECTED_AXE_PAIR(RandomSelectWeapon<item::weapon::axe_type::Enum>(WEAPON_CHANCES.axe_map));
        if ((RANDOM_SELECTED_AXE_PAIR.first != item::weapon::axe_type::Count) && (utilz::IsRealClose(RANDOM_SELECTED_AXE_PAIR.second, 0.0f) == false))
            typeKindChanceMap[item::weapon_type::Axe] = std::make_pair(RANDOM_SELECTED_AXE_PAIR.first, RANDOM_SELECTED_AXE_PAIR.second);

        std::pair<item::weapon::bladedstaff_type::Enum, float> RANDOM_SELECTED_BLADEDSTAFF_PAIR(RandomSelectWeapon<item::weapon::bladedstaff_type::Enum>(WEAPON_CHANCES.bladedstaff_map));
        if ((RANDOM_SELECTED_BLADEDSTAFF_PAIR.first != item::weapon::bladedstaff_type::Count) && (utilz::IsRealClose(RANDOM_SELECTED_BLADEDSTAFF_PAIR.second, 0.0f) == false))
            typeKindChanceMap[item::weapon_type::BladedStaff] = std::make_pair(RANDOM_SELECTED_BLADEDSTAFF_PAIR.first, RANDOM_SELECTED_BLADEDSTAFF_PAIR.second);

        std::pair<item::weapon::club_type::Enum, float> RANDOM_SELECTED_CLUB_PAIR(RandomSelectWeapon<item::weapon::club_type::Enum>(WEAPON_CHANCES.club_map));
        if ((RANDOM_SELECTED_CLUB_PAIR.first != item::weapon::club_type::Count) && (utilz::IsRealClose(RANDOM_SELECTED_CLUB_PAIR.second, 0.0f) == false))
            typeKindChanceMap[item::weapon_type::Club] = std::make_pair(RANDOM_SELECTED_CLUB_PAIR.first, RANDOM_SELECTED_CLUB_PAIR.second);

        std::pair<item::weapon::projectile_type::Enum, float> RANDOM_SELECTED_PROJECTILE_PAIR(RandomSelectWeapon<item::weapon::projectile_type::Enum>(WEAPON_CHANCES.projectile_map));
        if ((RANDOM_SELECTED_PROJECTILE_PAIR.first != item::weapon::projectile_type::Count) && (utilz::IsRealClose(RANDOM_SELECTED_PROJECTILE_PAIR.second, 0.0f) == false))
            typeKindChanceMap[item::weapon_type::Projectile] = std::make_pair(RANDOM_SELECTED_PROJECTILE_PAIR.first, RANDOM_SELECTED_PROJECTILE_PAIR.second);

        std::pair<item::weapon::sword_type::Enum, float> RANDOM_SELECTED_SWORD_PAIR(RandomSelectWeapon<item::weapon::sword_type::Enum>(WEAPON_CHANCES.sword_map));
        if ((RANDOM_SELECTED_SWORD_PAIR.first != item::weapon::sword_type::Count) && (utilz::IsRealClose(RANDOM_SELECTED_SWORD_PAIR.second, 0.0f) == false))
            typeKindChanceMap[item::weapon_type::Sword] = std::make_pair(RANDOM_SELECTED_SWORD_PAIR.first, RANDOM_SELECTED_SWORD_PAIR.second);

        std::pair<item::weapon::whip_type::Enum, float> RANDOM_SELECTED_WHIP_PAIR(RandomSelectWeapon<item::weapon::whip_type::Enum>(WEAPON_CHANCES.whip_map));
        if ((RANDOM_SELECTED_WHIP_PAIR.first != item::weapon::whip_type::Count) && (utilz::IsRealClose(RANDOM_SELECTED_WHIP_PAIR.second, 0.0f) == false))
            typeKindChanceMap[item::weapon_type::Whip] = std::make_pair(RANDOM_SELECTED_WHIP_PAIR.first, RANDOM_SELECTED_WHIP_PAIR.second);

        //knife/dagger
        chance::CountChanceMap_t::const_iterator KNIFE_ITER(WEAPON_CHANCES.knife.num_owned_map.find(1));
        if (KNIFE_ITER != chance::CountChanceMap_t::const_iterator())
            typeKindChanceMap[item::weapon_type::Knife] = std::make_pair(0, KNIFE_ITER->second);

        //staff/quarterstaff
        chance::CountChanceMap_t::const_iterator STAFF_ITER(WEAPON_CHANCES.staff.num_owned_map.find(1));
        if (STAFF_ITER != chance::CountChanceMap_t::const_iterator())
            typeKindChanceMap[item::weapon_type::Staff] = std::make_pair(0, STAFF_ITER->second);

        //check for no-weapon case
        if (typeKindChanceMap.empty())
            return itemsSPtrVecPair;

        //random select weapon
        item::weapon_type::Enum randomSelectedWeaponType(typeKindChanceMap.begin()->first);
        float chanceCumulative(0.0f);
        const float RAND_WEAPON_TYPE_CHANCE( utilz::random::Float() );
        for (auto const & NEXT_TYPEKIND_PAIR : typeKindChanceMap)
        {
            chanceCumulative += NEXT_TYPEKIND_PAIR.second.second;
            if (RAND_WEAPON_TYPE_CHANCE < chanceCumulative)
            {
                randomSelectedWeaponType = NEXT_TYPEKIND_PAIR.first;
                break;
            }
        }

        //create weapon
        switch (randomSelectedWeaponType)
        {
            case item::weapon_type::Knife:
            {
                //determine which size the knife/dagger will be
                const float RAND(utilz::random::Float());
                sfml_util::Size::Enum knifeSize(sfml_util::Size::Large);
                //
                chance::SizeChanceMap_t::const_iterator CITER_SMALL(WEAPON_CHANCES.knife.size_map.find(sfml_util::Size::Small));
                if (CITER_SMALL != chance::SizeChanceMap_t::const_iterator())
                    if (RAND < CITER_SMALL->second)
                        knifeSize = sfml_util::Size::Small;
                //
                if (knifeSize != sfml_util::Size::Small)
                {
                    chance::SizeChanceMap_t::const_iterator CITER_MED(WEAPON_CHANCES.knife.size_map.find(sfml_util::Size::Medium));
                    if (CITER_MED != chance::SizeChanceMap_t::const_iterator())
                        if (RAND < (CITER_SMALL->second + CITER_MED->second))
                            knifeSize = sfml_util::Size::Medium;
                }

                const bool IS_DAGGER(utilz::random::Float() < WEAPON_CHANCES.knife.is_dagger);

                itemsSPtrVecPair.first.push_back( item::weapon::WeaponFactory::Instance()->Make_Knife(IS_DAGGER,
                                                                                                      knifeSize,
                                                                                                      WEAPON_CHANCES.knife.RandomMaterialPri(),
                                                                                                      WEAPON_CHANCES.knife.RandomMaterialSec()) );

                break;
            }
            case item::weapon_type::Staff:
            {
                const bool IS_QUARTERSTAFF(utilz::random::Float() < WEAPON_CHANCES.staff.is_quarterstaff);
                itemsSPtrVecPair.first.push_back( item::weapon::WeaponFactory::Instance()->Make_Staff(IS_QUARTERSTAFF,
                                                                                                      WEAPON_CHANCES.staff.RandomMaterialPri(),
                                                                                                      WEAPON_CHANCES.staff.RandomMaterialSec()) );
                break;
            }
            case item::weapon_type::Axe:
            {
                const item::weapon::axe_type::Enum AXE_TYPE( static_cast<item::weapon::axe_type::Enum>(typeKindChanceMap[randomSelectedWeaponType].first) );
                const chance::AxeChanceMap_t::const_iterator CITER( WEAPON_CHANCES.axe_map.find(AXE_TYPE) );
                M_ASSERT_OR_LOGANDTHROW_SS((CITER != WEAPON_CHANCES.axe_map.end()), "game::non_player::ownership::InventoryFactory::MakeItemSet_Weapons(creature_name=\"" << CREATURE_SPTR->Name() << "\") randomly selected weapon type=\"" << item::weapon_type::ToString(randomSelectedWeaponType, false) << "\" and kind=\"" << item::weapon::axe_type::ToString(AXE_TYPE) << "\" -but that weapon was not found in the original WEAPON_CHANCES object.");

                //if the primary material is wood, make sure there is a valid secondary material
                const item::material::Enum MATERIAL_PRI(CITER->second.RandomMaterialPri());
                item::material::Enum materialSec(CITER->second.RandomMaterialSec());
                if ((MATERIAL_PRI == item::material::Wood) && (item::material::Nothing == materialSec))
                    materialSec = item::material::Steel;

                itemsSPtrVecPair.first.push_back( item::weapon::WeaponFactory::Instance()->Make_Axe(AXE_TYPE,
                                                                                                    MATERIAL_PRI,
                                                                                                    materialSec) );
                break;
            }
            case item::weapon_type::BladedStaff:
            {
                const item::weapon::bladedstaff_type::Enum BLADEDSTAFF_TYPE(static_cast<item::weapon::bladedstaff_type::Enum>(typeKindChanceMap[randomSelectedWeaponType].first));
                const chance::BladedStaffChanceMap_t::const_iterator CITER(WEAPON_CHANCES.bladedstaff_map.find(BLADEDSTAFF_TYPE));
                M_ASSERT_OR_LOGANDTHROW_SS((CITER != WEAPON_CHANCES.bladedstaff_map.end()), "game::non_player::ownership::InventoryFactory::MakeItemSet_Weapons(creature_name=\"" << CREATURE_SPTR->Name() << "\") randomly selected weapon type=\"" << item::weapon_type::ToString(randomSelectedWeaponType, false) << "\" and kind=\"" << item::weapon::bladedstaff_type::ToString(BLADEDSTAFF_TYPE) << "\" -but that weapon was not found in the original WEAPON_CHANCES object.");

                //if the primary material is wood, make sure there is a valid secondary material
                const item::material::Enum MATERIAL_PRI(CITER->second.RandomMaterialPri());
                item::material::Enum materialSec(CITER->second.RandomMaterialSec());
                if ((MATERIAL_PRI == item::material::Wood) && (item::material::Nothing == materialSec))
                    materialSec = item::material::Steel;

                itemsSPtrVecPair.first.push_back(item::weapon::WeaponFactory::Instance()->Make_BladedStaff(BLADEDSTAFF_TYPE,
                                                                                                           MATERIAL_PRI,
                                                                                                           materialSec));
                break;
            }
            case item::weapon_type::Club:
            {
                const item::weapon::club_type::Enum CLUB_TYPE(static_cast<item::weapon::club_type::Enum>(typeKindChanceMap[randomSelectedWeaponType].first));
                const chance::ClubChanceMap_t::const_iterator CITER(WEAPON_CHANCES.club_map.find(CLUB_TYPE));
                M_ASSERT_OR_LOGANDTHROW_SS((CITER != WEAPON_CHANCES.club_map.end()), "game::non_player::ownership::InventoryFactory::MakeItemSet_Weapons(creature_name=\"" << CREATURE_SPTR->Name() << "\") randomly selected weapon type=\"" << item::weapon_type::ToString(randomSelectedWeaponType, false) << "\" and kind=\"" << item::weapon::club_type::ToString(CLUB_TYPE) << "\" -but that weapon was not found in the original WEAPON_CHANCES object.");

                //if the primary material is wood, make sure there is a valid secondary material
                const item::material::Enum MATERIAL_PRI(CITER->second.RandomMaterialPri());
                item::material::Enum materialSec(CITER->second.RandomMaterialSec());
                if ((MATERIAL_PRI == item::material::Wood) && (item::material::Nothing == materialSec))
                    materialSec = item::material::Steel;

                itemsSPtrVecPair.first.push_back(item::weapon::WeaponFactory::Instance()->Make_Club(CLUB_TYPE,
                                                                                                    MATERIAL_PRI,
                                                                                                    materialSec));
                break;
            }
            case item::weapon_type::Projectile:
            {
                const item::weapon::projectile_type::Enum PROJECTILE_TYPE(static_cast<item::weapon::projectile_type::Enum>(typeKindChanceMap[randomSelectedWeaponType].first));
                const chance::ProjectileChanceMap_t::const_iterator CITER(WEAPON_CHANCES.projectile_map.find(PROJECTILE_TYPE));
                M_ASSERT_OR_LOGANDTHROW_SS((CITER != WEAPON_CHANCES.projectile_map.end()), "game::non_player::ownership::InventoryFactory::MakeItemSet_Weapons(creature_name=\"" << CREATURE_SPTR->Name() << "\") randomly selected weapon type=\"" << item::weapon_type::ToString(randomSelectedWeaponType, false) << "\" and kind=\"" << item::weapon::projectile_type::ToString(PROJECTILE_TYPE) << "\" -but that weapon was not found in the original WEAPON_CHANCES object.");
                itemsSPtrVecPair.first.push_back(item::weapon::WeaponFactory::Instance()->Make_Projectile(PROJECTILE_TYPE, CITER->second.RandomMaterialSec()));
                break;
            }
            case item::weapon_type::Sword:
            {
                const item::weapon::sword_type::Enum SWORD_TYPE(static_cast<item::weapon::sword_type::Enum>(typeKindChanceMap[randomSelectedWeaponType].first));
                const chance::SwordChanceMap_t::const_iterator CITER(WEAPON_CHANCES.sword_map.find(SWORD_TYPE));
                M_ASSERT_OR_LOGANDTHROW_SS((CITER != WEAPON_CHANCES.sword_map.end()), "game::non_player::ownership::InventoryFactory::MakeItemSet_Weapons(creature_name=\"" << CREATURE_SPTR->Name() << "\") randomly selected weapon type=\"" << item::weapon_type::ToString(randomSelectedWeaponType, false) << "\" and kind=\"" << item::weapon::sword_type::ToString(SWORD_TYPE) << "\" -but that weapon was not found in the original WEAPON_CHANCES object.");
                itemsSPtrVecPair.first.push_back(item::weapon::WeaponFactory::Instance()->Make_Sword(SWORD_TYPE,
                                                                                                     CITER->second.RandomMaterialPri(),
                                                                                                     CITER->second.RandomMaterialSec()));
                break;
            }
            case item::weapon_type::Whip:
            {
                const item::weapon::whip_type::Enum WHIP_TYPE(static_cast<item::weapon::whip_type::Enum>(typeKindChanceMap[randomSelectedWeaponType].first));
                const chance::WhipChanceMap_t::const_iterator CITER(WEAPON_CHANCES.whip_map.find(WHIP_TYPE));
                M_ASSERT_OR_LOGANDTHROW_SS((CITER != WEAPON_CHANCES.whip_map.end()), "game::non_player::ownership::InventoryFactory::MakeItemSet_Weapons(creature_name=\"" << CREATURE_SPTR->Name() << "\") randomly selected weapon type=\"" << item::weapon_type::ToString(randomSelectedWeaponType, false) << "\" and kind=\"" << item::weapon::whip_type::ToString(WHIP_TYPE) << "\" -but that weapon was not found in the original WEAPON_CHANCES object.");
                itemsSPtrVecPair.first.push_back(item::weapon::WeaponFactory::Instance()->Make_Whip(WHIP_TYPE, CITER->second.RandomMaterialSec()));
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
                ss << "game::non_player::ownership::InventoryFactory::MakeItemSet_Weapons(creature_name=\"" << CREATURE_SPTR->Name() << "\") failed to find a valid random selected weapon.  (weapon_type=" << item::weapon_type::ToString(randomSelectedWeaponType, false) << "\")";
                throw std::runtime_error(ss.str());
            }
        }

        return itemsSPtrVecPair;
    }


    const IItemSVecPair_t InventoryFactory::MakeItemSet_Armor(const chance::ArmorChances &        CHANCES,
                                                              const non_player::CharacterSPtr_t & CREATURE_SPTR,
                                                              const bool                          HAS_TWO_HANDED_WEAPON)
    {
        using namespace item::armor;

        IItemSVecPair_t itemsSPtrVecPair;

        if (CHANCES.aventail.IsOwned())
            itemsSPtrVecPair.first.push_back( ArmorFactory::Instance()->Make_Aventail(CHANCES.aventail.RandomArmorBaseType(),
                                                                                      CHANCES.aventail.RandomMaterialPri(),
                                                                                      CHANCES.aventail.RandomMaterialSec()) );

        if (CHANCES.boots.IsOwned())
            itemsSPtrVecPair.first.push_back( ArmorFactory::Instance()->Make_Boots(CHANCES.boots.RandomArmorBaseType(),
                                                                                   CHANCES.boots.RandomMaterialPri(),
                                                                                   CHANCES.boots.RandomMaterialSec()) );

        if (CHANCES.bracers.IsOwned())
            itemsSPtrVecPair.first.push_back( ArmorFactory::Instance()->Make_Bracer(CHANCES.bracers.RandomArmorBaseType(),
                                                                                    CHANCES.bracers.RandomMaterialPri(),
                                                                                    CHANCES.bracers.RandomMaterialSec()) );

        if (CHANCES.gauntlets.IsOwned())
            itemsSPtrVecPair.first.push_back( ArmorFactory::Instance()->Make_Gauntlets(CHANCES.gauntlets.RandomArmorBaseType(),
                                                                                       CHANCES.gauntlets.RandomMaterialPri(),
                                                                                       CHANCES.gauntlets.RandomMaterialSec()) );

        if (CHANCES.pants.IsOwned())
            itemsSPtrVecPair.first.push_back( ArmorFactory::Instance()->Make_Pants(CHANCES.pants.RandomArmorBaseType(),
                                                                                   CHANCES.pants.RandomMaterialSec()) );

        if (CHANCES.shirt.IsOwned())
            itemsSPtrVecPair.first.push_back( ArmorFactory::Instance()->Make_Shirt(CHANCES.shirt.RandomArmorBaseType(),
                                                                                   CHANCES.shirt.RandomMaterialSec()) );

        //covers (vest/robe/cloak/cape)
        const std::pair<cover_type::Enum, std::size_t> COVER_PAIR( CHANCES.RandomCover() );
        if (COVER_PAIR.second > 0)
        {
            const chance::CoverChanceMap_t::const_iterator COVER_CITER(CHANCES.cover_map.find(COVER_PAIR.first));
            M_ASSERT_OR_LOGANDTHROW_SS((COVER_CITER != CHANCES.cover_map.end()), "game::non_player::ownership::InventoryFactory::MakeItemSet_Armor(creature_name=\"" << CREATURE_SPTR->Name() << "\") ARMOR_CHANCES.RandomCover() returned \"" << cover_type::ToString(COVER_PAIR.first) << "\", but that item was not found in the ARMOR_CHANCES.cover_map.");
            itemsSPtrVecPair.first.push_back( ArmorFactory::Instance()->Make_Cover(COVER_PAIR.first,
                                                                                    COVER_CITER->second.RandomMaterialPri(),
                                                                                    COVER_CITER->second.RandomMaterialSec()) );
        }

        //helms
        const std::pair<helm_type::Enum, std::size_t> HELM_PAIR(CHANCES.RandomHelm());
        if (HELM_PAIR.second > 0)
        {
            const chance::HelmChanceMap_t::const_iterator HELM_CITER(CHANCES.helm_map.find(HELM_PAIR.first));
            M_ASSERT_OR_LOGANDTHROW_SS((HELM_CITER != CHANCES.helm_map.end()), "game::non_player::ownership::InventoryFactory::MakeItemSet_Armor(creature_name=\"" << CREATURE_SPTR->Name() << "\") ARMOR_CHANCES.RandomHelm() returned \"" << helm_type::ToString(HELM_PAIR.first) << "\", but that item was not found in the ARMOR_CHANCES.helm_map.");
            itemsSPtrVecPair.first.push_back( ArmorFactory::Instance()->Make_Helm(HELM_PAIR.first,
                                                                                  HELM_CITER->second.RandomMaterialPri(),
                                                                                  HELM_CITER->second.RandomMaterialSec()) );

        }

        //shields
        const std::pair<shield_type::Enum, std::size_t> SHIELD_PAIR(CHANCES.RandomShield());
        if (SHIELD_PAIR.second > 0)
        {
            const chance::ShieldChanceMap_t::const_iterator SHIELD_CITER(CHANCES.shield_map.find(SHIELD_PAIR.first));
            M_ASSERT_OR_LOGANDTHROW_SS((SHIELD_CITER != CHANCES.shield_map.end()), "game::non_player::ownership::InventoryFactory::MakeItemSet_Armor(creature_name=\"" << CREATURE_SPTR->Name() << "\") ARMOR_CHANCES.RandomShield() returned \"" << shield_type::ToString(SHIELD_PAIR.first) << "\", but that item was not found in the ARMOR_CHANCES.shield_map.");
            if (HAS_TWO_HANDED_WEAPON)
                itemsSPtrVecPair.second.push_back(ArmorFactory::Instance()->Make_Shield(SHIELD_PAIR.first,
                                                                                        SHIELD_CITER->second.RandomMaterialPri(),
                                                                                        SHIELD_CITER->second.RandomMaterialSec()));
            else
                itemsSPtrVecPair.first.push_back(ArmorFactory::Instance()->Make_Shield(SHIELD_PAIR.first,
                                                                                       SHIELD_CITER->second.RandomMaterialPri(),
                                                                                       SHIELD_CITER->second.RandomMaterialSec()));
        }

        return itemsSPtrVecPair;
    }


    const item::ItemSVec_t InventoryFactory::MakeItemSet_BodyWeapons(const chance::WeaponChances &       CHANCES,
                                                                     const non_player::CharacterSPtr_t & CREATURE_SPTR,
                                                                     const bool                          HAS_TWO_HANDED_WEAPON_EQUIPPED)
    {
        item::ItemSVec_t bodyWeaponsSVec;

        if (CHANCES.has_bite)
            bodyWeaponsSVec.push_back( item::weapon::WeaponFactory::Instance()->Make_Bite(CREATURE_SPTR.get()) );

        if (CHANCES.has_claws && (HAS_TWO_HANDED_WEAPON_EQUIPPED == false))
            bodyWeaponsSVec.push_back( item::weapon::WeaponFactory::Instance()->Make_Claws(CREATURE_SPTR.get()) );

        if (CHANCES.has_fists && (HAS_TWO_HANDED_WEAPON_EQUIPPED == false) && (CHANCES.has_claws == false))
            bodyWeaponsSVec.push_back( item::weapon::WeaponFactory::Instance()->Make_Fists() );

        if (CHANCES.has_tendrils && (HAS_TWO_HANDED_WEAPON_EQUIPPED == false))
            bodyWeaponsSVec.push_back( item::weapon::WeaponFactory::Instance()->Make_Tendrils(CREATURE_SPTR.get()) );

        if (CHANCES.has_breath)
            bodyWeaponsSVec.push_back( item::weapon::WeaponFactory::Instance()->Make_Breath(CREATURE_SPTR.get()) );

        return bodyWeaponsSVec;
    }


    item::Coin_t InventoryFactory::Make_Coins(const chance::InventoryChances & CHANCES)
    {
        if (CHANCES.coins_min < CHANCES.coins_max)
            return utilz::random::Int(CHANCES.coins_min, CHANCES.coins_max);
        else
            return CHANCES.coins_min;
    }


    bool InventoryFactory::ContainsTwoHandedWeapon(const item::ItemSVec_t & WEAPON_SVEC)
    {
        for (auto const & NEXT_WEAPON_SPTR : WEAPON_SVEC)
            if (NEXT_WEAPON_SPTR->IsTwoHanded())
                return true;

        return false;
    }

}
}
}
