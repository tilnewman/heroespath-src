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
#ifndef GAME_ITEM_ITEMPROFILE_HPP_INCLUDE
#define GAME_ITEM_ITEMPROFILE_HPP_INCLUDE
//
// item-profile.hpp
//
#include "game/creature/race-enum.hpp"
#include "game/item/item-type-enum.hpp"
#include "game/item/armor-types.hpp"
#include "game/item/weapon-types.hpp"

#include "sfml-util/size-enum.hpp"

#include "misc/handy-types.hpp"

#include <tuple>
#include <string>
#include <vector>


namespace game
{
namespace item
{

    struct MemberStrings
    {
        MemberStrings(const ::misc::StrVec_t & NORMAL_STRINGS,
                      const ::misc::StrVec_t & WEAPON_STRINGS,
                      const ::misc::StrVec_t & ARMOR_STRINGS)
        :
            normalStrings(NORMAL_STRINGS),
            weaponStrings(WEAPON_STRINGS),
            armorStrings(ARMOR_STRINGS)
        {}

        const std::string InvalidString() const;

        inline bool IsValid() const { return InvalidString().empty(); }

        const std::string ToString() const;

    private:
        ::misc::StrVec_t normalStrings;
        ::misc::StrVec_t weaponStrings;
        ::misc::StrVec_t armorStrings;
    };


    //A collection of details about an item that uniquely idenify an item.
    //A "Thin" Profile is an incomplete profile used as a placeholder for a
    //"Fat" Profile.  A "Fat" Profile is a complete profile that is used as
    //a placeholder for an actual Item.  See ItemProfileWarehouse.  That is
    //the only place where thin profiles are used, and where the complete vec
    //of Fat Profiles are permanently stored as placeholders for actual items.
    class ItemProfile
    {
    public:
        //use this constructor with the Set() functions for creating specific ItemProfiles
        ItemProfile();

        //use this constructor to create a unique profile for an item
        ItemProfile(const std::string &     BASE_NAME,
                    const category::Enum    CATEGORY,
                    const armor_type::Enum  ARMOR,
                    const weapon_type::Enum WEAPON,
                    const unique_type::Enum UNIQUE,
                    const misc_type::Enum   MISC,
                    const set_type::Enum    SET,
                    const named_type::Enum  NAMED);

        inline const std::string BaseName() const                      { return baseName_; }
        inline category::Enum Category() const                         { return category_; }
        inline armor_type::Enum ArmorType() const                      { return armor_; }
        inline weapon_type::Enum WeaponType() const                    { return weapon_; }
        inline unique_type::Enum UniqueType() const                    { return unique_; }
        inline misc_type::Enum MiscType() const                        { return misc_; }
        inline set_type::Enum SetType() const                          { return set_; }
        inline named_type::Enum NamedType() const                      { return named_; }
        inline element_type::Enum ElementType() const                  { return element_; }
        inline bool IsPixie() const                                    { return isPixie_; }

        inline armor::shield_type::Enum ShieldType() const              { return shield_; }
        inline armor::helm_type::Enum HelmType() const                  { return helm_; }
        inline armor::base_type::Enum BaseType() const                  { return base_; }
        inline armor::cover_type::Enum CoverType() const                { return cover_; }

        inline bool IsAventail() const                                  { return isAventail_; }
        inline bool IsBracer() const                                    { return isBracer_; }
        inline bool IsShirt() const                                     { return isShirt_; }
        inline bool IsBoots() const                                     { return isBoots_; }
        inline bool IsPants() const                                     { return isPants_; }
        inline bool IsGauntlets() const                                 { return isGauntlets_; }

        inline weapon::sword_type::Enum SwordType() const               { return sword_; }
        inline weapon::axe_type::Enum AxeType() const                   { return axe_; }
        inline weapon::club_type::Enum ClubType() const                 { return club_; }
        inline weapon::whip_type::Enum WhipType() const                 { return whip_; }
        inline weapon::projectile_type::Enum ProjectileType() const     { return proj_; }
        inline weapon::bladedstaff_type::Enum BladedStaffType() const   { return bstaff_; }

        inline sfml_util::Size::Enum Size() const                       { return size_; }

        inline bool IsKnife() const                                     { return isKnife_; }
        inline bool IsDagger() const                                    { return isDagger_; }
        inline bool IsStaff() const                                     { return isStaff_; }
        inline bool IsQuarterStaff() const                              { return isQStaff_; }

        inline material::Enum MaterialPrimary() const                   { return matPri_; }
        inline material::Enum MaterialSecondary() const                 { return matSec_; }

