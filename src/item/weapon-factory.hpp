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
//
#include "item/item-factory-base.hpp"
#include "item/item-type-wrapper.hpp"
#include "item/weapon-details.hpp"
#include "item/weapon-types.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/size-enum.hpp"

#include <memory>
#include <string>
#include <tuple>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
} // namespace creature
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;

    class ItemProfile;

    namespace weapon
    {

        // Responsible for creating weapon items.
        class WeaponFactory : public FactoryBase
        {
        public:
            WeaponFactory(const WeaponFactory &) = delete;
            WeaponFactory(WeaponFactory &&) = delete;
            WeaponFactory & operator=(const WeaponFactory &) = delete;
            WeaponFactory & operator=(WeaponFactory &&) = delete;
            WeaponFactory() = delete;

            static const ItemPtr_t Make(const ItemProfile &);
            static const ItemPtr_t Make(const body_part::Enum, const creature::CreaturePtr_t);

        private:
            static const ItemPtr_t Make_Fists();
            static const ItemPtr_t Make_Claws(const creature::CreaturePtr_t);
            static const ItemPtr_t Make_Tendrils(const creature::CreaturePtr_t);
            static const ItemPtr_t Make_Bite(const creature::CreaturePtr_t);
            static const ItemPtr_t Make_Breath(const creature::CreaturePtr_t);

            static const ItemPtr_t Make_Knife(
                const category::Enum CATEGORY,
                const weapon_type::Enum WEAPON_TYPE,
                const TypeWrapper & TYPE_WRAPPER,
                const bool IS_DAGGER,
                const sfml_util::Size::Enum SIZE,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC,
                const bool IS_PIXIE_ITEM);

            static const ItemPtr_t Make_Sword(
                const category::Enum CATEGORY,
                const weapon_type::Enum WEAPON_TYPE,
                const sword_type::Enum SWORD_TYPE,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC,
                const TypeWrapper & TYPE_WRAPPER);

            static const ItemPtr_t Make_Axe(
                const category::Enum CATEGORY,
                const weapon_type::Enum WEAPON_TYPE,
                const axe_type::Enum AXE_TYPE,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC,
                const TypeWrapper & TYPE_WRAPPER);

            static const ItemPtr_t Make_Club(
                const category::Enum CATEGORY,
                const weapon_type::Enum WEAPON_TYPE,
                const club_type::Enum CLUB_TYPE,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC,
                const TypeWrapper & TYPE_WRAPPER);

            static const ItemPtr_t Make_Whip(
                const category::Enum CATEGORY,
                const weapon_type::Enum WEAPON_TYPE,
                const whip_type::Enum WHIP_TYPE,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC,
                const TypeWrapper & TYPE_WRAPPER);

            static const ItemPtr_t Make_Projectile(
                const category::Enum CATEGORY,
                const weapon_type::Enum WEAPON_TYPE,
                const projectile_type::Enum PROJ_TYPE,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC,
                const TypeWrapper & TYPE_WRAPPER);

            static const ItemPtr_t Make_Staff(
                const category::Enum CATEGORY,
                const weapon_type::Enum WEAPON_TYPE,
                const bool IS_QUARTERSTAFF,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC,
                const bool IS_PIXIE_ITEM,
                const TypeWrapper & TYPE_WRAPPER);

            static const ItemPtr_t Make_Staff_AsMisc(const ItemProfile &);

            static const ItemPtr_t Make_BladedStaff(
                const category::Enum CATEGORY,
                const weapon_type::Enum WEAPON_TYPE,
                const bladedstaff_type::Enum STAFF_TYPE,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC,
                const TypeWrapper & TYPE_WRAPPER);
        };

    } // namespace weapon
} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_WEAPONFACTORY_HPP_INCLUDED
