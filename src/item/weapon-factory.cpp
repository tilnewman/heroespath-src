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
// weapon-facotry.cpp
//
#include "weapon-factory.hpp"

#include "creature/creature.hpp"
#include "item/armor-info.hpp"
#include "item/item-profile.hpp"
#include "item/item-type-enum.hpp"
#include "item/item-warehouse.hpp"
#include "item/item.hpp"
#include "sfml-util/gui/item-image-manager.hpp"
#include "stats/trait.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace item
{
    namespace weapon
    {

        std::unique_ptr<WeaponFactory> WeaponFactory::instanceUPtr_;
        std::unique_ptr<WeaponDetailLoader> WeaponFactory::weaponDetailsUPtr_;

        WeaponFactory::WeaponFactory() { M_HP_LOG_DBG("Singleton Construction: WeaponFactory"); }

        WeaponFactory::~WeaponFactory() { M_HP_LOG_DBG("Singleton Destruction: WeaponFactory"); }

        misc::NotNull<WeaponFactory *> WeaponFactory::Instance()
        {
            if (!instanceUPtr_)
            {
                M_HP_LOG_ERR("Singleton Instance() before Acquire(): WeaponFactory");
                Acquire();
            }

            return instanceUPtr_.get();
        }

        void WeaponFactory::Acquire()
        {
            if (!instanceUPtr_)
            {
                weaponDetailsUPtr_ = std::make_unique<WeaponDetailLoader>();
                instanceUPtr_ = std::make_unique<WeaponFactory>();
            }
            else
            {
                M_HP_LOG_ERR("Singleton Acquire() after Construction: WeaponFactory");
            }
        }

        void WeaponFactory::Release()
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (instanceUPtr_),
                "item::WeaponFactory::Release() found instanceUPtr that was null.");

            instanceUPtr_.reset();
        }

        const ItemPtr_t WeaponFactory::Make_Fists(const TypeWrapper & TYPE_WRAPPER)
        {
            WeaponInfo weaponInfo{ weapon_type::Fists };
            weaponInfo.is_fists = true;

            const WeaponDetails FISTS_DETAILS{ weaponDetailsUPtr_->LookupWeaponDetails("Fists") };

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                FISTS_DETAILS.name,
                FISTS_DETAILS.description,
                static_cast<category::Enum>(
                    category::Weapon | category::Equippable | category::TwoHanded
                    | category::BodyPart),
                static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Fists),
                armor_type::NotArmor,
                material::Bone,
                material::Flesh,
                FISTS_DETAILS.price,
                FISTS_DETAILS.weight,
                FISTS_DETAILS.damage_min,
                FISTS_DETAILS.damage_max,
                0_armor,
                TYPE_WRAPPER,
                weaponInfo));
        }

        const ItemPtr_t WeaponFactory::Make_Claws(
            const creature::CreaturePtr_t CREATURE_PTR, const TypeWrapper & TYPE_WRAPPER)
        {
            WeaponInfo weaponInfo{ weapon_type::Claws };
            weaponInfo.is_claws = true;

            const WeaponDetails CLAWS_DETAILS(weaponDetailsUPtr_->LookupWeaponDetails("Claws"));

            std::ostringstream ssName;
            ssName << CLAWS_DETAILS.name;

            std::ostringstream ssDesc;
            ssDesc << "The sharp tearing claws of a " << CREATURE_PTR->RaceName() << ".";

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                ssName.str(),
                ssDesc.str(),
                static_cast<category::Enum>(
                    category::Weapon | category::Equippable | category::TwoHanded
                    | category::BodyPart),
                static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Claws),
                armor_type::NotArmor,
                material::Claw,
                material::Nothing,
                CLAWS_DETAILS.price,
                CLAWS_DETAILS.weight,
                CLAWS_DETAILS.damage_min,
                CLAWS_DETAILS.damage_max,
                0_armor,
                TYPE_WRAPPER,
                weaponInfo));
        }

        const ItemPtr_t WeaponFactory::Make_Tendrils(
            const creature::CreaturePtr_t CREATURE_PTR, const TypeWrapper & TYPE_WRAPPER)
        {
            WeaponInfo weaponInfo{ weapon_type::Tendrils };
            weaponInfo.is_tendrils = true;

            const WeaponDetails TENDRIL_DETAILS(
                weaponDetailsUPtr_->LookupWeaponDetails("Tendrils"));

            std::ostringstream ssName;
            ssName << TENDRIL_DETAILS.name;

            std::ostringstream ssDesc;
            ssDesc << "The writhing lengths of a " << CREATURE_PTR->RaceName() << ".";

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                ssName.str(),
                ssDesc.str(),
                static_cast<category::Enum>(
                    category::Weapon | category::Equippable | category::TwoHanded
                    | category::BodyPart),
                static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Tendrils),
                armor_type::NotArmor,
                material::Flesh,
                material::Nothing,
                TENDRIL_DETAILS.price,
                TENDRIL_DETAILS.weight,
                TENDRIL_DETAILS.damage_min,
                TENDRIL_DETAILS.damage_max,
                0_armor,
                TYPE_WRAPPER,
                weaponInfo));
        }

        const ItemPtr_t WeaponFactory::Make_Bite(
            const creature::CreaturePtr_t CREATURE_PTR, const TypeWrapper & TYPE_WRAPPER)
        {
            WeaponInfo weaponInfo{ weapon_type::Bite };
            weaponInfo.is_bite = true;

            const WeaponDetails BITE_DETAILS(weaponDetailsUPtr_->LookupWeaponDetails("Bite"));

            std::ostringstream ssName;
            ssName << BITE_DETAILS.name;

            std::ostringstream ssDesc;
            ssDesc << "The fanged jaws of a " << CREATURE_PTR->RaceName() << ".";

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                ssName.str(),
                ssDesc.str(),
                static_cast<category::Enum>(
                    category::Weapon | category::Equippable | category::BodyPart),
                static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Bite),
                armor_type::NotArmor,
                material::Tooth,
                material::Nothing,
                BITE_DETAILS.price,
                BITE_DETAILS.weight,
                BITE_DETAILS.damage_min,
                BITE_DETAILS.damage_max,
                0_armor,
                TYPE_WRAPPER,
                weaponInfo));
        }

        const ItemPtr_t WeaponFactory::Make_Breath(
            const creature::CreaturePtr_t CREATURE_PTR, const TypeWrapper & TYPE_WRAPPER)
        {
            WeaponInfo weaponInfo{ weapon_type::Breath };
            weaponInfo.is_breath = true;

            // Hydras and other dragons use the Firebrand breath weapon.
            auto const BREATH_DETAILS_TITLE{ (
                (CREATURE_PTR->Role() == creature::role::Sylavin) ? "BreathSylavin"
                                                                  : "BreathFirebrand") };

            auto const BREATH_DETAILS{ weaponDetailsUPtr_->LookupWeaponDetails(
                BREATH_DETAILS_TITLE) };

            std::ostringstream ssName;
            ssName << ((CREATURE_PTR->Role() == creature::role::Firebrand) ? "Fiery"
                                                                           : "Toxic Freezing")
                   << " Breath";

            std::ostringstream ssDesc;
            if (CREATURE_PTR->Role() == creature::role::Sylavin)
            {
                ssDesc << "The toxic and freezing breath of a Sylavin dragon.";
            }
            else
            {
                ssDesc << "The fiery breath of a dragon.";
            }

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                ssName.str(),
                ssDesc.str(),
                static_cast<category::Enum>(
                    category::Weapon | category::Equippable | category::BodyPart),
                static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Breath),
                armor_type::NotArmor,
                material::Gas,
                material::Nothing,
                BREATH_DETAILS.price,
                BREATH_DETAILS.weight,
                BREATH_DETAILS.damage_min,
                BREATH_DETAILS.damage_max,
                0_armor,
                TYPE_WRAPPER,
                weaponInfo));
        }

        const ItemPtr_t WeaponFactory::Make_Knife(const ItemProfile & PROFILE)
        {
            return Make_Knife(
                TypeWrapper(PROFILE),
                PROFILE.IsDagger(),
                PROFILE.Size(),
                PROFILE.MaterialPrimary(),
                PROFILE.MaterialSecondary(),
                PROFILE.IsPixie());
        }

        const ItemPtr_t WeaponFactory::Make_Knife(
            const TypeWrapper & TYPE_WRAPPER,
            const bool IS_DAGGER,
            const sfml_util::Size::Enum SIZE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const bool IS_PIXIE_ITEM)
        {
            WeaponInfo weaponInfo{ weapon_type::Knife };
            weaponInfo.is_dagger = IS_DAGGER;
            weaponInfo.is_knife = !IS_DAGGER;
            weaponInfo.knife_size = SIZE;

            const WeaponDetails KNIFE_DETAILS{ weaponDetailsUPtr_->LookupWeaponDetails("Knife") };

            auto price{ KNIFE_DETAILS.price + material::PriceAdj(MATERIAL_PRI, MATERIAL_SEC) };

            auto weight{ Weight_t(static_cast<Weight_t::type>(
                KNIFE_DETAILS.weight.As<float>()
                * material::WeightMult(MATERIAL_PRI, MATERIAL_SEC))) };

            Health_t damageMin{ KNIFE_DETAILS.damage_min };
            Health_t damageMax{ KNIFE_DETAILS.damage_max };

            std::string nameDescriptor{ "" };

            if (SIZE == sfml_util::Size::Large)
            {
                nameDescriptor = "Large ";
                weight *= 2_weight;
                price += 10_coin;
                damageMin += 1_health;
                damageMax += 1_health;
            }
            else if (SIZE == sfml_util::Size::Small)
            {
                nameDescriptor = "Small ";
                weight /= 2_weight;
                price -= 10_coin;
                damageMin -= 1_health;
                damageMax -= 1_health;
            }

            std::string baseName("");
            if (IS_DAGGER)
            {
                baseName = "Dagger";
                damageMin += 1_health;
                damageMax += 1_health;
            }
            else
            {
                baseName = "Knife";
                damageMin -= 1_health;
                damageMax -= 1_health;
            }

            if (IS_PIXIE_ITEM)
            {
                damageMin = 1_health;
                damageMax = 2_health;
            }

            std::ostringstream ssName;
            ssName << nameDescriptor << baseName;

            std::ostringstream ssDesc;
            ssDesc << nameDescriptor << baseName;

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                Make_Name(ssName.str(), MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                Make_Desc(ssDesc.str(), MATERIAL_PRI, MATERIAL_SEC, "", IS_PIXIE_ITEM),
                static_cast<category::Enum>(
                    category::Equippable | category::Weapon | category::OneHanded),
                static_cast<weapon_type::Enum>(
                    weapon_type::Bladed | weapon_type::Knife | weapon_type::Melee
                    | weapon_type::Pointed),
                armor_type::NotArmor,
                MATERIAL_PRI,
                MATERIAL_SEC,
                price,
                weight,
                damageMin,
                damageMax,
                0_armor,
                TYPE_WRAPPER,
                weaponInfo,
                armor::ArmorInfo(),
                IS_PIXIE_ITEM));
        }

        const ItemPtr_t WeaponFactory::Make_Sword(const ItemProfile & PROFILE)
        {
            return Make_Sword(
                PROFILE.SwordType(),
                PROFILE.MaterialPrimary(),
                PROFILE.MaterialSecondary(),
                TypeWrapper(PROFILE));
        }

        const ItemPtr_t WeaponFactory::Make_Sword(
            const sword_type::Enum SWORD_TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const TypeWrapper & TYPE_WRAPPER)
        {
            WeaponInfo weaponInfo{ weapon_type::Sword };
            weaponInfo.sword = SWORD_TYPE;

            creature::role::Enum exclusiveRole(creature::role::Count);
            if ((SWORD_TYPE == sword_type::Knightlysword) || (SWORD_TYPE == sword_type::Claymore))
            {
                exclusiveRole = creature::role::Knight;
            }

            auto const DETAILS{ weaponDetailsUPtr_->LookupWeaponDetails(
                sword_type::ToString(SWORD_TYPE)) };

            auto price{ DETAILS.price };
            auto weight{ DETAILS.weight };

            AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC);
            AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC),
                Make_Desc(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC),
                static_cast<category::Enum>(
                    category::Weapon | category::Equippable | DETAILS.handedness),
                static_cast<weapon_type::Enum>(
                    weapon_type::Melee | weapon_type::Bladed | weapon_type::Pointed
                    | weapon_type::Sword),
                armor_type::NotArmor,
                MATERIAL_PRI,
                MATERIAL_SEC,
                price,
                weight,
                DETAILS.damage_min,
                DETAILS.damage_max,
                0_armor,
                TYPE_WRAPPER,
                weaponInfo,
                armor::ArmorInfo(),
                false,
                exclusiveRole));
        }

        const ItemPtr_t WeaponFactory::Make_Axe(const ItemProfile & PROFILE)
        {
            return Make_Axe(
                PROFILE.AxeType(),
                PROFILE.MaterialPrimary(),
                PROFILE.MaterialSecondary(),
                TypeWrapper(PROFILE));
        }

        const ItemPtr_t WeaponFactory::Make_Axe(
            const axe_type::Enum AXE_TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const TypeWrapper & TYPE_WRAPPER)
        {
            WeaponInfo weaponInfo{ weapon_type::Axe };
            weaponInfo.axe = AXE_TYPE;

            auto const DETAILS{ weaponDetailsUPtr_->LookupWeaponDetails(
                axe_type::ToString(AXE_TYPE)) };

            auto price{ DETAILS.price };
            auto weight{ DETAILS.weight };

            AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC);
            AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC),
                Make_Desc(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC),
                static_cast<category::Enum>(
                    category::Weapon | category::Equippable | DETAILS.handedness),
                static_cast<weapon_type::Enum>(
                    weapon_type::Melee | weapon_type::Bladed | weapon_type::Axe),
                armor_type::NotArmor,
                MATERIAL_PRI,
                MATERIAL_SEC,
                price,
                weight,
                DETAILS.damage_min,
                DETAILS.damage_max,
                0_armor,
                TYPE_WRAPPER,
                weaponInfo));
        }

        const ItemPtr_t WeaponFactory::Make_Club(const ItemProfile & PROFILE)
        {
            return Make_Club(
                PROFILE.ClubType(),
                PROFILE.MaterialPrimary(),
                PROFILE.MaterialSecondary(),
                TypeWrapper(PROFILE));
        }

        const ItemPtr_t WeaponFactory::Make_Club(
            const club_type::Enum CLUB_TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const TypeWrapper & TYPE_WRAPPER)
        {
            WeaponInfo weaponInfo{ weapon_type::Club };
            weaponInfo.club = CLUB_TYPE;

            auto const DETAILS{ weaponDetailsUPtr_->LookupWeaponDetails(
                club_type::ToString(CLUB_TYPE)) };

            auto price{ DETAILS.price };
            auto weight{ DETAILS.weight };

            AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC);
            AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC),
                Make_Desc(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC),
                static_cast<category::Enum>(
                    category::Weapon | category::Equippable | DETAILS.handedness),
                static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Club),
                armor_type::NotArmor,
                MATERIAL_PRI,
                MATERIAL_SEC,
                price,
                weight,
                DETAILS.damage_min,
                DETAILS.damage_min,
                0_armor,
                TYPE_WRAPPER,
                weaponInfo));
        }

        const ItemPtr_t WeaponFactory::Make_Whip(const ItemProfile & PROFILE)
        {
            return Make_Whip(
                PROFILE.WhipType(),
                PROFILE.MaterialPrimary(),
                PROFILE.MaterialSecondary(),
                TypeWrapper(PROFILE));
        }

        const ItemPtr_t WeaponFactory::Make_Whip(
            const whip_type::Enum WHIP_TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const TypeWrapper & TYPE_WRAPPER)
        {
            WeaponInfo weaponInfo{ weapon_type::Whip };
            weaponInfo.whip = WHIP_TYPE;

            auto const DETAILS{ weaponDetailsUPtr_->LookupWeaponDetails(
                whip_type::ToString(WHIP_TYPE)) };

            auto price{ DETAILS.price };
            auto weight{ DETAILS.weight };

            AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC);
            AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC),
                Make_Desc(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC, "handle"),
                static_cast<category::Enum>(
                    category::Weapon | category::Equippable | DETAILS.handedness),
                static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Whip),
                armor_type::NotArmor,
                MATERIAL_PRI,
                MATERIAL_SEC,
                price,
                weight,
                DETAILS.damage_min,
                DETAILS.damage_max,
                0_armor,
                TYPE_WRAPPER,
                weaponInfo));
        }

        const ItemPtr_t WeaponFactory::Make_Projectile(const ItemProfile & PROFILE)
        {
            return Make_Projectile(
                PROFILE.ProjectileType(),
                PROFILE.MaterialPrimary(),
                PROFILE.MaterialSecondary(),
                TypeWrapper(PROFILE));
        }

        const ItemPtr_t WeaponFactory::Make_Projectile(
            const projectile_type::Enum PROJ_TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const TypeWrapper & TYPE_WRAPPER)
        {
            WeaponInfo weaponInfo{ weapon_type::Projectile };
            weaponInfo.projectile = PROJ_TYPE;

            auto const DETAILS{ weaponDetailsUPtr_->LookupWeaponDetails(
                projectile_type::ToString(PROJ_TYPE)) };

            auto price{ DETAILS.price };
            auto weight{ DETAILS.weight };

            auto weaponType{ weapon_type::NotAWeapon };

            switch (PROJ_TYPE)
            {
                case projectile_type::Blowpipe:
                {
                    weaponType = weapon_type::Blowpipe;
                    break;
                }
                case projectile_type::Sling:
                {
                    weaponType = weapon_type::Sling;
                    break;
                }
                case projectile_type::Shortbow:
                case projectile_type::Longbow:
                {
                    weaponType = weapon_type::Bow;
                    break;
                }
                case projectile_type::CompositeBow:
                {
                    weaponType = weapon_type::Bow;
                    break;
                }
                case projectile_type::Crossbow:
                {
                    weaponType = weapon_type::Crossbow;
                    break;
                }
                case projectile_type::Count:
                default:
                {
                    std::ostringstream ss;
                    ss << "herospath::item::WeaponFactory::Make_Projectile(projectile_type="
                       << PROJ_TYPE << ")_InvalidValueError.";
                    throw std::range_error(ss.str());
                }
            }

            AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC);
            AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC),
                Make_Desc(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC, "grip"),
                static_cast<category::Enum>(
                    category::Weapon | category::TwoHanded | category::Equippable
                    | DETAILS.handedness),
                static_cast<weapon_type::Enum>(weapon_type::Projectile | weaponType),
                armor_type::NotArmor,
                MATERIAL_PRI,
                MATERIAL_SEC,
                price,
                weight,
                DETAILS.damage_min,
                DETAILS.damage_max,
                0_armor,
                TYPE_WRAPPER,
                weaponInfo));
        }

        const ItemPtr_t WeaponFactory::Make_Staff(const ItemProfile & PROFILE)
        {
            return Make_Staff(
                PROFILE.IsQuarterStaff(),
                PROFILE.MaterialPrimary(),
                PROFILE.MaterialSecondary(),
                PROFILE.IsPixie(),
                TypeWrapper(PROFILE));
        }

        const ItemPtr_t WeaponFactory::Make_Staff(
            const bool IS_QUARTERSTAFF,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const bool IS_PIXIE_ITEM,
            const TypeWrapper & TYPE_WRAPPER)
        {
            WeaponInfo weaponInfo{ weapon_type::Staff };
            weaponInfo.is_staff = !IS_QUARTERSTAFF;
            weaponInfo.is_quarterstaff = IS_QUARTERSTAFF;

            auto const DETAILS{ weaponDetailsUPtr_->LookupWeaponDetails(
                ((IS_QUARTERSTAFF) ? "Quarterstaff" : "Staff")) };

            auto price{ DETAILS.price };
            auto weight{ DETAILS.weight };

            AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM);
            AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                Make_Desc(
                    std::string("A ").append(DETAILS.name),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    "",
                    IS_PIXIE_ITEM),
                static_cast<category::Enum>(
                    category::Weapon | category::Equippable | DETAILS.handedness),
                static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Staff),
                armor_type::NotArmor,
                MATERIAL_PRI,
                MATERIAL_SEC,
                price,
                weight,
                DETAILS.damage_min,
                DETAILS.damage_max,
                0_armor,
                TYPE_WRAPPER,
                weaponInfo,
                armor::ArmorInfo(),
                IS_PIXIE_ITEM));
        }

        const ItemPtr_t WeaponFactory::Make_Staff_AsMisc(const ItemProfile & PROFILE)
        {
            Coin_t price{ TreasureScoreToCoins(PROFILE.TreasureScore()) };

            AdjustPrice(
                price, PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary(), PROFILE.IsPixie());

            Weight_t weight{ 47_weight };
            AdjustWeight(weight, PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary());

            auto const BASE_NAME{ item::misc_type::Name(PROFILE.MiscType()) };

            auto const DESC_PREFIX{ "A " + boost::to_lower_copy(BASE_NAME) + " " };

            weapon::WeaponInfo weaponInfo(weapon_type::Staff);
            weaponInfo.is_staff = true;
            weaponInfo.is_quarterstaff = false;
            auto const DETAILS{ weaponDetailsUPtr_->LookupWeaponDetails("Staff") };

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                Make_Name(
                    BASE_NAME,
                    PROFILE.MaterialPrimary(),
                    PROFILE.MaterialSecondary(),
                    PROFILE.IsPixie()),
                Make_Desc(
                    DESC_PREFIX,
                    PROFILE.MaterialPrimary(),
                    PROFILE.MaterialSecondary(),
                    "",
                    PROFILE.IsPixie()),
                PROFILE.Category(),
                PROFILE.WeaponType(),
                armor_type::NotArmor,
                PROFILE.MaterialPrimary(),
                PROFILE.MaterialSecondary(),
                price,
                weight,
                DETAILS.damage_min,
                DETAILS.damage_max,
                0_armor,
                TypeWrapper(PROFILE),
                weaponInfo,
                armor::ArmorInfo(),
                PROFILE.IsPixie()));
        }

        const ItemPtr_t WeaponFactory::Make_BladedStaff(const ItemProfile & PROFILE)
        {
            return Make_BladedStaff(
                PROFILE.BladedStaffType(),
                PROFILE.MaterialPrimary(),
                PROFILE.MaterialSecondary(),
                TypeWrapper(PROFILE));
        }

        const ItemPtr_t WeaponFactory::Make_BladedStaff(
            const bladedstaff_type::Enum STAFF_TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const TypeWrapper & TYPE_WRAPPER)
        {
            WeaponInfo weaponInfo{ weapon_type::BladedStaff };
            weaponInfo.bladedstaff = STAFF_TYPE;

            auto const DETAILS{ weaponDetailsUPtr_->LookupWeaponDetails(
                bladedstaff_type::ToString(STAFF_TYPE)) };

            auto price{ DETAILS.price };
            auto weight{ DETAILS.weight };

            AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC);
            AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

            auto spearWeapon{ weapon_type::BladedStaff };

            auto const IS_SPEAR{ (
                (STAFF_TYPE == bladedstaff_type::Spear)
                || (STAFF_TYPE == bladedstaff_type::ShortSpear)) };

            if (IS_SPEAR)
            {
                spearWeapon = weapon_type::Spear;
            }

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC),
                Make_Desc_BladdedStaff(DETAILS.name, IS_SPEAR, MATERIAL_PRI, MATERIAL_SEC),
                static_cast<category::Enum>(
                    category::Weapon | category::Equippable | DETAILS.handedness),
                static_cast<weapon_type::Enum>(
                    weapon_type::Melee | weapon_type::BladedStaff | spearWeapon),
                armor_type::NotArmor,
                MATERIAL_PRI,
                MATERIAL_SEC,
                price,
                weight,
                DETAILS.damage_min,
                DETAILS.damage_max,
                0_armor,
                TYPE_WRAPPER,
                weaponInfo));
        }

    } // namespace weapon
} // namespace item
} // namespace heroespath
