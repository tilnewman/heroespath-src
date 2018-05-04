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
// material-colections.hpp
//
#include "item/item-type-enum.hpp"

#include <vector>

namespace heroespath
{
namespace item
{

    // Responsible for making and storing colections of materials that are commonly needed by the
    // item system.
    class MaterialCollections
    {
    public:
        MaterialCollections();

        void Setup();

        const MaterialVec_t CoreMetal(const bool WILL_INCLUDE_NOTHING = false) const
        {
            return ((WILL_INCLUDE_NOTHING) ? coreMetalIncludingNothingEnums_ : coreMetalEnums_);
        }

        const MaterialVec_t CoreJewel(const bool WILL_INCLUDE_NOTHING = false) const
        {
            return ((WILL_INCLUDE_NOTHING) ? coreJewelIncludingNothingEnums_ : coreJewelEnums_);
        }

        const MaterialVec_t CoreMisc(const bool WILL_INCLUDE_NOTHING = false) const
        {
            return ((WILL_INCLUDE_NOTHING) ? coreMiscIncludingNothingEnums_ : coreMiscEnums_);
        }

        const MaterialVec_t CorePrimary() const { return corePrimaryEnums_; }

        const MaterialVec_t CorePrimaryWithoutPearl() const
        {
            return corePrimaryWithoutPearlEnums_;
        }

        const MaterialVec_t CoreSecondary(const bool WILL_INCLUDE_NOTHING = true) const
        {
            return (
                (WILL_INCLUDE_NOTHING) ? coreSecondaryIncludingNothingEnums_ : coreSecondaryEnums_);
        }

        const MaterialVecPair_t CorePrimaryAndSecondary() const
        {
            return corePrimaryAndSecondaryEnumsPair_;
        }

        const MaterialVecPair_t CorePrimaryWithoutSecondary() const
        {
            return corePrimaryWithoutSecondaryEnumsPair_;
        }

        const MaterialVecPair_t CoreMetalAndCoreSecondary() const
        {
            return coreMetalAndCoreSecondaryEnumsPair_;
        }

        const MaterialVecPair_t CoreJewelAndCoreSecondary() const
        {
            return coreJewelAndCoreSecondaryEnumsPair_;
        }

        const MaterialVecPair_t CoreMetalJewelAndCoreSecondary() const
        {
            return coreMetalJewelAndCoreSecondaryEnumsPair_;
        }

        const MaterialVecPair_t CorePrimaryWithoutPearlAndSecondary() const
        {
            return corePrimaryWithoutPearlAndSecondaryEnumsPair_;
        }

        const MaterialVecPair_t WandAndStaffMiscType() const
        {
            return wandAndStaffMiscTypeEnumsPair_;
        }

        const MaterialVecPair_t CoreMetalAndMiscWithCoreSecondary() const
        {
            return coreMetalAndMiscWithCoreSecondaryEnumsPair_;
        }

        const MaterialVecPair_t VectorPairByMiscType(const misc_type::Enum MISC_TYPE) const;
        const MaterialVecPair_t VectorPairByUniqueType(const unique_type::Enum UNIQUE_TyPE) const;
        const MaterialVecPair_t VectorPairByNamedType(const named_type::Enum NAMED_TYPE) const;

        static const MaterialVec_t MakeVectorSortedAndUnique(
            const MaterialVec_t & VECTOR1,
            const MaterialVec_t & VECTOR2 = {},
            const MaterialVec_t & VECTOR3 = {});

        static const MaterialVecPair_t MakeVectorPairSortedAndUnique(
            const MaterialVec_t & VECTOR1, const MaterialVec_t & VECTOR2);

