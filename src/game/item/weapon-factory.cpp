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

#include "game/stats/types.hpp"
#include "game/item/item.hpp"
#include "game/item/item-type-enum.hpp"
#include "game/item/weapon-details.hpp"
#include "game/item/armor-info.hpp"
#include "game/creature/creature.hpp"

#include <exception>
#include <sstream>


namespace game
{
namespace item
{
namespace weapon
{

    WeaponFactorySPtr_t WeaponFactory::instance_(nullptr);


    WeaponFactory::WeaponFactory()
    {}


    WeaponFactory::~WeaponFactory()
    {}


    WeaponFactorySPtr_t WeaponFactory::Instance()
    {
        if (instance_.get() == nullptr)
            instance_.reset( new WeaponFactory );

        return instance_;
    }


    ItemSPtr_t WeaponFactory::Make_Fists()
    {
        WeaponInfo weaponInfo(weapon_type::Fists);
        weaponInfo.is_fists = true;

        const WeaponDetails FISTS_DETAILS( WeaponDetailLoader::Instance()->LookupWeaponDetails("Fists") );
        return ItemSPtr_t( new Item(FISTS_DETAILS.name,
                                     FISTS_DETAILS.description,
                                     static_cast<category::Enum>(category::Weapon | category::Equippable | category::TwoHanded | category::BodyPart),
                                     misc_type::NotMisc,
                                     static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Fists),
                                     armor_type::NotArmor,
                                     material::Bone,
                                     material::Flesh,
                                     FISTS_DETAILS.image_filename,
                                     FISTS_DETAILS.price,
                                     FISTS_DETAILS.weight,
                                     FISTS_DETAILS.damage_min,
                                     FISTS_DETAILS.damage_max,
                                     0,
                                     creature::role::Count,
                                     weaponInfo) );
    }


    ItemSPtr_t WeaponFactory::Make_Claws(creature::CreatureCPtrC_t CREATURE_CPTRC)
    {
        WeaponInfo weaponInfo(weapon_type::Claws);
        weaponInfo.is_claws = true;

        const WeaponDetails CLAWS_DETAILS( WeaponDetailLoader::Instance()->LookupWeaponDetails("Claws") );

        std::ostringstream ssName;
        ssName << CLAWS_DETAILS.name;

        std::ostringstream ssDesc;
        ssDesc << "The sharp tearing claws of a " << CREATURE_CPTRC->Race().Name() << ".";

        return ItemSPtr_t( new Item(ssName.str(),
                                     ssDesc.str(),
                                     static_cast<category::Enum>(category::Weapon | category::Equippable | category::TwoHanded | category::BodyPart),
                                     misc_type::NotMisc,
                                     static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Claws),
                                     armor_type::NotArmor,
                                     material::Claw,
                                     material::Nothing,
                                     CLAWS_DETAILS.image_filename,
                                     CLAWS_DETAILS.price,
                                     CLAWS_DETAILS.weight,
                                     CLAWS_DETAILS.damage_min,
                                     CLAWS_DETAILS.damage_max,
                                     0,
                                     creature::role::Count,
                                     weaponInfo) );
    }


