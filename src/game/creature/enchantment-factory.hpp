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
#ifndef GAME_CREATURE_ENCHANTMENTFACTORY_HPP_INCLUDED
#define GAME_CREATURE_ENCHANTMENTFACTORY_HPP_INCLUDED
//
// enchantment-factory.hpp
//  Responsibel for making (new'ing) all Enchantment objects.
//
#include "game/creature/enchantment-type.hpp"
#include "game/creature/enchantment.hpp"
#include "game/creature/condition-enum.hpp"
#include "game/stats/types.hpp"
#include "game/stats/traits-set.hpp"
#include "game/stats/types.hpp"
#include "game/item/item-type-enum.hpp"

#include <string>
#include <memory>


namespace game
{
namespace item
{
    class Item;
    using ItemPtr_t = Item *;
}
namespace creature
{

    //Responsible for making all Enchantment objects, and automatically
    //storing them in the EnchantmentWarehouse for safe keeping.
    class EnchantmentFactory
    {
        //prevent copy construction
        EnchantmentFactory(const EnchantmentFactory &) =delete;

        //prevent copy assignment
        EnchantmentFactory & operator=(const EnchantmentFactory &) =delete;

        //prevent non-singleton construction
        EnchantmentFactory();

    public:
        ~EnchantmentFactory();

        static EnchantmentFactory * Instance();
        static void Acquire();
        static void Release();

        //use to make/store/attach/return enchantments base on item type
        item::ItemPtr_t MakeStoreAttachReturn(item::ItemPtr_t) const;

        //use to make/store/attach/return custom enchantments
        item::ItemPtr_t MakeStoreAttachReturn(
            item::ItemPtr_t             itemPtr,
            const EnchantmentType::Enum TYPE,
            const stats::TraitSet &     TRAIT_SET,
            const UseInfo &             USE_INFO    = UseInfo(),
            const SummonInfo &          SUMMON_INFO = SummonInfo()) const;

    private:
        item::ItemPtr_t MakeStoreAttachReturn(item::ItemPtr_t, Enchantment * const) const;

        Enchantment * MakeFromUniqueType(const item::unique_type::Enum,
                                         const item::material::Enum) const;

        Enchantment * MakeFromMiscType(const item::misc_type::Enum) const;
        Enchantment * MakeFromSetType(const item::set_type::Enum) const;
        Enchantment * MakeFromSetCompleteType(const item::set_type::Enum) const;

        Enchantment * MakeFromNamedType(const item::named_type::Enum,
                                        const item::material::Enum,
                                        const bool IS_WEAPON,
                                        const bool IS_ARMOR) const;

    private:
        static std::unique_ptr<EnchantmentFactory> instanceUPtr_;
    };

}
}

#endif //GAME_CREATURE_ENCHANTMENTFACTORY_HPP_INCLUDED
