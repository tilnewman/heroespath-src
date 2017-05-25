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
// chance-factory.cpp
//
#include "chance-factory.hpp"

#include "misc/boost-string-includes.hpp"

#include "game/item/item.hpp"
#include "game/item/weapon-factory.hpp"
#include "game/item/weapon-details.hpp"
#include "game/item/armor-details.hpp"
#include "misc/assertlogandthrow.hpp"
#include "game/log-macros.hpp"
#include "game/stats/types.hpp"
#include "game/non-player/character.hpp"

#include "stringutil/stringhelp.hpp"

#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <exception>
#include <utility>
#include <sstream>
#include <vector>
#include <map>
#include <string>


namespace game
{
namespace non_player
{
namespace ownership
{

    const float                 ChanceFactory::CHANCE_MINIMUM_(0.001f);
    const float                 ChanceFactory::CHANCE_MAXIMUM_(0.999f);
    const float                 ChanceFactory::CHANCE_WEAPON_ENUM_POS_ADJ_SET_(0.5f);
    const float                 ChanceFactory::CHANCE_WEAPON_ENUM_POS_ADJ_INDIVIDUAL_(0.1f);
    ChanceFactorySPtr_t         ChanceFactory::instance_(nullptr);
    chance::MaterialChanceMap_t ChanceFactory::materialChanceMapCool_;
    chance::MaterialChanceMap_t ChanceFactory::materialChanceMapMetal_;
    chance::MaterialChanceMap_t ChanceFactory::materialChanceMapPrecious_;


    ChanceFactory::ChanceFactory()
    {}


    ChanceFactory::~ChanceFactory()
    {}


    ChanceFactorySPtr_t ChanceFactory::Instance()
    {
        if (instance_.get() == nullptr)
            instance_.reset( new ChanceFactory );

        return instance_;
    }


    const chance::InventoryChances ChanceFactory::Make(const non_player::CharacterSPtr_t & CREATURE_SPTR)
    {
        return Make(Profile::Make_FromCreature(CREATURE_SPTR), CREATURE_SPTR);
    }


    const chance::InventoryChances ChanceFactory::Make(const Profile &                     PROFILE,
                                                       const non_player::CharacterSPtr_t & CREATURE_SPTR)
    {
        item::Coin_t coinsMin(0);
        item::Coin_t coinsMax(0);
        Make_Coins(PROFILE, coinsMin, coinsMax);

        return chance::InventoryChances(coinsMin,
                                        coinsMax,
                                        Make_ClothingChances(PROFILE, CREATURE_SPTR),
                                        Make_WeaponChances(PROFILE, CREATURE_SPTR),
                                        Make_ArmorChances(PROFILE, CREATURE_SPTR),
                                        Make_MiscItemChances(PROFILE, CREATURE_SPTR));
    }


    void ChanceFactory::Make_Coins(const Profile & PROFILE,
                                   item::Coin_t &  coinsMin_OutParam,
                                   item::Coin_t &  coinsMax_OutParam)
    {
        const std::string KEY_STR("heroespath-nonplayer-coins-bounds-" + wealth_type::ToString(PROFILE.wealthType) );
        const std::string VALUE_STR( GameDataFile::Instance()->GetCopyStr(KEY_STR) );

        using StrVec_t = std::vector<std::string>;
        StrVec_t strVec;
        appbase::stringhelp::SplitByChar(VALUE_STR, strVec, ',', true, true);

        M_ASSERT_OR_LOGANDTHROW_SS((strVec.size() == 2), "game::non_player::ownership::ChanceFactory::Make_Coins() looked up \"" << KEY_STR << "\", retrieving \"" << VALUE_STR << "\" which failed to be parsed into 2 comma sep strings.");

        try
        {
            coinsMin_OutParam = boost::lexical_cast<item::Coin_t>(strVec[0]);
        }
        catch (...)
        {
            coinsMin_OutParam = -1;
        }
        M_ASSERT_OR_LOGANDTHROW_SS((coinsMin_OutParam >= 0), "game::non_player::ownership::ChanceFactory::Make_Coins() looked up \"" << KEY_STR << "\", retrieving \"" << VALUE_STR << "\" which failed to parse the first comma sep field into a valid number of coins.  coinsMin=" << coinsMin_OutParam);

        try
        {
            coinsMax_OutParam = boost::lexical_cast<item::Coin_t>(strVec[1]);
        }
        catch (...)
        {
            coinsMax_OutParam = -1;
        }
        M_ASSERT_OR_LOGANDTHROW_SS((coinsMax_OutParam >= coinsMin_OutParam), "game::non_player::ownership::ChanceFactory::Make_Coins() looked up \"" << KEY_STR << "\", retrieving \"" << VALUE_STR << "\" which failed to parse the second comma sep field into a valid (>= the min of " << coinsMin_OutParam << ") number of coins.  coinsMax=" << coinsMax_OutParam);
    }


    const chance::ClothingChances ChanceFactory::Make_ClothingChances(const Profile &                     PROFILE,
                                                                      const non_player::CharacterSPtr_t & CREATURE_SPTR)
    {
        chance::ClothingChances clothingChances( Make_ClothingMaterialChances(PROFILE, CREATURE_SPTR) );

        //capes, cloaks, and robes are mutually exclusive, so pre-select which (if any) are worn
        if (clothingChances.cover_map[item::armor::cover_type::Cloak].IsOwned())
        {
            clothingChances.cover_map[item::armor::cover_type::Cloak].SetCountChanceSingleCertain();
            clothingChances.cover_map[item::armor::cover_type::Cape].SetCountChanceSingleNoChance();
            clothingChances.cover_map[item::armor::cover_type::Robe].SetCountChanceSingleNoChance();
        }
        else
        {
            if (clothingChances.cover_map[item::armor::cover_type::Robe].IsOwned())
            {
                clothingChances.cover_map[item::armor::cover_type::Cloak].SetCountChanceSingleNoChance();
                clothingChances.cover_map[item::armor::cover_type::Cape].SetCountChanceSingleNoChance();
                clothingChances.cover_map[item::armor::cover_type::Robe].SetCountChanceSingleCertain();
            }
            else
            {
                if (clothingChances.cover_map[item::armor::cover_type::Cape].IsOwned())
                {
                    clothingChances.cover_map[item::armor::cover_type::Cloak].SetCountChanceSingleNoChance();
                    clothingChances.cover_map[item::armor::cover_type::Cape].SetCountChanceSingleCertain();
                    clothingChances.cover_map[item::armor::cover_type::Robe].SetCountChanceSingleNoChance();
                }
            }
        }

        //enforce bodytype restrictions
        const creature::BodyType BODY_TYPE(CREATURE_SPTR->Body());
        if ((BODY_TYPE.NumArms() != 2) || (BODY_TYPE.HasWings() == true))
        {
            clothingChances.shirt.SetCountChanceSingleNoChance();
            clothingChances.cover_map[item::armor::cover_type::Cape].SetCountChanceSingleNoChance();
            clothingChances.cover_map[item::armor::cover_type::Cloak].SetCountChanceSingleNoChance();
            clothingChances.cover_map[item::armor::cover_type::Robe].SetCountChanceSingleNoChance();
        }

        if (BODY_TYPE.IsBiped())
            clothingChances.pants.SetCountChanceSingleCertain();//In this game, every 'humaniod' gets pants...Deal with it.
        else
        {
            clothingChances.pants.SetCountChanceSingleNoChance();
            clothingChances.boots.SetCountChanceSingleNoChance();
            clothingChances.shirt.SetCountChanceSingleNoChance();
            clothingChances.cover_map[item::armor::cover_type::Robe].SetCountChanceSingleNoChance();
            clothingChances.cover_map[item::armor::cover_type::Cloak].SetCountChanceSingleNoChance();
            clothingChances.cover_map[item::armor::cover_type::Cape].SetCountChanceSingleNoChance();
        }

        if ((BODY_TYPE.HasFingers() == false) || (BODY_TYPE.HasClaws() == true))
            clothingChances.gloves.SetCountChanceSingleNoChance();

        if (BODY_TYPE.IsBiped() == false)
            clothingChances.pants.SetCountChanceSingleNoChance();

        if (BODY_TYPE.HasFangs() == true)
            clothingChances.cover_map[item::armor::cover_type::Cloak].SetCountChanceSingleNoChance();

        if (BODY_TYPE.IsSerpentine())
            clothingChances = chance::ClothingChances::NoClothes();

        if (BODY_TYPE.NumHeads() != 1)
            clothingChances.cover_map[item::armor::cover_type::Cloak].SetCountChanceSingleNoChance();

        return clothingChances;
    }


    const chance::WeaponChances ChanceFactory::Make_WeaponChances(const Profile &                     PROFILE,
                                                                  const non_player::CharacterSPtr_t & CREATURE_SPTR)
    {
        chance::WeaponChances weaponChances( chance::WeaponChances::NoWeapon() );

        WeaponSetVec_t weaponSetVec;
        LookupPossibleWeaponsByRole(CREATURE_SPTR->Role().Which(), weaponSetVec);

        for(auto const & NEXT_WEAPON_SET : weaponSetVec)
            PopulateWeaponChances(weaponChances, NEXT_WEAPON_SET, PROFILE, CREATURE_SPTR);

        //Add weapons based on body type
        auto const BODY{ CREATURE_SPTR->Body() };
        weaponChances.has_bite = BODY.HasFangs();
        weaponChances.has_claws = BODY.HasClaws();
        weaponChances.has_fists = (BODY.HasArms() && BODY.HasFingers());
        weaponChances.has_tendrils = BODY.HasTendrils();
        weaponChances.has_breath = BODY.HasBreath();

        //enforce body-type/PROFILE.wealthType/PROFILE.wealthType
        // -what restrictions would there be when the weapon sets are based on creature roles in the GameDataFile?
        // -maybe race restrictions?  But those should already be accounted for in race/role combinations that are enforced elsewhere
        //TODO?

        return weaponChances;
    }