    ItemSPtr_t WeaponFactory::Make_Tendrils(creature::CreatureCPtrC_t CREATURE_CPTRC)
    {
        WeaponInfo weaponInfo(weapon_type::Tendrils);
        weaponInfo.is_tendrils = true;

        const WeaponDetails TENDRIL_DETAILS( WeaponDetailLoader::Instance()->LookupWeaponDetails("Tendrils") );

        std::ostringstream ssName;
        ssName << TENDRIL_DETAILS.name;

        std::ostringstream ssDesc;
        ssDesc << "The writhing lengths of a " << CREATURE_CPTRC->Race().Name() << ".";

        return ItemSPtr_t( new Item(ssName.str(),
                                     ssDesc.str(),
                                     static_cast<category::Enum>(category::Weapon | category::Equippable | category::TwoHanded | category::BodyPart),
                                     misc_type::NotMisc,
                                     static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Tendrils),
                                     armor_type::NotArmor,
                                     material::Flesh,
                                     material::Nothing,
                                     TENDRIL_DETAILS.image_filename,
                                     TENDRIL_DETAILS.price,
                                     TENDRIL_DETAILS.weight,
                                     TENDRIL_DETAILS.damage_min,
                                     TENDRIL_DETAILS.damage_max,
                                     0,
                                     creature::role::Count,
                                     weaponInfo) );
    }


    ItemSPtr_t WeaponFactory::Make_Bite(creature::CreatureCPtrC_t CREATURE_CPTRC)
    {
        WeaponInfo weaponInfo(weapon_type::Bite);
        weaponInfo.is_bite = true;

        const WeaponDetails BITE_DETAILS( WeaponDetailLoader::Instance()->LookupWeaponDetails("Bite") );

        std::ostringstream ssName;
        ssName << BITE_DETAILS.name;

        std::ostringstream ssDesc;
        ssDesc << "The fanged jaws of a " << CREATURE_CPTRC->Race().Name() << ".";

        return ItemSPtr_t( new Item(ssName.str(),
                                     ssDesc.str(),
                                     static_cast<category::Enum>(category::Weapon | category::Equippable | category::BodyPart),
                                     misc_type::NotMisc,
                                     static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Bite),
                                     armor_type::NotArmor,
                                     material::Tooth,
                                     material::Nothing,
                                     BITE_DETAILS.image_filename,
                                     BITE_DETAILS.price,
                                     BITE_DETAILS.weight,
                                     BITE_DETAILS.damage_min,
                                     BITE_DETAILS.damage_max,
                                     0,
                                     creature::role::Count,
                                     weaponInfo) );
    }


    ItemSPtr_t WeaponFactory::Make_Breath(creature::CreatureCPtrC_t CREATURE_CPTRC)
    {
        WeaponInfo weaponInfo(weapon_type::Breath);

        auto const BREATH_DETAILS( WeaponDetailLoader::Instance()->LookupWeaponDetails("Breath") );

        std::ostringstream ssName;
        ssName << ((CREATURE_CPTRC->Role().Which() == creature::role::Firebrand) ? "flaming" : "freezing") << " " << BREATH_DETAILS.name;

        std::ostringstream ssDesc;
        if (CREATURE_CPTRC->Role().Which() == creature::role::Sylavin)
            ssDesc << "The poisonous freezing breath of a " << CREATURE_CPTRC->Role().Name() << " " << CREATURE_CPTRC->Race().Name() << ".";
        else
            ssDesc << "The flaming breath of a " << CREATURE_CPTRC->Role().Name() << " " << CREATURE_CPTRC->Race().Name() << ".";

        return ItemSPtr_t( new Item(ssName.str(),
                                     ssDesc.str(),
                                     static_cast<category::Enum>(category::Weapon | category::Equippable | category::BodyPart),
                                     misc_type::NotMisc,
                                     static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Breath),
                                     armor_type::NotArmor,
                                     material::Gas,
                                     material::Nothing,
                                     BREATH_DETAILS.image_filename,
                                     BREATH_DETAILS.price,
                                     BREATH_DETAILS.weight,
                                     BREATH_DETAILS.damage_min,
                                     BREATH_DETAILS.damage_max,
                                     0,
                                     creature::role::Count,
                                     weaponInfo) );
    }


    ItemSPtr_t WeaponFactory::Make_Knife( const bool                  IS_DAGGER,
                                          const sfml_util::Size::Enum SIZE,
                                          const material::Enum        MATERIAL_PRI,
                                          const material::Enum        MATERIAL_SEC,
                                          const bool                  IS_PIXIE_ITEM)
    {
        WeaponInfo weaponInfo(weapon_type::Knife);
        weaponInfo.is_dagger = IS_DAGGER;
        weaponInfo.knife_size = SIZE;

        const WeaponDetails KNIFE_DETAILS( WeaponDetailLoader::Instance()->LookupWeaponDetails("Knife") );

        Coin_t price(KNIFE_DETAILS.price + material::PriceAdj(MATERIAL_PRI, MATERIAL_SEC));
        Weight_t weight(static_cast<item::Weight_t>(static_cast<float>(KNIFE_DETAILS.weight) * material::WeightMult(MATERIAL_PRI, MATERIAL_SEC)));

        stats::Health_t damageMin(KNIFE_DETAILS.damage_min);
        stats::Health_t damageMax(KNIFE_DETAILS.damage_max);
        std::string nameDescriptor("");
        if (SIZE == sfml_util::Size::Large)
        {
            nameDescriptor = "Large ";
            weight *= 2;
            price += 10;
            damageMin += 1;
            damageMax += 1;

        }
        else if (SIZE == sfml_util::Size::Small)
        {
            nameDescriptor = "Small ";
            weight /= 2;
            price -= 10;
            damageMin -= 1;
            damageMax -= 1;
        }

        std::string baseName("");
        if (IS_DAGGER)
        {
            baseName = "Dagger";
            damageMin += 1;
            damageMax += 1;
        }
        else
        {
            baseName = "Knife";
            damageMin -= 1;
            damageMax -= 1;
        }

        if (IS_PIXIE_ITEM)
        {
            damageMin = 1;
            damageMax = 2;
        }

        std::ostringstream ssName;
        ssName << nameDescriptor << baseName;

        std::ostringstream ssDesc;
        ssDesc << "A " << nameDescriptor << baseName;

        return ItemSPtr_t ( new Item(Make_Name(ssName.str(), MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                                      Make_Desc(ssDesc.str(), MATERIAL_PRI, MATERIAL_SEC, "", IS_PIXIE_ITEM),
                                      static_cast<category::Enum>(category::Equippable | category::Weapon | category::OneHanded),
                                      misc_type::NotMisc,
                                      static_cast<weapon_type::Enum>(weapon_type::Bladed | weapon_type::Knife | weapon_type::Melee | weapon_type::Pointed),
                                      armor_type::NotArmor,
                                      MATERIAL_PRI,
                                      MATERIAL_SEC,
                                      ((IS_DAGGER) ? "dagger" : "knife"),
                                      price,
                                      weight,
                                      damageMin,
                                      damageMax,
                                      0,
                                      creature::role::Count,
                                      weaponInfo,
                                      armor::ArmorInfo(),
                                      IS_PIXIE_ITEM) );
    }


    ItemSPtr_t WeaponFactory::Make_Sword(const sword_type::Enum SWORD_TYPE,
                                          const material::Enum   MATERIAL_PRI,
                                          const material::Enum   MATERIAL_SEC)
    {
        WeaponInfo weaponInfo(weapon_type::Sword);
        weaponInfo.sword = SWORD_TYPE;

        creature::role::Enum exclusiveRole(creature::role::Count);
        if ((SWORD_TYPE == sword_type::Knightlysword) || (SWORD_TYPE == sword_type::Claymore))
            exclusiveRole = creature::role::Knight;

        const WeaponDetails DETAILS(WeaponDetailLoader::Instance()->LookupWeaponDetails(sword_type::ToString(SWORD_TYPE)));

        Coin_t price(DETAILS.price);
        Weight_t weight(DETAILS.weight);

        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC);
        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

        return ItemSPtr_t ( new Item(Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC),
                                      Make_Desc(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC),
                                      static_cast<category::Enum>(category::Weapon | category::Equippable | DETAILS.handedness),
                                      misc_type::NotMisc,
                                      static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Bladed | weapon_type::Pointed | weapon_type::Sword),
                                      armor_type::NotArmor,
                                      MATERIAL_PRI,
                                      MATERIAL_SEC,
                                      DETAILS.image_filename,
                                      price,
                                      weight,
                                      DETAILS.damage_min,
                                      DETAILS.damage_max,
                                      0,
                                      exclusiveRole,
                                      weaponInfo) );
    }


    ItemSPtr_t WeaponFactory::Make_Axe(const axe_type::Enum AXE_TYPE,
                                        const material::Enum MATERIAL_PRI,
                                        const material::Enum MATERIAL_SEC)
    {
        WeaponInfo weaponInfo(weapon_type::Axe);
        weaponInfo.axe = AXE_TYPE;

        const WeaponDetails DETAILS( WeaponDetailLoader::Instance()->LookupWeaponDetails(axe_type::ToString(AXE_TYPE)) );

        Coin_t price(DETAILS.price);
        Weight_t weight(DETAILS.weight);

        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC);
        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

        return ItemSPtr_t( new Item(Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC),
                                     Make_Desc(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC),
                                     static_cast<category::Enum>(category::Weapon | category::Equippable | DETAILS.handedness),
                                     misc_type::NotMisc,
                                     static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Bladed | weapon_type::Axe),
                                     armor_type::NotArmor,
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     DETAILS.image_filename,
                                     price,
                                     weight,
                                     DETAILS.damage_min,
                                     DETAILS.damage_max,
                                     0,
                                     creature::role::Count,
                                     weaponInfo) );
    }


    ItemSPtr_t WeaponFactory::Make_Club(const club_type::Enum CLUB_TYPE,
                                         const material::Enum  MATERIAL_PRI,
                                         const material::Enum  MATERIAL_SEC)
    {
        WeaponInfo weaponInfo(weapon_type::Club);
        weaponInfo.club = CLUB_TYPE;

        const WeaponDetails DETAILS( WeaponDetailLoader::Instance()->LookupWeaponDetails(club_type::ToString(CLUB_TYPE)) );

        Coin_t price(DETAILS.price);
        Weight_t weight(DETAILS.weight);

        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC);
        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

        return ItemSPtr_t( new  Item(Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC),
                                      Make_Desc(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC),
                                      static_cast<category::Enum>(category::Weapon | category::Equippable | DETAILS.handedness),
                                      misc_type::NotMisc,
                                      static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Club),
                                      armor_type::NotArmor,
                                      material::Steel,
                                      material::Wood,
                                      DETAILS.image_filename,
                                      price,
                                      weight,
                                      DETAILS.damage_min,
                                      DETAILS.damage_min,
                                      0,
                                      creature::role::Count,
                                      weaponInfo) );
    }


    ItemSPtr_t WeaponFactory::Make_Whip(const whip_type::Enum WHIP_TYPE, const material::Enum MATERIAL_SEC)
    {
        WeaponInfo weaponInfo(weapon_type::Whip);
        weaponInfo.whip = WHIP_TYPE;

        const WeaponDetails DETAILS( WeaponDetailLoader::Instance()->LookupWeaponDetails(whip_type::ToString(WHIP_TYPE)) );

        Coin_t price(DETAILS.price);
        Weight_t weight(DETAILS.weight);

        material::Enum materialPri(material::Count);

        switch (WHIP_TYPE)
        {
            case whip_type::Bullwhip:
            {
                materialPri = material::SoftLeather;
                break;
            }
            case whip_type::Flail:
            {
                materialPri = material::Wood;
                break;
            }
            case whip_type::MaceAndChain:
            {
                materialPri = material::Steel;
                break;
            }
            case whip_type::Count:
            default:
            {
                std::ostringstream ss;
                ss << "herospath::item::WeaponFactory::Make_Whip(whip_type=" << WHIP_TYPE << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }

        AdjustPrice(price, materialPri, MATERIAL_SEC);
        AdjustWeight(weight, materialPri, MATERIAL_SEC);

        return ItemSPtr_t ( new Item(Make_Name(DETAILS.name, materialPri, MATERIAL_SEC),
                                      Make_Desc(DETAILS.description, materialPri, MATERIAL_SEC, "handle"),
                                      static_cast<category::Enum>(category::Weapon | category::Equippable | DETAILS.handedness),
                                      misc_type::NotMisc,
                                      static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Whip),
                                      armor_type::NotArmor,
                                      materialPri,
                                      MATERIAL_SEC,
                                      DETAILS.image_filename,
                                      price,
                                      weight,
                                      DETAILS.damage_min,
                                      DETAILS.damage_max,
                                      0,
                                      creature::role::Count,
                                      weaponInfo) );
    }


    ItemSPtr_t WeaponFactory::Make_Projectile(const projectile_type::Enum PROJ_TYPE,
                                               const material::Enum        MATERIAL_SEC)
    {
        WeaponInfo weaponInfo(weapon_type::Projectile);
        weaponInfo.projectile = PROJ_TYPE;

        const WeaponDetails DETAILS( WeaponDetailLoader::Instance()->LookupWeaponDetails(projectile_type::ToString(PROJ_TYPE)) );

        Coin_t price(DETAILS.price);
        Weight_t weight(DETAILS.weight);

        material::Enum materialPri(material::Count);
        weapon_type::Enum weaponType(weapon_type::NotAWeapon);

        switch (PROJ_TYPE)
        {
            case projectile_type::Blowpipe:
            {
                materialPri = material::Wood;
                weaponType = weapon_type::Blowpipe;
                break;
            }
            case projectile_type::Sling:
            {
                materialPri = material::SoftLeather;
                weaponType = weapon_type::Sling;
                break;
            }
            case projectile_type::Longbow:
            {
                materialPri = material::Wood;
                weaponType = weapon_type::Bow;
                break;
            }
            case projectile_type::CompositeBow:
            {
                materialPri = material::Horn;
                weaponType = weapon_type::Bow;
                break;
            }
            case projectile_type::Crossbow:
            {
                materialPri = material::Wood;
                weaponType = weapon_type::Crossbow;
                break;
            }
            case projectile_type::Count:
            default:
            {
                std::ostringstream ss;
                ss << "herospath::item::WeaponFactory::Make_Projectile(projectile_type=" << PROJ_TYPE << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }

        AdjustPrice(price, materialPri, MATERIAL_SEC);
        AdjustWeight(weight, materialPri, MATERIAL_SEC);

        return ItemSPtr_t( new Item(Make_Name(DETAILS.name, materialPri, MATERIAL_SEC),
                                     Make_Desc(DETAILS.description, materialPri, MATERIAL_SEC, "grip"),
                                     static_cast<category::Enum>(category::Weapon | category::TwoHanded | category::Equippable | DETAILS.handedness),
                                     misc_type::NotMisc,
                                     static_cast<weapon_type::Enum>(weapon_type::Projectile | weaponType),
                                     armor_type::NotArmor,
                                     materialPri,
                                     MATERIAL_SEC,
                                     DETAILS.image_filename,
                                     price,
                                     weight,
                                     DETAILS.damage_min,
                                     DETAILS.damage_max,
                                     0,
                                     creature::role::Count,
                                     weaponInfo) );
    }


    ItemSPtr_t WeaponFactory::Make_Staff(const bool           IS_QUARTERSTAFF,
                                          const material::Enum MATERIAL_PRI,
                                          const material::Enum MATERIAL_SEC,
                                          const bool           IS_PIXIE_ITEM)
    {
        WeaponInfo weaponInfo(weapon_type::Staff);
        weaponInfo.is_staff = ! IS_QUARTERSTAFF;
        weaponInfo.is_quarterstaff = IS_QUARTERSTAFF;

        const WeaponDetails DETAILS( WeaponDetailLoader::Instance()->LookupWeaponDetails(((IS_QUARTERSTAFF) ? "Quarterstaff" : "Staff")) );

        Coin_t price(DETAILS.price);
        Weight_t weight(DETAILS.weight);

        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM);
        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

        return ItemSPtr_t( new Item(Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                                     Make_Desc(std::string("A ").append(DETAILS.name), MATERIAL_PRI, MATERIAL_SEC, "", IS_PIXIE_ITEM),
                                     static_cast<category::Enum>(category::Weapon | category::Equippable | DETAILS.handedness),
                                     misc_type::NotMisc,
                                     static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Staff),
                                     armor_type::NotArmor,
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     DETAILS.image_filename,
                                     price,
                                     weight,
                                     DETAILS.damage_min,
                                     DETAILS.damage_max,
                                     0,
                                     creature::role::Count,
                                     weaponInfo,
                                     armor::ArmorInfo(),
                                     IS_PIXIE_ITEM) );
    }


    ItemSPtr_t WeaponFactory::Make_BladedStaff(const bladedstaff_type::Enum STAFF_TYPE,
                                                const material::Enum         MATERIAL_PRI,
                                                const material::Enum         MATERIAL_SEC)

    {
        WeaponInfo weaponInfo(weapon_type::BladedStaff);
        weaponInfo.bladedstaff = STAFF_TYPE;

        const WeaponDetails DETAILS( WeaponDetailLoader::Instance()->LookupWeaponDetails(bladedstaff_type::ToString(STAFF_TYPE)) );

        Coin_t price(DETAILS.price);
        Weight_t weight(DETAILS.weight);

        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC);
        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

        weapon_type::Enum spearWeapon(weapon_type::BladedStaff);
        if ((STAFF_TYPE == bladedstaff_type::Spear) || (STAFF_TYPE == bladedstaff_type::ShortSpear))
            spearWeapon = weapon_type::Spear;

        std::ostringstream ssDesc;
        ssDesc << "A " << DETAILS.name << " made of " << material::ToReadableString(MATERIAL_PRI);
        if ((MATERIAL_SEC != MATERIAL_PRI) && (MATERIAL_SEC != material::Nothing))
        {
            if (material::IsJewel(MATERIAL_SEC))
                ssDesc << " and jeweled with ";
            else if (material::IsPrecious(MATERIAL_SEC))
                ssDesc << " and adorned with ";
            else if (material::IsRigid(MATERIAL_SEC))
            {
                if ((STAFF_TYPE == bladedstaff_type::Spear) || (STAFF_TYPE == bladedstaff_type::ShortSpear))
                    ssDesc << " and tipped with ";
                else
                    ssDesc << " and bladed with ";
            }
            else if (material::IsLiquid(MATERIAL_SEC))
                ssDesc << " and coated in ";
            else
                ssDesc << " and ";

            ssDesc << material::ToReadableString(MATERIAL_SEC) << ".";
        }

        return ItemSPtr_t( new Item(Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC),
                                     ssDesc.str(),
                                     static_cast<category::Enum>(category::Weapon | category::Equippable | DETAILS.handedness),
                                     misc_type::NotMisc,
                                     static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::BladedStaff | spearWeapon),
                                     armor_type::NotArmor,
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     DETAILS.image_filename,
                                     price,
                                     weight,
                                     DETAILS.damage_min,
                                     DETAILS.damage_max,
                                     0,
                                     creature::role::Count,
                                     weaponInfo) );
    }

}
}
}