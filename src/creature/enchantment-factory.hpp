// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_ENCHANTMENTFACTORY_HPP_INCLUDED
#define HEROESPATH_CREATURE_ENCHANTMENTFACTORY_HPP_INCLUDED
//
// enchantment-factory.hpp
//
#include "creature/condition-enum.hpp"
#include "creature/enchantment-type.hpp"
#include "creature/enchantment.hpp"
#include "creature/trait.hpp"
#include "creature/traits-set.hpp"
#include "item/category-enum.hpp"
#include "item/element-enum.hpp"
#include "item/material-enum.hpp"
#include "item/misc-enum.hpp"
#include "item/named-enum.hpp"
#include "item/set-enum.hpp"
#include "item/weapon-enum.hpp"
#include "misc/not-null.hpp"

#include <string>

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
    // combinations of Enchantments.  Only the ItemProfieFactory queries the treasure scores, and
    // only the Item constructor calls MakeAndStore().
    //
    // Item types such as Set, Named, unique_type, and elemental_type are all distinct
    // item magical enchantments.  The only valid combination is that Nameds often are
    // Elements, and yes there are a very few unique_types that are also armor (cape's) that
    // also have element types.  Only weapons and armor can be Nameds and elemental_types.
    // Unique_types cannot be weapons or armor, they must be Miscs.  If an item is unique_type
    // then it will have Enchantments specifically for that unique_type and no Enchantments from the
    // Misc will be applied.
    struct EnchantmentFactory
    {
        EnchantmentFactory() = delete;

        // If part of a set, then only the set enchantments will apply.
        // If unique, then only the unique_type enchantments will be applied.
        // All unique_types are also Miscs but their enchantments are mutually exclusive.
        // Enchantments from Elements, Nameds, and Miscs can combine.
        static const EnchantmentPVec_t MakeAndStore(const item::Item &);

        // TODO this one appears to be broken and not doing anything
        // static Score_t Score(
        //    const item::Misc::Enum,
        //    const item::Material::Enum MAT_PRI,
        //    const item::Material::Enum MAT_SEC);

        static Score_t Score(const item::Set::Enum);

        static Score_t Score(
            const item::Named::Enum,
            const item::Material::Enum MAT_PRI,
            const item::Material::Enum MAT_SEC,
            const bool IS_WEAPON,
            const bool IS_ARMOR);

        static Score_t Score(
            const item::Element::Enum,
            const bool IS_WEAPON,
            const item::Material::Enum MAT_PRI,
            const item::Material::Enum MAT_SEC);

    private:
        static const EnchantmentPtr_t Make(
            const EnchantmentType::Enum TYPE, const TraitSet & TRAIT_SET, const UseInfo & USE_INFO);

        static const EnchantmentPtr_t Make(const Enchantment &);

        static const EnchantmentVec_t MakeFromMiscType(
            const item::Misc::Enum,
            const item::Material::Enum MAT_PRI,
            const item::Material::Enum MAT_SEC);

        static const EnchantmentPVec_t NewFromMiscType(
            const item::Misc::Enum,
            const item::Material::Enum MAT_PRI,
            const item::Material::Enum MAT_SEC);

        static const Enchantment MakeFromSetType(const item::Set::Enum);
        static const EnchantmentPtr_t NewFromSetType(const item::Set::Enum);

        static const Enchantment MakeFromSetCompleteType(const item::Set::Enum);
        static const EnchantmentPtr_t NewFromSetCompleteType(const item::Set::Enum);

        static const Enchantment MakeFromElementType(
            const item::Element::Enum,
            const bool IS_WEAPON,
            const item::Material::Enum MAT_PRI,
            const item::Material::Enum MAT_SEC);

        static const EnchantmentPtr_t NewFromElementType(
            const item::Element::Enum,
            const bool IS_WEAPON,
            const item::Material::Enum MAT_PRI,
            const item::Material::Enum MAT_SEC);

        static const Enchantment MakeFromNamedType(
            const item::Named::Enum,
            const item::Material::Enum MAT_PRI,
            const item::Material::Enum MAT_SEC,
            const bool IS_WEAPON,
            const bool IS_ARMOR);

        static const EnchantmentPtr_t NewFromNamedType(
            const item::Named::Enum,
            const item::Material::Enum MAT_PRI,
            const item::Material::Enum MAT_SEC,
            const bool IS_WEAPON,
            const bool IS_ARMOR);
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_ENCHANTMENTFACTORY_HPP_INCLUDED