    const chance::ArmorChances ChanceFactory::Make_ArmorChances(const Profile &                     PROFILE,
                                                                const non_player::CharacterSPtr_t & CREATURE_SPTR)
    {
        chance::ArmorChances armorChances( chance::ArmorChances::NoArmor() );
        LookupPossibleArmorByRole(PROFILE, CREATURE_SPTR, armorChances);

        //enforce bodytype restrictions
        const creature::BodyType BODY_TYPE(CREATURE_SPTR->Body());
        if (BODY_TYPE.HasArms() == false)
            armorChances.shirt.SetCountChanceSingleNoChance();

        if (BODY_TYPE.HasHead() == false)
            armorChances.helm_map.clear();
        //TODO more body-type restrictions


        //TODO -what else here?

        return armorChances;
    }


    const chance::ItemChancePair_t ChanceFactory::Make_MiscItemChances(const Profile &,
                                                                       const non_player::CharacterSPtr_t &)
    {
        chance::ItemChancePair_t miscItems;

        //TODO

        return miscItems;
    }


    void ChanceFactory::PopulateWeaponChances(chance::WeaponChances &             weaponChances,
                                              const WeaponSet &                   WEAPON_SET,
                                              const Profile &                     PROFILE,
                                              const non_player::CharacterSPtr_t & CREATURE_SPTR)
    {
        //find the total chance of all weapon possibilities combined
        float chanceCombined(0.0f);
        for (auto const & NEXT_WEAPONINFO_CHANCE_PAIR : WEAPON_SET.chanceMap)
            chanceCombined += NEXT_WEAPONINFO_CHANCE_PAIR.second;

        if (misc::IsRealClose(chanceCombined, 0.0f))
            return;

        const float RAND(misc::random::Float(0.0f, chanceCombined));

        //Need to select WEAPON_SET.count weapons in WEAPON_SET.chanceMap, so
        //make a mapping of count to chance that has been selected already or not.
        std::vector<bool> numberSelectedVec;
        numberSelectedVec.resize(WEAPON_SET.count, false);

        //weapon selection loop
        //add chances for each weapon in the WEAPON_SET, if randomly selected]
        float chanceCumulative(0.0f);
        for (auto const & NEXT_WEAPONINFO_CHANCE_PAIR : WEAPON_SET.chanceMap)
        {
            //if all weapons have already been chosen then break from the selection loop
            bool willBreak(true);
            for (auto const NEXT_BOOL : numberSelectedVec)
            {
                if (NEXT_BOOL == false)
                {
                    willBreak = false;
                    break;
                }
            }
            if (willBreak)
                break;

            //At this point there is guaranteed to be at least one false bool in numberSelectedVec

            //check if this iteration (weapon that may be selected) will be skipped
            bool willSkipThisIteration(true);
            std::size_t selectedIndex(0);
            const std::size_t NUM_WEPAONS_TO_SELECT(numberSelectedVec.size());
            for (; selectedIndex<NUM_WEPAONS_TO_SELECT; ++selectedIndex)
            {
                if (false == numberSelectedVec[selectedIndex])
                {
                    if (RAND < (chanceCumulative + NEXT_WEAPONINFO_CHANCE_PAIR.second))
                    {
                        willSkipThisIteration = false;
                        break;
                    }
                }
            }
            if (willSkipThisIteration)
            {
                chanceCumulative += NEXT_WEAPONINFO_CHANCE_PAIR.second;
                continue;
            }

            //At this point numberSelectedVec contians a value of false

            if (NEXT_WEAPONINFO_CHANCE_PAIR.first.is_bite)
                weaponChances.has_bite = true;

            if (NEXT_WEAPONINFO_CHANCE_PAIR.first.is_claws)
                weaponChances.has_claws = true;

            if (NEXT_WEAPONINFO_CHANCE_PAIR.first.is_fists)
                weaponChances.has_fists = true;

            if (NEXT_WEAPONINFO_CHANCE_PAIR.first.is_tendrils)
                weaponChances.has_tendrils = true;

            if (NEXT_WEAPONINFO_CHANCE_PAIR.first.is_breath)
                weaponChances.has_breath = true;

            if (NEXT_WEAPONINFO_CHANCE_PAIR.first.is_knife || NEXT_WEAPONINFO_CHANCE_PAIR.first.is_dagger)
            {
                //no need to check complexity because knives/daggers are simple

                const std::string WEAPON_NAME("Knife");
                weaponChances.knife.SetCountChanceIncrementAndEquip(NEXT_WEAPONINFO_CHANCE_PAIR.second);
                weaponChances.knife.is_dagger = NEXT_WEAPONINFO_CHANCE_PAIR.first.is_dagger;

                if (NEXT_WEAPONINFO_CHANCE_PAIR.first.knife_size == sfml_util::Size::Count)
                    Make_KnifeOrDaggerSizeChances(CREATURE_SPTR->RankClass(), weaponChances.knife.size_map);

                chance::MaterialChanceMap_t typicalKnifePrimaryMaterials;
                if (PROFILE.complexityType == complexity_type::Simple)
                {
                    typicalKnifePrimaryMaterials[item::material::Stone] = 0.75f;
                    typicalKnifePrimaryMaterials[item::material::Bone]  = 0.25f;
                }
                else if (PROFILE.complexityType == complexity_type::Moderate)
                {
                    typicalKnifePrimaryMaterials[item::material::Stone] = 0.05f;
                    typicalKnifePrimaryMaterials[item::material::Bone]  = 0.05f;
                    typicalKnifePrimaryMaterials[item::material::Steel] = 0.70f;
                    typicalKnifePrimaryMaterials[item::material::Iron]  = 0.20f;
                }
                else
                {
                    typicalKnifePrimaryMaterials[item::material::Steel] = 0.9f;
                    typicalKnifePrimaryMaterials[item::material::Iron] = 0.1f;
                }

                PopulateWeaponMaterials(WEAPON_NAME,
                                        typicalKnifePrimaryMaterials,
                                        PROFILE,
                                        CREATURE_SPTR,
                                        weaponChances.knife.mat_map_pri,
                                        weaponChances.knife.mat_map_sec);
            }

            if ((NEXT_WEAPONINFO_CHANCE_PAIR.first.is_staff) || (NEXT_WEAPONINFO_CHANCE_PAIR.first.is_quarterstaff))
            {
                //no need to check complexity because using a staff/quarterstaff is simple
                const std::string WEAPON_NAME((NEXT_WEAPONINFO_CHANCE_PAIR.first.is_quarterstaff) ? "Quarterstaff" : "Staff");

                weaponChances.staff.SetCountChanceIncrementAndEquip(NEXT_WEAPONINFO_CHANCE_PAIR.second);
                weaponChances.staff.is_quarterstaff = NEXT_WEAPONINFO_CHANCE_PAIR.first.is_quarterstaff;

                chance::MaterialChanceMap_t typicalStaffPrimaryMaterials;
                typicalStaffPrimaryMaterials[item::material::Wood] = 1.0f;

                PopulateWeaponMaterials(WEAPON_NAME,
                                        typicalStaffPrimaryMaterials,
                                        PROFILE,
                                        CREATURE_SPTR,
                                        weaponChances.staff.mat_map_pri,
                                        weaponChances.staff.mat_map_sec);
            }

            {
                chance::MaterialChanceMap_t typicalAxePrimaryMaterials;
                typicalAxePrimaryMaterials[item::material::Wood] = 1.0f;

                if (NEXT_WEAPONINFO_CHANCE_PAIR.first.axe != item::weapon::axe_type::Count)
                {
                    SetWeaponChances<item::weapon::axe_type::Enum>(item::weapon::axe_type::ToString(NEXT_WEAPONINFO_CHANCE_PAIR.first.axe),
                                                                   NEXT_WEAPONINFO_CHANCE_PAIR.first.axe,
                                                                   NEXT_WEAPONINFO_CHANCE_PAIR.second,
                                                                   PROFILE,
                                                                   CREATURE_SPTR,
                                                                   typicalAxePrimaryMaterials,
                                                                   weaponChances.axe_map);
                }
                else if (NEXT_WEAPONINFO_CHANCE_PAIR.first.type == item::weapon_type::Axe)
                {
                    const float CHANCE_PER_WEAPON(NEXT_WEAPONINFO_CHANCE_PAIR.second / static_cast<float>(item::weapon::axe_type::Count));
                    for (int i(0); i < item::weapon::axe_type::Count; ++i)
                    {
                        const item::weapon::axe_type::Enum TYPE(static_cast<item::weapon::axe_type::Enum>(i));
                        SetWeaponChances<item::weapon::axe_type::Enum>(item::weapon::axe_type::ToString(TYPE),
                                                                        TYPE,
                                                                        CHANCE_PER_WEAPON,
                                                                        PROFILE,
                                                                        CREATURE_SPTR,
                                                                        typicalAxePrimaryMaterials,
                                                                        weaponChances.axe_map);
                    }
                }
            }

            {
                chance::MaterialChanceMap_t typicalBladedStaffPrimaryMaterials;
                typicalBladedStaffPrimaryMaterials[item::material::Wood] = 1.0f;

                if (NEXT_WEAPONINFO_CHANCE_PAIR.first.bladedstaff != item::weapon::bladedstaff_type::Count)
                {
                    SetWeaponChances<item::weapon::bladedstaff_type::Enum>(item::weapon::bladedstaff_type::ToString(NEXT_WEAPONINFO_CHANCE_PAIR.first.bladedstaff),
                                                                           NEXT_WEAPONINFO_CHANCE_PAIR.first.bladedstaff,
                                                                           NEXT_WEAPONINFO_CHANCE_PAIR.second,
                                                                           PROFILE,
                                                                           CREATURE_SPTR,
                                                                           typicalBladedStaffPrimaryMaterials,
                                                                           weaponChances.bladedstaff_map);
                }
                else if (NEXT_WEAPONINFO_CHANCE_PAIR.first.type == item::weapon_type::BladedStaff)
                {
                    const float CHANCE_PER_WEAPON(NEXT_WEAPONINFO_CHANCE_PAIR.second / static_cast<float>(item::weapon::bladedstaff_type::Count));
                    for (int i(0); i < item::weapon::bladedstaff_type::Count; ++i)
                    {
                        const item::weapon::bladedstaff_type::Enum TYPE(static_cast<item::weapon::bladedstaff_type::Enum>(i));
                        SetWeaponChances<item::weapon::bladedstaff_type::Enum>(item::weapon::bladedstaff_type::ToString(TYPE),
                                                                               TYPE,
                                                                               CHANCE_PER_WEAPON,
                                                                               PROFILE,
                                                                               CREATURE_SPTR,
                                                                               typicalBladedStaffPrimaryMaterials,
                                                                               weaponChances.bladedstaff_map);
                    }
                }
            }

            {
                chance::MaterialChanceMap_t typicalClubPrimaryMaterials;
                typicalClubPrimaryMaterials[item::material::Wood] = 1.0f;

                if (NEXT_WEAPONINFO_CHANCE_PAIR.first.club != item::weapon::club_type::Count)
                {
                    SetWeaponChances<item::weapon::club_type::Enum>(item::weapon::club_type::ToString(NEXT_WEAPONINFO_CHANCE_PAIR.first.club),
                                                                    NEXT_WEAPONINFO_CHANCE_PAIR.first.club,
                                                                    NEXT_WEAPONINFO_CHANCE_PAIR.second,
                                                                    PROFILE,
                                                                    CREATURE_SPTR,
                                                                    typicalClubPrimaryMaterials,
                                                                    weaponChances.club_map);
                }
                else if (NEXT_WEAPONINFO_CHANCE_PAIR.first.type == item::weapon_type::Club)
                {
                    const float CHANCE_PER_WEAPON(NEXT_WEAPONINFO_CHANCE_PAIR.second / static_cast<float>(item::weapon::club_type::Count));
                    for (int i(0); i < item::weapon::club_type::Count; ++i)
                    {
                        const item::weapon::club_type::Enum TYPE(static_cast<item::weapon::club_type::Enum>(i));
                        SetWeaponChances<item::weapon::club_type::Enum>(item::weapon::club_type::ToString(TYPE),
                                                                        TYPE,
                                                                        CHANCE_PER_WEAPON,
                                                                        PROFILE,
                                                                        CREATURE_SPTR,
                                                                        typicalClubPrimaryMaterials,
                                                                        weaponChances.club_map);
                    }
                }
            }

            {
                chance::MaterialChanceMap_t typicalProjectilePrimaryMaterials;
                typicalProjectilePrimaryMaterials[item::material::Wood] = 1.0f;

                if (NEXT_WEAPONINFO_CHANCE_PAIR.first.projectile != item::weapon::projectile_type::Count)
                {
                    SetWeaponChances<item::weapon::projectile_type::Enum>(item::weapon::projectile_type::ToString(NEXT_WEAPONINFO_CHANCE_PAIR.first.projectile),
                                                                          NEXT_WEAPONINFO_CHANCE_PAIR.first.projectile,
                                                                          NEXT_WEAPONINFO_CHANCE_PAIR.second,
                                                                          PROFILE,
                                                                          CREATURE_SPTR,
                                                                          typicalProjectilePrimaryMaterials,
                                                                          weaponChances.projectile_map);
                }
                else if (NEXT_WEAPONINFO_CHANCE_PAIR.first.type == item::weapon_type::Projectile)
                {
                    const float CHANCE_PER_WEAPON(NEXT_WEAPONINFO_CHANCE_PAIR.second / static_cast<float>(item::weapon::projectile_type::Count));
                    for (int i(0); i < item::weapon::projectile_type::Count; ++i)
                    {
                        const item::weapon::projectile_type::Enum TYPE(static_cast<item::weapon::projectile_type::Enum>(i));
                        SetWeaponChances<item::weapon::projectile_type::Enum>(item::weapon::projectile_type::ToString(TYPE),
                                                                              TYPE,
                                                                              CHANCE_PER_WEAPON,
                                                                              PROFILE,
                                                                              CREATURE_SPTR,
                                                                              typicalProjectilePrimaryMaterials,
                                                                              weaponChances.projectile_map);
                    }
                }
            }

            {
                chance::MaterialChanceMap_t typicalSwordPrimaryMaterials;
                typicalSwordPrimaryMaterials[item::material::Steel] = 1.0f;

                if (NEXT_WEAPONINFO_CHANCE_PAIR.first.sword != item::weapon::sword_type::Count)
                {
                    SetWeaponChances<item::weapon::sword_type::Enum>(item::weapon::sword_type::ToString(NEXT_WEAPONINFO_CHANCE_PAIR.first.sword),
                                                                     NEXT_WEAPONINFO_CHANCE_PAIR.first.sword,
                                                                     NEXT_WEAPONINFO_CHANCE_PAIR.second,
                                                                     PROFILE,
                                                                     CREATURE_SPTR,
                                                                     typicalSwordPrimaryMaterials,
                                                                     weaponChances.sword_map);
                }
                else if (NEXT_WEAPONINFO_CHANCE_PAIR.first.type == item::weapon_type::Sword)
                {
                    const float CHANCE_PER_WEAPON(NEXT_WEAPONINFO_CHANCE_PAIR.second / static_cast<float>(item::weapon::sword_type::Count));
                    for (int i(0); i < item::weapon::sword_type::Count; ++i)
                    {
                        const item::weapon::sword_type::Enum TYPE(static_cast<item::weapon::sword_type::Enum>(i));
                        SetWeaponChances<item::weapon::sword_type::Enum>(item::weapon::sword_type::ToString(TYPE),
                                                                         TYPE,
                                                                         CHANCE_PER_WEAPON,
                                                                         PROFILE,
                                                                         CREATURE_SPTR,
                                                                         typicalSwordPrimaryMaterials,
                                                                         weaponChances.sword_map);
                    }
                }
            }

            {
                chance::MaterialChanceMap_t typicalWhipPrimaryMaterials;
                typicalWhipPrimaryMaterials[item::material::Steel] = 1.0f;

                if (NEXT_WEAPONINFO_CHANCE_PAIR.first.whip != item::weapon::whip_type::Count)
                {
                    SetWeaponChances<item::weapon::whip_type::Enum>(item::weapon::whip_type::ToString(NEXT_WEAPONINFO_CHANCE_PAIR.first.whip),
                                                                    NEXT_WEAPONINFO_CHANCE_PAIR.first.whip,
                                                                    NEXT_WEAPONINFO_CHANCE_PAIR.second,
                                                                    PROFILE,
                                                                    CREATURE_SPTR,
                                                                    typicalWhipPrimaryMaterials,
                                                                    weaponChances.whip_map);
                }
                else if (NEXT_WEAPONINFO_CHANCE_PAIR.first.type == item::weapon_type::Whip)
                {
                    const float CHANCE_PER_WEAPON(NEXT_WEAPONINFO_CHANCE_PAIR.second / static_cast<float>(item::weapon::whip_type::Count));
                    for (int i(0); i < item::weapon::whip_type::Count; ++i)
                    {
                        const item::weapon::whip_type::Enum TYPE(static_cast<item::weapon::whip_type::Enum>(i));
                        SetWeaponChances<item::weapon::whip_type::Enum>(item::weapon::whip_type::ToString(TYPE),
                                                                        TYPE,
                                                                        CHANCE_PER_WEAPON,
                                                                        PROFILE,
                                                                        CREATURE_SPTR,
                                                                        typicalWhipPrimaryMaterials,
                                                                        weaponChances.whip_map);
                    }
                }
            }

            chanceCumulative += NEXT_WEAPONINFO_CHANCE_PAIR.second;
            numberSelectedVec[selectedIndex] = true;
        }
    }