        inline armor::base_type::Enum ArmorTypeRestriction() const      { return armorType_; }

        inline creature::role::Enum Role() const                        { return role_; }

        inline int TreasureScore(const bool IS_RELIGIOUS = false) const
        {
            if (IS_RELIGIOUS)
            {
                return static_cast<int>(static_cast<float>(score_) * religious_);
            }
            else
            {
                return score_;
            }
        }

        inline const creature::SummonInfo Summoning() const             { return summonInfo_; }

        inline bool WillSummon() const
        {
            return summonInfo_.WillSummon();
        }

        void SetSummoningAndAdjustScore(const creature::SummonInfo &);

        inline float Religious() const                                  { return religious_; }
        inline void Religious(const float F)                            { religious_ = F; }

        inline bool IsWeapon() const
        {
            return (weapon_ != weapon_type::NotAWeapon);
        }

        inline bool IsArmor() const
        {
            return (armor_ != armor_type::NotArmor);
        }

        inline bool IsEquipment() const
        {
            return (IsWeapon() || IsArmor());
        }

        inline bool IsMisc() const
        {
            return ((misc_ != misc_type::Count) && (misc_ != misc_type::NotMisc));
        }

        inline bool IsUnique() const
        {
            return ((unique_ != unique_type::Count) && (unique_ != unique_type::NotUnique));
        }

        inline bool IsSet() const
        {
            return ((set_ != set_type::Count) && (set_ != set_type::NotASet));
        }

        inline bool IsNamed() const
        {
            return ((named_ != named_type::Count) && (named_ != named_type::NotNamed));
        }

        inline bool IsElemental() const
        {
            return (element_ != element_type::None);
        }

        inline bool IsStandard() const
        {
            return ((IsUnique() == false) &&
                    (IsSet() == false) &&
                    (IsNamed() == false) &&
                    (IsElemental() == false));
        }

        inline bool IsStandardEquipment() const
        {
            return (IsStandard() && IsEquipment());
        }

        const MemberStrings ToMemberStrings() const;

        inline const std::string ToString() const { return ToMemberStrings().ToString(); }

        inline bool IsValid() const { return ToMemberStrings().IsValid(); }

        void SetUnique(const unique_type::Enum,
                       const material::Enum MATERIAL_PRIMARY   = material::Nothing,
                       const material::Enum MATERIAL_SECONDARY = material::Nothing,
                       const element_type::Enum ELEMENT_TYPE   = element_type::None);

        void SetMisc(const misc_type::Enum,
                     const bool           IS_PIXIE           = false,
                     const material::Enum MATERIAL_PRIMARY   = material::Nothing,
                     const material::Enum MATERIAL_SECONDARY = material::Nothing,
                     const set_type::Enum SET_TYPE           = set_type::NotASet);

        void SetShield(const armor::shield_type::Enum,
                       const material::Enum     MATERIAL_PRIMARY   = material::Nothing,
                       const material::Enum     MATERIAL_SECONDARY = material::Nothing,
                       const named_type::Enum   NAMED_TYPE         = named_type::NotNamed,
                       const set_type::Enum     SET_TYPE           = set_type::NotASet,
                       const element_type::Enum ELEMENT_TYPE       = element_type::None);

        void SetHelm(const armor::helm_type::Enum,
                     const material::Enum     MATERIAL_PRIMARY   = material::Nothing,
                     const material::Enum     MATERIAL_SECONDARY = material::Nothing,
                     const named_type::Enum   NAMED_TYPE         = named_type::NotNamed,
                     const set_type::Enum     SET_TYPE           = set_type::NotASet,
                     const element_type::Enum ELEMENT_TYPE       = element_type::None);

        void SetCover(const armor::cover_type::Enum,
                      const material::Enum     MATERIAL_PRIMARY   = material::Nothing,
                      const material::Enum     MATERIAL_SECONDARY = material::Nothing,
                      const named_type::Enum   NAMED_TYPE         = named_type::NotNamed,
                      const set_type::Enum     SET_TYPE           = set_type::NotASet,
                      const element_type::Enum ELEMENT_TYPE       = element_type::None,
                      const bool               IS_PIXIE           = false);

        void SetAventail(const armor::base_type::Enum BASE_TYPE          = armor::base_type::Plain,
                         const material::Enum         MATERIAL_PRIMARY   = material::Nothing,
                         const material::Enum         MATERIAL_SECONDARY = material::Nothing,
                         const named_type::Enum       NAMED_TYPE         = named_type::NotNamed,
                         const set_type::Enum         SET_TYPE           = set_type::NotASet,
                         const element_type::Enum     ELEMENT_TYPE       = element_type::None);

