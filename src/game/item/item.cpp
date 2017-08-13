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

    Item::Item(const std::string &          NAME,
               const std::string &          DESC,
               const category::Enum         CATEGORY,
               const misc_type::Enum        MISC_TYPE,
               const weapon_type::Enum      WEAPON_TYPE,
               const armor_type::Enum       ARMOR_TYPE,
               const material::Enum         MATERIAL_PRIMARY,
               const material::Enum         MATERIAL_SECONDARY,
               const std::string &          IMAGE_FILENAME,
               const stats::Trait_t         PRICE,
               const stats::Trait_t         WEIGHT,
               const stats::Trait_t         DAMAGE_MIN,
               const stats::Trait_t         DAMAGE_MAX,
               const stats::Trait_t         ARMOR_RATING,
               const creature::role::Enum   EXCLUSIVE_TO_ROLE,
               const weapon::WeaponInfo &   WEAPON_INFO,
               const armor::ArmorInfo &     ARMOR_INFO,
               const bool                   IS_PIXIE_ITEM,
               const unique_type::Enum      UNIQUE_TYPE,
               const set_type::Enum         SET_TYPE,
               const named_type::Enum       NAMED_TYPE,
               const element_type::Enum     ELEMENT_TYPE,
               const creature::SummonInfo & SUMMON_INFO)
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
        uniqueType_      (UNIQUE_TYPE),
        setType_         (SET_TYPE),
        namedType_       (NAMED_TYPE),
        elementType_     (ELEMENT_TYPE),
        summonInfo_      (SUMMON_INFO),
        enchantmentsPVec_()
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
    {
        EnchantmentRemoveAndFreeAll();
    }


    void Item::EnchantmentAdd(const creature::EnchantmentPtr_t ENCHANTMENT_PTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((ENCHANTMENT_PTR != nullptr),
            "game::item::Item::EnchantmentAdd() was given a null ptr.");

        enchantmentsPVec_.push_back(ENCHANTMENT_PTR);
    }


    void Item::EnchantmentRemoveAndFree(creature::EnchantmentPtr_t enchantement_ptr)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((enchantement_ptr != nullptr),
            "game::item::Item::EnchantmentRemoveAndFree() was given a null ptr.");

        for (auto const NEXT_ENCHANTMENT_PTR : enchantmentsPVec_)
        {
            if (NEXT_ENCHANTMENT_PTR == enchantement_ptr)
            {
                enchantmentsPVec_.erase(std::remove(enchantmentsPVec_.begin(),
                                                   enchantmentsPVec_.end(),
                                                   enchantement_ptr),
                                       enchantmentsPVec_.end());

                creature::EnchantmentWarehouse::Instance()->Free(enchantement_ptr);
                return;
            }
        }

        std::ostringstream ss;
        
        ss << "game::item::Item::EnchantmentRemoveAndFree("
            << enchantement_ptr->EffectStr(nullptr)
            << ") but the pointer to that enchantment was not found.";

        throw std::runtime_error(ss.str());
    }


    void Item::EnchantmentRemoveAndFreeAll()
    {
        auto const ENCHANTMENT_PVEC_COPY{ enchantmentsPVec_ };
        for (auto nextEnchantmentPtr: ENCHANTMENT_PVEC_COPY)
        {
            EnchantmentRemoveAndFree(nextEnchantmentPtr);
        }
    }


    const std::string Item::BaseName() const
    {
        if ((uniqueType_ != unique_type::NotUnique) &&
            (uniqueType_ != unique_type::Count))
        {
            return unique_type::Name(uniqueType_);
        }

        if ((miscType_ != misc_type::NotMisc) &&
            (miscType_ != misc_type::Count))
        {
            return misc_type::Name(miscType_);
        }

        if ((weaponType_ & weapon_type::NotAWeapon) == false)
        {
            if (weaponType_ & weapon_type::Axe)         { return "Axe"; }
            if (weaponType_ & weapon_type::Bite)        { return "Bite"; }
            if (weaponType_ & weapon_type::BladedStaff) { return "Bladdedstaff"; }
            if (weaponType_ & weapon_type::Blowpipe)    { return "Blowpipe"; }
            if (weaponType_ & weapon_type::Breath)      { return "Breath"; }
            if (weaponType_ & weapon_type::Bow)         { return "Bow"; }
            if (weaponType_ & weapon_type::Claws)       { return "Claws"; }
            if (weaponType_ & weapon_type::Club)        { return "Club"; }
            if (weaponType_ & weapon_type::Crossbow)    { return "Crossbow"; }
            if (weaponType_ & weapon_type::Fists)       { return "Fists"; }
            if (weaponType_ & weapon_type::Knife)       { return "Knife"; }
            if (weaponType_ & weapon_type::Sling)       { return "Sling"; }
            if (weaponType_ & weapon_type::Spear)       { return "Spear"; }
            if (weaponType_ & weapon_type::Staff)       { return "Staff"; }
            if (weaponType_ & weapon_type::Sword)       { return "Sword"; }
            if (weaponType_ & weapon_type::Tendrils)    { return "Tendrils"; }
            if (weaponType_ & weapon_type::Whip)        { return "Whip"; }
        }
        
        if ((armorType_ & armor_type::NotArmor) == false)
        {
            if (armorType_ & armor_type::Aventail)  { return "Aventail"; }
            if (armorType_ & armor_type::Boots)     { return "Boots"; }
            if (armorType_ & armor_type::Bracer)    { return "Bracer"; }
            if (armorType_ & armor_type::Covering)  { return "Cover"; }
            if (armorType_ & armor_type::Gauntlets) { return "Gauntlets"; }
            if (armorType_ & armor_type::Helm)      { return "Helm"; }
            if (armorType_ & armor_type::Pants)     { return "Pants"; }
            if (armorType_ & armor_type::Sheild)    { return "Shield"; }
            if (armorType_ & armor_type::Shirt)     { return "Shirt"; }
            if (armorType_ & armor_type::Skin)      { return "Skin"; }
        }

        std::ostringstream ssErr;
        ssErr << "game::item::Item::BaseName(\"" << Name() << "\") found no base name.";
        throw std::runtime_error(ssErr.str());
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
                        L.isPixie_,
                        L.uniqueType_,
                        L.setType_,
                        L.namedType_,
                        L.summonInfo_,
                        L.elementType_)
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
                        R.isPixie_,
                        R.uniqueType_,
                        R.setType_,
                        R.namedType_,
                        R.summonInfo_,
                        R.elementType_) )
        {
            return true;
        }

        return misc::Vector::OrderlessCompareLess(L.enchantmentsPVec_, R.enchantmentsPVec_);
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
                     L.isPixie_,
                     L.uniqueType_,
                     L.setType_,
                     L.namedType_,
                     L.summonInfo_,
                     L.elementType_)
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
                      R.isPixie_,
                      R.uniqueType_,
                      R.setType_,
                      R.namedType_,
                      R.summonInfo_,
                      R.elementType_))
        {
            return false;
        }
        else
        {
            return misc::Vector::OrderlessCompareEqual(L.enchantmentsPVec_, R.enchantmentsPVec_);
        }
    }

}
}