    void ChanceFactory::LookupPossibleArmorByRole(const Profile &                     PROFILE,
                                                  const non_player::CharacterSPtr_t & CREATURE_SPTR,
                                                  chance::ArmorChances &              armorChances)
    {
        using namespace boost::algorithm;

        using StrVec_t = std::vector<std::string>;

        const std::string ROLE_STR(creature::role::ToString(CREATURE_SPTR->Role().Which()));
        const std::string KEY_STR("heroespath-nonplayer-armor-chances-role-" + ROLE_STR);
        const std::string VALUE_STR(GameDataFile::Instance()->GetCopyStr(KEY_STR));

        StrVec_t armorChancesVec;
        appbase::stringhelp::SplitByChar(VALUE_STR, armorChancesVec, '|', true, true);

        //loop over each possible piece of armor
        for (auto const & NEXT_ARMOR_CHANCE_STR : armorChancesVec)
        {
            StrVec_t piecesVec;
            appbase::stringhelp::SplitByChar(NEXT_ARMOR_CHANCE_STR, piecesVec, ',', true, true);

            M_ASSERT_OR_LOGANDTHROW_SS((piecesVec.size() >= 2), "game::non_player::ownership::ChanceFactory::LookupPossibleArmorByRole(role=\"" << creature::role::ToString(CREATURE_SPTR->Role().Which()) << "\") found value-str=\"" << VALUE_STR << "\" which failed to be parsed into the required 2  or more comma sep fields.");

            const std::string ARMOR_NAME_STR(piecesVec[0]);

            const std::string ARMOR_CHANCE_STR(piecesVec[piecesVec.size()-1]);
            float armorChanceVal(-1.0f);
            try
            {
                armorChanceVal = boost::lexical_cast<float>(ARMOR_CHANCE_STR);
            }
            catch (...)
            {
                armorChanceVal = -1.0f;
            }
            M_ASSERT_OR_LOGANDTHROW_SS((misc::IsRealClose(-1.0f, armorChanceVal) == false), "game::non_player::ownership::ChanceFactory::LookupPossibleArmorByRole(role=\"" << creature::role::ToString(CREATURE_SPTR->Role().Which()) << "\") found value-str=\"" << VALUE_STR << "\" which had float str=\"" << ARMOR_CHANCE_STR << "\" which was unable to be parsed into a valid (!= -1) float.  (" << armorChanceVal << ")");

            using namespace item::armor;

            const bool HAS_TYPE_STR(piecesVec.size() > 2);
            const std::string ARMOR_TYPE_STR(piecesVec[1]);
            const base_type::Enum ARMOR_TYPE( base_type::FromString(ARMOR_TYPE_STR) );
            if (HAS_TYPE_STR)
            {
                M_ASSERT_OR_LOGANDTHROW_SS((ARMOR_TYPE != base_type::Count), "game::non_player::ownership::ChanceFactor::LookupPossibleArmorByRole(role=\"" << creature::role::ToString(CREATURE_SPTR->Role().Which()) << "\") found value-str=\"" << VALUE_STR << "\" which had more than two comma sep fields, but the second field failed to be parsed as a valid item::armor::base_type::Enum.");
                M_ASSERT_OR_LOGANDTHROW_SS((ARMOR_TYPE != base_type::Plain), "game::non_player::ownership::ChanceFactor::LookupPossibleArmorByRole(role=\"" << creature::role::ToString(CREATURE_SPTR->Role().Which()) << "\") found value-str=\"" << VALUE_STR << "\" which had more than two comma sep fields, but the second field failed to be parsed as a valid non-Plain item::armor::base_type::Enum.");
            }

            const std::string ARMOR_NAME_COMPLETE_STR(((HAS_TYPE_STR) ? ARMOR_TYPE_STR : "") + ARMOR_NAME_STR);

            if (ARMOR_NAME_STR == "Shirt")
                SetArmorChancesGeneral(armorChances.shirt,
                                       ARMOR_NAME_COMPLETE_STR,
                                       ARMOR_TYPE,
                                       armorChanceVal,
                                       PROFILE,
                                       CREATURE_SPTR,
                                       false);

            if (ARMOR_NAME_STR == "Pants")
                SetArmorChancesGeneral(armorChances.pants,
                                       ARMOR_NAME_COMPLETE_STR,
                                       ARMOR_TYPE,
                                       armorChanceVal,
                                       PROFILE,
                                       CREATURE_SPTR,
                                       false);

            if (ARMOR_NAME_STR == "Boots")
                SetArmorChancesGeneral(armorChances.boots,
                                       ARMOR_NAME_COMPLETE_STR,
                                       ARMOR_TYPE,
                                       armorChanceVal,
                                       PROFILE,
                                       CREATURE_SPTR,
                                       false);

            if (ARMOR_NAME_STR == "Gauntlets")
                SetArmorChancesGeneral(armorChances.gauntlets,
                                       ARMOR_NAME_COMPLETE_STR,
                                       ARMOR_TYPE,
                                       armorChanceVal,
                                       PROFILE,
                                       CREATURE_SPTR,
                                       false);

            if (ARMOR_NAME_STR == "Aventail")
                SetArmorChancesGeneral(armorChances.aventail,
                                       ARMOR_NAME_COMPLETE_STR,
                                       ARMOR_TYPE,
                                       armorChanceVal,
                                       PROFILE,
                                       CREATURE_SPTR,
                                       false);

            //shields
            if (ARMOR_NAME_STR == "Buckler")
                SetArmorChancesSpecific(armorChances.shield_map[item::armor::shield_type::Buckler],
                                        ARMOR_NAME_COMPLETE_STR,
                                        armorChanceVal,
                                        PROFILE,
                                        CREATURE_SPTR,
                                        true);

            if (ARMOR_NAME_STR == "Kite")
                SetArmorChancesSpecific(armorChances.shield_map[item::armor::shield_type::Kite],
                                        ARMOR_NAME_COMPLETE_STR,
                                        armorChanceVal,
                                        PROFILE,
                                        CREATURE_SPTR,
                                        true);

            if (ARMOR_NAME_STR == "Heater")
                SetArmorChancesSpecific(armorChances.shield_map[item::armor::shield_type::Heater],
                                        ARMOR_NAME_COMPLETE_STR,
                                        armorChanceVal,
                                        PROFILE,
                                        CREATURE_SPTR,
                                        true);

            if (ARMOR_NAME_STR == "Pavis")
                SetArmorChancesSpecific(armorChances.shield_map[item::armor::shield_type::Pavis],
                                        ARMOR_NAME_COMPLETE_STR,
                                        armorChanceVal,
                                        PROFILE,
                                        CREATURE_SPTR,
                                        true);

            //helms
            if (ARMOR_NAME_STR == "Leather")
                SetArmorChancesSpecific(armorChances.helm_map[item::armor::helm_type::Leather],
                                        ARMOR_NAME_COMPLETE_STR,
                                        armorChanceVal,
                                        PROFILE,
                                        CREATURE_SPTR,
                                        false);//materials will be forced to make sense by ArmorFactory::Make_Helm()

            if (ARMOR_NAME_STR == "MailCoif")
                SetArmorChancesSpecific(armorChances.helm_map[item::armor::helm_type::MailCoif],
                                        ARMOR_NAME_COMPLETE_STR,
                                        armorChanceVal,
                                        PROFILE,
                                        CREATURE_SPTR,
                                        true);

            if (ARMOR_NAME_STR == "Kettle")
                SetArmorChancesSpecific(armorChances.helm_map[item::armor::helm_type::Kettle],
                                        ARMOR_NAME_COMPLETE_STR,
                                        armorChanceVal,
                                        PROFILE,
                                        CREATURE_SPTR,
                                        true);

            if (ARMOR_NAME_STR == "Archers")
                SetArmorChancesSpecific(armorChances.helm_map[item::armor::helm_type::Archers],
                                        ARMOR_NAME_COMPLETE_STR,
                                        armorChanceVal,
                                        PROFILE,
                                        CREATURE_SPTR,
                                        true);

            if (ARMOR_NAME_STR == "Bascinet")
                SetArmorChancesSpecific(armorChances.helm_map[item::armor::helm_type::Bascinet],
                                        ARMOR_NAME_COMPLETE_STR,
                                        armorChanceVal,
                                        PROFILE,
                                        CREATURE_SPTR,
                                        true);

            if (ARMOR_NAME_STR == "Great")
                SetArmorChancesSpecific(armorChances.helm_map[item::armor::helm_type::Great],
                                        ARMOR_NAME_COMPLETE_STR,
                                        armorChanceVal,
                                        PROFILE,
                                        CREATURE_SPTR,
                                        true);
        }
    }


