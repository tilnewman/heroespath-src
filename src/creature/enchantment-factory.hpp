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
#include "item/item-creation-packet.hpp"
#include "item/item-type-enum.hpp"
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
    // Item types such as set_type, named_type, unique_type, and elemental_type are all distinct
    // item magical enchantments.  The only valid combination is that named_types often are
    // element_types, and yes there are a very few unique_types that are also armor (cape's) that
    // also have element types.  Only weapons and armor can be named_types and elemental_types.
    // Unique_types cannot be weapons or armor, they must be misc_types.  If an item is unique_type
    // then it will have Enchantments specifically for that unique_type and no Enchantments from the
    // misc_type will be applied.
    struct EnchantmentFactory
    {
        EnchantmentFactory() = delete;

        // If part of a set, then only the set enchantments will apply.
        // If unique, then only the unique_type enchantments will be applied.
        // All unique_types are also misc_types but their enchantments are mutually exclusive.
        // Enchantments from element_types, named_types, and misc_types can combine.
        static const EnchantmentPVec_t MakeAndStore(const item::Item &);

        static Score_t TreasureScore(
            const item::misc_type::Enum,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY);

        static Score_t TreasureScore(const item::set_type::Enum);

        static Score_t TreasureScore(
            const item::named_type::Enum,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY,
            const bool IS_WEAPON,
            const bool IS_ARMOR);

        static Score_t TreasureScore(
            const item::element_type::Enum,
            const bool IS_WEAPON,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY);

    private:
        static const EnchantmentPtr_t Make(
            const EnchantmentType::Enum TYPE, const TraitSet & TRAIT_SET, const UseInfo & USE_INFO);

        static const EnchantmentPtr_t Make(const Enchantment &);

        static const EnchantmentVec_t MakeFromMiscType(
            const item::misc_type::Enum,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY);

        static const EnchantmentPVec_t NewFromMiscType(
            const item::misc_type::Enum,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY);

        static const Enchantment MakeFromSetType(const item::set_type::Enum);
        static const EnchantmentPtr_t NewFromSetType(const item::set_type::Enum);

        static const Enchantment MakeFromSetCompleteType(const item::set_type::Enum);
        static const EnchantmentPtr_t NewFromSetCompleteType(const item::set_type::Enum);

        static const Enchantment MakeFromElementType(
            const item::element_type::Enum,
            const bool IS_WEAPON,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY);

        static const EnchantmentPtr_t NewFromElementType(
            const item::element_type::Enum,
            const bool IS_WEAPON,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY);

        static const Enchantment MakeFromNamedType(
            const item::named_type::Enum,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY,
            const bool IS_WEAPON,
            const bool IS_ARMOR);

        static const EnchantmentPtr_t NewFromNamedType(
            const item::named_type::Enum,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY,
            const bool IS_WEAPON,
            const bool IS_ARMOR);
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_ENCHANTMENTFACTORY_HPP_INCLUDED
