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
// item-profile.cpp
//
#include "item-profile.hpp"

#include "game/creature/enchantment-factory.hpp"

#include "game/combat/treasure-factory.hpp"


namespace game
{
namespace item
{

    ItemProfile::ItemProfile(const std::string &     BASE_NAME,
                             const category::Enum    CATEGORY,
                             const armor_type::Enum  ARMOR,
                             const weapon_type::Enum WEAPON,
                             const unique_type::Enum UNIQUE,
                             const misc_type::Enum   MISC,
                             const set_type::Enum    SET,
                             const named_type::Enum  NAMED)
    :
        baseName_   (BASE_NAME),
        category_   (CATEGORY),
        armor_      (ARMOR),
        weapon_     (WEAPON),
        unique_     (UNIQUE),
        misc_       (MISC),
        set_        (SET),
        named_      (NAMED),
        shield_	    (armor::shield_type::Count),
        helm_	    (armor::helm_type::Count),
        base_	    (armor::base_type::Count),
        cover_	    (armor::cover_type::Count),
        isAventail_ (false),
        isBracer_   (false),
        isShirt_    (false),
        isBoots_    (false),
        isPants_    (false),
        isGauntlets_(false),
        sword_	    (weapon::sword_type::Count),
        axe_	    (weapon::axe_type::Count),
        club_	    (weapon::club_type::Count),
        whip_	    (weapon::whip_type::Count),
        proj_	    (weapon::projectile_type::Count),
        bstaff_	    (weapon::bladedstaff_type::Count),
        size_	    (sfml_util::Size::Medium),
        isKnife_	(false),
        isDagger_	(false),
        isStaff_	(false),
        isQStaff_	(false),
        matPri_     (material::Nothing),
        matSec_     (material::Nothing),
        score_      (0)
    {}


    void ItemProfile::SetUnique(const unique_type::Enum E,
                                const material::Enum    MATERIAL_PRIMARY)
    {
        unique_ = E;
        matPri_ = MATERIAL_PRIMARY;
        misc_ = unique_type::MiscType(E);
        score_ = (combat::TreasureFactory::Score(MATERIAL_PRIMARY) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(E, MATERIAL_PRIMARY));
    }
        

    void ItemProfile::SetMisc(const misc_type::Enum E)
    {
        using namespace item;

        misc_ = E;

        if (E == misc_type::Cape)
        {
            matPri_ = material::Cloth;
            category_ = static_cast<category::Enum>(category::Armor |
                                                    category::Equippable);
            armor_ = armor_type::Covering;
            cover_ = armor::cover_type::Cape;
        }
        else if (E == misc_type::Cloak)
        {
            matPri_ = material::Cloth;
            category_ = static_cast<category::Enum>(category::Armor |
                                                    category::Equippable |
                                                    category::Wearable);
            armor_ = armor_type::Covering;
            cover_ = armor::cover_type::Cloak;
        }
    }
    

