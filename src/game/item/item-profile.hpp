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
#include "game/item/item-type-enum.hpp"
#include "game/item/armor-types.hpp"
#include "game/item/weapon-types.hpp"
#include "sfml-util/size-enum.hpp"

#include <tuple>
#include <string>
#include <vector>


namespace game
{
namespace item
{

    //A collection of details about an item that uniquely idenify an item.
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

        const std::string BaseName() const                      { return baseName_; }
        category::Enum Category() const                         { return category_; }
        armor_type::Enum ArmorType() const                      { return armor_; }
        weapon_type::Enum WeaponType() const                    { return weapon_; }
        unique_type::Enum UniqueType() const                    { return unique_; }
        misc_type::Enum MiscType() const                        { return misc_; }
        set_type::Enum SetType() const                          { return set_; }
        named_type::Enum NamedType() const                      { return named_; }

        armor::shield_type::Enum ShieldType() const             { return shield_; }
        armor::helm_type::Enum HelmType() const                 { return helm_; }
        armor::base_type::Enum BaseType() const                 { return base_; }
        armor::cover_type::Enum CoverType() const               { return cover_; }

        bool IsAventail() const                                 { return isAventail_; }
        bool IsBracer() const                                   { return isBracer_; }
        bool IsShirt() const                                    { return isShirt_; }
        bool IsBoots() const                                    { return isBoots_; }
        bool IsPants() const                                    { return isPants_; }
        bool IsGauntlets() const                                { return isGauntlets_; }

        weapon::sword_type::Enum SwordType() const              { return sword_; }
        weapon::axe_type::Enum AxeType() const                  { return axe_; }
        weapon::club_type::Enum ClubType() const                { return club_; }
        weapon::whip_type::Enum WhipType() const                { return whip_; }
        weapon::projectile_type::Enum ProjectileType() const    { return proj_; }
        weapon::bladedstaff_type::Enum BladdedStaffType() const { return bstaff_; }

        sfml_util::Size::Enum Size() const { return size_; }

        bool IsKnife() const                                    { return isKnife_; }
        bool IsDagger() const                                   { return isDagger_; }
        bool IsStaff() const                                    { return isStaff_; }
        bool IsQuarterStaff() const                             { return isQStaff_; }

        material::Enum MaterialPrimary() const                  { return matPri_; }
        material::Enum MaterialSecondary() const                { return matSec_; }

        int TreasureScore() const                               { return score_; }

        void SetUnique(const unique_type::Enum, const material::Enum);
        
        void SetMisc(const misc_type::Enum);
        
        void SetShield(const armor::shield_type::Enum,
                       const material::Enum,
                       const material::Enum,
                       const named_type::Enum NAMED_TYPE = named_type::NotNamed,
                       const set_type::Enum   SET_TYPE   = set_type::NotASet);

        void SetHelm(const armor::helm_type::Enum,
                     const material::Enum,
                     const material::Enum   MATERIAL_SECONDARY = material::Nothing,
                     const named_type::Enum NAMED_TYPE         = named_type::NotNamed,
                     const set_type::Enum   SET_TYPE           = set_type::NotASet);

        void SetCover(const armor::cover_type::Enum,
                      const material::Enum,
                      const material::Enum   MATERIAL_SECONDARY = material::Nothing,
                      const named_type::Enum NAMED_TYPE         = named_type::NotNamed,
                      const set_type::Enum   SET_TYPE           = set_type::NotASet);

        void SetAventail(const armor::base_type::Enum,
                         const material::Enum,
                         const material::Enum   MATERIAL_SECONDARY = material::Nothing,
                         const named_type::Enum NAMED_TYPE         = named_type::NotNamed,
                         const set_type::Enum   SET_TYPE           = set_type::NotASet);

        void SetBracer(const armor::base_type::Enum,
                       const material::Enum,
                       const material::Enum   MATERIAL_SECONDARY = material::Nothing,
                       const named_type::Enum NAMED_TYPE         = named_type::NotNamed,
                       const set_type::Enum   SET_TYPE           = set_type::NotASet);

        void SetShirt(const armor::base_type::Enum,
                      const material::Enum,
                      const material::Enum   MATERIAL_SECONDARY = material::Nothing,
                      const named_type::Enum NAMED_TYPE         = named_type::NotNamed,
                      const set_type::Enum   SET_TYPE           = set_type::NotASet);

        void SetBoots(const armor::base_type::Enum,
                      const material::Enum,
                      const material::Enum   MATERIAL_SECONDARY = material::Nothing,
                      const named_type::Enum NAMED_TYPE         = named_type::NotNamed,
                      const set_type::Enum   SET_TYPE           = set_type::NotASet);

