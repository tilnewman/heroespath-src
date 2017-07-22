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
#ifndef GAME_ITEM_ITEMWAREHOUSE_HPP_INCLUDED
#define GAME_ITEM_ITEMWAREHOUSE_HPP_INCLUDED
//
// item-warehouse.hpp
//
#include <memory>
#include <vector>


namespace game
{
namespace item
{

    //forward declarations
    class Item;
    using ItemPtr_t  = Item *;
    using ItemUPtr_t = std::unique_ptr<Item>;
    using ItemUVec_t = std::vector<ItemUPtr_t>;


    //Singleton responsible for the lifetimes of item objects.
    //This class does not new the objects, but it does delete them.
    class ItemWarehouse
    {
        //prevent copy constructor
        ItemWarehouse(const ItemWarehouse &);

        //prevent copy assignment
        ItemWarehouse & operator=(const ItemWarehouse &);

        //prevent non-singleton construction
        ItemWarehouse();

    public:
        ~ItemWarehouse();

        static ItemWarehouse * Instance();
        static void Acquire();
        static void Release();

        ItemPtr_t Store(const ItemPtr_t);
        void Free(ItemPtr_t &);

    private:
        static std::unique_ptr<ItemWarehouse> instanceUPtr_;
        ItemUVec_t itemUVec_;
    };

}
}

#endif //GAME_ITEM_ITEMWAREHOUSE_HPP_INCLUDED
