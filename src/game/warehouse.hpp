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
#ifndef GAME_WAREHOUSE_HPP_INCLUDED
#define GAME_WAREHOUSE_HPP_INCLUDED
//
// warehouse.hpp
//  A template for pointer storing (warehouse) (object lifetime management) classes.
//
#include "game/log-macros.hpp"

#include "misc/assertlogandthrow.hpp"

#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include <exception>


namespace game
{

    //Template responsible for the lifetimes of various types of objects.
    //This class does not new the objects, but it does delete them, hence
    //the name 'warehouse'.
    template<typename T>
    class Warehouse
    {
        //prevent copy constructor
        Warehouse(const Warehouse &) =delete;

        //prevent copy assignment
        Warehouse & operator=(const Warehouse &) =delete;

    public:
        Warehouse()
        :
            uPtrVec_()
        {}


        //returns the pointer given for caller ease of use only
        T * Store(T * ptr_to_store, const std::string & NAME)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((ptr_to_store != nullptr),
                "game::Warehouse::Store(name=\"" << NAME << "\") given a nullptr.");

            std::size_t indexToSaveAt{ uPtrVec_.size() };

            //Ensure this object is not already stored, and along the way,
            //look for an abandoned slot to use as indexToSaveAt.
            auto const NUM_OBJECTS{ indexToSaveAt };
            for (std::size_t i(0); i<NUM_OBJECTS; ++i)
            {
                auto const NEXT_PTR{ uPtrVec_[i].get() };
                if (NEXT_PTR == ptr_to_store)
                {
                    std::ostringstream ss;
                    ss << "game::Warehouse::Store((" << ptr_to_store << ")"
                        << ", name=\"" << NAME << "\") was already stored.";

                    throw std::runtime_error(ss.str());
                }
                else if (NEXT_PTR == nullptr)
                {
                    indexToSaveAt = i;
                }
            }

            if (indexToSaveAt < NUM_OBJECTS)
            {
                uPtrVec_[indexToSaveAt].reset(ptr_to_store);
            }
            else
            {
                std::unique_ptr<T> tempUPtr;
                tempUPtr.reset(ptr_to_store);
                uPtrVec_.push_back( std::move(tempUPtr) );
            }

            return ptr_to_store;
        }


        void Free(T * ptr_to_free, const std::string & NAME)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((ptr_to_free != nullptr),
                "game::Warehouse::Free(name=\"" << NAME << "\") given a nullptr.");

            for (auto & nextUPtr : uPtrVec_)
            {
                if (nextUPtr.get() == ptr_to_free)
                {
                    ptr_to_free = nullptr;
                    nextUPtr.reset();
                    return;
                }
            }

            M_HP_LOG_ERR("game::Warehouse::Free((" << ptr_to_free
                << "), name=\"" << NAME << "\") not found.");
        }


    private:
        std::vector< std::unique_ptr<T> > uPtrVec_;
    };


}

#endif //GAME_WAREHOUSE_HPP_INCLUDED
