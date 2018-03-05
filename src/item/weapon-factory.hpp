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
#ifndef HEROESPATH_ITEM_WEAPONFACTORY_HPP_INCLUDED
#define HEROESPATH_ITEM_WEAPONFACTORY_HPP_INCLUDED
//
// weapons-factory.hpp
//  Code responsible for creating weapon objects.
//
#include "sfml-util/size-enum.hpp"

#include "item/item-factory-base.hpp"
#include "item/weapon-types.hpp"

#include <memory>
#include <string>
#include <tuple>

namespace heroespath
{

// forward declarations
namespace creature
{
    class Creature;
    using CreatureCPtrC_t = const Creature * const;
} // namespace creature
namespace item
{
    class Item;
    using ItemPtr_t = Item *;

    namespace weapon
    {

        // A singleton class that creates weapon objects.
        class WeaponFactory : public FactoryBase
        {
            WeaponFactory(const WeaponFactory &) = delete;
            WeaponFactory(const WeaponFactory &&) = delete;
            WeaponFactory & operator=(const WeaponFactory &) = delete;
            WeaponFactory & operator=(const WeaponFactory &&) = delete;

        public:
            WeaponFactory();
            virtual ~WeaponFactory();

            static WeaponFactory * Instance();
            static void Acquire();
            static void Release();

            static ItemPtr_t Make_Fists();

            static ItemPtr_t Make_Claws(creature::CreatureCPtrC_t);

            static ItemPtr_t Make_Tendrils(creature::CreatureCPtrC_t);

            static ItemPtr_t Make_Bite(creature::CreatureCPtrC_t);

            static ItemPtr_t Make_Breath(creature::CreatureCPtrC_t);

            static ItemPtr_t Make_Knife(
                const bool IS_DAGGER = false,
                const sfml_util::Size::Enum SIZE = sfml_util::Size::Medium,
                const material::Enum MATERIAL_PRI = material::Steel,
                const material::Enum MATERIAL_SEC = material::Nothing,
                const bool IS_PIXIE_ITEM = false);

            static ItemPtr_t Make_Sword(
                const sword_type::Enum SWORD_TYPE,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC);

            static ItemPtr_t Make_Axe(
                const axe_type::Enum AXE_TYPE,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC);

            static ItemPtr_t Make_Club(
                const club_type::Enum CLUB_TYPE,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC);

            static ItemPtr_t Make_Whip(
                const whip_type::Enum WHIP_TYPE,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC);

            static ItemPtr_t Make_Projectile(
                const projectile_type::Enum PROJ_TYPE,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC);

            static ItemPtr_t Make_Staff(
                const bool IS_QUARTERSTAFF,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC,
                const bool IS_PIXIE_ITEM = false);

            static ItemPtr_t Make_BladedStaff(
                const bladedstaff_type::Enum STAFF_TYPE,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC);

        private:
            static std::unique_ptr<WeaponFactory> instanceUPtr_;
        };
    } // namespace weapon
} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_WEAPONFACTORY_HPP_INCLUDED
