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

#include "creature/enchantment-factory.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/serialize-helpers.hpp"
#include "misc/vectors.hpp"
#include "sfml-util/gui/item-image-manager.hpp"

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
        const weapon_type::Enum WEAPON_TYPE,
        const armor_type::Enum ARMOR_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const Coin_t & PRICE,
        const Weight_t & WEIGHT,
        const Health_t & DAMAGE_MIN,
        const Health_t & DAMAGE_MAX,
        const Armor_t & ARMOR_RATING,
        const TypeWrapper & TYPE_WRAPPER,
        const weapon::WeaponInfo & WEAPON_INFO,
        const armor::ArmorInfo & ARMOR_INFO,
        const bool IS_PIXIE_ITEM,
        const creature::role::Enum EXCLUSIVE_ROLE_BASED_ON_ITEM_TYPE)
        : name_(NAME)
        , desc_(DESC)
        , price_(PRICE)
        , weight_(WEIGHT)
        , damageMin_(DAMAGE_MIN)
        , damageMax_(DAMAGE_MAX)
        , armorRating_(ARMOR_RATING)
        // if valid the role restriction based on Misc or Set type is used, otherwise the role
        // restriction based on item type is used.
        , exclusiveToRole_(
              ((TYPE_WRAPPER.roleRestrictionBasedOnMiscAndSetType != creature::role::Count)
                   ? TYPE_WRAPPER.roleRestrictionBasedOnMiscAndSetType
                   : EXCLUSIVE_ROLE_BASED_ON_ITEM_TYPE))
        , armorType_(ARMOR_TYPE)
        , weaponType_(WEAPON_TYPE)
        , category_(CATEGORY)
        , miscType_(TYPE_WRAPPER.misc)
        , materialPri_(MATERIAL_PRIMARY)
        , materialSec_(MATERIAL_SECONDARY)
        , weaponInfo_(WEAPON_INFO)
        , armorInfo_(ARMOR_INFO)
        , isPixie_(IS_PIXIE_ITEM)
        , uniqueType_(TYPE_WRAPPER.unique)
        , setType_(TYPE_WRAPPER.set)
        , namedType_(TYPE_WRAPPER.name)
        , elementType_(TYPE_WRAPPER.element)
        , summonInfo_(TYPE_WRAPPER.summon)
        , enchantmentsPVec_(creature::EnchantmentFactory::Instance()->MakeAndStore(
              TYPE_WRAPPER, IsWeapon(), IsArmor(), MATERIAL_PRIMARY, MATERIAL_SECONDARY))
        , imageFilename_(sfml_util::gui::ItemImageManager::Instance()->GetImageFilename(
              NAME,
              CATEGORY,
              IsJeweled(),
              IsWeapon(),
              WEAPON_INFO,
              ARMOR_INFO,
              TYPE_WRAPPER.misc,
              TYPE_WRAPPER.summon.Race(),
              TYPE_WRAPPER.summon.Role(),
              MATERIAL_PRIMARY,
              MATERIAL_SECONDARY,
              true))
    {
        if (unique_type::MagnifyingGlass == uniqueType_)
        {
            AddCategory(category::ShowsEnemyInfo);
        }

        if (item::unique_type::IsUseable(uniqueType_))
        {
            AddCategory(category::Useable);
        }

        if (element_type::None != elementType_)
        {
            name_ += element_type::Name(elementType_);
        }

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

    Item::~Item() { creature::EnchantmentFactory::Instance()->Warehouse().Free(enchantmentsPVec_); }

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
        misc::SerializeHelp::BeforeSerialize(enchantmentsPVec_, enchantmentsToSerializePVec_);
    }

    void Item::AfterSerialize()
    {
        misc::SerializeHelp::AfterSerialize(
            enchantmentsToSerializePVec_, creature::EnchantmentFactory::Instance()->Warehouse());
    }

    void Item::AfterDeserialize()
    {
        misc::SerializeHelp::AfterDeserialize(
            enchantmentsPVec_,
            enchantmentsToSerializePVec_,
            creature::EnchantmentFactory::Instance()->Warehouse());
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