        void SetBracer(const armor::base_type::Enum BASE_TYPE          = armor::base_type::Plain,
                       const material::Enum         MATERIAL_PRIMARY   = material::Nothing,
                       const material::Enum         MATERIAL_SECONDARY = material::Nothing,
                       const named_type::Enum       NAMED_TYPE         = named_type::NotNamed,
                       const set_type::Enum         SET_TYPE           = set_type::NotASet,
                       const element_type::Enum     ELEMENT_TYPE       = element_type::None,
                       const bool                   IS_PIXIE           = false);

        void SetShirt(const armor::base_type::Enum BASE_TYPE          = armor::base_type::Plain,
                      const material::Enum         MATERIAL_PRIMARY   = material::Nothing,
                      const material::Enum         MATERIAL_SECONDARY = material::Nothing,
                      const named_type::Enum       NAMED_TYPE         = named_type::NotNamed,
                      const set_type::Enum         SET_TYPE           = set_type::NotASet,
                      const element_type::Enum     ELEMENT_TYPE       = element_type::None,
                      const bool                   IS_PIXIE           = false);

        void SetBoots(const armor::base_type::Enum BASE_TYPE          = armor::base_type::Plain,
                      const material::Enum         MATERIAL_PRIMARY   = material::Nothing,
                      const material::Enum         MATERIAL_SECONDARY = material::Nothing,
                      const named_type::Enum       NAMED_TYPE         = named_type::NotNamed,
                      const set_type::Enum         SET_TYPE           = set_type::NotASet,
                      const element_type::Enum     ELEMENT_TYPE       = element_type::None,
                      const bool                   IS_PIXIE           = false);

        void SetPants(const armor::base_type::Enum BASE_TYPE          = armor::base_type::Plain,
                      const material::Enum         MATERIAL_PRIMARY   = material::Nothing,
                      const material::Enum         MATERIAL_SECONDARY = material::Nothing,
                      const named_type::Enum       NAMED_TYPE         = named_type::NotNamed,
                      const set_type::Enum         SET_TYPE           = set_type::NotASet,
                      const element_type::Enum     ELEMENT_TYPE       = element_type::None,
                      const bool                   IS_PIXIE           = false);

        void SetGauntlets(const armor::base_type::Enum BASE_TYPE          = armor::base_type::Plain,
                          const material::Enum         MATERIAL_PRIMARY   = material::Nothing,
                          const material::Enum         MATERIAL_SECONDARY = material::Nothing,
                          const named_type::Enum       NAMED_TYPE         = named_type::NotNamed,
                          const set_type::Enum         SET_TYPE           = set_type::NotASet,
                          const element_type::Enum     ELEMENT_TYPE       = element_type::None,
                          const bool                   IS_PIXIE           = false);

        void SetSword(const weapon::sword_type::Enum,
                      const material::Enum     MATERIAL_PRIMARY   = material::Nothing,
                      const material::Enum     MATERIAL_SECONDARY = material::Nothing,
                      const named_type::Enum   NAMED_TYPE         = named_type::NotNamed,
                      const set_type::Enum     SET_TYPE           = set_type::NotASet,
                      const element_type::Enum ELEMENT_TYPE       = element_type::None);

        void SetAxe(const weapon::axe_type::Enum,
                    const material::Enum     MATERIAL_PRIMARY   = material::Nothing,
                    const material::Enum     MATERIAL_SECONDARY = material::Nothing,
                    const named_type::Enum   NAMED_TYPE         = named_type::NotNamed,
                    const set_type::Enum     SET_TYPE           = set_type::NotASet,
                    const element_type::Enum ELEMENT_TYPE       = element_type::None);

        void SetClub(const weapon::club_type::Enum,
                     const material::Enum     MATERIAL_PRIMARY   = material::Nothing,
                     const material::Enum     MATERIAL_SECONDARY = material::Nothing,
                     const named_type::Enum   NAMED_TYPE         = named_type::NotNamed,
                     const set_type::Enum     SET_TYPE           = set_type::NotASet,
                     const element_type::Enum ELEMENT_TYPE       = element_type::None);

        void SetWhip(const weapon::whip_type::Enum,
                     const material::Enum     MATERIAL_PRIMARY   = material::Nothing,
                     const material::Enum     MATERIAL_SECONDARY = material::Nothing,
                     const named_type::Enum   NAMED_TYPE         = named_type::NotNamed,
                     const set_type::Enum     SET_TYPE           = set_type::NotASet,
                     const element_type::Enum ELEMENT_TYPE       = element_type::None);

