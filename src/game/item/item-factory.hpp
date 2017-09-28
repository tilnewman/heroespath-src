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
#ifndef GAME_ITEM_ITEMFACTORY_HPP_INCLUDED
#define GAME_ITEM_ITEMFACTORY_HPP_INCLUDED
//
// item-factory.hpp
//
#include <memory>
#include <string>


namespace game
{
namespace item
{
    class ItemProfile;

    class Item;
    using ItemPtr_t = Item *;


    //Responsible for making new (and properly stored) item objects from fat ItemProfiles.
    class ItemFactory
    {
        ItemFactory(const ItemFactory &) =delete;
        ItemFactory & operator=(const ItemFactory &) = delete;

    public:
        ItemFactory();
        virtual ~ItemFactory();

        static ItemFactory * Instance();
        static void Acquire();
        static void Release();
        static bool Test();

        static void TestItem(const std::string & WHICH_TEST,
                             const ItemPtr_t     ITEM_PTR,
                             const ItemProfile & ITEM_PROFILE);

        ItemPtr_t Make(const ItemProfile &) const;

    private:
        ItemPtr_t SetTypesAndReturn(const ItemProfile &, ItemPtr_t) const;
        void AppendElementTypeToName(ItemPtr_t, const ItemProfile &) const;

    private:
        static std::unique_ptr<ItemFactory> instanceUPtr_;
    };

}
}

#endif //GAME_ITEM_ITEMFACTORY_HPP_INCLUDED
