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
#ifndef HEROESPATH_ITEM_MISCITEMFACTORY_HPP_INCLUDED
#define HEROESPATH_ITEM_MISCITEMFACTORY_HPP_INCLUDED
//
// misc-item-factory.hpp
//  Responsible for making individual item objects.
//
#include "item/item-factory-base.hpp"
#include "item/item-type-enum.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace item
{

    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;

    class ItemProfile;

    // A Subsystem class responsible for making misc item objects.
    class MiscItemFactory : public ItemFactoryBase
    {
    public:
        MiscItemFactory(const MiscItemFactory &) = delete;
        MiscItemFactory(MiscItemFactory &&) = delete;
        MiscItemFactory & operator=(const MiscItemFactory &) = delete;
        MiscItemFactory & operator=(MiscItemFactory &&) = delete;
        MiscItemFactory() = delete;

        static const ItemPtr_t Make(const ItemProfile &);
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_MISCITEMFACTORY_HPP_INCLUDED