        void SetPants(const armor::base_type::Enum,
                      const material::Enum,
                      const material::Enum   MATERIAL_SECONDARY = material::Nothing,
                      const named_type::Enum NAMED_TYPE         = named_type::NotNamed,
                      const set_type::Enum   SET_TYPE           = set_type::NotASet);

        void SetGauntlets(const armor::base_type::Enum,
                          const material::Enum,
                          const material::Enum   MATERIAL_SECONDARY = material::Nothing,
                          const named_type::Enum NAMED_TYPE         = named_type::NotNamed,
                          const set_type::Enum   SET_TYPE           = set_type::NotASet);

        void SetSword(const weapon::sword_type::Enum,
                      const material::Enum,
                      const material::Enum   MATERIAL_SECONDARY = material::Nothing,
                      const named_type::Enum NAMED_TYPE         = named_type::NotNamed,
                      const set_type::Enum   SET_TYPE           = set_type::NotASet);

        void SetAxe(const weapon::axe_type::Enum,
                    const material::Enum,
                    const material::Enum   MATERIAL_SECONDARY = material::Nothing,
                    const named_type::Enum NAMED_TYPE         = named_type::NotNamed,
                    const set_type::Enum   SET_TYPE           = set_type::NotASet);

        void SetClub(const weapon::club_type::Enum,
                     const material::Enum,
                     const material::Enum   MATERIAL_SECONDARY = material::Nothing,
                     const named_type::Enum NAMED_TYPE         = named_type::NotNamed,
                     const set_type::Enum   SET_TYPE           = set_type::NotASet);

        void SetWhip(const weapon::whip_type::Enum,
                     const material::Enum,
                     const material::Enum   MATERIAL_SECONDARY = material::Nothing,
                     const named_type::Enum NAMED_TYPE         = named_type::NotNamed,
                     const set_type::Enum   SET_TYPE           = set_type::NotASet);
        
        void SetProjectile(const weapon::projectile_type::Enum,
                           const material::Enum,
                           const material::Enum   MATERIAL_SECONDARY = material::Nothing,
                           const named_type::Enum NAMED_TYPE         = named_type::NotNamed,
                           const set_type::Enum   SET_TYPE           = set_type::NotASet);

        void SetBladdedStaff(const weapon::bladedstaff_type::Enum,
                             const material::Enum,
                             const material::Enum   MATERIAL_SECONDARY = material::Nothing,
                             const named_type::Enum NAMED_TYPE         = named_type::NotNamed,
                             const set_type::Enum   SET_TYPE           = set_type::NotASet);

        void SetKnife(const sfml_util::Size::Enum,
                      const material::Enum,
                      const material::Enum   MATERIAL_SECONDARY = material::Nothing,
                      const named_type::Enum NAMED_TYPE         = named_type::NotNamed,
                      const set_type::Enum   SET_TYPE           = set_type::NotASet);

        void SetDagger(const sfml_util::Size::Enum,
                       const material::Enum,
                       const material::Enum   MATERIAL_SECONDARY = material::Nothing,
                       const named_type::Enum NAMED_TYPE         = named_type::NotNamed,
                       const set_type::Enum   SET_TYPE           = set_type::NotASet);

        void SetStaff(const material::Enum,
                      const material::Enum   MATERIAL_SECONDARY = material::Nothing,
                      const named_type::Enum NAMED_TYPE         = named_type::NotNamed,
                      const set_type::Enum   SET_TYPE           = set_type::NotASet);

        void SetQuarterStaff(const material::Enum,
                             const material::Enum   MATERIAL_SECONDARY = material::Nothing,
                             const named_type::Enum NAMED_TYPE         = named_type::NotNamed,
                             const set_type::Enum   SET_TYPE           = set_type::NotASet);

        friend bool operator==(const ItemProfile & L, const ItemProfile & R);
        friend bool operator<(const ItemProfile & L, const ItemProfile & R);

    private:
        void SetHelper(const material::Enum,
                       const material::Enum,
                       const named_type::Enum,
                       const set_type::Enum);

    private:
        std::string       baseName_;
        category::Enum    category_;
        armor_type::Enum  armor_;
        weapon_type::Enum weapon_;
        unique_type::Enum unique_;
        misc_type::Enum   misc_;
        set_type::Enum    set_;
        named_type::Enum  named_;

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

        int score_;
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
                        L.score_)
                ==
               std::tie(R.baseName_,
                        R.category_,
                        R.armor_,
                        R.weapon_,
                        R.unique_,
                        R.misc_,
                        R.set_,
                        R.named_,
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
                        R.score_);
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
                        L.score_)
                <
               std::tie(R.baseName_,
                        R.category_,
                        R.armor_,
                        R.weapon_,
                        R.unique_,
                        R.misc_,
                        R.set_,
                        R.named_,
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
                        R.score_);
    }

}
}

#endif //GAME_ITEM_ITEMPROFILE_HPP_INCLUDE
