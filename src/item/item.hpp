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
#ifndef HEROESPATH_ITEM_ITEM_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEM_HPP_INCLUDED
//
// item.hpp
//  A base class for all items.
//
#include "misc/boost-serialize-includes.hpp"

#include "misc/types.hpp"
#include "item/item-type-enum.hpp"
#include "item/weapon-info.hpp"
#include "item/armor-info.hpp"
#include "item/item-profile.hpp"
#include "creature/role-enum.hpp"
#include "creature/enchantment.hpp"

#include <memory>
#include <string>
#include <vector>


namespace heroespath
{
namespace item
{

    //A base class for all items.
    class Item
    {
        Item(const Item &) =delete;
        Item & operator=(const Item &) =delete;

    public:
        explicit Item(
            const std::string &          NAME               = "no_name_error",
            const std::string &          DESC               = "no-desc_error",
            const category::Enum         CATEGORY           = category::Broken,
            const misc_type::Enum        MISC_TYPE          = misc_type::NotMisc,
            const weapon_type::Enum      WEAPON_TYPE        = weapon_type::NotAWeapon,
            const armor_type::Enum       ARMOR_TYPE         = armor_type::NotArmor,
            const material::Enum         MATERIAL_PRIMARY   = material::Nothing,
            const material::Enum         MATERIAL_SECONDARY = material::Nothing,
            const std::string &          IMAGE_FILENAME     = "",
            const Coin_t &               PRICE              = 0_coin,
            const Weight_t &             WEIGHT             = 0_weight,
            const Health_t &             DAMAGE_MIN         = 0_health,
            const Health_t &             DAMAGE_MAX         = 0_health,
            const Armor_t &              ARMOR_RATING       = 0_armor,
            const creature::role::Enum   EXCLUSIVE_TO_ROLE  = creature::role::Count,
            const weapon::WeaponInfo &   WEAPON_INFO        = weapon::WeaponInfo(),
            const armor::ArmorInfo &     ARMOR_INFO         = armor::ArmorInfo(),
            const bool                   IS_PIXIE_ITEM      = false,
            const unique_type::Enum      UNIQUE_TYPE        = unique_type::NotUnique,
            const set_type::Enum         SET_TYPE           = set_type::NotASet,
            const named_type::Enum       NAMED_TYPE         = named_type::NotNamed,
            const element_type::Enum     ELEMENT_TYPE       = element_type::None,
            const creature::SummonInfo & SUMMON_INFO        = creature::SummonInfo());

        virtual ~Item();

        inline const std::string Name() const                       { return name_; }
        inline const std::string Desc() const                       { return desc_; }

        inline const std::string ImageFilename() const              { return imageFilename_; }
        inline void ImageFilename(const std::string & S)            { imageFilename_ = S; }

        inline const weapon::WeaponInfo Weapon_Info() const         { return weaponInfo_; }
        inline const armor::ArmorInfo Armor_Info() const            { return armorInfo_; }

        inline category::Enum Category() const                      { return category_; }
        inline material::Enum MaterialPrimary() const               { return materialPri_; }
        inline material::Enum MaterialSecondary() const             { return materialSec_; }
        inline weapon_type::Enum WeaponType() const                 { return weaponType_; }
        inline armor_type::Enum ArmorType() const                   { return armorType_; }
        inline misc_type::Enum MiscType() const                     { return miscType_; }
        inline unique_type::Enum UniqueType() const                 { return uniqueType_; }
        inline set_type::Enum SetType() const                       { return setType_; }
        inline named_type::Enum NamedType() const                   { return namedType_; }
        inline element_type::Enum ElementType() const               { return elementType_; }

        inline bool IsCategoryType(const category::Enum E) const    { return (category_ & E); }
        inline bool IsWeaponType(const weapon_type::Enum E) const   { return (weaponType_ & E); }
        inline bool IsArmorType(const armor_type::Enum E) const     { return (armorType_ & E); }
        inline bool IsMiscType(const misc_type::Enum E) const       { return (miscType_ & E); }

        inline Coin_t Price() const                                 { return price_; }
        inline Weight_t Weight() const                              { return weight_; }

        inline Health_t DamageMin() const                           { return damageMin_; }
        inline Health_t DamageMax() const                           { return damageMax_; }
        inline Armor_t ArmorRating() const                          { return armorRating_; }

        inline bool IsBroken() const        { return (category_ == category::Broken); }
        inline bool IsArmor() const         { return (category_ & category::Armor); }
        inline bool IsWeapon() const        { return (category_ & category::Weapon); }
        inline bool IsWearable() const      { return (category_ & category::Wearable); }
        inline bool IsOneHanded() const     { return (category_ & category::OneHanded); }
        inline bool IsTwoHanded() const     { return (category_ & category::TwoHanded); }
        inline bool IsBodypart() const      { return (category_ & category::BodyPart); }

        inline bool IsRigid() const
        {
            return material::IsRigid(materialPri_);
        }

        inline bool ContainsSpirit() const
        {
            return material::ContainsSpirit(materialPri_, materialSec_);
        }