    private:
        const MaterialVec_t MakeCoreMetal(const bool WILL_INCLUDE_NOTHING = false);
        const MaterialVec_t MakeCoreJewel(const bool WILL_INCLUDE_NOTHING = false);
        const MaterialVec_t MakeCoreMisc(const bool WILL_INCLUDE_NOTHING = false);
        const MaterialVec_t MakeCorePrimary();
        const MaterialVec_t MakeCorePrimaryWithoutPearl();
        const MaterialVec_t MakeCoreSecondary(const bool WILL_INCLUDE_NOTHING = true);

        const MaterialVecPair_t MakeCorePrimaryAndSecondary()
        {
            return MakeVectorPairSortedAndUnique(CorePrimary(), CoreSecondary());
        }

        const MaterialVecPair_t MakeCorePrimaryWithoutSecondary()
        {
            return MakeVectorPairSortedAndUnique(CorePrimary(), MaterialVec_t());
        }

        const MaterialVecPair_t MakeCoreMetalAndCoreSecondary()
        {
            return MakeVectorPairSortedAndUnique(CoreMetal(), CoreSecondary());
        }

        const MaterialVecPair_t MakeCoreJewelAndCoreSecondary()
        {
            return MakeVectorPairSortedAndUnique(CoreJewel(), CoreSecondary());
        }

        const MaterialVecPair_t MakeCoreMetalJewelAndCoreSecondary()
        {
            return MakeVectorPairSortedAndUnique(
                MakeVectorSortedAndUnique(CoreMetal(), CoreJewel()), CoreSecondary());
        }

        const MaterialVecPair_t MakeCorePrimaryWithoutPearlAndSecondary()
        {
            return MakeVectorPairSortedAndUnique(CorePrimaryWithoutPearl(), CoreSecondary());
        }

        const MaterialVecPair_t MakeForWandAndStaffMiscTypes()
        {
            return MakeVectorPairSortedAndUnique(
                MakeVectorSortedAndUnique(
                    CorePrimaryWithoutPearl(), { material::Wood, material::Glass }),
                CoreSecondary());
        }

        const MaterialVecPair_t MakeCoreMetalAndMiscWithCoreSecondary()
        {
            return MakeVectorPairSortedAndUnique(
                MakeVectorSortedAndUnique(CoreMetal(), CoreMisc()), CoreSecondary());
        }

        const MaterialVecPair_t MakeNamedDaggerMaterialsPair()
        {
            return MakeVectorPairSortedAndUnique(
                MakeVectorSortedAndUnique(
                    CoreMetal(), { material::Bone, material::Stone, material::Obsidian }),
                {});
        }

    private:
        MaterialVec_t coreMetalEnums_;
        MaterialVec_t coreMetalIncludingNothingEnums_;
        MaterialVec_t coreJewelEnums_;
        MaterialVec_t coreJewelIncludingNothingEnums_;
        MaterialVec_t coreMiscEnums_;
        MaterialVec_t coreMiscIncludingNothingEnums_;
        MaterialVec_t corePrimaryEnums_;
        MaterialVec_t corePrimaryWithoutPearlEnums_;
        MaterialVec_t coreSecondaryEnums_;
        MaterialVec_t coreSecondaryIncludingNothingEnums_;

        MaterialVecPair_t corePrimaryAndSecondaryEnumsPair_;
        MaterialVecPair_t corePrimaryWithoutSecondaryEnumsPair_;
        MaterialVecPair_t coreMetalAndCoreSecondaryEnumsPair_;
        MaterialVecPair_t coreJewelAndCoreSecondaryEnumsPair_;
        MaterialVecPair_t coreMetalJewelAndCoreSecondaryEnumsPair_;
        MaterialVecPair_t corePrimaryWithoutPearlAndSecondaryEnumsPair_;
        MaterialVecPair_t wandAndStaffMiscTypeEnumsPair_;
        MaterialVecPair_t coreMetalAndMiscWithCoreSecondaryEnumsPair_;
        MaterialVecPair_t namedDaggerEnumsPair_;
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_MATERIAL_COLLECTIONS_HPP_INCLUDED