    void ChanceFactory::LookupPossibleWeaponsByRole(const creature::role::Enum ROLE, WeaponSetVec_t & weaponSetVec_OutParam)
    {
        using namespace boost::algorithm;

        using StrVec_t = std::vector<std::string>;

        const std::string ROLE_STR(creature::role::ToString(ROLE));
        const std::string KEY_STR("heroespath-nonplayer-weapon-chances-role-" + ROLE_STR);
        const std::string VALUE_STR_LOWER(to_lower_copy(GameDataFile::Instance()->GetCopyStr(KEY_STR)) );

        StrVec_t weaponSetVec;
        appbase::stringhelp::SplitByChars(VALUE_STR_LOWER, weaponSetVec, "{}", true, true);

        //Loop over each weapon set, denoted by curly brackets {} in the GameDataFile.
        for (auto const & NEXT_WEAPON_SET_STR : weaponSetVec)
        {
            WeaponSet nextWeaponSet;

            StrVec_t instructionsVec;
            appbase::stringhelp::SplitByChar(NEXT_WEAPON_SET_STR, instructionsVec, '|', true, true);

            //Loop over each instruction, denoted by the pipe character in the GameDataFile.
            //A collection of instructions composes a WeaponSet
            for (auto const & NEXT_INSTRUCTION_STR : instructionsVec)
            {
                if (boost::algorithm::starts_with(NEXT_INSTRUCTION_STR, "["))
                {
                    if (boost::algorithm::starts_with(NEXT_INSTRUCTION_STR, "[pick-"))
                    {
                        //assume the instruction takes the form: [pick-<number>], where <number> must be a positive integer greater than zero
                        const std::string COUNT_STR( erase_first_copy(erase_first_copy(NEXT_INSTRUCTION_STR,"[pick-"), "]") );

                        try
                        {
                            nextWeaponSet.count = boost::lexical_cast<std::size_t>(COUNT_STR);
                        }
                        catch(...)
                        {
                            nextWeaponSet.count = 0;
                        }

                        M_ASSERT_OR_LOGANDTHROW_SS((nextWeaponSet.count > 0), "game::non_player::ownership::LookupPossibleWeaponsByRole(role=" << ROLE_STR << ") with KEY=\"" << KEY_STR << "\" and VALUE=\"" << VALUE_STR_LOWER << "\" found INSTRUCTION=\"" << NEXT_INSTRUCTION_STR << "\" and parsed pick count string to be \"" << COUNT_STR << "\", but failed to convert it into a valid (> zero) std::size_t.  It was converted into " << nextWeaponSet.count << ".");
                    }
                    else if (NEXT_INSTRUCTION_STR == "[body]")
                    {
                        nextWeaponSet.chanceMap.clear();
                    }
                    else
                    {
                        M_HP_LOG("game::non_player::ownership::LookupPossibleWeaponsByRole(role=" << ROLE_STR << ") with KEY=\"" << KEY_STR << "\" and VALUE=\"" << VALUE_STR_LOWER << "\" found INSTRUCTION=\"" << NEXT_INSTRUCTION_STR << "\" which was not recognized, and will be ignored.");
                    }
                }
                else
                {
                    //At this point, assume the instruction is in the form of:  <weapon name>,<chance float> or (<weapon name>),<chance float> -so it must have two comma separated strings.
                    StrVec_t partsVec;
                    appbase::stringhelp::SplitByChar(NEXT_INSTRUCTION_STR, partsVec, ',', true, true);

                    M_ASSERT_OR_LOGANDTHROW_SS(((partsVec.size() > 1) && (partsVec.at(0).size() > 2) && (partsVec.at(1).size() > 2)), "game::non_player::ownership::LookupPossibleWeaponsByRole(role=" << ROLE_STR << ") with KEY=\"" << KEY_STR << "\" and VALUE=\"" << VALUE_STR_LOWER << "\" found INSTRUCTION=\"" << NEXT_INSTRUCTION_STR << "\" but was unable to parse as at least two strings separated by a comma.");

                    const std::string WEAPON_NAME(partsVec[0]);
                    const std::string CHANCE_STR(partsVec[1]);

                    //parse weapon name and type
                    const item::weapon::WeaponInfo WEAPON_INFO(item::weapon_type::NotAWeapon, WEAPON_NAME);

                    //parse weapon chance
                    float chance(0.0f);
                    try
                    {
                        chance = boost::lexical_cast<float>(CHANCE_STR);
                    }
                    catch (...)
                    {
                        chance = -1.0f;
                    }

                    M_ASSERT_OR_LOGANDTHROW_SS((chance > 0.0f), "game::non_player::ownership::LookupPossibleWeaponsByRole(role=" << ROLE_STR << ") with KEY=\"" << KEY_STR << "\" and VALUE=\"" << VALUE_STR_LOWER << "\" found INSTRUCTION=\"" << NEXT_INSTRUCTION_STR << "\", weapon name=\"" << WEAPON_NAME << "\", and chance str=\"" << CHANCE_STR << "\", but was unable to convert this into a valid (> zero) float.  Converted instead to:" << chance << ".");

                    nextWeaponSet.chanceMap[WEAPON_INFO] = chance;
                }
            }

            weaponSetVec_OutParam.push_back(nextWeaponSet);
        }
    }


