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
#ifndef HEROESPATH_CREATURE_ENCHANTMENTFACTORY_HPP_INCLUDED
#define HEROESPATH_CREATURE_ENCHANTMENTFACTORY_HPP_INCLUDED
//
// enchantment-factory.hpp
//  Responsibel for making (new'ing) all Enchantment objects.
//
#include "creature/condition-enum.hpp"
#include "creature/enchantment-type.hpp"
#include "creature/enchantment.hpp"
#include "item/item-type-enum.hpp"
#include "item/item-type-wrapper.hpp"
#include "misc/not-null.hpp"
#include "stats/trait.hpp"
#include "stats/traits-set.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
} // namespace item
namespace creature
{

    // Responsible for making all Enchantment objects and for calculating the treasure scores for
    // combinations of Enchantments.  Only the ItemProfieFactory querries the treasure scores, and
    // only the Item constructor calls MakeAndStore().
    //
    // Item types such as set_type, named_type, unique_type, and elemental_type are all distinct
    // item magical enchantments.  The only valid combination is that named_types often are
    // element_types, and yes there are a very few unique_types that are also armor (cape's) that
    // also have element types.  Only weapons and armor can be named_types and elemental_types.
    // Unique_types cannot be weapons or armor, they must be misc_types.  If an item is unique_type
    // then it will have Enchantments specifically for that unique_type and no Enchantments from the
    // misc_type will be applied.
    class EnchantmentFactory
    {
    public:
        EnchantmentFactory(const EnchantmentFactory &) = delete;
        EnchantmentFactory(EnchantmentFactory &&) = delete;
        EnchantmentFactory & operator=(const EnchantmentFactory &) = delete;
        EnchantmentFactory & operator=(EnchantmentFactory &&) = delete;

    public:
        EnchantmentFactory();
        ~EnchantmentFactory();

        static misc::NotNull<EnchantmentFactory *> Instance();
        static void Acquire();
        static void Release();

        // If part of a set, then only the set enchantments will apply.
        // If unique, then only the unique_type enchantments will be applied.
        // All unique_types are also misc_types but their enchantments are mutually exclusive.
        // Enchantments from element_types, named_types, and misc_types can combine.
        const EnchantmentPVec_t MakeAndStore(
            const item::TypeWrapper &,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY,
            const bool IS_WEAPON,
            const bool IS_ARMOR) const;

        Score_t TreasureScore(
            const item::misc_type::Enum,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY) const;

        Score_t TreasureScore(const item::set_type::Enum) const;

        Score_t TreasureScore(
            const item::named_type::Enum,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY,
            const bool IS_WEAPON,
            const bool IS_ARMOR) const;

        Score_t TreasureScore(
            const item::element_type::Enum,
            const bool IS_WEAPON,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY) const;

    private:
        const EnchantmentPtr_t Make(
            const EnchantmentType::Enum TYPE,
            const stats::TraitSet & TRAIT_SET,
            const UseInfo & USE_INFO) const;

        const EnchantmentPtr_t Make(const Enchantment &) const;

        const EnchantmentVec_t MakeFromMiscType(
            const item::misc_type::Enum,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY) const;

        const EnchantmentPVec_t NewFromMiscType(
            const item::misc_type::Enum,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY) const;

        const Enchantment MakeFromSetType(const item::set_type::Enum) const;
        const EnchantmentPtr_t NewFromSetType(const item::set_type::Enum) const;

        const Enchantment MakeFromSetCompleteType(const item::set_type::Enum) const;
        const EnchantmentPtr_t NewFromSetCompleteType(const item::set_type::Enum) const;

        const Enchantment MakeFromElementType(
            const item::element_type::Enum,
            const bool IS_WEAPON,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY) const;

        const EnchantmentPtr_t NewFromElementType(
            const item::element_type::Enum,
            const bool IS_WEAPON,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY) const;

        const Enchantment MakeFromNamedType(
            const item::named_type::Enum,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY,
            const bool IS_WEAPON,
            const bool IS_ARMOR) const;

        const EnchantmentPtr_t NewFromNamedType(
            const item::named_type::Enum,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY,
            const bool IS_WEAPON,
            const bool IS_ARMOR) const;

    private:
        static std::unique_ptr<EnchantmentFactory> instanceUPtr_;
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_ENCHANTMENTFACTORY_HPP_INCLUDED
