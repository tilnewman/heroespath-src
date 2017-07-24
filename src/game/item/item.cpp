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
// item.cpp
//
#include "item.hpp"

#include "game/creature/enchantment-warehouse.hpp"

#include "misc/vectors.hpp"
#include "misc/assertlogandthrow.hpp"

#include <tuple>
#include <sstream>
#include <exception>


namespace game
{
namespace item
{

    Item::Item(const std::string &        NAME,
               const std::string &        DESC,
               const category::Enum       CATEGORY,
               const misc_type::Enum      MISC_TYPE,
               const weapon_type::Enum    WEAPON_TYPE,
               const armor_type::Enum     ARMOR_TYPE,
               const material::Enum       MATERIAL_PRIMARY,
               const material::Enum       MATERIAL_SECONDARY,
               const std::string &        IMAGE_FILENAME,
               const Coin_t               PRICE,
               const Weight_t             WEIGHT,
               const stats::Health_t      DAMAGE_MIN,
               const stats::Health_t      DAMAGE_MAX,
               const stats::Armor_t       ARMOR_RATING,
               const creature::role::Enum EXCLUSIVE_TO_ROLE,
               const weapon::WeaponInfo & WEAPON_INFO,
               const armor::ArmorInfo &   ARMOR_INFO,
               const bool                 IS_PIXIE_ITEM)
    :
        name_            (NAME),
        desc_            (DESC),
        price_           (PRICE),
        weight_          (WEIGHT),
        damageMin_       (DAMAGE_MIN),
        damageMax_       (DAMAGE_MAX),
        armorRating_     (ARMOR_RATING),
        exclusiveToRole_ (EXCLUSIVE_TO_ROLE),
        armorType_       (ARMOR_TYPE),
        weaponType_      (WEAPON_TYPE),
        category_        (CATEGORY),
        miscType_        (MISC_TYPE),
        materialPri_     (MATERIAL_PRIMARY),
        materialSec_     (MATERIAL_SECONDARY),
        imageFilename_   (IMAGE_FILENAME),
        weaponInfo_      (WEAPON_INFO),
        armorInfo_       (ARMOR_INFO),
        isPixie_         (IS_PIXIE_ITEM),//see constructor body
        enchantmentsVec_ ()
    {
        //adjust the weight for pixie items
        if (isPixie_)
        {
            weight_ /= 250;

            if (weight_ < 1)
            {
                weight_ = 1;
            }
        }
    }


    Item::~Item()
    {}


    void Item::EnchantmentAdd(const creature::EnchantmentPtr_t ENCHANTMENT_PTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((ENCHANTMENT_PTR != nullptr),
            "game::item::Item::EnchantmentAdd() was given a null ptr.");

        enchantmentsVec_.push_back(ENCHANTMENT_PTR);
    }


    void Item::EnchantmentRemoveAndFree(creature::EnchantmentPtr_t enchantement_ptr)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((enchantement_ptr != nullptr),
            "game::item::Item::EnchantmentRemoveAndFree() was given a null ptr.");

        for (auto const NEXT_ENCHANTMENT_PTR : enchantmentsVec_)
        {
            if (NEXT_ENCHANTMENT_PTR == enchantement_ptr)
            {
                enchantmentsVec_.erase(std::remove(enchantmentsVec_.begin(),
                                                   enchantmentsVec_.end(),
                                                   enchantement_ptr),
                                       enchantmentsVec_.end());

                creature::EnchantmentWarehouse::Instance()->Free(enchantement_ptr);
                return;
            }
        }

        std::ostringstream ss;
        
        ss << "game::item::Item::EnchantmentRemoveAndFree(" << enchantement_ptr->Name()
            << ") but the pointer to that enchantment was not found.";

        throw std::runtime_error(ss.str());
    }


    bool operator<(const Item & L, const Item & R)
    {
        if (   std::tie(L.name_,
                        L.desc_,
                        L.price_,
                        L.weight_,
                        L.damageMin_,
                        L.damageMax_,
                        L.armorRating_,
                        L.exclusiveToRole_,
                        L.armorType_,
                        L.weaponType_,
                        L.category_,
                        L.miscType_,
                        L.materialPri_,
                        L.materialSec_,
                        L.imageFilename_,
                        L.weaponInfo_,
                        L.armorInfo_,
                        L.isPixie_)
               <
               std::tie(R.name_,
                        R.desc_,
                        R.price_,
                        R.weight_,
                        R.damageMin_,
                        R.damageMax_,
                        R.armorRating_,
                        R.exclusiveToRole_,
                        R.armorType_,
                        R.weaponType_,
                        R.category_,
                        R.miscType_,
                        R.materialPri_,
                        R.materialSec_,
                        R.imageFilename_,
                        R.weaponInfo_,
                        R.armorInfo_,
                        R.isPixie_) )
        {
            return true;
        }

        return misc::Vector::OrderlessCompareLess(L.enchantmentsVec_, R.enchantmentsVec_);
    }


    bool operator==(const Item & L, const Item & R)
    {
        if (std::tie(L.name_,
                     L.desc_,
                     L.price_,
                     L.weight_,
                     L.damageMin_,
                     L.damageMax_,
                     L.armorRating_,
                     L.exclusiveToRole_,
                     L.armorType_,
                     L.weaponType_,
                     L.category_,
                     L.miscType_,
                     L.materialPri_,
                     L.materialSec_,
                     L.imageFilename_,
                     L.weaponInfo_,
                     L.armorInfo_,
                     L.isPixie_)
             !=
             std::tie(R.name_,
                      R.desc_,
                      R.price_,
                      R.weight_,
                      R.damageMin_,
                      R.damageMax_,
                      R.armorRating_,
                      R.exclusiveToRole_,
                      R.armorType_,
                      R.weaponType_,
                      R.category_,
                      R.miscType_,
                      R.materialPri_,
                      R.materialSec_,
                      R.imageFilename_,
                      R.weaponInfo_,
                      R.armorInfo_,
                      R.isPixie_))
        {
            return false;
        }
        else
        {
            return misc::Vector::OrderlessCompareLess(L.enchantmentsVec_, R.enchantmentsVec_);
        }
    }

}
}
