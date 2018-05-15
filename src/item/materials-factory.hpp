// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_MATERIAL_COLLECTIONS_HPP_INCLUDED
#define HEROESPATH_ITEM_MATERIAL_COLLECTIONS_HPP_INCLUDED
//
// materials-factory.hpp
//
#include "item/item-profile-thin.hpp"
#include "item/item-type-enum.hpp"
#include "misc/vector-map.hpp"

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
            const named_type::Enum NAMED_TYPE = named_type::Not,
            const set_type::Enum SET_TYPE = set_type::Not);

        const MaterialPairVec_t
            MakeForMiscType(const misc_type::Enum MISC_TYPE, const bool IS_MAGICAL) const;

    private:
        const MaterialPairVec_t MakeForMiscInner(const misc_type::Enum MISC_TYPE) const;
        const MaterialPairVec_t MakeForWeapon(const ItemProfileThin & THIN_PROFILE) const;
        const MaterialPairVec_t MakeForArmor(const ItemProfileThin & THIN_PROFILE) const;

        // note that this function often returns nothin
        void LimitForNamedType(
            const ItemProfileThin & THIN_PROFILE,
            const named_type::Enum NAMED_TYPE,
            MaterialPairVec_t &) const;

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
            const MaterialVec_t & VECTOR4 = {},
            const MaterialVec_t & VECTOR5 = {}) const;

        const MaterialVec_t AppendCopy(
            const MaterialVec_t & ORIG_MATERIALS, const material::Enum MATERIAL_TO_ADD) const
        {
            return Combine(ORIG_MATERIALS, { MATERIAL_TO_ADD });
        }

        const MaterialVec_t AppendNothingCopy(const MaterialVec_t & ORIG_MATERIALS) const
        {
            return AppendCopy(ORIG_MATERIALS, material::Nothing);
        }

        const MaterialPairVec_t MakePairs(const MaterialVec_t & PRIMARIES) const;

        const MaterialPairVec_t
            MakePairs(const MaterialVec_t & PRIMARIES, const MaterialVec_t & SECONDARIES) const;

        const MaterialPairVec_t MakePairs(const material::Enum MATERIAL) const;

        void CleanupMaterialPairVectorAndEnsureNotEmpty(
            MaterialPairVec_t & materialPairs, const bool WILL_REMOVE_LAME_MATERIALS) const;

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
        misc::VectorMap<material::Enum, MaterialVec_t> invalidMetalsMap_;
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_MATERIAL_COLLECTIONS_HPP_INCLUDED
