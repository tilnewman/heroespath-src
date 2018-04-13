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
#include "misc/not-null-warehouse.hpp"
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

    // Responsible for making and storing all Enchantment objects.
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

        // use to make/store/attach/return enchantments base on item type
        const EnchantmentPVec_t MakeAndStore(
            const item::TypeWrapper &,
            const bool IS_WEAPON,
            const bool IS_ARMOR,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY) const;

        Score_t TreasureScore(const item::unique_type::Enum, const item::material::Enum) const;

        Score_t TreasureScore(
            const item::misc_type::Enum,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY) const;

        Score_t TreasureScore(const item::set_type::Enum) const;

        Score_t TreasureScore(
            const item::named_type::Enum,
            const item::material::Enum,
            const bool IS_WEAPON,
            const bool IS_ARMOR) const;

        Score_t TreasureScore(
            const item::element_type::Enum, const bool IS_WEAPON, const item::material::Enum) const;

        misc::NotNullWarehouse<Enchantment> & Warehouse() { return warehouse_; }

    private:
        const EnchantmentPtr_t Make(
            const EnchantmentType::Enum TYPE,
            const stats::TraitSet & TRAIT_SET,
            const UseInfo & USE_INFO) const;

        const EnchantmentPtr_t Make(const Enchantment &) const;

        const std::vector<Enchantment>
            MakeFromUniqueType(const item::unique_type::Enum, const item::material::Enum) const;

        const EnchantmentPVec_t
            NewFromUniqueType(const item::unique_type::Enum, const item::material::Enum) const;

        const Enchantment MakeFromMiscType(
            const item::misc_type::Enum,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY) const;

        const EnchantmentPtrOpt_t NewFromMiscType(
            const item::misc_type::Enum,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY) const;

        const Enchantment MakeFromSetType(const item::set_type::Enum) const;
        const EnchantmentPtr_t NewFromSetType(const item::set_type::Enum) const;

        const Enchantment MakeFromSetCompleteType(const item::set_type::Enum) const;
        const EnchantmentPtr_t NewFromSetCompleteType(const item::set_type::Enum) const;

        const Enchantment MakeFromElementType(
            const item::element_type::Enum, const bool IS_WEAPON, const item::material::Enum) const;

        const EnchantmentPtr_t NewFromElementType(
            const item::element_type::Enum, const bool IS_WEAPON, const item::material::Enum) const;

        const Enchantment MakeFromNamedType(
            const item::named_type::Enum,
            const item::material::Enum,
            const bool IS_WEAPON,
            const bool IS_ARMOR) const;

        const EnchantmentPtr_t NewFromNamedType(
            const item::named_type::Enum,
            const item::material::Enum,
            const bool IS_WEAPON,
            const bool IS_ARMOR) const;

    private:
        static std::unique_ptr<EnchantmentFactory> instanceUPtr_;
        mutable misc::NotNullWarehouse<Enchantment> warehouse_;
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_ENCHANTMENTFACTORY_HPP_INCLUDED
