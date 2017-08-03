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

    //forward declarations
    class Enchantment;
    using EnchantmentPtr_t = Enchantment *;


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

    private:
        void MakeStoreAttach(
            item::ItemPtr_t             itemPtr,
            const EnchantmentType::Enum TYPE,
            const stats::TraitSet &     TRAIT_SET = stats::TraitSet(),
            const int                   USE_COUNT = 0, //negative means infinite
            const CondEnumVec_t &       CONDS_VEC = CondEnumVec_t()) const;

    private:
        static std::unique_ptr<EnchantmentFactory> instanceUPtr_;
    };

}
}

#endif //GAME_CREATURE_ENCHANTMENTFACTORY_HPP_INCLUDED
