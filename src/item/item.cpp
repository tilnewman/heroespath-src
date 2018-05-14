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
#include "creature/enchantment-warehouse.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/serialize-helpers.hpp"
#include "misc/vectors.hpp"
#include "sfml-util/gui/item-image-loader.hpp"

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
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const Coin_t & PRICE,
        const Weight_t & WEIGHT,
        const Health_t & DAMAGE_MIN,
        const Health_t & DAMAGE_MAX,
        const Armor_t & ARMOR_RATING,
        const TypeWrapper & TYPE_WRAPPER,
        const weapon::WeaponTypeWrapper & WEAPON_INFO,
        const armor::ArmorTypeWrapper & ARMOR_INFO,
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
              ((TYPE_WRAPPER.roleRestriction != creature::role::Count)
                   ? TYPE_WRAPPER.roleRestriction
                   : EXCLUSIVE_ROLE_BASED_ON_ITEM_TYPE))
        , category_(CATEGORY)
        , miscType_(TYPE_WRAPPER.misc)
        , materialPri_(MATERIAL_PRIMARY)
        , materialSec_(MATERIAL_SECONDARY)
        , weaponInfo_(WEAPON_INFO)
        , armorInfo_(ARMOR_INFO)
        , isPixie_(IS_PIXIE_ITEM)
        , setType_(TYPE_WRAPPER.set)
        , namedType_(TYPE_WRAPPER.name)
        , elementType_(TYPE_WRAPPER.element)
        , summonInfo_(TYPE_WRAPPER.summon)
        , enchantmentsPVec_()
        , imageFilename_("")
    {
        creature::EnchantmentFactory enchantmentFactory;

        enchantmentsPVec_ = enchantmentFactory.MakeAndStore(
            TYPE_WRAPPER, MATERIAL_PRIMARY, MATERIAL_SECONDARY, IsWeapon(), IsArmor());

        sfml_util::gui::ItemImageLoader itemImageMachine;
        imageFilename_ = itemImageMachine.Filename(this, true);
    }

    Item::~Item() { creature::EnchantmentWarehouse::Access().Free(enchantmentsPVec_); }

    const std::string Item::Name() const { return ComposeName(name_); }

    const std::string Item::Desc() const
    {
        std::ostringstream ss;

        ss << "A ";

        if (IsMagical())
        {
            ss << "magical ";
        }
        else if (IsBroken())
        {
            ss << "broken ";
        }

        ss << desc_ << ".";
        return ss.str();
    }

    const std::string Item::ShortName() const
    {
        if (IsUnique())
        {
            return ComposeName(misc_type::Name(miscType_));
        }
        else if (IsSet())
        {
            return ComposeName(set_type::Name(SetType()) + " " + ReadableName());
        }
        else if (IsNamed())
        {
            return ComposeName(named_type::Name(NamedType()) + " " + ReadableName());
        }
        else
        {
            return BaseName();
        }
    }

    const std::string Item::BaseName() const { return ComposeName(ReadableName()); }

    bool Item::MustBePixieVersionForPixiesToEquip() const
    {
        if (IsWeapon() || IsArmor())
        {
            return true;
        }
        else // assume must be misc
        {
            return misc_type::MustBePixieVersionForPixiesToEquip(MiscType());
        }
    }

    const std::string Item::ToString() const
    {
        std::ostringstream ss;

        ss << "name=" << std::quoted(Name()) << ", desc=" << std::quoted(Desc());

        if (category::None != category_)
        {
            ss << ", category=" << category::ToString(category_, misc::Wrap::Yes);
        }

        if (IsUnique())
        {
            ss << ", unique";
        }

        if (IsQuestItem())
        {
            ss << ", quest";
        }

        if (armor::ArmorTypeWrapper() != armorInfo_)
        {
            ss << ", armor_info=" << armorInfo_.ToString();
        }

        if (weapon::WeaponTypeWrapper() != weaponInfo_)
        {
            ss << ", weapon_info=" << weaponInfo_.ToString();
        }

        if (misc_type::Not != miscType_)
        {
            ss << ", misc_type="
               << ((misc_type::Count == miscType_) ? "Count" : misc_type::ToString(miscType_));
        }

        if (isPixie_)
        {
            ss << ", is_pixie=" << std::boolalpha << isPixie_;
        }

        if (creature::role::Count != exclusiveToRole_)
        {
            ss << ", role_restriction=" << creature::role::ToString(exclusiveToRole_);
        }

        ss << ", mat_pri=" << material::ToString(materialPri_);

        if (material::Nothing != materialSec_)
        {
            ss << ", mat_sec=" << material::ToString(materialSec_);
        }

        if (set_type::Not != setType_)
        {
            ss << ", set_type="
               << ((set_type::Count == setType_) ? "Count" : set_type::ToString(setType_));
        }

        if (named_type::Not != namedType_)
        {
            ss << ", named_type="
               << ((named_type::Count == namedType_) ? "Count" : named_type::ToString(namedType_));
        }

        if (element_type::None != elementType_)
        {
            ss << ", element_type=" << element_type::ToString(elementType_, misc::Wrap::Yes);
        }

        if (summonInfo_.CanSummon())
        {
            ss << ", summonInfo=" << summonInfo_.ToString();
        }
        else if (summonInfo_.IsDefaultAndCompletelyInvalid() == false)
        {
            ss << ", summonInfo=" << summonInfo_.ToString() << "(but CanSummon()=false?)";
        }

        if (enchantmentsPVec_.empty() == false)
        {
            ss << ", enchantments={";

            for (auto const & ENCHANTMENT_PTR : enchantmentsPVec_)
            {
                ss << ENCHANTMENT_PTR->ToString() << " | ";
            }

            ss << "}";
        }

        return ss.str();
    }

    void Item::BeforeSerialize()
    {
        misc::SerializeHelp::BeforeSerialize(enchantmentsPVec_, enchantmentsToSerializePVec_);
    }

    void Item::AfterSerialize()
    {
        misc::SerializeHelp::AfterSerialize(
            enchantmentsToSerializePVec_, creature::EnchantmentWarehouse::Access());
    }

    void Item::AfterDeserialize()
    {
        misc::SerializeHelp::AfterDeserialize(
            enchantmentsPVec_,
            enchantmentsToSerializePVec_,
            creature::EnchantmentWarehouse::Access());
    }

    const std::string Item::ReadableName() const
    {
        if (IsWeapon())
        {
            return weaponInfo_.ReadableName();
        }
        else if (IsArmor())
        {
            return armorInfo_.ReadableName();
        }
        else if (IsMisc())
        {
            return misc_type::Name(miscType_);
        }
        else
        {
            return "";
        }
    }

    const std::string Item::ComposeName(const std::string ROOT_NAME) const
    {
        std::ostringstream ss;

        if (IsBroken())
        {
            ss << "broken ";
        }

        ss << ROOT_NAME;

        if (IsMagical())
        {
            ss << " *";
        }

        return ss.str();
    }

    bool operator<(const Item & L, const Item & R)
    {
        // name_ and desc_ contain random adjectives so don't include them in comparisons
        // image filename is generated entirely by other members, so don't include in comparisons

        if (std::tie(
                L.price_,
                L.weight_,
                L.damageMin_,
                L.damageMax_,
                L.armorRating_,
                L.exclusiveToRole_,
                L.category_,
                L.miscType_,
                L.materialPri_,
                L.materialSec_,
                L.weaponInfo_,
                L.armorInfo_,
                L.isPixie_,
                L.setType_,
                L.namedType_,
                L.summonInfo_,
                L.elementType_)
            < std::tie(
                  R.price_,
                  R.weight_,
                  R.damageMin_,
                  R.damageMax_,
                  R.armorRating_,
                  R.exclusiveToRole_,
                  R.category_,
                  R.miscType_,
                  R.materialPri_,
                  R.materialSec_,
                  R.weaponInfo_,
                  R.armorInfo_,
                  R.isPixie_,
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
        // name_ and desc_ contain random adjectives so don't include them in comparisons
        // image filename is generated entirely by other members, so don't include in comparisons

        if (std::tie(
                L.price_,
                L.weight_,
                L.damageMin_,
                L.damageMax_,
                L.armorRating_,
                L.exclusiveToRole_,
                L.category_,
                L.miscType_,
                L.materialPri_,
                L.materialSec_,
                L.weaponInfo_,
                L.armorInfo_,
                L.isPixie_,
                L.setType_,
                L.namedType_,
                L.summonInfo_,
                L.elementType_)
            != std::tie(
                   R.price_,
                   R.weight_,
                   R.damageMin_,
                   R.damageMax_,
                   R.armorRating_,
                   R.exclusiveToRole_,
                   R.category_,
                   R.miscType_,
                   R.materialPri_,
                   R.materialSec_,
                   R.weaponInfo_,
                   R.armorInfo_,
                   R.isPixie_,
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
