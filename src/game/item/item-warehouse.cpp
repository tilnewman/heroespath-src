// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// item-warehouse.cpp
//
#include "item-warehouse.hpp"

#include "game/item/item.hpp"

#include "misc/assertlogandthrow.hpp"

#include <sstream>
#include <exception>


namespace game
{
namespace item
{

    std::unique_ptr<ItemWarehouse> ItemWarehouse::instanceUPtr_{ nullptr };

    
    ItemWarehouse::ItemWarehouse()
    :
        itemUVec_()
    {}


    ItemWarehouse * ItemWarehouse::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void ItemWarehouse::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new ItemWarehouse);
        }
    }


    void ItemWarehouse::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
            "game::item::ItemWarehouse::Release() found instanceUPtr that was null.");
        instanceUPtr_.reset();
    }


    ItemPtr_t ItemWarehouse::Store(const ItemPtr_t ITEM_PTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((ITEM_PTR != nullptr),
            "game::item::ItemWarehouse::Store() given a nullptr.");

        std::size_t indexToSaveAt{ itemUVec_.size() };

        //ensure this item is not already stored
        //along the way, look for an abandoned index to use with indexToSaveAt
        auto const NUM_ITEMS{ indexToSaveAt };
        for (std::size_t i(0); i<NUM_ITEMS; ++i)
        {
            auto const NEXT_ITEM_PTR{ itemUVec_[i].get() };
            if (NEXT_ITEM_PTR == ITEM_PTR)
            {
                std::ostringstream ss;
                ss << "game::item::ItemWarehouse::Store(item="
                    << ITEM_PTR->Name() << ") was already in the warehouse.";
                throw std::runtime_error(ss.str());
            }
            else if (NEXT_ITEM_PTR == nullptr)
            {
                indexToSaveAt = i;
            }
        }

        if (indexToSaveAt < NUM_ITEMS)
        {
            itemUVec_[indexToSaveAt].reset(ITEM_PTR);
            return itemUVec_[indexToSaveAt].get();
        }
        else
        {
            std::unique_ptr<Item> tempUPtr;
            tempUPtr.reset(ITEM_PTR);
            itemUVec_.push_back(std::move(tempUPtr));
            return itemUVec_[itemUVec_.size() - 1].get();
        }
    }


    void ItemWarehouse::Free(ItemPtr_t & itemPtr)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((itemPtr != nullptr),
            "game::item::ItemWarehouse::Free() given a nullptr.");

        for (auto & nextItemUPtr : itemUVec_)
        {
            if (nextItemUPtr.get() == itemPtr)
            {
                itemPtr = nullptr;
                nextItemUPtr.reset();
                return;
            }
        }
    }

}
}
