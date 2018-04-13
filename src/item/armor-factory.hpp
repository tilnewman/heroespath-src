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
#ifndef HEROESPATH_ITEM_ARMORFACTORY_HPP_INCLUDED
#define HEROESPATH_ITEM_ARMORFACTORY_HPP_INCLUDED
//
// armor-factory.hpp
//  Code that is responsible for making armor objects.
//
#include "armor-details.hpp"
#include "item/armor-types.hpp"
#include "item/item-factory-base.hpp"
#include "item/item-type-wrapper.hpp"
#include "misc/not-null.hpp"
#include "misc/types.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;

    class ItemProfile;

    namespace armor
    {

        // A singleton class responsible for creating armor objects.
        class ArmorFactory : public FactoryBase
        {
        public:
            ArmorFactory(const ArmorFactory &) = delete;
            ArmorFactory(ArmorFactory &&) = delete;
            ArmorFactory & operator=(const ArmorFactory &) = delete;
            ArmorFactory & operator=(ArmorFactory &&) = delete;

        public:
            ArmorFactory();
            virtual ~ArmorFactory();

            static misc::NotNull<ArmorFactory *> Instance();
            static void Acquire();
            static void Release();

            static const ArmorDetails LookupArmorDetails(const std::string & NAME)
            {
                return armorDetailsUPtr_->LookupArmorDetails(NAME);
            }

            static const ItemPtr_t Make_Shield(const ItemProfile &);

            // primary material can be wood, secondary material can be Nothing
            static const ItemPtr_t Make_Shield(
                const TypeWrapper & TYPE_WRAPPER,
                const shield_type::Enum TYPE,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC = material::Nothing);

            static const ItemPtr_t Make_Helm(const ItemProfile &);

            static const ItemPtr_t Make_Helm(
                const TypeWrapper & TYPE_WRAPPER,
                const helm_type::Enum TYPE,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC = material::Nothing);

            static const ItemPtr_t Make_Gauntlets(const ItemProfile &);

            static const ItemPtr_t Make_Gauntlets(
                const TypeWrapper & TYPE_WRAPPER,
                const base_type::Enum TYPE,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC = material::Nothing,
                const bool IS_PIXIE_ITEM = false);

            static const ItemPtr_t Make_Pants(const ItemProfile &);

            static const ItemPtr_t Make_Pants(
                const TypeWrapper & TYPE_WRAPPER,
                const base_type::Enum TYPE,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC = material::Nothing,
                const bool IS_PIXIE_ITEM = false);

            static const ItemPtr_t Make_Boots(const ItemProfile &);

            static const ItemPtr_t Make_Boots(
                const TypeWrapper & TYPE_WRAPPER,
                const base_type::Enum TYPE,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC = material::Nothing,
                const bool IS_PIXIE_ITEM = false);

            static const ItemPtr_t Make_Shirt(const ItemProfile &);

            static const ItemPtr_t Make_Shirt(
                const TypeWrapper & TYPE_WRAPPER,
                const base_type::Enum TYPE,
                const material::Enum MATERIAL_PRI = material::Cloth,
                const material::Enum MATERIAL_SEC = material::Nothing,
                const bool IS_PIXIE_ITEM = false);

            static const ItemPtr_t Make_Bracer(const ItemProfile &);

            static const ItemPtr_t Make_Bracer(
                const TypeWrapper & TYPE_WRAPPER,
                const base_type::Enum TYPE,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC = material::Nothing,
                const bool IS_PIXIE_ITEM = false);

            static const ItemPtr_t Make_Aventail(const ItemProfile &);

            static const ItemPtr_t Make_Aventail(
                const TypeWrapper & TYPE_WRAPPER,
                const base_type::Enum TYPE,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC = material::Nothing);

            static const ItemPtr_t Make_Cover(const ItemProfile &);

            // primary material should always be either cloth/softleather/hardleather
            static const ItemPtr_t Make_Cover(
                const TypeWrapper & TYPE_WRAPPER,
                const cover_type::Enum TYPE,
                const material::Enum MATERIAL_PRI,
                const material::Enum MATERIAL_SEC = material::Nothing,
                const bool IS_PIXIE_ITEM = false);

            static const ItemPtr_t Make_Cape_AsMiscItem(const ItemProfile &);

            // material must be plant, flesh, scale, hide, fur, or featers
            static const ItemPtr_t Make_Skin(
                const material::Enum MATERIAL,
                const Rank_t & CREATURE_RANK,
                const bool IS_PIXIE_ITEM);

        private:
            static std::unique_ptr<ArmorFactory> instanceUPtr_;
            static std::unique_ptr<ArmorDetailLoader> armorDetailsUPtr_;
        };

    } // namespace armor
} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ARMORFACTORY_HPP_INCLUDED
