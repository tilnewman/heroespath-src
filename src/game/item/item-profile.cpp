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
        isAventail_(false),
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
        size_	    (sfml_util::Size::Count),
        isKnife_	(false),
        isDagger_	(false),
        isStaff_	(false),
        isQStaff_	(false),
        matPri_     (material::Nothing),
        matSec_     (material::Nothing)
    {}


    void ItemProfile::SetUnique(const unique_type::Enum,
                                const material::Enum)
    {
        //TODO
    }
        

    void ItemProfile::SetMisc(const misc_type::Enum)
    {
        //TODO
    }
    

    void ItemProfile::SetShield(const armor::shield_type::Enum E,
                                const material::Enum   MATERIAL_PRIMARY,
                                const material::Enum   MATERIAL_SECONDARY,
                                const named_type::Enum NAMED_TYPE,
                                const set_type::Enum   SET_TYPE)
    {
        shield_ = E;
        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
    }


    void ItemProfile::SetHelm(const armor::helm_type::Enum E,
                              const material::Enum   MATERIAL_PRIMARY,
                              const material::Enum   MATERIAL_SECONDARY,
                              const named_type::Enum NAMED_TYPE,
                              const set_type::Enum   SET_TYPE)
    {
        helm_ = E;
        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
    }


    void ItemProfile::SetCover(const armor::cover_type::Enum E,
                               const material::Enum   MATERIAL_PRIMARY,
                               const material::Enum   MATERIAL_SECONDARY,
                               const named_type::Enum NAMED_TYPE,
                               const set_type::Enum   SET_TYPE)
    {
        cover_ = E;
        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
    }


    void ItemProfile::SetAventail(const armor::base_type::Enum E,
                                  const material::Enum   MATERIAL_PRIMARY,
                                  const material::Enum   MATERIAL_SECONDARY,
                                  const named_type::Enum NAMED_TYPE,
                                  const set_type::Enum   SET_TYPE)
    {
        isAventail_ = true;
        base_ = E;
        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
    }


    void ItemProfile::SetBracer(const armor::base_type::Enum E,
                                const material::Enum   MATERIAL_PRIMARY,
                                const material::Enum   MATERIAL_SECONDARY,
                                const named_type::Enum NAMED_TYPE,
                                const set_type::Enum   SET_TYPE)
    {
        isBracer_ = true;
        base_ = E;
        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
    }


    void ItemProfile::SetShirt(const armor::base_type::Enum E,
                               const material::Enum   MATERIAL_PRIMARY,
                               const material::Enum   MATERIAL_SECONDARY,
                               const named_type::Enum NAMED_TYPE,
                               const set_type::Enum   SET_TYPE)
    {
        isShirt_ = true;
        base_ = E;
        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
    }


    void ItemProfile::SetBoots(const armor::base_type::Enum E,
                               const material::Enum   MATERIAL_PRIMARY,
                               const material::Enum   MATERIAL_SECONDARY,
                               const named_type::Enum NAMED_TYPE,
                               const set_type::Enum   SET_TYPE)
    {
        isBoots_ = true;
        base_ = E;
        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
    }


    void ItemProfile::SetPants(const armor::base_type::Enum E,
                               const material::Enum   MATERIAL_PRIMARY,
                               const material::Enum   MATERIAL_SECONDARY,
                               const named_type::Enum NAMED_TYPE,
                               const set_type::Enum   SET_TYPE)
    {
        isPants_ = true;
        base_ = E;
        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
    }


    void ItemProfile::SetGauntlets(const armor::base_type::Enum E,
                                   const material::Enum   MATERIAL_PRIMARY,
                                   const material::Enum   MATERIAL_SECONDARY,
                                   const named_type::Enum NAMED_TYPE,
                                   const set_type::Enum   SET_TYPE)
    {
        isGauntlets_ = true;
        base_ = E;
        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
    }


    void ItemProfile::SetSword(const weapon::sword_type::Enum E,
                               const material::Enum   MATERIAL_PRIMARY,
                               const material::Enum   MATERIAL_SECONDARY,
                               const named_type::Enum NAMED_TYPE,
                               const set_type::Enum   SET_TYPE)
    {
        sword_ = E;
        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
    }


    void ItemProfile::SetAxe(const weapon::axe_type::Enum E,
                             const material::Enum   MATERIAL_PRIMARY,
                             const material::Enum   MATERIAL_SECONDARY,
                             const named_type::Enum NAMED_TYPE,
                             const set_type::Enum   SET_TYPE)
    {
        axe_ = E;
        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
    }


    void ItemProfile::SetClub(const weapon::club_type::Enum E,
                              const material::Enum   MATERIAL_PRIMARY,
                              const material::Enum   MATERIAL_SECONDARY,
                              const named_type::Enum NAMED_TYPE,
                              const set_type::Enum   SET_TYPE)
    {
        club_ = E;
        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
    }


    void ItemProfile::SetWhip(const weapon::whip_type::Enum E,
                              const material::Enum   MATERIAL_PRIMARY,
                              const material::Enum   MATERIAL_SECONDARY,
                              const named_type::Enum NAMED_TYPE,
                              const set_type::Enum   SET_TYPE)
    {
        whip_ = E;
        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
    }
        
    
    void ItemProfile::SetProjectile(const weapon::projectile_type::Enum E,
                                    const material::Enum   MATERIAL_PRIMARY,
                                    const material::Enum   MATERIAL_SECONDARY,
                                    const named_type::Enum NAMED_TYPE,
                                    const set_type::Enum   SET_TYPE)
    {
        proj_ = E;
        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
    }

    
    void ItemProfile::SetBladdedStaff(const weapon::bladedstaff_type::Enum E,
                                      const material::Enum   MATERIAL_PRIMARY,
                                      const material::Enum   MATERIAL_SECONDARY,
                                      const named_type::Enum NAMED_TYPE,
                                      const set_type::Enum   SET_TYPE)
    {
        bstaff_ = E;
        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
    }


    void ItemProfile::SetKnife(const sfml_util::Size::Enum E,
                               const material::Enum   MATERIAL_PRIMARY,
                               const material::Enum   MATERIAL_SECONDARY,
                               const named_type::Enum NAMED_TYPE,
                               const set_type::Enum   SET_TYPE)
    {
        isKnife_ = true;
        size_ = E;
        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
    }


    void ItemProfile::SetDagger(const sfml_util::Size::Enum E,
                                const material::Enum   MATERIAL_PRIMARY,
                                const material::Enum   MATERIAL_SECONDARY,
                                const named_type::Enum NAMED_TYPE,
                                const set_type::Enum   SET_TYPE)
    {
        isDagger_ = true;
        size_ = E;
        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
    }

    
    void ItemProfile::SetStaff(const material::Enum   MATERIAL_PRIMARY,
                               const material::Enum   MATERIAL_SECONDARY,
                               const named_type::Enum NAMED_TYPE,
                               const set_type::Enum   SET_TYPE)
    {
        isStaff_ = true;
        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
    }

    
    void ItemProfile::SetQuarterStaff(const material::Enum   MATERIAL_PRIMARY,
                                      const material::Enum   MATERIAL_SECONDARY,
                                      const named_type::Enum NAMED_TYPE,
                                      const set_type::Enum   SET_TYPE)
    {
        isQStaff_ = true;
        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE);
    }


    void ItemProfile::SetHelper(const material::Enum     MATERIAL_PRIMARY,
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
