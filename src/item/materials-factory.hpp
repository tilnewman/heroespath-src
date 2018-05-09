///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event wil the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the folowing restrictions:
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
#ifndef HEROESPATH_ITEM_MATERIAL_COLLECTIONS_HPP_INCLUDED
#define HEROESPATH_ITEM_MATERIAL_COLLECTIONS_HPP_INCLUDED
//
// materials-factory.hpp
//
#include "item/item-profile-thin.hpp"
#include "item/item-type-enum.hpp"

#include <initializer_list>
#include <vector>

namespace heroespath
{
namespace item
{

    // Responsible for making and storing colections of materials that are commonly needed by the
    // item system.
    class MaterialFactory
    {
    public:
        MaterialFactory();

        const MaterialPairVec_t MakeForEquipment(
            const ItemProfileThin & THIN_PROFILE,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet);

        const MaterialPairVec_t
            MakeForMiscType(const misc_type::Enum MISC_TYPE, const bool IS_MAGICAL) const;

        const MaterialPairVec_t MakeForUniqueType(const unique_type::Enum UNIQUE_TyPE) const;

    private:
        const MaterialPairVec_t MakeForWeapon(const ItemProfileThin & THIN_PROFILE) const;
        const MaterialPairVec_t MakeForArmor(const ItemProfileThin & THIN_PROFILE) const;

        // note that this function often returns nothin
        const MaterialPairVec_t MakeForNamedType(const named_type::Enum NAMED_TYPE) const;

        const MaterialPairVec_t MakeForSetType(
            const ItemProfileThin & THIN_PROFILE, const set_type::Enum SET_TYPE) const;

        bool IsCombinationValid(const MaterialPair_t & MATERIAL_PAIR) const;

        const MaterialPairVec_t RemoveLameMaterialsForSpecialItems(const MaterialPairVec_t &) const;

        const MaterialVec_t RemoveCopy(
            const MaterialVec_t & ORIG_MATERIALS, const material::Enum MATERIAL_TO_REMOVE) const;

        const MaterialVec_t Combine(
            const MaterialVec_t & VECTOR1,
            const MaterialVec_t & VECTOR2 = {},
            const MaterialVec_t & VECTOR3 = {},
            const MaterialVec_t & VECTOR4 = {}) const;

        const MaterialVec_t AppendNothingCopy(const MaterialVec_t & ORIG_MATERIALS) const
        {
            return Combine(ORIG_MATERIALS, { material::Nothing });
        }

        const MaterialPairVec_t MakePairs(const MaterialVec_t & PRIMARIES) const;

        const MaterialPairVec_t
            MakePairs(const MaterialVec_t & PRIMARIES, const MaterialVec_t & SECONDARIES) const;

        const MaterialPairVec_t MakePairs(const material::Enum MATERIAL) const;

        const MaterialPairVec_t CleanupMaterialPairVectorAndEnsureNotEmpty(
            const MaterialPairVec_t & MATERIAL_PAIRS,
            const ItemProfileThin & THIN_PROFILE,
            const bool IS_MAGICAL,
            const std::string & GENERATING_FUNCTION_NAME) const
        {
            return CleanupMaterialPairVectorAndEnsureNotEmpty(
                MATERIAL_PAIRS,
                IS_MAGICAL,
                GENERATING_FUNCTION_NAME + " for profile={" + THIN_PROFILE.ToString() + "}");
        }

        const MaterialPairVec_t CleanupMaterialPairVectorAndEnsureNotEmpty(
            const MaterialPairVec_t & MATERIAL_PAIRS,
            const bool IS_MAGICAL,
            const std::string & ERROR_MSG) const;

    private:
        MaterialVec_t metal_;
        MaterialVec_t fancyJewel_;
        MaterialVec_t fancyJewelWithoutDiamond_;
        MaterialVec_t fancyMetal_;
        MaterialVec_t fancyOpaque_;
        MaterialVec_t fancyTribal_;
        MaterialVec_t fancy_;
        MaterialVec_t coreBlade_;
        MaterialVec_t smallWeaponHandle_;
        MaterialVec_t verySmallWeaponHandle_;
        MaterialVec_t fancyReinforced_;
        MaterialVec_t fancyReinforcedArmor_;
        MaterialVec_t corePole_;
        MaterialVec_t fancyClasp_;
        MaterialVec_t mailArmorMetals_;
        MaterialVec_t nonFleshEyesWithNothing_;
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_MATERIAL_COLLECTIONS_HPP_INCLUDED