    float ChanceFactory::GetFloatFromGameDataFile(const std::string & KEY, const float MIN, const float MAX)
    {
        const std::string MIN_STR("min");
        const std::string MAX_STR("max");
        const std::string REMAINING_STR("remaining");

        const std::string VALUE_STR( GameDataFile::Instance()->GetCopyStr(KEY) );

        if (VALUE_STR == MIN_STR)
        {
            return MIN;
        }
        else
        {
            if (VALUE_STR == MAX_STR)
            {
                return MAX;
            }
            else
            {
                if (VALUE_STR == REMAINING_STR)
                {
                    return -1.0f;
                }
                else
                {
                    std::string exceptionWhatStr("");
                    try
                    {
                        return boost::lexical_cast<float>(VALUE_STR);
                    }
                    catch (const std::exception & EX)
                    {
                        exceptionWhatStr = EX.what();
                    }

                    M_ASSERT_OR_LOGANDTHROW_SS((exceptionWhatStr.empty()), "game::non-player::ownership::ChanceFactory::GetFloatFromGameDataFile(key=\"" << KEY << "\"), found value \"" << VALUE_STR << "\" that failed to be parsed as a float, throwing exception: \"" << exceptionWhatStr << "\".");
                    return -1.0f;
                }
            }
        }
    }


    void ChanceFactory::LookupClothingMaterialChances(const Profile &                     PROFILE,
                                                      const non_player::CharacterSPtr_t & CREATURE_SPTR,
                                                      float &                             clothChance,
                                                      float &                             softleatherChance,
                                                      float &                             hardleatherChance)
    {
        std::ostringstream ss;
        ss << "heroespath-inventory-clothing-" << wealth_type::ToString(PROFILE.wealthType) << "-chances";
        const std::string WEARABLE_STR_BASE(ss.str());

        //find the chance-subtotal and the material that will have the remaining chance
        float chanceSubTotal(0.0f);
        item::material::Enum materialWithChanceRemaining(item::material::Nothing);
        const float CHANCE_MIN(GameDataFile::Instance()->GetCopyFloat("heroespath-inventory-clothing-chance-min"));
        const float CHANCE_MAX(GameDataFile::Instance()->GetCopyFloat("heroespath-inventory-clothing-chance-max"));

        clothChance = GetFloatFromGameDataFile(WEARABLE_STR_BASE + "-wearable-cloth", CHANCE_MIN, CHANCE_MAX);
        if (clothChance < 0.0f)
            materialWithChanceRemaining = item::material::Cloth;
        else
            chanceSubTotal += clothChance;

        softleatherChance = GetFloatFromGameDataFile(WEARABLE_STR_BASE + "-wearable-soft-leather", CHANCE_MIN, CHANCE_MAX);
        if (softleatherChance < 0.0f)
            materialWithChanceRemaining = item::material::SoftLeather;
        else
            chanceSubTotal += softleatherChance;

        hardleatherChance = GetFloatFromGameDataFile(WEARABLE_STR_BASE + "-wearable-hard-leather", CHANCE_MIN, CHANCE_MAX);
        if (hardleatherChance < 0.0f)
            materialWithChanceRemaining = item::material::HardLeather;
        else
            chanceSubTotal += hardleatherChance;

        //set the chance for the material with the remaining chance
        if (item::material::Cloth == materialWithChanceRemaining)
            clothChance = 1.0f - chanceSubTotal;
        else
        {
            if (item::material::SoftLeather == materialWithChanceRemaining)
                softleatherChance = 1.0f - chanceSubTotal;
            else
                hardleatherChance = 1.0f - chanceSubTotal;
        }

        //adjust chances by rank, where higher rank means a higher chance of more valuable materials
        const float RANK_RATIO(static_cast<float>(CREATURE_SPTR->Rank()) / GameDataFile::Instance()->GetCopyFloat("heroespath-rankclass-Master-rankmax"));
        hardleatherChance += RANK_RATIO;
        softleatherChance += RANK_RATIO * 0.5f;
        clothChance       -= RANK_RATIO;

        //enfore the min/max after all adjustments
        ChanceFactory::ForceMinMax(clothChance, CHANCE_MIN, CHANCE_MAX);
        ChanceFactory::ForceMinMax(softleatherChance, CHANCE_MIN, CHANCE_MAX);
        ChanceFactory::ForceMinMax(hardleatherChance, CHANCE_MIN, CHANCE_MAX);
    }


    void ChanceFactory::Make_ClothingMaterialChancesPrimary(const Profile &                     PROFILE,
                                                            const non_player::CharacterSPtr_t & CREATURE_SPTR,
                                                            chance::ItemChances &               itemChancesBase)
    {
        //set the chance base object with data common to all clothing chances (shared between boots, cloaks, capes, etc.)

        //all clothing items are equipped
        itemChancesBase.chance_equipped = 1.0f;

        //determine the final chances for each possible primary material type, which for clothing are restricted to cloth, soft leather, and hard leather
        float clothChance(0.0f);
        float softleatherChance(0.0f);
        float hardleatherChance(0.0f);
        //
        LookupClothingMaterialChances(PROFILE,
                                      CREATURE_SPTR,
                                      clothChance,
                                      softleatherChance,
                                      hardleatherChance);
        //
        itemChancesBase.mat_map_pri[item::material::Cloth] = clothChance;
        itemChancesBase.mat_map_pri[item::material::SoftLeather] = softleatherChance;
        itemChancesBase.mat_map_pri[item::material::HardLeather] = hardleatherChance;
    }