        void SetProjectile(const weapon::projectile_type::Enum,
                           const material::Enum     MATERIAL_PRIMARY   = material::Nothing,
                           const material::Enum     MATERIAL_SECONDARY = material::Nothing,
                           const named_type::Enum   NAMED_TYPE         = named_type::NotNamed,
                           const set_type::Enum     SET_TYPE           = set_type::NotASet,
                           const element_type::Enum ELEMENT_TYPE       = element_type::None);

        void SetBladedStaff(const weapon::bladedstaff_type::Enum,
                             const material::Enum     MATERIAL_PRIMARY   = material::Nothing,
                             const material::Enum     MATERIAL_SECONDARY = material::Nothing,
                             const named_type::Enum   NAMED_TYPE         = named_type::NotNamed,
                             const set_type::Enum     SET_TYPE           = set_type::NotASet,
                             const element_type::Enum ELEMENT_TYPE       = element_type::None);

        void SetKnife(const sfml_util::Size::Enum SIZE               = sfml_util::Size::Medium,
                      const material::Enum        MATERIAL_PRIMARY   = material::Nothing,
                      const material::Enum        MATERIAL_SECONDARY = material::Nothing,
                      const named_type::Enum      NAMED_TYPE         = named_type::NotNamed,
                      const set_type::Enum        SET_TYPE           = set_type::NotASet,
                      const element_type::Enum    ELEMENT_TYPE       = element_type::None,
                      const bool                  IS_PIXIE           = false);

        void SetDagger(const sfml_util::Size::Enum SIZE               = sfml_util::Size::Medium,
                       const material::Enum        MATERIAL_PRIMARY   = material::Nothing,
                       const material::Enum        MATERIAL_SECONDARY = material::Nothing,
                       const named_type::Enum      NAMED_TYPE         = named_type::NotNamed,
                       const set_type::Enum        SET_TYPE           = set_type::NotASet,
                       const element_type::Enum    ELEMENT_TYPE       = element_type::None,
                       const bool                  IS_PIXIE           = false);

        void SetStaff(const material::Enum     MATERIAL_PRIMARY   = material::Nothing,
                      const material::Enum     MATERIAL_SECONDARY = material::Nothing,
                      const named_type::Enum   NAMED_TYPE         = named_type::NotNamed,
                      const set_type::Enum     SET_TYPE           = set_type::NotASet,
                      const element_type::Enum ELEMENT_TYPE       = element_type::None);

        void SetQuarterStaff(const material::Enum     MATERIAL_PRIMARY   = material::Nothing,
                             const material::Enum     MATERIAL_SECONDARY = material::Nothing,
                             const named_type::Enum   NAMED_TYPE         = named_type::NotNamed,
                             const set_type::Enum     SET_TYPE           = set_type::NotASet,
                             const element_type::Enum ELEMENT_TYPE       = element_type::None);

        inline void SetArmorTypeRestriction(const armor::base_type::Enum E)  { armorType_ = E; }

        inline void Role(const creature::role::Enum E) { role_ = E; }

        friend bool operator==(const ItemProfile & L, const ItemProfile & R);
        friend bool operator<(const ItemProfile & L, const ItemProfile & R);

    private:
        void SetHelper(const material::Enum,
                       const material::Enum,
                       const named_type::Enum,
                       const set_type::Enum,
                       const element_type::Enum,
                       const bool IS_PIXIE = false);

        int ScoreHelper(const item::material::Enum     MATERIAL_PRI,
                        const item::material::Enum     MATERIAL_SEC,
                        const item::named_type::Enum   NAMED_TYPE,
                        const item::set_type::Enum     SET_TYPE,
                        const item::element_type::Enum ELEMENT_TYPE,
                        const bool                     IS_WEAPON) const;

        void EquippableHelper(const int SCORE_BONUS);

    private:
        std::string        baseName_;
        category::Enum     category_;
        armor_type::Enum   armor_;
        weapon_type::Enum  weapon_;
        unique_type::Enum  unique_;
        misc_type::Enum    misc_;
        set_type::Enum     set_;
        named_type::Enum   named_;
        element_type::Enum element_;

        bool isPixie_;

        armor::shield_type::Enum shield_;
        armor::helm_type::Enum   helm_;
        armor::base_type::Enum   base_;
        armor::cover_type::Enum  cover_;

        bool isAventail_;
        bool isBracer_;
        bool isShirt_;
        bool isBoots_;
        bool isPants_;
        bool isGauntlets_;

        weapon::sword_type::Enum        sword_;
        weapon::axe_type::Enum          axe_;
        weapon::club_type::Enum         club_;
        weapon::whip_type::Enum         whip_;
        weapon::projectile_type::Enum   proj_;
        weapon::bladedstaff_type::Enum  bstaff_;

