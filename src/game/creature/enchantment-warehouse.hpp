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
#ifndef GAME_CREATURE_ENCHANTMENTWAREHOUSE_HPP_INCLUDED
#define GAME_CREATURE_ENCHANTMENTWAREHOUSE_HPP_INCLUDED
//
// enchantment-warehouse.hpp
//  Responsible for managing the lifetime of all enchantment objects.
//
#include "game/warehouse.hpp"

#include <memory>
#include <vector>


namespace game
{
namespace creature
{

    //forward declarations
    class Enchantment;
    using EnchantmentPtr_t = Enchantment *;


    //Singleton responsible for the lifetimes of Enchantment objects.
    //This class does not new the objects, but it does delete them.
    class EnchantmentWarehouse
    {
        EnchantmentWarehouse(const EnchantmentWarehouse &) =delete;
        EnchantmentWarehouse & operator=(const EnchantmentWarehouse &) =delete;

        //singleton construction only
        EnchantmentWarehouse();

    public:
        ~EnchantmentWarehouse();

        static EnchantmentWarehouse * Instance();
        static void Acquire();
        static void Release();

        EnchantmentPtr_t Store(const EnchantmentPtr_t);
        void Free(EnchantmentPtr_t &);

    private:
        static std::unique_ptr<EnchantmentWarehouse> instanceUPtr_;
        Warehouse<Enchantment> warehouse_;
    };

}
}

#endif //GAME_CREATURE_ENCHANTMENTWAREHOUSE_HPP_INCLUDED