    void ChanceFactory::Make_MaterialChancesPrimary(const Profile &                     PROFILE,
                                                    const non_player::CharacterSPtr_t & CREATURE_SPTR,
                                                    const chance::MaterialChanceMap_t & MATERIALS_TYPICAL,
                                                    const item::Weight_t                ITEM_WEIGHT,
                                                    chance::MaterialChanceMap_t &       materialsMap_OutParam)
    {
        //establish the base chances for a special primary material
        float chanceCool    (1.0f / GameDataFile::Instance()->GetCopyFloat("heroespath-material-primary-chance-base-Cool-onein"));
        float chanceMetal   (1.0f / GameDataFile::Instance()->GetCopyFloat("heroespath-material-primary-chance-base-Metal-onein"));
        float chancePrecious(1.0f / GameDataFile::Instance()->GetCopyFloat("heroespath-material-primary-chance-base-Precious-onein"));
        //
        chance::MaterialChanceMap_t chanceMapCool(Make_MaterialChanceMapCool());
        chance::MaterialChanceMap_t chanceMapMetal(Make_MaterialChanceMapMetal());
        chance::MaterialChanceMap_t chanceMapPrecious(Make_MaterialChanceMapPrecious());

        //adjustments that make higher ranks more likely to have special materials
        const float RANK_DIVISOR(static_cast<float>(GameDataFile::Instance()->GetCopyFloat("heroespath-rankclass-Master-rankmax")));
        const float RANK_CHANCE_INCREASE(static_cast<float>(CREATURE_SPTR->Rank()) / RANK_DIVISOR);
        chanceCool += RANK_CHANCE_INCREASE;
        chanceMetal += RANK_CHANCE_INCREASE;
        chancePrecious += RANK_CHANCE_INCREASE;

        //adjustments that make higher ranks have special materials that are more rare
        for (auto & nextMapPair : chanceMapCool)
            nextMapPair.second /= RANK_CHANCE_INCREASE;
        //
        for (auto & nextMapPair : chanceMapMetal)
            nextMapPair.second /= RANK_CHANCE_INCREASE;
        //
        for (auto & nextMapPair : chanceMapPrecious)
            nextMapPair.second /= RANK_CHANCE_INCREASE;

        //adjustments that make more wealth equal better chances for special materials
        const float WEALTH_CHANCE_ADJUSTMENT(GameDataFile::Instance()->GetCopyFloat("heroespath-material-wealth-chance-base-adjustment-" + wealth_type::ToString(PROFILE.wealthType)));
        chanceCool += WEALTH_CHANCE_ADJUSTMENT;
        chanceMetal += WEALTH_CHANCE_ADJUSTMENT;
        chancePrecious += WEALTH_CHANCE_ADJUSTMENT;

        //adjustments that make more wealth equal already acquired special materials more rare
        for (auto & nextMapPair : chanceMapCool)
            nextMapPair.second += WEALTH_CHANCE_ADJUSTMENT;
        //
        for (auto & nextMapPair : chanceMapMetal)
            nextMapPair.second += WEALTH_CHANCE_ADJUSTMENT;
        //
        for (auto & nextMapPair : chanceMapPrecious)
            nextMapPair.second += WEALTH_CHANCE_ADJUSTMENT;

        //collector adjustments
        if ((PROFILE.collectorType & collector_type::Collector) > 0)
        {
            //adjustments that make collectors more likely to have special materials
            chanceCool += GameDataFile::Instance()->GetCopyFloat("heroespath-material-collector-chance-base-increase-Cool");
            chanceMetal += GameDataFile::Instance()->GetCopyFloat("heroespath-material-collector-chance-base-increase-Metal");
            chancePrecious += GameDataFile::Instance()->GetCopyFloat("heroespath-material-collector-chance-base-increase-Precious");

            //adjustments that make collector's already acquired special materials more rare
            const float CHANCE_DIV_COOL(GameDataFile::Instance()->GetCopyFloat("heroespath-material-collector-chance-per-divisor-Cool"));
            for (auto & nextMapPair : chanceMapCool)
                nextMapPair.second /= CHANCE_DIV_COOL;
            //
            const float CHANCE_DIV_METAL(GameDataFile::Instance()->GetCopyFloat("heroespath-material-collector-chance-per-divisor-Metal"));
            for (auto & nextMapPair : chanceMapMetal)
                nextMapPair.second /= CHANCE_DIV_METAL;
            //
            const float CHANCE_DIV_PRECIOUS(GameDataFile::Instance()->GetCopyFloat("heroespath-material-collector-chance-per-divisor-Precious"));
            for (auto & nextMapPair : chanceMapPrecious)
                nextMapPair.second /= CHANCE_DIV_PRECIOUS;
        }

        //adjust for item weight that can make special materials less likely
        const float WEIGHT_CHANCE_ADJUSTMENT(static_cast<float>(ITEM_WEIGHT) / 2500.0f);
        chanceCool -= WEIGHT_CHANCE_ADJUSTMENT;
        chanceMetal -= WEIGHT_CHANCE_ADJUSTMENT;
        chancePrecious -= WEIGHT_CHANCE_ADJUSTMENT;

        //enforce minimum chances
        ForceMin(chanceCool);
        ForceMin(chanceMetal);
        ForceMin(chancePrecious);

        //adjust material chances based on creture PROFILE.complexity
        RestrictMaterialsByComplexity(PROFILE.complexityType,
                                      chanceCool,
                                      chanceMapCool,
                                      chanceMetal,
                                      chanceMapMetal,
                                      chancePrecious,
                                      chanceMapPrecious);

        //final determination of which material will be secondary
        const float SUBTOTAL(chanceCool + chanceMetal + chancePrecious);
        const float RAND(misc::random::Float(0.0f, std::max(1.0f, SUBTOTAL)));

        if ((SUBTOTAL < 1.0f) && (RAND < (1.0f - SUBTOTAL)))
        {
            materialsMap_OutParam = MATERIALS_TYPICAL;
        }
        else
        {
            if (RAND < chanceCool)
            {
                materialsMap_OutParam = chanceMapCool;
            }
            else
            {
                if (RAND < chanceMetal)
                    materialsMap_OutParam = chanceMapMetal;
                else
                    materialsMap_OutParam = chanceMapPrecious;
            }
        }
        M_ASSERT_OR_LOGANDTHROW_SS((materialsMap_OutParam.empty() == false), "game::non_player::ownership::ChanceFactory::Make_MaterialChancesPrimary(\"" << CREATURE_SPTR->Name() << "\") final materials map was empty.");
    }