        sfml_util::Size::Enum size_;

        bool isKnife_;
        bool isDagger_;
        bool isStaff_;
        bool isQStaff_;

        material::Enum matPri_;
        material::Enum matSec_;

        armor::base_type::Enum armorType_;

        creature::role::Enum role_;

        int score_;

        creature::SummonInfo summonInfo_;

        float religious_;
    };


    using ItemProfileVec_t = std::vector<ItemProfile>;


    inline bool operator==(const ItemProfile & L, const ItemProfile & R)
    {
        return std::tie(L.baseName_,
                        L.category_,
                        L.armor_,
                        L.weapon_,
                        L.unique_,
                        L.misc_,
                        L.set_,
                        L.named_,
                        L.element_,
                        L.isPixie_,
                        L.shield_,
                        L.helm_,
                        L.base_,
                        L.cover_,
                        L.isAventail_,
                        L.isBracer_,
                        L.isShirt_,
                        L.isBoots_,
                        L.isPants_,
                        L.isGauntlets_,
                        L.sword_,
                        L.axe_,
                        L.club_,
                        L.whip_,
                        L.proj_,
                        L.bstaff_,
                        L.size_,
                        L.isKnife_,
                        L.isDagger_,
                        L.isStaff_,
                        L.isQStaff_,
                        L.matPri_,
                        L.matSec_,
                        L.armorType_,
                        L.role_,
                        L.score_,
                        L.summonInfo_)
                ==
               std::tie(R.baseName_,
                        R.category_,
                        R.armor_,
                        R.weapon_,
                        R.unique_,
                        R.misc_,
                        R.set_,
                        R.named_,
                        R.element_,
                        R.isPixie_,
                        R.shield_,
                        R.helm_,
                        R.base_,
                        R.cover_,
                        R.isAventail_,
                        R.isBracer_,
                        R.isShirt_,
                        R.isBoots_,
                        R.isPants_,
                        R.isGauntlets_,
                        R.sword_,
                        R.axe_,
                        R.club_,
                        R.whip_,
                        R.proj_,
                        R.bstaff_,
                        R.size_,
                        R.isKnife_,
                        R.isDagger_,
                        R.isStaff_,
                        R.isQStaff_,
                        R.matPri_,
                        R.matSec_,
                        R.armorType_,
                        R.role_,
                        R.score_,
                        R.summonInfo_);
    }


    inline bool operator!=(const ItemProfile & L, const ItemProfile & R)
    {
        return ! (L == R);
    }


    inline bool operator<(const ItemProfile & L, const ItemProfile & R)
    {
        return std::tie(L.baseName_,
                        L.category_,
                        L.armor_,
                        L.weapon_,
                        L.unique_,
                        L.misc_,
                        L.set_,
                        L.named_,
                        L.element_,
                        L.isPixie_,
                        L.shield_,
                        L.helm_,
                        L.base_,
                        L.cover_,
                        L.isAventail_,
                        L.isBracer_,
                        L.isShirt_,
                        L.isBoots_,
                        L.isPants_,
                        L.isGauntlets_,
                        L.sword_,
                        L.axe_,
                        L.club_,
                        L.whip_,
                        L.proj_,
                        L.bstaff_,
                        L.size_,
                        L.isKnife_,
                        L.isDagger_,
                        L.isStaff_,
                        L.isQStaff_,
                        L.matPri_,
                        L.matSec_,
                        L.armorType_,
                        L.role_,
                        L.score_,
                        L.summonInfo_)
                <
               std::tie(R.baseName_,
                        R.category_,
                        R.armor_,
                        R.weapon_,
                        R.unique_,
                        R.misc_,
                        R.set_,
                        R.named_,
                        R.element_,
                        R.isPixie_,
                        R.shield_,
                        R.helm_,
                        R.base_,
                        R.cover_,
                        R.isAventail_,
                        R.isBracer_,
                        R.isShirt_,
                        R.isBoots_,
                        R.isPants_,
                        R.isGauntlets_,
                        R.sword_,
                        R.axe_,
                        R.club_,
                        R.whip_,
                        R.proj_,
                        R.bstaff_,
                        R.size_,
                        R.isKnife_,
                        R.isDagger_,
                        R.isStaff_,
                        R.isQStaff_,
                        R.matPri_,
                        R.matSec_,
                        R.armorType_,
                        R.role_,
                        R.score_,
                        R.summonInfo_);
    }

}
}

#endif //GAME_ITEM_ITEMPROFILE_HPP_INCLUDE
