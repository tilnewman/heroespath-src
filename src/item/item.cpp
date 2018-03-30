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

#include "creature/enchantment-warehouse.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/vectors.hpp"

#include <exception>
#include <iomanip>
#include <sstream>
#include <tuple>

namespace heroespath
{
namespace item
{

    Item::Item(
        const std::string & NAME,
        const std::string & DESC,
        const category::Enum CATEGORY,
        const misc_type::Enum MISC_TYPE,
        const weapon_type::Enum WEAPON_TYPE,
        const armor_type::Enum ARMOR_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const std::string & IMAGE_FILENAME,
        const Coin_t & PRICE,
        const Weight_t & WEIGHT,
        const Health_t & DAMAGE_MIN,
        const Health_t & DAMAGE_MAX,
        const Armor_t & ARMOR_RATING,
        const creature::role::Enum EXCLUSIVE_TO_ROLE,
        const weapon::WeaponInfo & WEAPON_INFO,
        const armor::ArmorInfo & ARMOR_INFO,
        const bool IS_PIXIE_ITEM,
        const unique_type::Enum UNIQUE_TYPE,
        const set_type::Enum SET_TYPE,
        const named_type::Enum NAMED_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const creature::SummonInfo & SUMMON_INFO)
        : name_(NAME)
        , desc_(DESC)
        , price_(PRICE)
        , weight_(WEIGHT)
        , damageMin_(DAMAGE_MIN)
        , damageMax_(DAMAGE_MAX)
        , armorRating_(ARMOR_RATING)
        , exclusiveToRole_(EXCLUSIVE_TO_ROLE)
        , armorType_(ARMOR_TYPE)
        , weaponType_(WEAPON_TYPE)
        , category_(CATEGORY)
        , miscType_(MISC_TYPE)
        , materialPri_(MATERIAL_PRIMARY)
        , materialSec_(MATERIAL_SECONDARY)
        , imageFilename_(IMAGE_FILENAME)
        , weaponInfo_(WEAPON_INFO)
        , armorInfo_(ARMOR_INFO)
        , isPixie_(IS_PIXIE_ITEM)
        , // see constructor body
        uniqueType_(UNIQUE_TYPE)
        , setType_(SET_TYPE)
        , namedType_(NAMED_TYPE)
        , elementType_(ELEMENT_TYPE)
        , summonInfo_(SUMMON_INFO)
        , enchantmentsPVec_()
    {
        // adjust the weight for pixie items
        if (isPixie_)
        {
            weight_ /= 250_weight;

            if (weight_ < 1_weight)
            {
                weight_ = 1_weight;
            }
        }
    }

    Item::~Item() { creature::EnchantmentWarehouse::Instance()->Free(enchantmentsPVec_); }

    void Item::EnchantmentAdd(const creature::EnchantmentPtr_t ENCHANTMENT_PTR)
    {
        enchantmentsPVec_.emplace_back(ENCHANTMENT_PTR);
    }

