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
#ifndef HEROESPATH_ITEM_ITEMPROFILEWAREHOUSE_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEMPROFILEWAREHOUSE_HPP_INCLUDED
//
// item-profile-warehouse.hpp
//
#include "item/item-profile-thin.hpp"
#include "item/item-profile.hpp"
#include "item/material-collections.hpp"
#include "misc/not-null.hpp"
#include "misc/types.hpp"
#include "stats/traits-set.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace heroespath
{
namespace item
{

    using BaseMaterialVecPair_t = std::pair<armor::base_type::Enum, MaterialVecPair_t>;
    using BaseMaterialVecPairVec_t = std::vector<BaseMaterialVecPair_t>;

    // Responsible for creating and storing an ItemProfile for each item in the game that can be
    // found after comabt in the random assortment of things left behind by a group of enemy
    // creatures.
    //
    // Also responsible for calculating non-magical based scores for ItemProfiles.
    class ItemProfileWarehouse
    {
    public:
        ItemProfileWarehouse(const ItemProfileWarehouse &) = delete;
        ItemProfileWarehouse(ItemProfileWarehouse &&) = delete;
        ItemProfileWarehouse & operator=(const ItemProfileWarehouse &) = delete;
        ItemProfileWarehouse & operator=(ItemProfileWarehouse &&) = delete;

    public:
        ItemProfileWarehouse();
        ~ItemProfileWarehouse();

        static misc::NotNull<ItemProfileWarehouse *> Instance();
        static void Acquire();
        static void Release();

        void Setup();

        void Setup_StandardEquipment();
        void Setup_UniqueItems();
        void Setup_MiscItems();
        void Setup_NamedEquipment();
        void Setup_SetEquipment();
        void Setup_SummoningItems();

        bool IsSetup() const { return (profiles_.empty() == false); }

        const ItemProfileVec_t & GetNormalProfiles();
        const ItemProfileVec_t & GetReligiousProfiles();

    private:
        const ElementEnumVec_t ElementTypesIncludingNone(
            const ItemProfileThin & THIN_PROFILE,
            const named_type::Enum NAMED_TYPE,
            const set_type::Enum SET_TYPE,
            const armor::base_type::Enum MISSING_BASE_TYPE = armor::base_type::Count);

        void MakeLoopOverMaterialsForThinProfile(
            const ItemProfileThin & THIN_PROFILE,
            const named_type::Enum = named_type::NotNamed,
            const set_type::Enum = set_type::NotASet);

        void MakeLoopOverElementTypesForThinProfile(
            const ItemProfileThin & THIN_PROFILE,
            const named_type::Enum NAMED_TYPE,
            const set_type::Enum SET_TYPE,
            const material::Enum MATERIAL_PRIMARY,
            const material::Enum MATERIAL_SECONDARY,
            const armor::base_type::Enum MISSING_BASE_TYPE);

        void MakeFromThinProfile(
            const ItemProfileThin & THIN_PROFILE,
            const named_type::Enum NAMED_TYPE,
            const set_type::Enum SET_TYPE,
            const element_type::Enum ELEMENT_TYPE,
            const material::Enum MATERIAL_PRIMARY,
            const material::Enum MATERIAL_SECONDARY,
            const armor::base_type::Enum MISSING_BASE_TYPE);

        const BaseMaterialVecPairVec_t GetMaterialsFromThinProfile(
            const ItemProfileThin & THIN_PROFILE,
            const named_type::Enum NAMED_TYPE,
            const set_type::Enum SET_TYPE);

        void MakeLoopOverMaterialsForMiscType(
            const misc_type::Enum,
            const element_type::Enum ELEMENT_TYPE,
            const creature::SummonInfo & SUMMON_INFO = creature::SummonInfo());

        static const MaterialVecPair_t Materials(const weapon::axe_type::Enum);
        static const MaterialVecPair_t Materials(const weapon::bladedstaff_type::Enum);
        static const MaterialVecPair_t Materials(const weapon::club_type::Enum);
        static const MaterialVecPair_t Materials(const weapon::projectile_type::Enum);
        static const MaterialVecPair_t Materials(const weapon::sword_type::Enum);
        static const MaterialVecPair_t Materials(const weapon::whip_type::Enum);

        static const MaterialVecPair_t MaterialsKnife();
        static const MaterialVecPair_t MaterialsDagger();
        static const MaterialVecPair_t MaterialsStaff();
        static const MaterialVecPair_t MaterialsQuarterStaff();

        static const MaterialVecPair_t Materials(const armor::cover_type::Enum);
        static const MaterialVecPair_t Materials(const armor::helm_type::Enum);
        static const MaterialVecPair_t Materials(const armor::shield_type::Enum);

        static const MaterialVecPair_t MaterialsAventail(armor::base_type::Enum);
        static const MaterialVecPair_t MaterialsBracer(armor::base_type::Enum);
        static const MaterialVecPair_t MaterialsShirt(armor::base_type::Enum);
        static const MaterialVecPair_t MaterialsBoots(armor::base_type::Enum);
        static const MaterialVecPair_t MaterialsPants(armor::base_type::Enum);
        static const MaterialVecPair_t MaterialsGauntlets(armor::base_type::Enum);

        static const MaterialVec_t MaterialsPrimaryFromArmorBaseType(const armor::base_type::Enum);

        static const MaterialVecPair_t MaterialsFromBaseType(const armor::base_type::Enum);
        static const MaterialVecPair_t MaterialsFromBaseTypeNoCloth(const armor::base_type::Enum);

        static const ItemProfileThinVec_t ThinProfilesWeaponsSwords();
        static const ItemProfileThinVec_t ThinProfilesWeaponsProjectiles();
        static const ItemProfileThinVec_t ThinProfilesWeaponsAll();

        static const ItemProfileThinVec_t ThinProfilesArmor(
            const bool WILL_INCLUDE_COVERINGS,
            const armor::base_type::Enum BASE_TYPE_RESTRICTION = armor::base_type::Count);

        static const ItemProfileThinVec_t ThinProfiles(const named_type::Enum);
        static const ItemProfileThinVec_t ThinProfiles(const set_type::Enum);

        void AppendToEitherNormalOrReligiousVector(const ItemProfile & PROFILE)
        {
            if (PROFILE.IsReligious())
            {
                religiousProfiles_.emplace_back(PROFILE);
            }
            else
            {
                profiles_.emplace_back(PROFILE);
            }
        }

    private:
        static std::unique_ptr<ItemProfileWarehouse> instanceUPtr_;

        static MaterialCollections matColl_;

        // these are the fat profiles that are stored here for use by the Treasure Stage when
        // deciding which items to give the players after defeating creatures in combet.
        ItemProfileVec_t profiles_;
        ItemProfileVec_t religiousProfiles_;
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEMPROFILEWAREHOUSE_HPP_INCLUDED
