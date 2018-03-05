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
#ifndef HEROESPATH_MISC_WAREHOUSE_HPP_INCLUDED
#define HEROESPATH_MISC_WAREHOUSE_HPP_INCLUDED
//
// warehouse.hpp
//  A template for pointer storing (warehouse) (object lifetime management) classes.
//
#include "log/log-macros.hpp"

#include "misc/assertlogandthrow.hpp"

#include <exception>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace heroespath
{
namespace misc
{

    // Template responsible for the lifetimes of various objects.  This class does
    // not new the objects, but it does delete them, hence the name 'warehouse'.
    template <typename T>
    class Warehouse
    {
        Warehouse(const Warehouse &) = delete;
        Warehouse & operator=(const Warehouse &) = delete;

    public:
        Warehouse()
            : uPtrVec_()
        {}

        inline std::size_t Size() const { return uPtrVec_.size(); }

        // returns the pointer given for caller ease of use only
        T * Store(T * ptr_to_store, const std::string & NAME)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ptr_to_store != nullptr),
                "Warehouse::Store(name=\"" << NAME << "\") given a nullptr.");

            std::size_t indexToSaveAt{ uPtrVec_.size() };

            // Ensure this object is not already stored, and along the way,
            // look for an abandoned slot to use as indexToSaveAt.
            auto const NUM_OBJECTS{ indexToSaveAt };
            for (std::size_t i(0); i < NUM_OBJECTS; ++i)
            {
                auto const NEXT_PTR{ uPtrVec_[i].get() };
                if (NEXT_PTR == ptr_to_store)
                {
                    std::ostringstream ss;
                    ss << "Warehouse::Store((" << ptr_to_store << ")"
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
                uPtrVec_.push_back(std::move(tempUPtr));
            }

            return ptr_to_store;
        }

        void Free(T *& ptr_to_free, const std::string & NAME)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ptr_to_free != nullptr),
                "Warehouse::Free(name=\"" << NAME << "\") given a nullptr.");

            for (auto & nextUPtr : uPtrVec_)
            {
                if (nextUPtr.get() == ptr_to_free)
                {
                    ptr_to_free = nullptr;
                    nextUPtr.reset();
                    return;
                }
            }

            M_HP_LOG_ERR(
                "Warehouse::Free((" << ptr_to_free << "), name=\"" << NAME << "\") not found.");
        }

    private:
        std::vector<std::unique_ptr<T>> uPtrVec_;
    };
} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_WAREHOUSE_HPP_INCLUDED