    void ItemProfile::SetShield(const armor::shield_type::Enum E,
                                const material::Enum   MATERIAL_PRIMARY,
                                const material::Enum   MATERIAL_SECONDARY,
                                const named_type::Enum NAMED_TYPE,
                                const set_type::Enum   SET_TYPE)
    {
        shield_ = E;

        category_ = static_cast<category::Enum>(category::Armor |
                                                category::Equippable |
                                                category::Wearable |
                                                category::OneHanded);

        armor_ = item::armor_type::Sheild;

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
        score_ = (combat::TreasureFactory::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY) +
                  combat::TreasureFactory::Score(E) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(NAMED_TYPE,
                                                                          MATERIAL_PRIMARY,
                                                                          false,
                                                                          true) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE));
    }


    void ItemProfile::SetHelm(const armor::helm_type::Enum E,
                              const material::Enum   MATERIAL_PRIMARY,
                              const material::Enum   MATERIAL_SECONDARY,
                              const named_type::Enum NAMED_TYPE,
                              const set_type::Enum   SET_TYPE)
    {
        helm_ = E;

        category_ = static_cast<category::Enum>(category::Armor |
                                                category::Equippable |
                                                category::Wearable);

        armor_ = item::armor_type::Helm;

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
        score_ = (combat::TreasureFactory::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY) +
                  combat::TreasureFactory::Score(E) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(NAMED_TYPE,
                                                                          MATERIAL_PRIMARY,
                                                                          false,
                                                                          true) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE));
    }


    void ItemProfile::SetCover(const armor::cover_type::Enum E,
                               const material::Enum   MATERIAL_PRIMARY,
                               const material::Enum   MATERIAL_SECONDARY,
                               const named_type::Enum NAMED_TYPE,
                               const set_type::Enum   SET_TYPE)
    {
        cover_ = E;
        
        category_ = static_cast<category::Enum>(category::Armor |
                                                category::Equippable |
                                                category::Wearable);

        armor_ = item::armor_type::Covering;

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
        score_ = (combat::TreasureFactory::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY) +
                  combat::TreasureFactory::Score(E) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(NAMED_TYPE,
                                                                          MATERIAL_PRIMARY,
                                                                          false,
                                                                          true) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE));
    }


    void ItemProfile::SetAventail(const armor::base_type::Enum E,
                                  const material::Enum   MATERIAL_PRIMARY,
                                  const material::Enum   MATERIAL_SECONDARY,
                                  const named_type::Enum NAMED_TYPE,
                                  const set_type::Enum   SET_TYPE)
    {
        isAventail_ = true;
        base_ = E;
        
        category_ = static_cast<category::Enum>(category::Armor |
                                                category::Equippable |
                                                category::Wearable);

        armor_ = item::armor_type::Aventail;

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
        score_ = (combat::TreasureFactory::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY) +
                  combat::TreasureFactory::ScoreAventail(E) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(NAMED_TYPE,
                                                                          MATERIAL_PRIMARY,
                                                                          false,
                                                                          true) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE));
    }


    void ItemProfile::SetBracer(const armor::base_type::Enum E,
                                const material::Enum   MATERIAL_PRIMARY,
                                const material::Enum   MATERIAL_SECONDARY,
                                const named_type::Enum NAMED_TYPE,
                                const set_type::Enum   SET_TYPE)
    {
        isBracer_ = true;
        base_ = E;
        
        category_ = static_cast<category::Enum>(category::Armor |
                                                category::Equippable |
                                                category::Wearable);

        armor_ = item::armor_type::Bracer;

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
        score_ = (combat::TreasureFactory::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY) +
                  combat::TreasureFactory::ScoreBracer(E) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(NAMED_TYPE,
                                                                          MATERIAL_PRIMARY,
                                                                          false,
                                                                          true) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE));
    }


    void ItemProfile::SetShirt(const armor::base_type::Enum E,
                               const material::Enum   MATERIAL_PRIMARY,
                               const material::Enum   MATERIAL_SECONDARY,
                               const named_type::Enum NAMED_TYPE,
                               const set_type::Enum   SET_TYPE)
    {
        isShirt_ = true;
        base_ = E;
        
        category_ = static_cast<category::Enum>(category::Armor |
                                                category::Equippable |
                                                category::Wearable);

        armor_ = item::armor_type::Shirt;

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
        score_ = (combat::TreasureFactory::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY) +
                  combat::TreasureFactory::ScoreShirt(E) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(NAMED_TYPE,
                                                                          MATERIAL_PRIMARY,
                                                                          false,
                                                                          true) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE));
    }


    void ItemProfile::SetBoots(const armor::base_type::Enum E,
                               const material::Enum   MATERIAL_PRIMARY,
                               const material::Enum   MATERIAL_SECONDARY,
                               const named_type::Enum NAMED_TYPE,
                               const set_type::Enum   SET_TYPE)
    {
        isBoots_ = true;
        base_ = E;
        
        category_ = static_cast<category::Enum>(category::Armor |
                                                category::Equippable |
                                                category::Wearable);

        armor_ = item::armor_type::Boots;

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
        score_ = (combat::TreasureFactory::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY) +
                  combat::TreasureFactory::ScoreBoots(E) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(NAMED_TYPE,
                                                                          MATERIAL_PRIMARY,
                                                                          false,
                                                                          true) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE));
    }


    void ItemProfile::SetPants(const armor::base_type::Enum E,
                               const material::Enum   MATERIAL_PRIMARY,
                               const material::Enum   MATERIAL_SECONDARY,
                               const named_type::Enum NAMED_TYPE,
                               const set_type::Enum   SET_TYPE)
    {
        isPants_ = true;
        base_ = E;
        
        category_ = static_cast<category::Enum>(category::Armor |
                                                category::Equippable |
                                                category::Wearable);

        armor_ = item::armor_type::Pants;

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
        score_ = (combat::TreasureFactory::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY) +
                  combat::TreasureFactory::ScorePants(E) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(NAMED_TYPE,
                                                                          MATERIAL_PRIMARY,
                                                                          false,
                                                                          true) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE));
    }


    void ItemProfile::SetGauntlets(const armor::base_type::Enum E,
                                   const material::Enum   MATERIAL_PRIMARY,
                                   const material::Enum   MATERIAL_SECONDARY,
                                   const named_type::Enum NAMED_TYPE,
                                   const set_type::Enum   SET_TYPE)
    {
        isGauntlets_ = true;
        base_ = E;
        
        category_ = static_cast<category::Enum>(category::Armor |
                                                category::Equippable |
                                                category::Wearable);

        armor_ = item::armor_type::Gauntlets;

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
        score_ = (combat::TreasureFactory::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY) +
                  combat::TreasureFactory::ScoreGauntlets(E) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(NAMED_TYPE,
                                                                          MATERIAL_PRIMARY,
                                                                          false,
                                                                          true) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE));
    }


    void ItemProfile::SetSword(const weapon::sword_type::Enum E,
                               const material::Enum   MATERIAL_PRIMARY,
                               const material::Enum   MATERIAL_SECONDARY,
                               const named_type::Enum NAMED_TYPE,
                               const set_type::Enum   SET_TYPE)
    {
        using namespace item;
        using namespace item::weapon;

        sword_ = E;
        
        auto const HANDED{ [E]()
            {
                if ((E == sword_type::Claymore) ||
                    (E == sword_type::Longsword) ||
                    (E == sword_type::Knightlysword))
                {
                    return category::TwoHanded;
                }
                else
                {
                    return category::OneHanded;
                }
            }() };

        category_ = static_cast<category::Enum>(category::Weapon |
                                                category::Equippable |
                                                HANDED);

        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Melee |
                                                 weapon_type::Sword |
                                                 weapon_type::Bladed |
                                                 weapon_type::Pointed);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
        score_ = (combat::TreasureFactory::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY) +
                  combat::TreasureFactory::Score(E) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(NAMED_TYPE,
                                                                          MATERIAL_PRIMARY,
                                                                          true,
                                                                          false) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE));
    }


    void ItemProfile::SetAxe(const weapon::axe_type::Enum E,
                             const material::Enum   MATERIAL_PRIMARY,
                             const material::Enum   MATERIAL_SECONDARY,
                             const named_type::Enum NAMED_TYPE,
                             const set_type::Enum   SET_TYPE)
    {
        using namespace item::weapon;

        axe_ = E;
        
        auto const HANDED{ [E]()
            {
                if ((E == axe_type::Battleaxe) ||
                    (E == axe_type::Waraxe))
                {
                    return category::TwoHanded;
                }
                else
                {
                    return category::OneHanded;
                }
            }() };

        category_ = static_cast<category::Enum>(category::Weapon |
                                                category::Equippable |
                                                HANDED);

        
        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Bladed |
                                                 weapon_type::Axe |
                                                 weapon_type::Melee);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
        score_ = (combat::TreasureFactory::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY) +
                  combat::TreasureFactory::Score(E) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(NAMED_TYPE,
                                                                          MATERIAL_PRIMARY,
                                                                          true,
                                                                          false) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE));
    }


    void ItemProfile::SetClub(const weapon::club_type::Enum E,
                              const material::Enum   MATERIAL_PRIMARY,
                              const material::Enum   MATERIAL_SECONDARY,
                              const named_type::Enum NAMED_TYPE,
                              const set_type::Enum   SET_TYPE)
    {
        using namespace item::weapon;

        club_ = E;

        auto const HANDED{ [E]()
            {
                if ((E == club_type::Spiked))
                {
                    return category::OneHanded;
                }
                else
                {
                    return category::TwoHanded;
                }
            }() };

        category_ = static_cast<category::Enum>(category::Weapon |
                                                category::Equippable |
                                                HANDED);

        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Melee |
                                                 weapon_type::Club);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
        score_ = (combat::TreasureFactory::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY) +
                  combat::TreasureFactory::Score(E) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(NAMED_TYPE,
                                                                          MATERIAL_PRIMARY,
                                                                          true,
                                                                          false) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE));
    }


    void ItemProfile::SetWhip(const weapon::whip_type::Enum E,
                              const material::Enum   MATERIAL_PRIMARY,
                              const material::Enum   MATERIAL_SECONDARY,
                              const named_type::Enum NAMED_TYPE,
                              const set_type::Enum   SET_TYPE)
    {
        using namespace item::weapon;

        whip_ = E;

        auto const HANDED{ [E]()
            {
                if ((E == whip_type::Flail))
                {
                    return category::TwoHanded;
                }
                else
                {
                    return category::OneHanded;
                }
            }() };

        category_ = static_cast<category::Enum>(category::Weapon |
                                                category::Equippable |
                                                HANDED);

        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Melee |
                                                 weapon_type::Whip);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
        score_ = (combat::TreasureFactory::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY) +
                  combat::TreasureFactory::Score(E) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(NAMED_TYPE,
                                                                          MATERIAL_PRIMARY,
                                                                          true,
                                                                          false) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE));
    }
        
    
    void ItemProfile::SetProjectile(const weapon::projectile_type::Enum E,
                                    const material::Enum   MATERIAL_PRIMARY,
                                    const material::Enum   MATERIAL_SECONDARY,
                                    const named_type::Enum NAMED_TYPE,
                                    const set_type::Enum   SET_TYPE)
    {
        using namespace item;
        using namespace item::weapon;

        proj_ = E;

        category_ = static_cast<category::Enum>(category::Weapon |
                                                category::Equippable |
                                                category::TwoHanded);

        auto const WEAPON_TYPE{ [E]()
            {
                if (E == projectile_type::Blowpipe) { return weapon_type::Blowpipe; }
                if (E == projectile_type::Sling)    { return weapon_type::Sling; }
                if (E == projectile_type::Crossbow) { return weapon_type::Crossbow; }
                return weapon_type::Bow;
            }() };

        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Projectile | WEAPON_TYPE);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
        score_ = (combat::TreasureFactory::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY) +
                  combat::TreasureFactory::Score(E) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(NAMED_TYPE,
                                                                          MATERIAL_PRIMARY,
                                                                          true,
                                                                          false) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE));
    }

    
    void ItemProfile::SetBladdedStaff(const weapon::bladedstaff_type::Enum E,
                                      const material::Enum   MATERIAL_PRIMARY,
                                      const material::Enum   MATERIAL_SECONDARY,
                                      const named_type::Enum NAMED_TYPE,
                                      const set_type::Enum   SET_TYPE)
    {
        using namespace item;
        using namespace item::weapon;

        bstaff_ = E;
        
        category_ = static_cast<category::Enum>(category::Weapon |
                                                category::Equippable |
                                                category::TwoHanded);

        auto const BLADDED_TYPE{ ((E == bladedstaff_type::Spear) ? 0 : weapon_type::Bladed) };
        auto const POINTED_TYPE{ ((E == bladedstaff_type::Scythe) ? 0 : weapon_type::Pointed) };
        auto const SPEAR_TYPE{ ((E == bladedstaff_type::Spear) ? weapon_type::Spear : 0) };

        weapon_ = static_cast<weapon_type::Enum>(BLADDED_TYPE |
                                                 weapon_type::BladedStaff |
                                                 weapon_type::Melee |
                                                 POINTED_TYPE |
                                                 SPEAR_TYPE);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
        score_ = (combat::TreasureFactory::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY) +
                  combat::TreasureFactory::Score(E) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(NAMED_TYPE,
                                                                          MATERIAL_PRIMARY,
                                                                          true,
                                                                          false) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE));
    }


    void ItemProfile::SetKnife(const sfml_util::Size::Enum E,
                               const material::Enum   MATERIAL_PRIMARY,
                               const material::Enum   MATERIAL_SECONDARY,
                               const named_type::Enum NAMED_TYPE,
                               const set_type::Enum   SET_TYPE)
    {
        isKnife_ = true;
        size_ = E;

        category_ = static_cast<category::Enum>(category::Weapon |
                                                category::Equippable |
                                                category::OneHanded);

        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Bladed |
                                                 weapon_type::Knife |
                                                 weapon_type::Melee |
                                                 weapon_type::Pointed);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
        score_ = (combat::TreasureFactory::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY) +
                  combat::TreasureFactory::ScoreKnife(E) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(NAMED_TYPE,
                                                                          MATERIAL_PRIMARY,
                                                                          true,
                                                                          false) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE));
    }


    void ItemProfile::SetDagger(const sfml_util::Size::Enum E,
                                const material::Enum   MATERIAL_PRIMARY,
                                const material::Enum   MATERIAL_SECONDARY,
                                const named_type::Enum NAMED_TYPE,
                                const set_type::Enum   SET_TYPE)
    {
        isDagger_ = true;
        size_ = E;
        
        category_ = static_cast<category::Enum>(category::Weapon |
                                                category::Equippable |
                                                category::OneHanded);

        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Bladed |
                                                 weapon_type::Knife |
                                                 weapon_type::Melee |
                                                 weapon_type::Pointed);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
        score_ = (combat::TreasureFactory::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY) +
                  combat::TreasureFactory::ScoreDagger(E) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(NAMED_TYPE,
                                                                          MATERIAL_PRIMARY,
                                                                          true,
                                                                          false) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE));
    }

    
    void ItemProfile::SetStaff(const material::Enum   MATERIAL_PRIMARY,
                               const material::Enum   MATERIAL_SECONDARY,
                               const named_type::Enum NAMED_TYPE,
                               const set_type::Enum   SET_TYPE)
    {
        isStaff_ = true;
        
        category_ = static_cast<category::Enum>(category::Weapon |
                                                category::Equippable |
                                                category::TwoHanded);

        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Staff |
                                                 weapon_type::Melee);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
        score_ = (combat::TreasureFactory::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY) +
                  combat::TreasureFactory::ScoreStaff() +
                  creature::EnchantmentFactory::Instance()->TreasureScore(NAMED_TYPE,
                                                                          MATERIAL_PRIMARY,
                                                                          true,
                                                                          false) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE));
    }

    
    void ItemProfile::SetQuarterStaff(const material::Enum   MATERIAL_PRIMARY,
                                      const material::Enum   MATERIAL_SECONDARY,
                                      const named_type::Enum NAMED_TYPE,
                                      const set_type::Enum   SET_TYPE)
    {
        isQStaff_ = true;
        
        category_ = static_cast<category::Enum>(category::Weapon |
                                                category::Equippable |
                                                category::TwoHanded);

        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Staff |
                                                 weapon_type::Melee);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
        score_ = (combat::TreasureFactory::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY) +
                  combat::TreasureFactory::ScoreQuarterStaff() +
                  creature::EnchantmentFactory::Instance()->TreasureScore(NAMED_TYPE,
                                                                          MATERIAL_PRIMARY,
                                                                          true,
                                                                          false) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE));
    }


    void ItemProfile::SetHelper(const material::Enum   MATERIAL_PRIMARY,
                                const material::Enum   MATERIAL_SECONDARY,
                                const named_type::Enum NAMED_ENUM,
                                const set_type::Enum   SET_ENUM)
    {
        matPri_ = MATERIAL_PRIMARY;
        matSec_ = MATERIAL_SECONDARY;
        named_ = NAMED_ENUM;
        set_ = SET_ENUM;
    }

}
}