        inline bool IsBloody() const
        {
            return material::IsBloody(materialPri_, materialSec_);
        }

        inline bool ContainsFlesh() const
        {
            return material::ContainsFlesh(materialPri_, materialSec_);
        }

        inline float FireDamageRatio() const
        {
            return material::FireDamageRatio(materialPri_, materialSec_);
        }

        inline bool ContainsMetal() const
        {
            return material::ContainsMetal(materialPri_, materialSec_);
        }

        inline bool IsMetal() const
        {
            return material::IsMetal(materialPri_);
        }

        inline bool IsStone() const
        {
            return material::IsStone(materialPri_);
        }

        inline bool ContiansPrecious() const
        {
            return material::ContiansPrecious(materialPri_, materialSec_);
        }

        inline virtual creature::role::Enum ExclusiveRole() const
        {
            return exclusiveToRole_;
        }

        inline bool IsJeweled() const
        {
            return material::IsJewel(materialSec_);
        }

        inline bool IsJeweledRing() const
        {
            return ((miscType_ == misc_type::Ring) && IsJeweled());
        }

        inline bool IsPixie() const
        {
            return isPixie_;
        }

        inline const creature::EnchantmentPVec_t & Enchantments() const
        {
            return enchantmentsPVec_;
        }

        inline void AddCategory(const category::Enum E)
        {
            category_ = static_cast<category::Enum>(category_ | E);
        }

        void EnchantmentAdd(const creature::EnchantmentPtr_t ENCHANTMENT_PTR);

        void EnchantmentRemoveAndFree(creature::EnchantmentPtr_t);

        void EnchantmentRemoveAndFreeAll();

        const std::string BaseName() const;

        inline void SetName(const std::string & NEW_NAME)           { name_ = NEW_NAME; }
        inline void SetElementType(const element_type::Enum E)      { elementType_ = E; }
        inline void SetUniqueType(const unique_type::Enum E)        { uniqueType_ = E; }
        inline void SetSetType(const set_type::Enum E)              { setType_ = E; }
        inline void SetNamedType(const named_type::Enum E)          { namedType_ = E; }
        inline void SetMiscType(const misc_type::Enum E)            { miscType_ = E; }
        inline void SetRestrictedRole(const creature::role::Enum R) { exclusiveToRole_ = R; }

        inline void SetSummonInfo(const creature::SummonInfo & SI)  { summonInfo_ = SI; }
        inline const creature::SummonInfo & GetSummonInfo() const   { return summonInfo_; }

        inline bool IsSummoning() const
        {
            return summonInfo_.WillSummon();
        }

        inline bool IsEnchanted() const
        {
            return ((enchantmentsPVec_.empty() == false) || IsSummoning());
        }

        friend bool operator<(const Item & L, const Item & R);
        friend bool operator==(const Item & L, const Item & R);

    protected:
        std::string          name_;
        std::string          desc_;
        Coin_t               price_;
        Weight_t             weight_;
        Health_t             damageMin_;
        Health_t             damageMax_;
        Armor_t              armorRating_;
        creature::role::Enum exclusiveToRole_;
        armor_type::Enum     armorType_;
        weapon_type::Enum    weaponType_;
        category::Enum       category_;
        misc_type::Enum      miscType_;
        material::Enum       materialPri_;
        material::Enum       materialSec_;
        std::string          imageFilename_;
        weapon::WeaponInfo   weaponInfo_;
        armor::ArmorInfo     armorInfo_;
        bool                 isPixie_;
        unique_type::Enum    uniqueType_;
        set_type::Enum       setType_;
        named_type::Enum     namedType_;
        element_type::Enum   elementType_;
        creature::SummonInfo summonInfo_;

        //The Item class owns the Enchantment objects and is responsible
        //for their lifetimes.
        creature::EnchantmentPVec_t enchantmentsPVec_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & name_;
            ar & desc_;
            ar & price_;
            ar & weight_;
            ar & damageMin_;
            ar & damageMax_;
            ar & armorRating_;
            ar & exclusiveToRole_;
            ar & armorType_;
            ar & weaponType_;
            ar & category_;
            ar & miscType_;
            ar & materialPri_;
            ar & materialSec_;
            ar & imageFilename_;
            ar & weaponInfo_;
            ar & armorInfo_;
            ar & isPixie_;
            ar & uniqueType_;
            ar & setType_;
            ar & namedType_;
            ar & elementType_;
            ar & summonInfo_;
            ar & enchantmentsPVec_;
        }
    };

    using ItemPtr_t  = Item *;
    using ItemUPtr_t = std::unique_ptr<Item>;
    using ItemUVec_t = std::vector<ItemUPtr_t>;


    bool operator<(const Item & L, const Item & R);


    bool operator==(const Item & L, const Item & R);


    inline bool operator!=(const Item & L, const Item & R)
    {
        return ! (L == R);
    }

}
}

#endif //HEROESPATH_ITEM_ITEM_HPP_INCLUDED