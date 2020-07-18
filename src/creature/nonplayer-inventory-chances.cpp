// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// nonplayer-inventory-chances.cpp
//
/*
#include "nonplayer-inventory-chances.hpp"

#include "creature/creature.hpp"
#include "creature/trait.hpp"
#include "item/equipment-details.hpp"
#include "item/item.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/config-file.hpp"
#include "misc/enum-util.hpp"
#include "misc/log-macros.hpp"
#include "misc/strings.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace heroespath
{
namespace creature
{
    namespace nonplayer
    {

        const float ChanceFactory::CHANCE_MINIMUM_ { 0.001f };
        const float ChanceFactory::CHANCE_MAXIMUM_ { 0.999f };
        const float ChanceFactory::CHANCE_WEAPON_ENUM_POS_ADJ_SET_ { 0.5f };
        const float ChanceFactory::CHANCE_WEAPON_ENUM_POS_ADJ_INDIVIDUAL_ { 0.1f };

        std::unique_ptr<ChanceFactory> ChanceFactory::instanceUPtr_;

        ChanceFactory::ChanceFactory()
            : masterRankMax_(
                misc::ConfigFile::Instance()->ValueOrDefault<float>("rankclass-Master-rankmax"))
            , clothingChanceMin_(misc::ConfigFile::Instance()->ValueOrDefault<float>(
                  "inventory-clothing-chance-min"))
            , clothingChanceMax_(misc::ConfigFile::Instance()->ValueOrDefault<float>(
                  "inventory-clothing-chance-max"))
            , materialPrimaryChanceCool_(misc::ConfigFile::Instance()->ValueOrDefault<float>(
                  "material-primary-chance-base-Cool-onein"))
            , materialPrimaryChanceMetal_(misc::ConfigFile::Instance()->ValueOrDefault<float>(
                  "material-primary-chance-base-Metal-onein"))
            , materialPrimaryChancePrecious_(misc::ConfigFile::Instance()->ValueOrDefault<float>(
                  "material-primary-chance-base-Precious-onein"))
            , collectorMaterialChanceIncreaseCool_(
                  misc::ConfigFile::Instance()->ValueOrDefault<float>(
                      "material-collector-chance-base-increase-Cool"))
            , collectorMaterialChanceIncreaseMetal_(
                  misc::ConfigFile::Instance()->ValueOrDefault<float>(
                      "material-collector-chance-base-increase-Metal"))
            , collectorMaterialChanceIncreasePrecious_(
                  misc::ConfigFile::Instance()->ValueOrDefault<float>(
                      "material-collector-chance-base-increase-Precious"))
            , collectorMaterialChancePerCool_(misc::ConfigFile::Instance()->ValueOrDefault<float>(
                  "material-collector-chance-per-divisor-Cool"))
            , collectorMaterialChancePerMetal_(misc::ConfigFile::Instance()->ValueOrDefault<float>(
                  "material-collector-chance-per-divisor-Metal"))
            , collectorMaterialChancePerPrecious_(
                  misc::ConfigFile::Instance()->ValueOrDefault<float>(
                      "material-collector-chance-per-divisor-Precious"))
            , materialSecondaryChanceCool_(misc::ConfigFile::Instance()->ValueOrDefault<float>(
                  "material-secondary-chance-base-Cool-onein"))
            , materialSecondaryChanceMetal_(misc::ConfigFile::Instance()->ValueOrDefault<float>(
                  "material-secondary-chance-base-Metal-onein"))
            , materialSecondaryChancePrecious_(misc::ConfigFile::Instance()->ValueOrDefault<float>(
                  "material-secondary-chance-base-Precious-onein"))
            , materialChanceMapCool_(Make_MaterialChanceMapCool())
            , materialChanceMapMetal_(Make_MaterialChanceMapMetal())
            , materialChanceMapPrecious_(Make_MaterialChanceMapPrecious())
            , roleArmorChanceMap_()
            , roleWeaponChanceMap_()
        {
            M_HP_LOG_DBG("Subsystem Construction: ChanceFactory");
            roleArmorChanceMap_.Reserve(4096);
            roleWeaponChanceMap_.Reserve(4096);
        }

        ChanceFactory::~ChanceFactory() { M_HP_LOG_DBG("Subsystem Destruction: ChanceFactory"); }

        misc::NotNull<ChanceFactory *> ChanceFactory::Instance()
        {
            if (!instanceUPtr_)
            {
                M_HP_LOG_ERR(
                    "Subsystem Instance() called but instanceUPtr_ was null: ChanceFactory");
                Create();
            }

            return misc::NotNull<ChanceFactory *>(instanceUPtr_.get());
        }

        void ChanceFactory::Create()
        {
            if (!instanceUPtr_)
            {
                instanceUPtr_ = std::make_unique<ChanceFactory>();
            }
            else
            {
                M_HP_LOG_ERR("Subsystem Create() after Construction: ChanceFactory");
            }
        }

        void ChanceFactory::Destroy() { instanceUPtr_.reset(); }

        void ChanceFactory::Initialize()
        {
            CacheRoleArmorChances();
            CacheRoleWeaponChances();
        }

        const InventoryChances ChanceFactory::Make(const CreaturePtr_t CHARACTER_PTR) const
        {
            return Make(Profile::Make_FromCreature(CHARACTER_PTR), CHARACTER_PTR);
        }

        const InventoryChances
            ChanceFactory::Make(const Profile & PROFILE, const CreaturePtr_t CHARACTER_PTR) const
        {
            auto coinsMin { 0_coin };
            auto coinsMax { 0_coin };
            Make_Coins(PROFILE, coinsMin, coinsMax);

            return InventoryChances(
                coinsMin,
                coinsMax,
                Make_ClothingChances(PROFILE, CHARACTER_PTR),
                Make_WeaponChances(PROFILE, CHARACTER_PTR),
                Make_ArmorChances(PROFILE, CHARACTER_PTR),
                Make_MiscItemChances(PROFILE, CHARACTER_PTR));
        }

        void ChanceFactory::Make_Coins(
            const Profile & PROFILE, Coin_t & coinsMin_OutParam, Coin_t & coinsMax_OutParam) const
        {
            const auto KEY_STR { "nonplayer-coins-bounds-" + NAMEOF_ENUM_STR(PROFILE.wealthType) };

            const auto VALUE_STR { misc::ConfigFile::Instance()->Value(KEY_STR) };

            const std::vector<std::string> STR_VEC { misc::SplitByChars(
                VALUE_STR, misc::SplitHow(",")) };

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (STR_VEC.size() == 2),
                "nonplayerChanceFactory::Make_Coins() looked up \""
                    << KEY_STR << "\", retrieving \"" << VALUE_STR
                    << "\" which failed to be parsed into 2 comma sep strings.");

            const Coin_t::value_type ERROR_NUMBER(-1);
            coinsMin_OutParam = Coin_t::Make(misc::ToNumberOr(STR_VEC[0], ERROR_NUMBER));

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (coinsMin_OutParam >= 0_coin),
                "nonplayerChanceFactory::Make_Coins() looked up \""
                    << KEY_STR << "\", retrieving \"" << VALUE_STR
                    << "\" which failed to parse the first comma sep field into a valid number of "
                       "coins."
                    << "  coinsMin=" << coinsMin_OutParam);

            coinsMax_OutParam = Coin_t::Make(misc::ToNumberOr(STR_VEC[1], ERROR_NUMBER));

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (coinsMax_OutParam >= coinsMin_OutParam),
                "nonplayerChanceFactory::Make_Coins() looked up \""
                    << KEY_STR << "\", retrieving \"" << VALUE_STR
                    << "\" which failed to parse the second comma sep field into a valid (>= the "
                       "min"
                    << " of " << coinsMin_OutParam
                    << ") number of coins.  coinsMax=" << coinsMax_OutParam);
        }

        const ClothingChances ChanceFactory::Make_ClothingChances(
            const Profile & PROFILE, const CreaturePtr_t CHARACTER_PTR) const
        {
            ClothingChances clothingChances(Make_ClothingMaterialChances(PROFILE, CHARACTER_PTR));

            // capes, cloaks, and robes are mutually exclusive, so pre-select which (if any) are
            // worn

            const auto IS_CLOAK_OWNED { [&]() {
                try
                {
                    return clothingChances.cover_map[item::Covers::Cloak].IsOwned();
                }
                catch (...)
                {
                    M_HP_LOG_ERR(
                        "nonplayerMake_ClothingChances(creature={"
                        << CHARACTER_PTR->ToString()
                        << "})  threw exception during "
                           "clothingChances.cover_map[item::Covers::Cloak].IsOwned()");

                    throw;
                }
            }() };

            if (IS_CLOAK_OWNED)
            {
                clothingChances.cover_map[item::Covers::Cloak].SetCountChanceSingleCertain();
                clothingChances.cover_map[item::Covers::Cape].SetCountChanceSingleNoChance();
                clothingChances.cover_map[item::Covers::Robe].SetCountChanceSingleNoChance();
            }
            else
            {
                const auto IS_ROBE_OWNED { [&]() {
                    try
                    {
                        return clothingChances.cover_map[item::Covers::Robe].IsOwned();
                    }
                    catch (...)
                    {
                        M_HP_LOG_ERR(
                            "nonplayerMake_ClothingChances(creature={"
                            << CHARACTER_PTR->ToString()
                            << "})  threw exception during "
                               "clothingChances.cover_map[item::Covers::Robe].IsOwned("
                               ")");

                        throw;
                    }
                }() };

                if (IS_ROBE_OWNED)
                {
                    clothingChances.cover_map[item::Covers::Cloak]
                        .SetCountChanceSingleNoChance();

                    clothingChances.cover_map[item::Covers::Cape]
                        .SetCountChanceSingleNoChance();

                    clothingChances.cover_map[item::Covers::Robe]
                        .SetCountChanceSingleCertain();
                }
                else
                {
                    const auto IS_CAPE_OWNED { [&]() {
                        try
                        {
                            return clothingChances.cover_map[item::Covers::Cape].IsOwned();
                        }
                        catch (...)
                        {
                            M_HP_LOG_ERR(
                                "nonplayerMake_ClothingChances(creature={"
                                << CHARACTER_PTR->ToString()
                                << "})  threw exception during "
                                   "clothingChances.cover_map[item::Covers::Cape]."
                                   "IsOwned()");

                            throw;
                        }
                    }() };

                    if (IS_CAPE_OWNED)
                    {
                        clothingChances.cover_map[item::Covers::Cloak]
                            .SetCountChanceSingleNoChance();

                        clothingChances.cover_map[item::Covers::Cape]
                            .SetCountChanceSingleCertain();

                        clothingChances.cover_map[item::Covers::Robe]
                            .SetCountChanceSingleNoChance();
                    }
                }
            }

            // enforce bodytype restrictions
            const BodyType BODY_TYPE(CHARACTER_PTR->Body());
            if ((BODY_TYPE.NumArms() != 2) || (BODY_TYPE.HasWings() == true))
            {
                clothingChances.shirt.SetCountChanceSingleNoChance();
                clothingChances.cover_map[item::Covers::Cape].SetCountChanceSingleNoChance();
                clothingChances.cover_map[item::Covers::Cloak]
                    .SetCountChanceSingleNoChance();
                clothingChances.cover_map[item::Covers::Robe].SetCountChanceSingleNoChance();
            }

            if (BODY_TYPE.IsBiped())
            {
                // In this game, every 'humanoid' gets pants...Deal with it.
                clothingChances.pants.SetCountChanceSingleCertain();
            }
            else
            {
                clothingChances.pants.SetCountChanceSingleNoChance();
                clothingChances.boots.SetCountChanceSingleNoChance();
                clothingChances.shirt.SetCountChanceSingleNoChance();
                clothingChances.cover_map[item::Covers::Robe].SetCountChanceSingleNoChance();
                clothingChances.cover_map[item::Covers::Cloak]
                    .SetCountChanceSingleNoChance();
                clothingChances.cover_map[item::Covers::Cape].SetCountChanceSingleNoChance();
            }

            if ((BODY_TYPE.HasFingers() == false) || (BODY_TYPE.HasClaws() == true))
            {
                clothingChances.gloves.SetCountChanceSingleNoChance();
            }

            if (BODY_TYPE.IsBiped() == false)
            {
                clothingChances.pants.SetCountChanceSingleNoChance();
            }

            if (BODY_TYPE.HasFangs() == true)
            {
                clothingChances.cover_map[item::Covers::Cloak]
                    .SetCountChanceSingleNoChance();
            }

            if (BODY_TYPE.IsSerpentine())
            {
                clothingChances = ClothingChances::NoClothes();
            }

            if (BODY_TYPE.NumHeads() != 1)
            {
                clothingChances.cover_map[item::Covers::Cloak]
                    .SetCountChanceSingleNoChance();
            }

            return clothingChances;
        }

        const WeaponChances ChanceFactory::Make_WeaponChances(
            const Profile & PROFILE, const CreaturePtr_t CHARACTER_PTR) const
        {
            WeaponChances weaponChances(WeaponChances::NoWeapon());

            WeaponSetVec_t WEAPONS_SET_VEC;
            LookupPossibleWeaponsByRole(CHARACTER_PTR->Role(), WEAPONS_SET_VEC);

            for (const auto & NEXT_WEAPON_SET : WEAPONS_SET_VEC)
            {
                PopulateWeaponChances(weaponChances, NEXT_WEAPON_SET, PROFILE, CHARACTER_PTR);
            }

            // Add weapons based on body type
            const auto BODY { CHARACTER_PTR->Body() };
            weaponChances.has_bite = BODY.HasFangs();
            weaponChances.has_claws = BODY.HasClaws();
            weaponChances.has_fists = (BODY.HasArms() && BODY.HasFingers());
            weaponChances.has_tentacles = BODY.HasTentacles();

            weaponChances.has_breath = BODY.HasBreath()
                && ((CHARACTER_PTR->Role() == role::Firebrand)
                    || (CHARACTER_PTR->Role() == role::Sylavin));

            // enforce body-type/PROFILE.wealthType/PROFILE.wealthType
            // -what restrictions would there be when the weapon sets are
            //  based on creature roles in the GameDataFile?
            // -maybe race restrictions?  But those should already be accounted
            //  for in race/role combinations that are enforced elsewhere
            // TODO?

            return weaponChances;
        }

        const ArmorChances ChanceFactory::Make_ArmorChances(
            const Profile & PROFILE, const CreaturePtr_t CHARACTER_PTR) const
        {
            auto armorChances { ArmorChances::NoArmor() };
            LookupPossibleArmorByRole(PROFILE, CHARACTER_PTR, armorChances);

            // enforce bodytype restrictions
            const BodyType BODY_TYPE(CHARACTER_PTR->Body());

            if (BODY_TYPE.HasArms() == false)
            {
                armorChances.shirt.SetCountChanceSingleNoChance();
            }

            if (BODY_TYPE.HasHead() == false)
            {
                armorChances.helm_map.Clear();
            }

            return armorChances;
        }

        const ItemChanceMap_t
            ChanceFactory::Make_MiscItemChances(const Profile &, const CreaturePtr_t) const
        {
            ItemChanceMap_t miscItems;

            // Leave this as is for now, since there will be code elsewhere that determines
            // what items are left behind by a party of enemies (non-player characters) after
            // combat as treasure.  Since misc items are not used by enemies during combat,
            // there is no need for misc items at this point, wait until after combat treasure
            // is determined.

            return miscItems;
        }

        void ChanceFactory::PopulateWeaponChances(
            WeaponChances & weaponChances,
            const WeaponSet & WEAPON_SET,
            const Profile & PROFILE,
            const CreaturePtr_t CHARACTER_PTR) const
        {
            // find the total chance of all weapon possibilities combined
            float chanceCombined(0.0f);
            for (const auto & NEXT_WEAPONINFO_CHANCE_PAIR : WEAPON_SET.chanceMap)
            {
                chanceCombined += NEXT_WEAPONINFO_CHANCE_PAIR.second;
            }

            if ((misc::IsRealClose(chanceCombined, 0.0f)) || (chanceCombined < 0.0f))
            {
                return;
            }

            const float RAND(misc::Random(0.0f, chanceCombined));

            // Need to select WEAPON_SET.count weapons in WEAPON_SET.chanceMap, so
            // make a mapping of count to chance that has been selected already or not.
            std::vector<bool> numberSelectedVec;
            numberSelectedVec.resize(WEAPON_SET.count, false);

            // weapon selection loop
            // add chances for each weapon in the WEAPON_SET, if randomly selected]
            float chanceCumulative(0.0f);
            for (const auto & NEXT_WEAPONINFO_CHANCE_PAIR : WEAPON_SET.chanceMap)
            {
                // if all weapons have already been chosen then break from the selection loop
                bool willBreak(true);
                for (const auto NEXT_BOOL : numberSelectedVec)
                {
                    if (NEXT_BOOL == false)
                    {
                        willBreak = false;
                        break;
                    }
                }

                if (willBreak)
                {
                    break;
                }

                // At this point there is guaranteed to be at least one false bool in
                // numberSelectedVec

                // check if this iteration (weapon that may be selected) will be skipped
                bool willSkipThisIteration(true);
                std::size_t selectedIndex(0);
                const std::size_t NUM_WEPAONS_TO_SELECT(numberSelectedVec.size());
                for (; selectedIndex < NUM_WEPAONS_TO_SELECT; ++selectedIndex)
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

                // At this point numberSelectedVec contains a value of false

                if (NEXT_WEAPONINFO_CHANCE_PAIR.first.IsBite())
                {
                    weaponChances.has_bite = true;
                }

                if (NEXT_WEAPONINFO_CHANCE_PAIR.first.IsClaws())
                {
                    weaponChances.has_claws = true;
                }

                if (NEXT_WEAPONINFO_CHANCE_PAIR.first.IsFists())
                {
                    weaponChances.has_fists = true;
                }

                if (NEXT_WEAPONINFO_CHANCE_PAIR.first.IsTentacles())
                {
                    weaponChances.has_tentacles = true;
                }

                if (NEXT_WEAPONINFO_CHANCE_PAIR.first.IsBreath())
                {
                    weaponChances.has_breath = true;
                }

                if (NEXT_WEAPONINFO_CHANCE_PAIR.first.IsKnife()
                    || NEXT_WEAPONINFO_CHANCE_PAIR.first.IsDagger())
                {
                    // no need to check complexity because knives/daggers are simple

                    weaponChances.knife.SetCountChanceIncrementAndEquip(
                        NEXT_WEAPONINFO_CHANCE_PAIR.second);

                    weaponChances.knife.is_dagger = NEXT_WEAPONINFO_CHANCE_PAIR.first.IsDagger();

                    MaterialChanceMap_t typicalKnifePrimaryMaterials;
                    typicalKnifePrimaryMaterials.Reserve(32);
                    if (PROFILE.complexityType == complexity_type::Simple)
                    {
                        typicalKnifePrimaryMaterials[item::Material::Stone] = 0.75f;
                        typicalKnifePrimaryMaterials[item::Material::Bone] = 0.25f;
                    }
                    else if (PROFILE.complexityType == complexity_type::Moderate)
                    {
                        typicalKnifePrimaryMaterials[item::Material::Stone] = 0.05f;
                        typicalKnifePrimaryMaterials[item::Material::Bone] = 0.05f;
                        typicalKnifePrimaryMaterials[item::Material::Steel] = 0.70f;
                        typicalKnifePrimaryMaterials[item::Material::Iron] = 0.20f;
                    }
                    else
                    {
                        typicalKnifePrimaryMaterials[item::Material::Steel] = 0.87f;
                        typicalKnifePrimaryMaterials[item::Material::Iron] = 0.1f;
                        typicalKnifePrimaryMaterials[item::Material::Platinum] = 0.03f;
                    }

                    PopulateWeaponMaterials(
                        NEXT_WEAPONINFO_CHANCE_PAIR.first.DetailsKeyName(),
                        typicalKnifePrimaryMaterials,
                        PROFILE,
                        CHARACTER_PTR,
                        weaponChances.knife.mat_map_pri,
                        weaponChances.knife.mat_map_sec);
                }

                if ((NEXT_WEAPONINFO_CHANCE_PAIR.first.IsStaff())
                    || (NEXT_WEAPONINFO_CHANCE_PAIR.first.IsQuarterstaff()))
                {
                    // no need to check complexity because using a staff/quarterstaff is simple

                    weaponChances.staff.SetCountChanceIncrementAndEquip(
                        NEXT_WEAPONINFO_CHANCE_PAIR.second);

                    weaponChances.staff.is_quarterstaff
                        = NEXT_WEAPONINFO_CHANCE_PAIR.first.IsQuarterstaff();

                    MaterialChanceMap_t typicalStaffPrimaryMaterials;
                    typicalStaffPrimaryMaterials.Reserve(32);
                    typicalStaffPrimaryMaterials[item::Material::Wood] = 1.0f;

                    PopulateWeaponMaterials(
                        NEXT_WEAPONINFO_CHANCE_PAIR.first.DetailsKeyName(),
                        typicalStaffPrimaryMaterials,
                        PROFILE,
                        CHARACTER_PTR,
                        weaponChances.staff.mat_map_pri,
                        weaponChances.staff.mat_map_sec);
                }

                {
                    MaterialChanceMap_t typicalAxePrimaryMaterials;
                    typicalAxePrimaryMaterials.Reserve(32);
                    typicalAxePrimaryMaterials[item::Material::Wood] = 1.0f;

                    if (NEXT_WEAPONINFO_CHANCE_PAIR.first.IsAxe())
                    {
                        SetWeaponChances<item::Axes::Enum>(
                            NEXT_WEAPONINFO_CHANCE_PAIR.first.DetailsKeyName(),
                            NEXT_WEAPONINFO_CHANCE_PAIR.first.AxeType(),
                            NEXT_WEAPONINFO_CHANCE_PAIR.second,
                            PROFILE,
                            CHARACTER_PTR,
                            typicalAxePrimaryMaterials,
                            weaponChances.axe_map);
                    }
                    else if (NEXT_WEAPONINFO_CHANCE_PAIR.first.Type() & item::Weapon::Axe)
                    {
                        const auto CHANCE_PER_WEAPON { NEXT_WEAPONINFO_CHANCE_PAIR.second
                                                       / static_cast<float>(
                                                           item::Axes::Count) };

                        for (const auto & WEAPON_TEMPLATE :
                             item::WeaponTemplate::MakeSpecificSet<item::Axes>())
                        {
                            SetWeaponChances<item::Axes::Enum>(
                                WEAPON_TEMPLATE.DetailsKeyName(),
                                WEAPON_TEMPLATE.AxeType(),
                                CHANCE_PER_WEAPON,
                                PROFILE,
                                CHARACTER_PTR,
                                typicalAxePrimaryMaterials,
                                weaponChances.axe_map);
                        }
                    }
                }

                {
                    MaterialChanceMap_t typicalBladedstaffPrimaryMaterials;
                    typicalBladedstaffPrimaryMaterials.Reserve(32);
                    typicalBladedstaffPrimaryMaterials[item::Material::Wood] = 1.0f;

                    if (NEXT_WEAPONINFO_CHANCE_PAIR.first.IsBladedstaff())
                    {
                        SetWeaponChances<item::Bladedstaffs::Enum>(
                            NEXT_WEAPONINFO_CHANCE_PAIR.first.DetailsKeyName(),
                            NEXT_WEAPONINFO_CHANCE_PAIR.first.BladedstaffType(),
                            NEXT_WEAPONINFO_CHANCE_PAIR.second,
                            PROFILE,
                            CHARACTER_PTR,
                            typicalBladedstaffPrimaryMaterials,
                            weaponChances.bladedstaff_map);
                    }
                    else if (NEXT_WEAPONINFO_CHANCE_PAIR.first.Type() & item::Weapon::Bladedstaff)
                    {
                        const auto CHANCE_PER_WEAPON { NEXT_WEAPONINFO_CHANCE_PAIR.second
                                                       / static_cast<float>(
                                                           item::Bladedstaffs::Count) };

                        for (const auto & WEAPON_TEMPLATE :
                             item::WeaponTemplate::MakeSpecificSet<
                                 item::Bladedstaffs>())
                        {
                            SetWeaponChances<item::Bladedstaffs::Enum>(
                                WEAPON_TEMPLATE.DetailsKeyName(),
                                WEAPON_TEMPLATE.BladedstaffType(),
                                CHANCE_PER_WEAPON,
                                PROFILE,
                                CHARACTER_PTR,
                                typicalBladedstaffPrimaryMaterials,
                                weaponChances.bladedstaff_map);
                        }
                    }
                }

                {
                    MaterialChanceMap_t typicalClubPrimaryMaterials;
                    typicalClubPrimaryMaterials.Reserve(32);
                    typicalClubPrimaryMaterials[item::Material::Wood] = 1.0f;

                    if (NEXT_WEAPONINFO_CHANCE_PAIR.first.IsClub())
                    {
                        SetWeaponChances<item::Clubs::Enum>(
                            NEXT_WEAPONINFO_CHANCE_PAIR.first.DetailsKeyName(),
                            NEXT_WEAPONINFO_CHANCE_PAIR.first.ClubType(),
                            NEXT_WEAPONINFO_CHANCE_PAIR.second,
                            PROFILE,
                            CHARACTER_PTR,
                            typicalClubPrimaryMaterials,
                            weaponChances.club_map);
                    }
                    else if (NEXT_WEAPONINFO_CHANCE_PAIR.first.Type() & item::Weapon::Club)
                    {
                        const auto CHANCE_PER_WEAPON { NEXT_WEAPONINFO_CHANCE_PAIR.second
                                                       / static_cast<float>(
                                                           item::Clubs::Count) };

                        for (const auto & WEAPON_TEMPLATE :
                             item::WeaponTemplate::MakeSpecificSet<
                                 item::Clubs>())
                        {
                            SetWeaponChances<item::Clubs::Enum>(
                                WEAPON_TEMPLATE.DetailsKeyName(),
                                WEAPON_TEMPLATE.ClubType(),
                                CHANCE_PER_WEAPON,
                                PROFILE,
                                CHARACTER_PTR,
                                typicalClubPrimaryMaterials,
                                weaponChances.club_map);
                        }
                    }
                }

                {
                    MaterialChanceMap_t typicalProjectilePrimaryMaterials;
                    typicalProjectilePrimaryMaterials.Reserve(32);
                    typicalProjectilePrimaryMaterials[item::Material::Wood] = 1.0f;

                    if (NEXT_WEAPONINFO_CHANCE_PAIR.first.IsProjectile())
                    {
                        SetWeaponChances<item::Projectiles::Enum>(
                            NEXT_WEAPONINFO_CHANCE_PAIR.first.DetailsKeyName(),
                            NEXT_WEAPONINFO_CHANCE_PAIR.first.ProjectileType(),
                            NEXT_WEAPONINFO_CHANCE_PAIR.second,
                            PROFILE,
                            CHARACTER_PTR,
                            typicalProjectilePrimaryMaterials,
                            weaponChances.projectile_map);
                    }
                    else if (NEXT_WEAPONINFO_CHANCE_PAIR.first.Type() & item::Weapon::Projectile)
                    {
                        const auto CHANCE_PER_WEAPON { NEXT_WEAPONINFO_CHANCE_PAIR.second
                                                       / static_cast<float>(
                                                           item::Projectiles::Count) };

                        for (const auto & WEAPON_TEMPLATE :
                             item::WeaponTemplate::MakeSpecificSet<
                                 item::Projectiles>())
                        {
                            SetWeaponChances<item::Projectiles::Enum>(
                                WEAPON_TEMPLATE.DetailsKeyName(),
                                WEAPON_TEMPLATE.ProjectileType(),
                                CHANCE_PER_WEAPON,
                                PROFILE,
                                CHARACTER_PTR,
                                typicalProjectilePrimaryMaterials,
                                weaponChances.projectile_map);
                        }
                    }
                }

                {
                    MaterialChanceMap_t typicalSwordPrimaryMaterials;
                    typicalSwordPrimaryMaterials.Reserve(32);
                    typicalSwordPrimaryMaterials[item::Material::Steel] = 1.0f;

                    if (NEXT_WEAPONINFO_CHANCE_PAIR.first.IsSword())
                    {
                        SetWeaponChances<item::Swords::Enum>(
                            NEXT_WEAPONINFO_CHANCE_PAIR.first.DetailsKeyName(),
                            NEXT_WEAPONINFO_CHANCE_PAIR.first.MinorAs<Swords>(),
                            NEXT_WEAPONINFO_CHANCE_PAIR.second,
                            PROFILE,
                            CHARACTER_PTR,
                            typicalSwordPrimaryMaterials,
                            weaponChances.sword_map);
                    }
                    else if (NEXT_WEAPONINFO_CHANCE_PAIR.first.Type() & item::Weapon::Sword)
                    {
                        const auto CHANCE_PER_WEAPON { NEXT_WEAPONINFO_CHANCE_PAIR.second
                                                       / static_cast<float>(
                                                           item::Swords::Count) };

                        for (const auto & WEAPON_TEMPLATE :
                             item::WeaponTemplate::MakeSpecificSet<
                                 item::Swords>())
                        {
                            SetWeaponChances<item::Swords::Enum>(
                                WEAPON_TEMPLATE.DetailsKeyName(),
                                WEAPON_TEMPLATE.MinorAs<Swords>(),
                                CHANCE_PER_WEAPON,
                                PROFILE,
                                CHARACTER_PTR,
                                typicalSwordPrimaryMaterials,
                                weaponChances.sword_map);
                        }
                    }
                }

                {
                    auto whipMaterialChanceMapMaker {
                        [](const item::Whips::Enum WHIP_TYPE) {
                            MaterialChanceMap_t typicalWhipPrimaryMaterials;
                            typicalWhipPrimaryMaterials.Reserve(32);

                            if (WHIP_TYPE == item::Whips::Bullwhip)
                            {
                                typicalWhipPrimaryMaterials[item::Material::Leather] = 1.0f;
                            }
                            else
                            {
                                typicalWhipPrimaryMaterials[item::Material::Steel] = 1.0f;
                            }

                            return typicalWhipPrimaryMaterials;
                        }
                    };

                    if (NEXT_WEAPONINFO_CHANCE_PAIR.first.IsWhip())
                    {
                        SetWeaponChances<item::Whips::Enum>(
                            NEXT_WEAPONINFO_CHANCE_PAIR.first.DetailsKeyName(),
                            NEXT_WEAPONINFO_CHANCE_PAIR.first.WhipType(),
                            NEXT_WEAPONINFO_CHANCE_PAIR.second,
                            PROFILE,
                            CHARACTER_PTR,
                            whipMaterialChanceMapMaker(
                                NEXT_WEAPONINFO_CHANCE_PAIR.first.WhipType()),
                            weaponChances.whip_map);
                    }
                    else if (NEXT_WEAPONINFO_CHANCE_PAIR.first.Type() & item::Weapon::Whip)
                    {
                        const auto CHANCE_PER_WEAPON { NEXT_WEAPONINFO_CHANCE_PAIR.second
                                                       / static_cast<float>(
                                                           item::Whips::Count) };

                        for (const auto & WEAPON_TEMPLATE :
                             item::WeaponTemplate::MakeSpecificSet<
                                 item::Whips>())
                        {
                            SetWeaponChances<item::Whips::Enum>(
                                WEAPON_TEMPLATE.DetailsKeyName(),
                                WEAPON_TEMPLATE.WhipType(),
                                CHANCE_PER_WEAPON,
                                PROFILE,
                                CHARACTER_PTR,
                                whipMaterialChanceMapMaker(WEAPON_TEMPLATE.WhipType()),
                                weaponChances.whip_map);
                        }
                    }
                }

                chanceCumulative += NEXT_WEAPONINFO_CHANCE_PAIR.second;
                numberSelectedVec[selectedIndex] = true;
            }
        }

        void ChanceFactory::LookupPossibleArmorByRole(
            const Profile & PROFILE,
            const CreaturePtr_t CHARACTER_PTR,
            ArmorChances & armorChances) const
        {
            RoleArmorChanceVec_t ignored;
            auto & roleArmorChances { ignored };
            roleArmorChanceMap_.Find(CHARACTER_PTR->Role(), roleArmorChances);

            for (const auto & ROLE_ARMOR_CHANCE : roleArmorChances)
            {
                switch (ROLE_ARMOR_CHANCE.type_wrapper.Type())
                {
                    case item::Armor::Cover:
                    {
                        SetArmorChancesSpecific(
                            armorChances.cover_map[ROLE_ARMOR_CHANCE.type_wrapper
                                                       .MinorAs<Covers>()],
                            ROLE_ARMOR_CHANCE,
                            PROFILE,
                            CHARACTER_PTR,
                            false);

                        break;
                    }
                    case item::Armor::Shield:
                    {
                        SetArmorChancesSpecific(
                            armorChances.shield_map[ROLE_ARMOR_CHANCE.type_wrapper
                                                        .MinorAs<Shields>()],
                            ROLE_ARMOR_CHANCE,
                            PROFILE,
                            CHARACTER_PTR,
                            true);

                        break;
                    }
                    case item::Armor::Helm:
                    {
                        SetArmorChancesSpecific(
                            armorChances
                                .helm_map[ROLE_ARMOR_CHANCE.type_wrapper.MinorAs<Helms>()],
                            ROLE_ARMOR_CHANCE,
                            PROFILE,
                            CHARACTER_PTR,
                            false,
                            item::Material::Leather);

                        break;
                    }
                    case item::Armor::Gauntlet:
                    {
                        SetArmorChancesGeneral(
                            armorChances.gauntlets,
                            ROLE_ARMOR_CHANCE,
                            PROFILE,
                            CHARACTER_PTR,
                            false);

                        break;
                    }
                    case item::Armor::Pant:
                    {
                        SetArmorChancesGeneral(
                            armorChances.pants, ROLE_ARMOR_CHANCE, PROFILE, CHARACTER_PTR, false);

                        break;
                    }
                    case item::Armor::Boot:
                    {
                        SetArmorChancesGeneral(
                            armorChances.boots, ROLE_ARMOR_CHANCE, PROFILE, CHARACTER_PTR, false);

                        break;
                    }
                    case item::Armor::Shirt:
                    {
                        SetArmorChancesGeneral(
                            armorChances.shirt, ROLE_ARMOR_CHANCE, PROFILE, CHARACTER_PTR, false);

                        break;
                    }
                    case item::Armor::Bracer:
                    {
                        SetArmorChancesGeneral(
                            armorChances.bracers, ROLE_ARMOR_CHANCE, PROFILE, CHARACTER_PTR, false);

                        break;
                    }
                    case item::Armor::Aventail:
                    {
                        SetArmorChancesGeneral(
                            armorChances.aventail,
                            ROLE_ARMOR_CHANCE,
                            PROFILE,
                            CHARACTER_PTR,
                            false);

                        break;
                    }

                    case item::Armor::Skin:
                    case item::Armor::Count:
                    default:
                    {
                        break;
                    }
                }
            }
        }

        void ChanceFactory::LookupPossibleWeaponsByRole(
            const role::Enum ROLE, WeaponSetVec_t & WEAPONS_SET_VEC_OutParam) const
        {
            const auto WAS_FOUND { roleWeaponChanceMap_.Find(ROLE, WEAPONS_SET_VEC_OutParam) };

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (WAS_FOUND),
                "nonplayerChanceFactory::LookupPossibleWeaponsByRole(role="
                    << NAMEOF_ENUM(ROLE) << ") was unable to find that role in the map.");
        }

        float ChanceFactory::GetFloatFromGameDataFile(
            const std::string & KEY, const float MIN, const float MAX) const
        {
            const float ERROR_NUMBER(-1.0f);
            const std::string MIN_STR { "min" };
            const std::string MAX_STR { "max" };
            const std::string REMAINING_STR { "remaining" };
            const std::string VALUE_STR(misc::ConfigFile::Instance()->Value(KEY));

            if (VALUE_STR == MIN_STR)
            {
                return MIN;
            }
            else if (VALUE_STR == MAX_STR)
            {
                return MAX;
            }
            else if (VALUE_STR == REMAINING_STR)
            {
                return ERROR_NUMBER;
            }
            else
            {
                const auto RESULT = misc::ToNumberOr(VALUE_STR, ERROR_NUMBER);

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (!misc::IsRealClose(RESULT, ERROR_NUMBER)),
                    "creature::nonplayer::ChanceFactory::GetFloatFromGameDataFile(key=\""
                        << KEY << "\"), found value \"" << VALUE_STR
                        << "\" that failed to be parsed as a float.");

                return RESULT;
            }
        }

        void ChanceFactory::LookupClothingMaterialChances(
            const Profile & PROFILE,
            const CreaturePtr_t CHARACTER_PTR,
            float & clothChance,
            float & leatherChance,
            float & silkChance) const
        {
            const auto WEARABLE_STR_BASE { "inventory-clothing-"
                                           + NAMEOF_ENUM_STR(PROFILE.wealthType) + "-chances" };

            // find the chance-subtotal and the material that will have the remaining chance
            auto chanceSubTotal(0.0f);
            item::Material::Enum materialWithChanceRemaining(item::Material::Count);
            const auto CHANCE_MIN { clothingChanceMin_ };

            const auto CHANCE_MAX { clothingChanceMax_ };

            clothChance = GetFloatFromGameDataFile(
                WEARABLE_STR_BASE + "-wearable-cloth", CHANCE_MIN, CHANCE_MAX);

            if (clothChance < 0.0f)
            {
                materialWithChanceRemaining = item::Material::Cloth;
            }
            else
            {
                chanceSubTotal += clothChance;
            }

            leatherChance = GetFloatFromGameDataFile(
                WEARABLE_STR_BASE + "-wearable-leather", CHANCE_MIN, CHANCE_MAX);

            if (leatherChance < 0.0f)
            {
                materialWithChanceRemaining = item::Material::Leather;
            }
            else
            {
                chanceSubTotal += leatherChance;
            }

            silkChance = GetFloatFromGameDataFile(
                WEARABLE_STR_BASE + "-wearable-silk", CHANCE_MIN, CHANCE_MAX);

            if (silkChance < 0.0f)
            {
                materialWithChanceRemaining = item::Material::Silk;
            }
            else
            {
                chanceSubTotal += silkChance;
            }

            // set the chance for the material with the remaining chance
            if (item::Material::Cloth == materialWithChanceRemaining)
            {
                clothChance = 1.0f - chanceSubTotal;
            }
            else
            {
                if (item::Material::Leather == materialWithChanceRemaining)
                {
                    leatherChance = 1.0f - chanceSubTotal;
                }
                else
                {
                    silkChance = 1.0f - chanceSubTotal;
                }
            }

            // adjust chances by rank, where higher rank means a higher
            // chance of more valuable materials
            const auto RANK_RATIO { CHARACTER_PTR->Rank().GetAs<float>() / masterRankMax_ };

            silkChance += RANK_RATIO;
            leatherChance += RANK_RATIO * 0.5f;
            clothChance -= RANK_RATIO;

            // enforce the min/max after all adjustments
            ChanceFactory::ForceMinMax(clothChance, CHANCE_MIN, CHANCE_MAX);
            ChanceFactory::ForceMinMax(leatherChance, CHANCE_MIN, CHANCE_MAX);
            ChanceFactory::ForceMinMax(silkChance, CHANCE_MIN, CHANCE_MAX);
        }

        void ChanceFactory::Make_ClothingMaterialChancesPrimary(
            const Profile & PROFILE,
            const CreaturePtr_t CHARACTER_PTR,
            ItemChances & itemChancesBase) const
        {
            // set the chance base object with data common to all clothing chances
            //(shared between boots, cloaks, capes, etc.)

            // all clothing items are equipped
            itemChancesBase.chance_equipped = 1.0f;

            // determine the final chances for each possible primary material type,
            // which for clothing are restricted to cloth, soft leather, and hard leather
            auto clothChance { 0.0f };
            auto leatherChance { 0.0f };
            auto silkChance { 0.0f };

            LookupClothingMaterialChances(
                PROFILE, CHARACTER_PTR, clothChance, leatherChance, silkChance);

            itemChancesBase.mat_map_pri[item::Material::Cloth] = clothChance;
            itemChancesBase.mat_map_pri[item::Material::Leather] = leatherChance;
            itemChancesBase.mat_map_pri[item::Material::Silk] = silkChance;
        }

        void ChanceFactory::Make_MaterialChancesPrimary(
            const Profile & PROFILE,
            const CreaturePtr_t CHARACTER_PTR,
            const MaterialChanceMap_t & MATERIALS_TYPICAL,
            const Weight_t & ITEM_WEIGHT,
            MaterialChanceMap_t & materialsMap_OutParam) const
        {
            // establish the base chances for a special primary material
            auto chanceCool { 1.0f / materialPrimaryChanceCool_ };

            auto chanceMetal { 1.0f / materialPrimaryChanceMetal_ };

            auto chancePrecious { 1.0f / materialPrimaryChancePrecious_ };

            auto chanceMapCool { Make_MaterialChanceMapCool() };
            auto chanceMapMetal { Make_MaterialChanceMapMetal() };
            auto chanceMapPrecious { Make_MaterialChanceMapPrecious() };

            // adjustments that make higher ranks more likely to have special materials
            const auto RANK_CHANCE_INCREASE { CHARACTER_PTR->Rank().GetAs<float>()
                                              / masterRankMax_ };

            chanceCool += RANK_CHANCE_INCREASE;
            chanceMetal += RANK_CHANCE_INCREASE;
            chancePrecious += RANK_CHANCE_INCREASE;

            // adjustments that make higher ranks have special materials that are more rare
            for (auto & nextMapPair : chanceMapCool)
            {
                nextMapPair.second /= RANK_CHANCE_INCREASE;
            }

            for (auto & nextMapPair : chanceMapMetal)
            {
                nextMapPair.second /= RANK_CHANCE_INCREASE;
            }

            for (auto & nextMapPair : chanceMapPrecious)
            {
                nextMapPair.second /= RANK_CHANCE_INCREASE;
            }

            // adjustments that make more wealth equal better chances for special materials
            const auto WEALTH_CHANCE_ADJUSTMENT {
                misc::ConfigFile::Instance()->ValueOrDefault<float>(
                    "material-wealth-chance-base-adjustment-" + NAMEOF_ENUM_STR(PROFILE.wealthType))
            };

            chanceCool += WEALTH_CHANCE_ADJUSTMENT;
            chanceMetal += WEALTH_CHANCE_ADJUSTMENT;
            chancePrecious += WEALTH_CHANCE_ADJUSTMENT;

            // adjustments that make more wealth equal already acquired special materials more rare
            for (auto & nextMapPair : chanceMapCool)
            {
                nextMapPair.second += WEALTH_CHANCE_ADJUSTMENT;
            }

            for (auto & nextMapPair : chanceMapMetal)
            {
                nextMapPair.second += WEALTH_CHANCE_ADJUSTMENT;
            }

            for (auto & nextMapPair : chanceMapPrecious)
            {
                nextMapPair.second += WEALTH_CHANCE_ADJUSTMENT;
            }

            // collector adjustments
            if ((PROFILE.collectorType & collector_type::Collector) > 0)
            {
                // adjustments that make collectors more likely to have special materials
                chanceCool += collectorMaterialChanceIncreaseCool_;
                chanceMetal += collectorMaterialChanceIncreaseMetal_;
                chancePrecious += collectorMaterialChanceIncreasePrecious_;

                // adjustments that make collector's already acquired special materials more rare
                for (auto & nextMapPair : chanceMapCool)
                {
                    nextMapPair.second /= collectorMaterialChancePerCool_;
                }

                for (auto & nextMapPair : chanceMapMetal)
                {
                    nextMapPair.second /= collectorMaterialChancePerMetal_;
                }

                for (auto & nextMapPair : chanceMapPrecious)
                {
                    nextMapPair.second /= collectorMaterialChancePerPrecious_;
                }
            }

            // adjust for item weight that can make special materials less likely
            const auto WEIGHT_CHANCE_ADJUSTMENT { ITEM_WEIGHT.GetAs<float>() / 2500.0f };
            chanceCool -= WEIGHT_CHANCE_ADJUSTMENT;
            chanceMetal -= WEIGHT_CHANCE_ADJUSTMENT;
            chancePrecious -= WEIGHT_CHANCE_ADJUSTMENT;

            // enforce minimum chances
            ForceMin(chanceCool);
            ForceMin(chanceMetal);
            ForceMin(chancePrecious);

            // adjust material chances based on creature PROFILE.complexity
            RestrictMaterialsByComplexity(
                PROFILE.complexityType,
                chanceCool,
                chanceMapCool,
                chanceMetal,
                chanceMapMetal,
                chancePrecious,
                chanceMapPrecious);

            // final determination of which material will be primary
            const auto SUBTOTAL { chanceCool + chanceMetal + chancePrecious };
            const auto RAND { misc::Random(0.0f, misc::Max(1.0f, SUBTOTAL)) };

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
                    {
                        materialsMap_OutParam = chanceMapMetal;
                    }
                    else
                    {
                        materialsMap_OutParam = chanceMapPrecious;
                    }
                }
            }

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (materialsMap_OutParam.Empty() == false),
                "nonplayerChanceFactory::Make_MaterialChancesPrimary(creature={"
                    << CHARACTER_PTR->ToString() << "}) final materials map was empty.");

            for (auto & nextPair : materialsMap_OutParam)
            {
                if (nextPair.second < 0.0f)
                {
                    nextPair.second = 0.0f;
                }
            }
        }

        void ChanceFactory::Make_MaterialChancesSecondary(
            const Profile & PROFILE,
            const CreaturePtr_t CHARACTER_PTR,
            MaterialChanceMap_t & materialsMap_OutParam) const
        {
            // establish the base chances for a secondary material
            auto chanceCool { 1.0f / materialSecondaryChanceCool_ };
            auto chanceMetal { 1.0f / materialSecondaryChanceMetal_ };
            auto chancePrecious { 1.0f / materialSecondaryChancePrecious_ };

            auto chanceMapCool { Make_MaterialChanceMapCool() };
            auto chanceMapMetal { Make_MaterialChanceMapMetal() };
            auto chanceMapPrecious { Make_MaterialChanceMapPrecious() };

            // adjustments that make higher ranks more likely to have special materials
            const auto RANK_CHANCE_INCREASE { CHARACTER_PTR->Rank().GetAs<float>()
                                              / masterRankMax_ };

            chanceCool += RANK_CHANCE_INCREASE;
            chanceMetal += RANK_CHANCE_INCREASE;
            chancePrecious += RANK_CHANCE_INCREASE;

            // adjustments that make higher ranks already acquired special materials more rare
            for (auto & nextMapPair : chanceMapCool)
            {
                nextMapPair.second /= RANK_CHANCE_INCREASE;
            }

            for (auto & nextMapPair : chanceMapMetal)
            {
                nextMapPair.second /= RANK_CHANCE_INCREASE;
            }

            for (auto & nextMapPair : chanceMapPrecious)
            {
                nextMapPair.second /= RANK_CHANCE_INCREASE;
            }

            // adjustments that make more wealth equal better chances for special materials
            const auto WEALTH_CHANCE_ADJUSTMENT(misc::ConfigFile::Instance()->ValueOrDefault<float>(
                "material-wealth-chance-base-adjustment-" + NAMEOF_ENUM_STR(PROFILE.wealthType)));

            chanceCool += WEALTH_CHANCE_ADJUSTMENT;
            chanceMetal += WEALTH_CHANCE_ADJUSTMENT;
            chancePrecious += WEALTH_CHANCE_ADJUSTMENT;

            // adjustments that make more wealth equal already acquired special materials more rare
            for (auto & nextMapPair : chanceMapCool)
            {
                nextMapPair.second += WEALTH_CHANCE_ADJUSTMENT;
            }

            for (auto & nextMapPair : chanceMapMetal)
            {
                nextMapPair.second += WEALTH_CHANCE_ADJUSTMENT;
            }

            for (auto & nextMapPair : chanceMapPrecious)
            {
                nextMapPair.second += WEALTH_CHANCE_ADJUSTMENT;
            }

            // collector adjustments
            if ((PROFILE.collectorType & collector_type::Collector) > 0)
            {
                // adjustments that make collectors more likely to have special materials
                chanceCool += collectorMaterialChanceIncreaseCool_;
                chanceMetal += collectorMaterialChanceIncreaseMetal_;
                chancePrecious += collectorMaterialChanceIncreasePrecious_;

                // adjustments that make collector's already acquired special materials more rare
                for (auto & nextMapPair : chanceMapCool)
                {
                    nextMapPair.second /= collectorMaterialChancePerCool_;
                }

                for (auto & nextMapPair : chanceMapMetal)
                {
                    nextMapPair.second /= collectorMaterialChancePerMetal_;
                }

                for (auto & nextMapPair : chanceMapPrecious)
                {
                    nextMapPair.second /= collectorMaterialChancePerPrecious_;
                }
            }

            RestrictMaterialsByComplexity(
                PROFILE.complexityType,
                chanceCool,
                chanceMapCool,
                chanceMetal,
                chanceMapMetal,
                chancePrecious,
                chanceMapPrecious);

            // final determination of which material will be secondary
            const auto SUBTOTAL { chanceCool + chanceMetal + chancePrecious };
            const auto RAND { misc::Random(0.0f, misc::Max(1.0f, SUBTOTAL)) };

            if ((SUBTOTAL < 1.0f) && (RAND < (1.0f - SUBTOTAL)))
            {
                materialsMap_OutParam.Clear();
                materialsMap_OutParam[item::Material::Count] = 1.0f;
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
                    {
                        materialsMap_OutParam = chanceMapMetal;
                    }
                    else
                    {
                        materialsMap_OutParam = chanceMapPrecious;
                    }
                }
            }

            for (auto & nextPair : materialsMap_OutParam)
            {
                if (nextPair.second < 0.0f)
                {
                    nextPair.second = 0.0f;
                }
            }
        }

        const MaterialChanceMap_t ChanceFactory::Make_MaterialChanceMap(
            const std::string & PREFIX,
            const std::string & POSTFIX,
            const item::MaterialVec_t & MATERIALS_VEC) const
        {
            auto cumulativeChance { 0.0f };
            MaterialChanceMap_t materialChanceMap;
            materialChanceMap.Reserve(64);
            auto materialWithRemainingChance(item::Material::Count);

            for (const auto NEXT_MATERIAL : MATERIALS_VEC)
            {
                const auto NEXT_VALUE_STR { misc::ConfigFile::Instance()->Value(
                    (PREFIX + NAMEOF_ENUM_STR(NEXT_MATERIAL)).append(POSTFIX)) };

                if (NEXT_VALUE_STR == "remaining")
                {
                    materialWithRemainingChance = NEXT_MATERIAL;
                }
                else
                {
                    const auto NEXT_VALUE_FLOAT {
                        misc::ConfigFile::Instance()->ValueOrDefault<float>(
                            (PREFIX + NAMEOF_ENUM_STR(NEXT_MATERIAL)).append(POSTFIX))
                    };

                    if (false == misc::IsRealClose(0.0f, NEXT_VALUE_FLOAT))
                    {
                        const auto NEXT_CHANCE { 1.0f / NEXT_VALUE_FLOAT };
                        materialChanceMap[NEXT_MATERIAL] = NEXT_CHANCE;
                        cumulativeChance += NEXT_CHANCE;
                    }
                }
            }

            if (materialWithRemainingChance < item::Material::Count)
            {
                materialChanceMap[materialWithRemainingChance] = 1.0f - cumulativeChance;
            }

            return materialChanceMap;
        }

        const MaterialChanceMap_t ChanceFactory::Make_MaterialChanceMapCool() const
        {
            const item::MaterialVec_t MATERIAL_VEC
                = { item::Material::Stone,    item::Material::Bone,   item::Material::Horn,
                    item::Material::Tooth,    item::Material::Bronze, item::Material::Jade,
                    item::Material::Obsidian, item::Material::Scales, item::Material::Lazuli,
                    item::Material::Gold,     item::Material::Pearl };

            return Make_MaterialChanceMap("material-chance-base-cool-", "-onein", MATERIAL_VEC);
        }

        const MaterialChanceMap_t ChanceFactory::Make_MaterialChanceMapPrecious() const
        {
            const item::MaterialVec_t MATERIAL_VEC
                = { item::Material::Jade,     item::Material::Amethyst, item::Material::Emerald,
                    item::Material::Silver,   item::Material::Lazuli,   item::Material::Gold,
                    item::Material::Platinum, item::Material::Ruby,     item::Material::Pearl,
                    item::Material::Sapphire, item::Material::Diamond };

            return Make_MaterialChanceMap("material-chance-base-precious-", "-onein", MATERIAL_VEC);
        }

        const MaterialChanceMap_t ChanceFactory::Make_MaterialChanceMapMetal() const
        {
            const item::MaterialVec_t MATERIAL_VEC
                = { item::Material::Tin,     item::Material::Bronze, item::Material::Iron,
                    item::Material::Steel,   item::Material::Silver, item::Material::Gold,
                    item::Material::Platinum };

            return Make_MaterialChanceMap("material-chance-base-metal-", "-onein", MATERIAL_VEC);
        }

        const ClothingChances ChanceFactory::Make_ClothingMaterialChances(
            const Profile & PROFILE, const CreaturePtr_t CHARACTER_PTR) const
        {
            ItemChances itemChancesBase;
            Make_ClothingMaterialChancesPrimary(PROFILE, CHARACTER_PTR, itemChancesBase);
            Make_MaterialChancesSecondary(PROFILE, CHARACTER_PTR, itemChancesBase.mat_map_sec);
            ;
            const std::string KEY_BASE { "inventory-clothing-" };

            const std::string WEARABLE_STR_BASE { KEY_BASE + NAMEOF_ENUM_STR(PROFILE.wealthType)
                                                  + "-chances-" };

            const auto MIN_VAL { misc::ConfigFile::Instance()->ValueOrDefault<float>(
                KEY_BASE + "chance-min") };

            const auto MAX_VAL { misc::ConfigFile::Instance()->ValueOrDefault<float>(
                KEY_BASE + "chance-max") };

            // the final resulting object that will hold the chances for all clothing articles
            ClothingChances clothingChances;

            clothingChances.boots = itemChancesBase;
            clothingChances.boots.SetCountChanceSingle(
                GetFloatFromGameDataFile(WEARABLE_STR_BASE + "boots", MIN_VAL, MAX_VAL));

            clothingChances.gloves = itemChancesBase;
            clothingChances.gloves.SetCountChanceSingle(
                GetFloatFromGameDataFile(WEARABLE_STR_BASE + "gloves", MIN_VAL, MAX_VAL));

            // lets just say everybody gets pants...
            clothingChances.pants = itemChancesBase;
            clothingChances.pants.SetCountChanceSingleCertain();

            clothingChances.shirt = itemChancesBase;
            clothingChances.shirt.SetCountChanceSingle(
                GetFloatFromGameDataFile(WEARABLE_STR_BASE + "shirt", MIN_VAL, MAX_VAL));

            clothingChances.vest = itemChancesBase;
            clothingChances.vest.SetCountChanceSingle(
                GetFloatFromGameDataFile(WEARABLE_STR_BASE + "vest", MIN_VAL, MAX_VAL));

            clothingChances.cover_map[item::Covers::Cape] = itemChancesBase;
            clothingChances.cover_map[item::Covers::Cape].SetCountChanceSingle(
                GetFloatFromGameDataFile(WEARABLE_STR_BASE + "cape", MIN_VAL, MAX_VAL));

            clothingChances.cover_map[item::Covers::Cloak] = itemChancesBase;
            clothingChances.cover_map[item::Covers::Cloak].SetCountChanceSingle(
                GetFloatFromGameDataFile(WEARABLE_STR_BASE + "cloak", MIN_VAL, MAX_VAL));

            clothingChances.cover_map[item::Covers::Robe] = itemChancesBase;
            clothingChances.cover_map[item::Covers::Robe].SetCountChanceSingle(
                GetFloatFromGameDataFile(WEARABLE_STR_BASE + "robe", MIN_VAL, MAX_VAL));

            return clothingChances;
        }

        void ChanceFactory::PopulateWeaponMaterials(
            const std::string & WEAPON_NAME,
            const MaterialChanceMap_t & TYPICAL_PRI_MATERIALS,
            const Profile & PROFILE,
            const CreaturePtr_t CREATURE_PTR,
            MaterialChanceMap_t & materialsMapPri,
            MaterialChanceMap_t & materialsMapSec) const
        {
            const auto WEAPON_DETAILS { item::WeaponDetailLoader::LookupWeaponDetails(
                WEAPON_NAME) };

            PopulatMaterials(
                TYPICAL_PRI_MATERIALS,
                PROFILE,
                CREATURE_PTR,
                materialsMapPri,
                materialsMapSec,
                WEAPON_DETAILS.weight);
        }

        void ChanceFactory::PopulatMaterials(
            const MaterialChanceMap_t & TYPICAL_PRI_MATERIALS,
            const Profile & PROFILE,
            const CreaturePtr_t CREATURE_PTR,
            MaterialChanceMap_t & materialsMapPri,
            MaterialChanceMap_t & materialsMapSec,
            const Weight_t & WEIGHT,
            const item::Material::Enum FORCED_MAT_PRI) const
        {
            if (FORCED_MAT_PRI >= item::Material::Count)
            {
                Make_MaterialChancesPrimary(
                    PROFILE, CREATURE_PTR, TYPICAL_PRI_MATERIALS, WEIGHT, materialsMapPri);
            }
            else
            {
                materialsMapPri.Clear();
                materialsMapPri[FORCED_MAT_PRI] = 1.0f;
            }

            Make_MaterialChancesSecondary(PROFILE, CREATURE_PTR, materialsMapSec);
        }

        void ChanceFactory::RestrictMaterialsByComplexity(
            const complexity_type::Enum COMPLEXITY,
            float &, // cool materials are always possible, so this value is not changed here
            MaterialChanceMap_t & chanceMapCool,
            float & chanceMetal,
            MaterialChanceMap_t & chanceMapMetal,
            float & chancePrecious,
            MaterialChanceMap_t &) const // individual precious material chances are not
                                         // changed here
        {
            // enforce creature complexity type restrictions on which materials are possible
            if (COMPLEXITY == complexity_type::Simple)
            {
                chanceMapCool[item::Material::Scales] = 0.0f;
                chanceMapCool[item::Material::Gold] = 0.0f;
                chanceMapCool[item::Material::Pearl] = 0.0f;
                NormalizeChanceMap<item::Material::Enum>(chanceMapCool);

                chanceMetal = 0.0f;

                chancePrecious = 0.0f;
            }
            else if (COMPLEXITY == complexity_type::Moderate)
            {
                chanceMapCool[item::Material::Gold] = 0.0f;
                chanceMapCool[item::Material::Pearl] = 0.0f;
                chanceMapCool[item::Material::Scales] = 0.0f;
                NormalizeChanceMap<item::Material::Enum>(chanceMapCool);

                chanceMapMetal[item::Material::Steel] = 0.0f;
                chanceMapMetal[item::Material::Silver] = 0.0f;
                chanceMapMetal[item::Material::Gold] = 0.0f;
                chanceMapMetal[item::Material::Platinum] = 0.0f;
                NormalizeChanceMap<item::Material::Enum>(chanceMapMetal);

                chancePrecious = 0.0f;
            }
        }

        bool ChanceFactory::IsWeaponPossibleConsideringComplexity(
            const std::string & WEAPON_NAME, const complexity_type::Enum CREATURE_COMPLEXITY)
        {
            const auto WEAPON_DETAILS { item::WeaponDetailLoader::LookupWeaponDetails(
                WEAPON_NAME) };

            return (WEAPON_DETAILS.complexity <= CREATURE_COMPLEXITY);
        }

        const MaterialChanceMap_t ChanceFactory::MakeTypicalArmorMaterials(
            const Profile &, const CreaturePtr_t, const bool INCLUDE_WOOD) const
        {
            MaterialChanceMap_t materialChanceMap;
            materialChanceMap.Reserve(64);

            if (INCLUDE_WOOD)
            {
                materialChanceMap[item::Material::Wood] = 0.500f;
                materialChanceMap[item::Material::Iron] = 0.250f;
                materialChanceMap[item::Material::Steel] = 0.175f;
                materialChanceMap[item::Material::Tin] = 0.075f;
            }
            else
            {
                materialChanceMap[item::Material::Iron] = 0.500f;
                materialChanceMap[item::Material::Steel] = 0.375f;
                materialChanceMap[item::Material::Tin] = 0.125f;
            }

            // adjust for wealth type
            // TODO

            // adjust for rank
            // TODO

            return materialChanceMap;
        }

        void ChanceFactory::SetArmorChancesGeneral(
            ArmorItemChances & armorItemChances,
            const RoleArmorChance & ROLE_ARMOR_CHANCE,
            const Profile & PROFILE,
            const CreaturePtr_t CREATURE_PTR,
            const bool WILL_MATERIALS_INCLUDED_WOOD) const
        {
            const auto DETAILS { item::ArmorDetailLoader::LookupArmorDetails(
                ROLE_ARMOR_CHANCE.type_wrapper.DetailsKeyName()) };

            if (PROFILE.complexityType < DETAILS.complexity)
            {
                armorItemChances.SetCountChanceSingleNoChance();
                armorItemChances.type_map.Clear();
            }
            else
            {
                armorItemChances.SetCountChanceIncrementAndEquip(ROLE_ARMOR_CHANCE.chance);
                armorItemChances.type_map[ROLE_ARMOR_CHANCE.type_wrapper.Form()]
                    = ROLE_ARMOR_CHANCE.chance;

                PopulatMaterials(
                    MakeTypicalArmorMaterials(PROFILE, CREATURE_PTR, WILL_MATERIALS_INCLUDED_WOOD),
                    PROFILE,
                    CREATURE_PTR,
                    armorItemChances.mat_map_pri,
                    armorItemChances.mat_map_sec,
                    DETAILS.weight);
            }
        }

        void ChanceFactory::SetArmorChancesSpecific(
            ItemChances & itemChances,
            const RoleArmorChance & ROLE_ARMOR_CHANCE,
            const Profile & PROFILE,
            const CreaturePtr_t CHARACTER_PTR,
            const bool WILL_MATERIALS_INCLUDED_WOOD,
            const item::Material::Enum FORCED_MAT_PRI) const
        {
            const auto DETAILS { item::ArmorDetailLoader::LookupArmorDetails(
                ROLE_ARMOR_CHANCE.type_wrapper.DetailsKeyName()) };

            if (PROFILE.complexityType < DETAILS.complexity)
            {
                itemChances.SetCountChanceSingleNoChance();
            }
            else
            {
                itemChances.SetCountChanceIncrementAndEquip(ROLE_ARMOR_CHANCE.chance);

                PopulatMaterials(
                    MakeTypicalArmorMaterials(PROFILE, CHARACTER_PTR, WILL_MATERIALS_INCLUDED_WOOD),
                    PROFILE,
                    CHARACTER_PTR,
                    itemChances.mat_map_pri,
                    itemChances.mat_map_sec,
                    DETAILS.weight,
                    FORCED_MAT_PRI);
            }
        }

        void ChanceFactory::CacheRoleArmorChances()
        {
            roleArmorChanceMap_.Clear();

            for (EnumUnderlying_t i(0); i < role::Count; ++i)
            {
                const auto ROLE { static_cast<role::Enum>(i) };

                const auto ROLE_STR { NAMEOF_ENUM_STR(ROLE) };
                const auto KEY_STR { "nonplayer-armor-chances-role-" + ROLE_STR };
                const auto VALUE_STR { misc::ConfigFile::Instance()->Value(KEY_STR) };

                const std::vector<std::string> ARMOR_CHANCES_VEC { misc::SplitByChars(
                    VALUE_STR, misc::SplitHow("|")) };

                RoleArmorChanceVec_t roleArmorChanceVec;

                for (const auto & NEXT_ARMOR_CHANCE_STR : ARMOR_CHANCES_VEC)
                {
                    const std::vector<std::string> PIECES_VEC { misc::SplitByChars(
                        NEXT_ARMOR_CHANCE_STR, misc::SplitHow(",")) };

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        (PIECES_VEC.size() >= 2),
                        "nonplayerChanceFactory::CacheRoleArmorChances() role=\""
                            << ROLE_STR << "\") found value-str=\"" << VALUE_STR
                            << "\" which failed to be parsed into the required 2  or more comma "
                               "sep "
                               "fields.");

                    // note that since we skipped empties in SplitByChars() call above, this string
                    // will not be empty
                    const auto ARMOR_NAME_STR { PIECES_VEC.front() };

                    const auto ARMOR_CHANCE_STR { PIECES_VEC.back() };

                    const auto INVALID_CHANCE { -1.0f };
                    auto armorChanceVal = misc::ToNumberOr(ARMOR_CHANCE_STR, INVALID_CHANCE);

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        (!misc::IsRealClose(armorChanceVal, INVALID_CHANCE)),
                        "nonplayerChanceFactory::CacheRoleArmorChances() role=\""
                            << ROLE_STR << "\") found value-str=\"" << VALUE_STR
                            << "\" which had float str=\"" << ARMOR_CHANCE_STR
                            << "\" val=" << armorChanceVal
                            << " which was unable to be parsed into a valid (must != "
                            << INVALID_CHANCE << ") float.");

                    // I must have been lazy and left zeros in the mix...zTn  2017-6-24
                    if (misc::IsRealZero(armorChanceVal))
                    {
                        // if there is a zero chance then just skip it
                        continue;
                    }

                    item::Forms::Enum baseType { item::Forms::Count };
                    const auto HAS_BASE_TYPE_STR { PIECES_VEC.size() > 2 };

                    if (HAS_BASE_TYPE_STR)
                    {
                        baseType = EnumUtil<item::Forms>::FromString(PIECES_VEC[1]);

                        M_HP_ASSERT_OR_LOG_AND_THROW(
                            (baseType < item::Forms::Count),
                            "nonplayerChanceFactor::CacheRoleArmorChances() role=\""
                                << ROLE_STR << "\") found value-str=\"" << VALUE_STR
                                << "\" which had more than two comma sep fields, but the second "
                                   "field "
                                   "failed "
                                << "to be parsed as a valid item::Forms::Enum.");

                        M_HP_ASSERT_OR_LOG_AND_THROW(
                            (baseType != item::Forms::Plain),
                            "nonplayerChanceFactor::CacheRoleArmorChances() role=\""
                                << ROLE_STR << "\") found value-str=\"" << VALUE_STR
                                << "\" which had more than two comma sep fields, but the second "
                                   "field "
                                   "failed "
                                << "to be parsed as a valid non-Plain "
                                   "item::Forms::Enum.");
                    }

                    const item::ArmorTemplate ARMOR_TEMPLATE(
                        ARMOR_NAME_STR, baseType, true);

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        (ARMOR_TEMPLATE.IsTypeValid()),
                        "nonplayerChanceFactor::CacheRoleArmorChances() role = \""
                            << ROLE_STR << "\", NEXT_ARMOR_CHANCE_STR=\"" << NEXT_ARMOR_CHANCE_STR
                            << "\", failed to create a valid ArmorTemplate.");

                    roleArmorChanceVec.emplace_back(
                        RoleArmorChance(ARMOR_TEMPLATE, armorChanceVal));
                }

                roleArmorChanceMap_[ROLE] = roleArmorChanceVec;
            }
        }

        void ChanceFactory::CacheRoleWeaponChances()
        {
            roleWeaponChanceMap_.Clear();

            for (EnumUnderlying_t i(0); i < role::Count; ++i)
            {
                const auto ROLE { static_cast<role::Enum>(i) };
                roleWeaponChanceMap_[ROLE] = GetRoleWeaponChances(ROLE);
            }
        }

        const ChanceFactory::WeaponSetVec_t
            ChanceFactory::GetRoleWeaponChances(const role::Enum ROLE)
        {
            ChanceFactory::WeaponSetVec_t weaponSets;

            namespace ba = boost::algorithm;

            const auto ROLE_STR { NAMEOF_ENUM_STR(ROLE) };
            const std::string KEY_STR("nonplayer-weapon-chances-role-" + ROLE_STR);

            const auto VALUE_STR_LOWER { misc::ToLowerCopy(
                misc::ConfigFile::Instance()->Value(KEY_STR)) };

            const std::vector<std::string> WEAPONS_SET_VEC { misc::SplitByChars(
                VALUE_STR_LOWER, misc::SplitHow("{}")) };

            // Loop over each weapon set, denoted by curly brackets {} in the GameDataFile.
            for (const auto & NEXT_WEAPON_SET_STR : WEAPONS_SET_VEC)
            {
                WeaponSet nextWeaponSet;

                const std::vector<std::string> INSTRUCTIONS_VEC { misc::SplitByChars(
                    NEXT_WEAPON_SET_STR, misc::SplitHow("|")) };

                // Loop over each instruction, denoted by the pipe character in the GameDataFile.
                // A collection of instructions composes a WeaponSet
                for (const auto & NEXT_INSTRUCTION_STR : INSTRUCTIONS_VEC)
                {
                    if (ba::starts_with(NEXT_INSTRUCTION_STR, "["))
                    {
                        const auto PICK_CSTR { "[pick-" };
                        if (ba::starts_with(NEXT_INSTRUCTION_STR, PICK_CSTR))
                        {
                            // assume the instruction takes the form: [pick-<number>],
                            // where <number> must be a positive integer greater than zero
                            const auto COUNT_STR { ba::erase_first_copy(
                                ba::erase_first_copy(NEXT_INSTRUCTION_STR, PICK_CSTR), "]") };

                            nextWeaponSet.count = misc::ToNumberOrZero<std::size_t>(COUNT_STR);

                            M_HP_ASSERT_OR_LOG_AND_THROW(
                                (nextWeaponSet.count > 0),
                                "nonplayerLookupPossibleWeaponsByRole(role="
                                    << ROLE_STR << ") with KEY=\"" << KEY_STR << "\" and VALUE=\""
                                    << VALUE_STR_LOWER << "\" found INSTRUCTION=\""
                                    << NEXT_INSTRUCTION_STR
                                    << "\" and parsed pick count string to be \"" << COUNT_STR
                                    << "\", but failed to convert it into a valid (> zero) "
                                       "std::size_t.  "
                                    << "It was converted into " << nextWeaponSet.count << ".");
                        }
                        else if (NEXT_INSTRUCTION_STR == "[body]")
                        {
                            nextWeaponSet.chanceMap.Clear();
                        }
                        else
                        {
                            M_HP_LOG(
                                "nonplayerLookupPossibleWeaponsByRole(role="
                                << ROLE_STR << ") with KEY=\"" << KEY_STR << "\" and VALUE=\""
                                << VALUE_STR_LOWER << "\" found INSTRUCTION=\""
                                << NEXT_INSTRUCTION_STR
                                << "\" which was not recognized, and will be ignored.");
                        }
                    }
                    else
                    {
                        // At this point, assume the instruction is in the form of:
                        // <weapon name>,<chance float> or (<weapon name>),<chance float>
                        //-so it must have two comma separated strings.
                        const std::vector<std::string> PARTS_VEC { misc::SplitByChars(
                            NEXT_INSTRUCTION_STR, misc::SplitHow(",")) };

                        M_HP_ASSERT_OR_LOG_AND_THROW(
                            ((PARTS_VEC.size() > 1) && (PARTS_VEC.at(0).size() > 2)
                             && (PARTS_VEC.at(1).size() > 2)),
                            "nonplayerLookupPossibleWeaponsByRole(role="
                                << ROLE_STR << ") with KEY=\"" << KEY_STR << "\" and VALUE=\""
                                << VALUE_STR_LOWER << "\" found INSTRUCTION=\""
                                << NEXT_INSTRUCTION_STR
                                << "\" but was unable to parse as at least two strings separated "
                                   "by "
                                << "a comma.");

                        const auto WEAPON_NAME { PARTS_VEC[0] };
                        const auto CHANCE_STR { PARTS_VEC[1] };

                        const item::WeaponTemplate WEAPON_INFO(WEAPON_NAME);

                        // parse weapon chance
                        float chance = misc::ToNumberOrZero<float>(CHANCE_STR);

                        M_HP_ASSERT_OR_LOG_AND_THROW(
                            (chance > 0.0f),
                            "nonplayerLookupPossibleWeaponsByRole(role="
                                << ROLE_STR << ") with KEY=\"" << KEY_STR << "\" and VALUE=\""
                                << VALUE_STR_LOWER << "\" found INSTRUCTION=\""
                                << NEXT_INSTRUCTION_STR << "\", weapon name=\"" << WEAPON_NAME
                                << "\", and chance str=\"" << CHANCE_STR
                                << "\", but was unable to convert this into a valid "
                                << "(> zero) float.  Converted instead to:" << chance << ".");

                        nextWeaponSet.chanceMap[WEAPON_INFO] = chance;
                    }
                }

                weaponSets.emplace_back(nextWeaponSet);
            }

            return weaponSets;
        }

    } // namespace nonplayer
} // namespace creature
} // namespace heroespath
*/