    void ChanceFactory::Make_MaterialChancesSecondary(const Profile &                     PROFILE,
                                                      const non_player::CharacterSPtr_t & CREATURE_SPTR,
                                                      chance::MaterialChanceMap_t &       materialsMap_OutParam)
    {
        //establish the base chances for a secondary material
        float chanceCool(    1.0f / GameDataFile::Instance()->GetCopyFloat("heroespath-material-secondary-chance-base-Cool-onein") );
        float chanceMetal(   1.0f / GameDataFile::Instance()->GetCopyFloat("heroespath-material-secondary-chance-base-Metal-onein"));
        float chancePrecious(1.0f / GameDataFile::Instance()->GetCopyFloat("heroespath-material-secondary-chance-base-Precious-onein"));
        //
        chance::MaterialChanceMap_t chanceMapCool( Make_MaterialChanceMapCool() );
        chance::MaterialChanceMap_t chanceMapMetal( Make_MaterialChanceMapMetal() );
        chance::MaterialChanceMap_t chanceMapPrecious( Make_MaterialChanceMapPrecious() );

        //adjustments that make higher ranks more likely to have special materials
        const float RANK_DIVISOR( static_cast<float>(GameDataFile::Instance()->GetCopyFloat("heroespath-rankclass-Master-rankmax")) );
        const float RANK_CHANCE_INCREASE(static_cast<float>(CREATURE_SPTR->Rank()) / RANK_DIVISOR);
        chanceCool += RANK_CHANCE_INCREASE;
        chanceMetal += RANK_CHANCE_INCREASE;
        chancePrecious += RANK_CHANCE_INCREASE;

        //adjustments that make higher ranks already acquired special materials more rare
        for (auto & nextMapPair : chanceMapCool)
            nextMapPair.second /= RANK_CHANCE_INCREASE;
        //
        for (auto & nextMapPair : chanceMapMetal)
            nextMapPair.second /= RANK_CHANCE_INCREASE;
        //
        for (auto & nextMapPair : chanceMapPrecious)
            nextMapPair.second /= RANK_CHANCE_INCREASE;

        //adjustments that make more wealth equal better chances for special materials
        const float WEALTH_CHANCE_ADJUSTMENT( GameDataFile::Instance()->GetCopyFloat("heroespath-material-wealth-chance-base-adjustment-" + wealth_type::ToString(PROFILE.wealthType)) );
        chanceCool += WEALTH_CHANCE_ADJUSTMENT;
        chanceMetal += WEALTH_CHANCE_ADJUSTMENT;
        chancePrecious += WEALTH_CHANCE_ADJUSTMENT;

        //adjustments that make more wealth equal already acquired special materials more rare
        for (auto & nextMapPair : chanceMapCool)
            nextMapPair.second += WEALTH_CHANCE_ADJUSTMENT;
        //
        for (auto & nextMapPair : chanceMapMetal)
            nextMapPair.second += WEALTH_CHANCE_ADJUSTMENT;
        //
        for (auto & nextMapPair : chanceMapPrecious)
            nextMapPair.second += WEALTH_CHANCE_ADJUSTMENT;

        //collector adjustments
        if ((PROFILE.collectorType & collector_type::Collector) > 0)
        {
            //adjustments that make collectors more likely to have special materials
            chanceCool += GameDataFile::Instance()->GetCopyFloat("heroespath-material-collector-chance-base-increase-Cool");
            chanceMetal += GameDataFile::Instance()->GetCopyFloat("heroespath-material-collector-chance-base-increase-Metal");
            chancePrecious += GameDataFile::Instance()->GetCopyFloat("heroespath-material-collector-chance-base-increase-Precious");

            //adjustments that make collector's already acquired special materials more rare
            const float CHANCE_DIV_COOL( GameDataFile::Instance()->GetCopyFloat("heroespath-material-collector-chance-per-divisor-Cool") );
            for (auto & nextMapPair : chanceMapCool)
                nextMapPair.second /= CHANCE_DIV_COOL;
            //
            const float CHANCE_DIV_METAL( GameDataFile::Instance()->GetCopyFloat("heroespath-material-collector-chance-per-divisor-Metal") );
            for (auto & nextMapPair : chanceMapMetal)
                nextMapPair.second /= CHANCE_DIV_METAL;
            //
            const float CHANCE_DIV_PRECIOUS( GameDataFile::Instance()->GetCopyFloat("heroespath-material-collector-chance-per-divisor-Precious") );
            for (auto & nextMapPair : chanceMapPrecious)
                nextMapPair.second /= CHANCE_DIV_PRECIOUS;
        }

        RestrictMaterialsByComplexity(PROFILE.complexityType,
                                      chanceCool,
                                      chanceMapCool,
                                      chanceMetal,
                                      chanceMapMetal,
                                      chancePrecious,
                                      chanceMapPrecious);


        //final determination of which material will be secondary
        const float SUBTOTAL(chanceCool + chanceMetal + chancePrecious);
        const float RAND( misc::random::Float(0.0f, std::max(1.0f, SUBTOTAL)) );

        if ((SUBTOTAL < 1.0f) && (RAND < (1.0f - SUBTOTAL)))
        {
            materialsMap_OutParam.clear();
            materialsMap_OutParam[item::material::Nothing] = 1.0f;
        }
        else
        {
            if (RAND < chanceCool)
            {
                materialsMap_OutParam = chanceMapCool;
            }
            else
            {
                if (RAND < chanceMetal)
                    materialsMap_OutParam = chanceMapMetal;
                else
                    materialsMap_OutParam = chanceMapPrecious;
            }
        }
    }


    const chance::MaterialChanceMap_t ChanceFactory::Make_MaterialChanceMap(const std::string &           PREFIX,
                                                                            const std::string &           POSTFIX,
                                                                            const chance::MaterialVec_t & MATERIALS_VEC)
    {
        float cumulativeChance(0.0f);
        chance::MaterialChanceMap_t materialChanceMap;
        item::material::Enum materialWithRemainingChance(item::material::Nothing);

        for (auto const NEXT_MATERIAL : MATERIALS_VEC)
        {
            const std::string NEXT_VALUE_STR(GameDataFile::Instance()->GetCopyStr(PREFIX + item::material::ToString(NEXT_MATERIAL) + POSTFIX));
            if (NEXT_VALUE_STR == "remaining")
            {
                materialWithRemainingChance = NEXT_MATERIAL;
            }
            else
            {
                const float NEXT_VALUE_FLOAT(GameDataFile::Instance()->GetCopyFloat(PREFIX + item::material::ToString(NEXT_MATERIAL) + POSTFIX));

                if (false == misc::IsRealClose(0.0f, NEXT_VALUE_FLOAT))
                {
                    const float NEXT_CHANCE(1.0f / NEXT_VALUE_FLOAT);
                    materialChanceMap[NEXT_MATERIAL] = NEXT_CHANCE;
                    cumulativeChance += NEXT_CHANCE;
                }
            }
        }

        if (materialWithRemainingChance != item::material::Nothing)
            materialChanceMap[materialWithRemainingChance] = 1.0f - cumulativeChance;

        return materialChanceMap;
    }


    const chance::MaterialChanceMap_t ChanceFactory::Make_MaterialChanceMapCool()
    {
        if (materialChanceMapCool_.empty())
        {
            const chance::MaterialVec_t MATERIAL_VEC = { item::material::Stone,
                                                         item::material::Bone,
                                                         item::material::Horn,
                                                         item::material::Tooth,
                                                         item::material::Bronze,
                                                         item::material::Obsidian,
                                                         item::material::Scale,
                                                         item::material::Lapis,
                                                         item::material::Gold,
                                                         item::material::Pearl };

            materialChanceMapCool_ = Make_MaterialChanceMap("heroespath-material-chance-base-cool-", "-onein", MATERIAL_VEC);
        }

        return materialChanceMapCool_;
    }


    const chance::MaterialChanceMap_t ChanceFactory::Make_MaterialChanceMapPrecious()
    {
        if (materialChanceMapPrecious_.empty())
        {
            const chance::MaterialVec_t MATERIAL_VEC = { item::material::Amethyst,
                                                         item::material::Emerald,
                                                         item::material::Silver,
                                                         item::material::Pearl,
                                                         item::material::Gold,
                                                         item::material::Platinum,
                                                         item::material::Ruby,
                                                         item::material::Lapis,
                                                         item::material::Sapphire,
                                                         item::material::Diamond };

            materialChanceMapPrecious_ = Make_MaterialChanceMap("heroespath-material-chance-base-precious-", "-onein", MATERIAL_VEC);
        }

        return materialChanceMapPrecious_;
    }


    const chance::MaterialChanceMap_t ChanceFactory::Make_MaterialChanceMapMetal()
    {
        if (materialChanceMapMetal_.empty())
        {
            const chance::MaterialVec_t MATERIAL_VEC = { item::material::Tin,
                                                         item::material::Bronze,
                                                         item::material::Iron,
                                                         item::material::Steel,
                                                         item::material::Silver,
                                                         item::material::Gold,
                                                         item::material::Platinum };

            materialChanceMapMetal_ = Make_MaterialChanceMap("heroespath-material-chance-base-metal-", "-onein", MATERIAL_VEC);
        }

        return materialChanceMapMetal_;
    }


    const chance::ClothingChances ChanceFactory::Make_ClothingMaterialChances(const Profile &                     PROFILE,
                                                                              const non_player::CharacterSPtr_t & CREATURE_SPTR)
    {
        chance::ItemChances itemChancesBase;
        Make_ClothingMaterialChancesPrimary(PROFILE, CREATURE_SPTR, itemChancesBase);
        Make_MaterialChancesSecondary(PROFILE, CREATURE_SPTR, itemChancesBase.mat_map_sec);

        std::ostringstream ss;
        const std::string KEY_BASE("heroespath-inventory-clothing-");
        ss << KEY_BASE << wealth_type::ToString(PROFILE.wealthType) << "-chances-";
        const std::string WEARABLE_STR_BASE(ss.str());

        const float MIN_VAL( GameDataFile::Instance()->GetCopyFloat(KEY_BASE + "chance-min") );
        const float MAX_VAL( GameDataFile::Instance()->GetCopyFloat(KEY_BASE + "chance-max") );

        //the final resulting object that will hold the chances for all clothing articles
        chance::ClothingChances clothingChances;

        clothingChances.boots = itemChancesBase;
        clothingChances.boots.SetCountChanceSingle(GetFloatFromGameDataFile(WEARABLE_STR_BASE + "boots", MIN_VAL, MAX_VAL));

        clothingChances.gloves = itemChancesBase;
        clothingChances.gloves.SetCountChanceSingle(GetFloatFromGameDataFile(WEARABLE_STR_BASE + "gloves", MIN_VAL, MAX_VAL));

        clothingChances.pants = itemChancesBase;
        clothingChances.pants.SetCountChanceSingleCertain();//lets just say everybody gets pants...

        clothingChances.shirt = itemChancesBase;
        clothingChances.shirt.SetCountChanceSingle(GetFloatFromGameDataFile(WEARABLE_STR_BASE + "shirt", MIN_VAL, MAX_VAL));

        clothingChances.vest = itemChancesBase;
        clothingChances.vest.SetCountChanceSingle(GetFloatFromGameDataFile(WEARABLE_STR_BASE + "vest", MIN_VAL, MAX_VAL));

        clothingChances.cover_map[item::armor::cover_type::Cape] = itemChancesBase;
        clothingChances.cover_map[item::armor::cover_type::Cape].SetCountChanceSingle(GetFloatFromGameDataFile(WEARABLE_STR_BASE + "cape", MIN_VAL, MAX_VAL));

        clothingChances.cover_map[item::armor::cover_type::Cloak] = itemChancesBase;
        clothingChances.cover_map[item::armor::cover_type::Cloak].SetCountChanceSingle(GetFloatFromGameDataFile(WEARABLE_STR_BASE + "cloak", MIN_VAL, MAX_VAL));

        clothingChances.cover_map[item::armor::cover_type::Robe] = itemChancesBase;
        clothingChances.cover_map[item::armor::cover_type::Robe].SetCountChanceSingle(GetFloatFromGameDataFile(WEARABLE_STR_BASE + "robe", MIN_VAL, MAX_VAL));

        return clothingChances;
    }


