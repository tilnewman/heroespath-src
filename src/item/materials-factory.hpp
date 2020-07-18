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
#include "creature/race-enum.hpp"
#include "item/armor-enum.hpp"
#include "item/material-enum.hpp"
#include "item/misc-enum.hpp"
#include "item/named-enum.hpp"
#include "item/set-enum.hpp"
#include "item/weapon-enum.hpp"
#include "misc/not-null.hpp"
#include "misc/vector-map.hpp"

#include <vector>

namespace heroespath
{
namespace item
{

    class ItemTemplate;
    class WeaponTemplate;

    // Responsible for making and storing collections of materials that are commonly needed by the
    // item system.
    class MaterialFactory
    {
    public:
        MaterialFactory();
        ~MaterialFactory();

        MaterialFactory(const MaterialFactory &) = delete;
        MaterialFactory(MaterialFactory &&) = delete;
        MaterialFactory & operator=(const MaterialFactory &) = delete;
        MaterialFactory & operator=(MaterialFactory &&) = delete;

        static misc::NotNull<MaterialFactory *> Instance();
        static void Create();
        static void Destroy();

        void Initialize();

        const MaterialPairVec_t MakePairs(
            const ItemTemplate & TEMPLATE,
            const Misc::Enum MISC,
            const Named::Enum NAMED,
            const Set::Enum SET,
            const bool IS_MAGICAL_IN_OTHER_WAYS);

        const MaterialPair_t
            MakePairForBodyPartWeapon(const BodyPartWeapons::Enum BODY_PART_WEAPON_TYPE) const;

        const MaterialPair_t MakePairForSkinArmor(const creature::race::Enum RACE) const;

    private:
        const MaterialPairVec_t MakeForEquipment(
            const ItemTemplate & TEMPLATE,
            const Named::Enum NAMED = Named::Count,
            const Set::Enum SET = Set::Count);

        const MaterialPairVec_t MakeForMiscType(const Misc::Enum MISC, const bool IS_MAGICAL) const;
        const MaterialPairVec_t MakeForMiscInner(const Misc::Enum MISC) const;
        const MaterialPairVec_t MakeForWeapon(const ItemTemplate & TEMPLATE) const;
        const MaterialPairVec_t MakeForWeaponBase(const WeaponTemplate & TEMPLATE) const;
        const MaterialPairVec_t MakeForWeaponSolid(const WeaponTemplate & TEMPLATE) const;
        const MaterialPairVec_t MakeForArmor(const ItemTemplate & TEMPLATE) const;

        // note that this function often returns nothing
        void LimitForNamedType(
            const ItemTemplate & TEMPLATE, const Named::Enum NAMED, MaterialPairVec_t &) const;

        const MaterialPairVec_t
            MakeForSetType(const ItemTemplate & TEMPLATE, const Set::Enum SET) const;

        bool IsCombinationValid(const MaterialPair_t & MATERIAL_PAIR) const;

        const MaterialPairVec_t
            RemoveLameMaterialsFromSpecialItems(const MaterialPairVec_t &) const;

        const MaterialVec_t RemoveCopy(
            const MaterialVec_t & ORIG_MATERIALS, const Material::Enum MATERIAL_TO_REMOVE) const;

        const MaterialVec_t Combine(
            const MaterialVec_t & VECTOR1,
            const MaterialVec_t & VECTOR2 = {},
            const MaterialVec_t & VECTOR3 = {},
            const MaterialVec_t & VECTOR4 = {},
            const MaterialVec_t & VECTOR5 = {}) const;

        const MaterialVec_t AppendCopy(
            const MaterialVec_t & ORIG_MATERIALS, const Material::Enum MATERIAL_TO_ADD) const
        {
            return Combine(ORIG_MATERIALS, { MATERIAL_TO_ADD });
        }

        const MaterialVec_t AppendNothingCopy(const MaterialVec_t & ORIG_MATERIALS) const
        {
            return AppendCopy(ORIG_MATERIALS, Material::Count);
        }

        const MaterialPairVec_t MakePairs(const MaterialVec_t & PRIMARIES) const;

        const MaterialPairVec_t
            MakePairs(const MaterialVec_t & PRIMARIES, const MaterialVec_t & SECONDARIES) const;

        const MaterialPairVec_t MakePairs(const Material::Enum MATERIAL) const;

        void CleanupMaterialPairs(
            MaterialPairVec_t & materialPairs,
            const bool WILL_REMOVE_LAME_MATERIALS,
            const bool ALLOW_INVALID) const;

    private:
        static std::unique_ptr<MaterialFactory> instanceUPtr_;

        static const MaterialVec_t metal_;
        static const MaterialVec_t fancyJewel_;
        static const MaterialVec_t fancyMetal_;
        static const MaterialVec_t fancyOpaque_;
        static const MaterialVec_t fancyTribal_;
        static const MaterialVec_t coreBlade_;
        static const MaterialVec_t smallWeaponHandle_;
        static const MaterialVec_t corePole_;
        static const MaterialVec_t mailArmorMetals_;

        MaterialVec_t fancyJewelWithoutDiamond_;
        MaterialVec_t fancy_;
        MaterialVec_t verySmallWeaponHandle_;
        MaterialVec_t fancyReinforced_;
        MaterialVec_t fancyReinforcedArmor_;
        MaterialVec_t fancyClasp_;
        MaterialVec_t nonFleshEyesWithNothing_;
        misc::VectorMap<Material::Enum, MaterialVec_t> invalidMetalsMap_;
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_MATERIAL_COLLECTIONS_HPP_INCLUDED