    const std::string Item::BaseName() const
    {
        if ((uniqueType_ != unique_type::NotUnique) && (uniqueType_ != unique_type::Count))
        {
            return unique_type::Name(uniqueType_);
        }

        if ((miscType_ != misc_type::NotMisc) && (miscType_ != misc_type::Count))
        {
            return misc_type::Name(miscType_);
        }

        if (weaponType_ != weapon_type::NotAWeapon)
        {
            if (weaponType_ & weapon_type::Axe)
            {
                return "Axe";
            }
            if (weaponType_ & weapon_type::Bite)
            {
                return "Bite";
            }
            if (weaponType_ & weapon_type::BladedStaff)
            {
                return "Bladdedstaff";
            }
            if (weaponType_ & weapon_type::Blowpipe)
            {
                return "Blowpipe";
            }
            if (weaponType_ & weapon_type::Breath)
            {
                return "Breath";
            }
            if (weaponType_ & weapon_type::Bow)
            {
                return "Bow";
            }
            if (weaponType_ & weapon_type::Claws)
            {
                return "Claws";
            }
            if (weaponType_ & weapon_type::Club)
            {
                return "Club";
            }
            if (weaponType_ & weapon_type::Crossbow)
            {
                return "Crossbow";
            }
            if (weaponType_ & weapon_type::Fists)
            {
                return "Fists";
            }
            if (weaponType_ & weapon_type::Knife)
            {
                return "Knife";
            }
            if (weaponType_ & weapon_type::Sling)
            {
                return "Sling";
            }
            if (weaponType_ & weapon_type::Spear)
            {
                return "Spear";
            }
            if (weaponType_ & weapon_type::Staff)
            {
                return "Staff";
            }
            if (weaponType_ & weapon_type::Sword)
            {
                return "Sword";
            }
            if (weaponType_ & weapon_type::Tendrils)
            {
                return "Tendrils";
            }
            if (weaponType_ & weapon_type::Whip)
            {
                return "Whip";
            }
        }

        if (armorType_ != armor_type::NotArmor)
        {
            if (armorType_ & armor_type::Aventail)
            {
                return "Aventail";
            }
            if (armorType_ & armor_type::Boots)
            {
                return "Boots";
            }
            if (armorType_ & armor_type::Bracer)
            {
                return "Bracer";
            }
            if (armorType_ & armor_type::Covering)
            {
                return "Cover";
            }
            if (armorType_ & armor_type::Gauntlets)
            {
                return "Gauntlets";
            }
            if (armorType_ & armor_type::Helm)
            {
                return "Helm";
            }
            if (armorType_ & armor_type::Pants)
            {
                return "Pants";
            }
            if (armorType_ & armor_type::Sheild)
            {
                return "Shield";
            }
            if (armorType_ & armor_type::Shirt)
            {
                return "Shirt";
            }
            if (armorType_ & armor_type::Skin)
            {
                return "Skin";
            }
        }

        std::ostringstream ssErr;
        ssErr << "item::Item::BaseName(\"" << Name() << "\") found no base name.";
        throw std::runtime_error(ssErr.str());
    }

    const std::string Item::ToString() const
    {
        std::ostringstream ss;

        ss << "name=" << std::quoted(name_) << ", desc=" << std::quoted(desc_)
           << ", base_name=" << std::quoted(BaseName())
           << ", category=" << category::ToString(category_, true)
           << ", mat_pri=" << material::ToString(materialPri_)
           << ", mat_sec=" << material::ToString(materialSec_)
           << ", armor_type=" << armor_type::ToString(armorType_, true)
           << ", weapon_type=" << weapon_type::ToString(weaponType_, true)
           << ", misc_type=" << misc_type::ToString(miscType_)
           << ", unique_type=" << unique_type::ToString(uniqueType_)
           << ", set_type=" << set_type::ToString(setType_)
           << ", named_type=" << named_type::ToString(namedType_)
           << ", element_type=" << element_type::ToString(elementType_)
           << ", is_pixie=" << std::boolalpha << isPixie_ << ", enchantments={";

        for (auto const & ENCHANTMENT_PTR : enchantmentsPVec_)
        {
            ss << ENCHANTMENT_PTR->ToString() << " | ";
        }

        ss << "}";
        return ss.str();
    }

    void Item::BeforeSerialize()
    {
        enchantmentsToSerializePVec_.clear();
        for (auto const & ENCHANTMENT_PTR : enchantmentsPVec_)
        {
            enchantmentsToSerializePVec_.emplace_back(ENCHANTMENT_PTR.Ptr());
        }

        // everything in enchantmentsPVec_ is free'd in the Item::~Item()
    }

    void Item::AfterSerialize()
    {
        enchantmentsPVec_.clear();
        for (auto const ENCHANTMENT_RAW_PTR : enchantmentsToSerializePVec_)
        {
            enchantmentsPVec_.emplace_back(ENCHANTMENT_RAW_PTR);
        }
        enchantmentsToSerializePVec_.clear();
    }

    bool operator<(const Item & L, const Item & R)
    {
        if (std::tie(
                L.name_,
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
            < std::tie(
                  R.name_,
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
            return true;
        }

        return misc::Vector::OrderlessCompareLess(L.enchantmentsPVec_, R.enchantmentsPVec_);
    }

    bool operator==(const Item & L, const Item & R)
    {
        if (std::tie(
                L.name_,
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
            != std::tie(
                   R.name_,
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
} // namespace item
} // namespace heroespath