    void ChanceFactory::Make_KnifeOrDaggerSizeChances(const creature::rank_class::Enum RANK_CLASS, chance::SizeChanceMap_t & sizeChanceMap_OutParam)
    {
        switch (RANK_CLASS)
        {
            case creature::rank_class::Novice:
            {
                sizeChanceMap_OutParam[sfml_util::Size::Small] = 0.9f;
                sizeChanceMap_OutParam[sfml_util::Size::Medium] = 0.1f;
                break;
            }
            case creature::rank_class::Trainee:
            {
                sizeChanceMap_OutParam[sfml_util::Size::Small] = 0.45f;
                sizeChanceMap_OutParam[sfml_util::Size::Medium] = 0.45f;
                sizeChanceMap_OutParam[sfml_util::Size::Large] = 0.1f;
                break;
            }
            case creature::rank_class::Skilled:
            {
                sizeChanceMap_OutParam[sfml_util::Size::Small] = 0.1f;
                sizeChanceMap_OutParam[sfml_util::Size::Medium] = 0.45f;
                sizeChanceMap_OutParam[sfml_util::Size::Large] = 0.45f;
                break;
            }
            case creature::rank_class::Expert:
            {
                sizeChanceMap_OutParam[sfml_util::Size::Medium] = 0.1f;
                sizeChanceMap_OutParam[sfml_util::Size::Large] = 0.9f;
                break;
            }
            case creature::rank_class::Master:
            case creature::rank_class::GrandMaster:
            case creature::rank_class::Count:
            default:
            {
                sizeChanceMap_OutParam[sfml_util::Size::Large] = 1.0f;
                break;
            }
        }
    }


    void ChanceFactory::PopulateWeaponMaterials(const std::string &                 WEAPON_NAME,
                                                const chance::MaterialChanceMap_t & TYPICAL_PRI_MATERIALS,
                                                const Profile &                     PROFILE,
                                                const non_player::CharacterSPtr_t & CREATURE_SPTR,
                                                chance::MaterialChanceMap_t &       materialsMapPri,
                                                chance::MaterialChanceMap_t &       materialsMapSec)
    {
        const item::weapon::WeaponDetails WEAPON_DETAILS(item::weapon::WeaponDetailLoader::Instance()->LookupWeaponDetails(WEAPON_NAME));

        PopulatMaterials(TYPICAL_PRI_MATERIALS,
                         PROFILE,
                         CREATURE_SPTR,
                         materialsMapPri,
                         materialsMapSec,
                         WEAPON_DETAILS.weight);
    }


    void ChanceFactory::PopulatMaterials(const chance::MaterialChanceMap_t & TYPICAL_PRI_MATERIALS,
                                         const Profile &                     PROFILE,
                                         const non_player::CharacterSPtr_t & CREATURE_SPTR,
                                         chance::MaterialChanceMap_t &       materialsMapPri,
                                         chance::MaterialChanceMap_t &       materialsMapSec,
                                         const item::Weight_t                WEIGHT)
    {
        Make_MaterialChancesPrimary(PROFILE,
                                    CREATURE_SPTR,
                                    TYPICAL_PRI_MATERIALS,
                                    WEIGHT,
                                    materialsMapPri);

        Make_MaterialChancesSecondary(PROFILE,
                                      CREATURE_SPTR,
                                      materialsMapSec);
    }


    void ChanceFactory::RestrictMaterialsByComplexity(const complexity_type::Enum   COMPLEXITY,
                                                      float &,//cool materials are always possible, so this value is not changed here
                                                      chance::MaterialChanceMap_t & chanceMapCool,
                                                      float &                       chanceMetal,
                                                      chance::MaterialChanceMap_t & chanceMapMetal,
                                                      float &                       chancePrecious,
                                                      chance::MaterialChanceMap_t &)//individual precious material chances are not changed here either
    {
        //enforce creature complexity type restrictions on which materials are possible
        if (COMPLEXITY == complexity_type::Simple)
        {
            chanceMapCool[item::material::Scale] = 0.0f;
            chanceMapCool[item::material::Gold]  = 0.0f;
            chanceMapCool[item::material::Pearl] = 0.0f;
            chanceMapCool[item::material::Scale] = 0.0f;
            NormalizeChanceMap<item::material::Enum>(chanceMapCool);

            chanceMetal = 0.0f;

            chancePrecious = 0.0f;
        }
        else if (COMPLEXITY == complexity_type::Moderate)
        {
            chanceMapCool[item::material::Gold] = 0.0f;
            chanceMapCool[item::material::Pearl] = 0.0f;
            chanceMapCool[item::material::Scale] = 0.0f;
            NormalizeChanceMap<item::material::Enum>(chanceMapCool);

            chanceMapMetal[item::material::Steel]    = 0.0f;
            chanceMapMetal[item::material::Silver]   = 0.0f;
            chanceMapMetal[item::material::Gold]     = 0.0f;
            chanceMapMetal[item::material::Platinum] = 0.0f;
            NormalizeChanceMap<item::material::Enum>(chanceMapMetal);

            chancePrecious = 0.0f;
        }
    }


    bool ChanceFactory::IsWeaponPossibleConsideringComplexity(const std::string &         WEAPON_NAME,
                                                              const complexity_type::Enum CREATURE_COMPLEXITY)
    {
        const item::weapon::WeaponDetails WEAPON_DETAILS(item::weapon::WeaponDetailLoader::Instance()->LookupWeaponDetails(WEAPON_NAME));
        return (WEAPON_DETAILS.complexity <= CREATURE_COMPLEXITY);
    }


    const chance::MaterialChanceMap_t ChanceFactory::MakeTypicalArmorMaterials(const Profile &,
                                                                               const non_player::CharacterSPtr_t &,
                                                                               const bool                          INCLUDE_WOOD)
    {
        chance::MaterialChanceMap_t materialChanceMap;

        if (INCLUDE_WOOD)
        {
            materialChanceMap[item::material::Wood]  = 0.500f;
            materialChanceMap[item::material::Iron]  = 0.250f;
            materialChanceMap[item::material::Steel] = 0.175f;
            materialChanceMap[item::material::Tin]   = 0.075f;
        }
        else
        {
            materialChanceMap[item::material::Iron]  = 0.500f;
            materialChanceMap[item::material::Steel] = 0.375f;
            materialChanceMap[item::material::Tin]   = 0.125f;
        }

        //adjust for wealth type
        //TODO

        //adjust for rank
        //TODO

        return materialChanceMap;
    }


    void ChanceFactory::SetArmorChancesGeneral(chance::ArmorItemChances &          armorItemChances,
                                               const std::string &                 COMPLETE_NAME,
                                               const item::armor::base_type::Enum  TYPE,
                                               const float                         CHANCE,
                                               const Profile &                     PROFILE,
                                               const non_player::CharacterSPtr_t & CREATURE_SPTR,
                                               const bool                          WILL_MATERIALS_INCLUDED_WOOD)
    {
        const item::armor::ArmorDetails DETAILS( item::armor::ArmorDetailLoader::Instance()->LookupArmorDetails(COMPLETE_NAME));

        if (PROFILE.complexityType < DETAILS.complexity)
        {
            armorItemChances.SetCountChanceSingleNoChance();
            armorItemChances.type_map.clear();
        }
        else
        {
            armorItemChances.SetCountChanceIncrementAndEquip(CHANCE);
            armorItemChances.type_map[TYPE] = CHANCE;

            PopulatMaterials(MakeTypicalArmorMaterials(PROFILE,
                                                       CREATURE_SPTR,
                                                       WILL_MATERIALS_INCLUDED_WOOD),
                             PROFILE,
                             CREATURE_SPTR,
                             armorItemChances.mat_map_pri,
                             armorItemChances.mat_map_sec,
                             DETAILS.weight);
        }
    }


    void ChanceFactory::SetArmorChancesSpecific(chance::ItemChances &               itemChances,
                                                const std::string &                 COMPLETE_NAME,
                                                const float                         CHANCE,
                                                const Profile &                     PROFILE,
                                                const non_player::CharacterSPtr_t & CREATURE_SPTR,
                                                const bool                          WILL_MATERIALS_INCLUDED_WOOD)
    {
        const item::armor::ArmorDetails DETAILS( item::armor::ArmorDetailLoader::Instance()->LookupArmorDetails(COMPLETE_NAME) );

        if (PROFILE.complexityType < DETAILS.complexity)
        {
            itemChances.SetCountChanceSingleNoChance();
        }
        else
        {
            itemChances.SetCountChanceIncrementAndEquip(CHANCE);
            PopulatMaterials(MakeTypicalArmorMaterials(PROFILE,
                                                       CREATURE_SPTR,
                                                       WILL_MATERIALS_INCLUDED_WOOD),
                             PROFILE,
                             CREATURE_SPTR,
                             itemChances.mat_map_pri,
                             itemChances.mat_map_sec,
                             DETAILS.weight);
        }
    }

}
}
}
