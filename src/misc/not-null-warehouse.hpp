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
#ifndef HEROESPATH_MISC_NOT_NULLWAREHOUSE_HPP_INCLUDED
#define HEROESPATH_MISC_NOT_NULLWAREHOUSE_HPP_INCLUDED
//
// not-null-warehouse.hpp
//
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/not-null.hpp"
#include <boost/type_index.hpp>

#include <exception>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace heroespath
{
namespace misc
{

    // Template responsible for the lifetimes of various objects on the heap.  This class does
    // not new the objects, but it does delete them, hence the name 'warehouse'.
    // This is different from misc::Warehouse because it is designed to work with the
    // misc::NotNull<> template.
    template <typename T>
    class NotNullWarehouse
    {
    public:
        NotNullWarehouse(const NotNullWarehouse &) = delete;
        NotNullWarehouse(NotNullWarehouse &&) = delete;
        NotNullWarehouse & operator=(const NotNullWarehouse &) = delete;
        NotNullWarehouse & operator=(NotNullWarehouse &&) = delete;

    public:
        NotNullWarehouse()
            : uPtrVec_()
        {}

        ~NotNullWarehouse()
        {
            auto const SIZE{ Size() };

            std::ostringstream ss;
            ss << "misc::NotNullWarehouse<" << boost::typeindex::type_id<T>().pretty_name()
               << "> destructing, ";

            if (SIZE != 0)
            {
                ss << "with " << SIZE << " objects NOT free'd, ";
            }

            ss << "at most " << uPtrVec_.size() << " were held at once.";

            if (SIZE == 0)
            {
                M_HP_LOG_DBG(ss.str());
            }
            else
            {
                M_HP_LOG_WRN(ss.str());

                for (auto & uPtr : uPtrVec_)
                {
                    if (uPtr)
                    {
                        M_HP_LOG_ERR(
                            "misc::NotNullWarehouse<"
                            << boost::typeindex::type_id<T>().pretty_name()
                            << ">::Destructor found an object that was not free'd: "
                            << uPtr->ToString());
                    }
                }
            }
        }

        std::size_t Size() const
        {
            std::size_t count{ 0 };
            for (auto const & UPTR : uPtrVec_)
            {
                if (UPTR)
                {
                    ++count;
                }
            }

            return count;
        }

        const misc::NotNull<T *> Store(const misc::NotNull<T *> NOT_NULL_PTR)
        {
            return Store(NOT_NULL_PTR.Ptr());
        }

        const misc::NotNull<T *> Store(std::unique_ptr<T> uPtrToStore)
        {
            auto const NOTNULL_PTR_TO_RETURN{ Store(uPtrToStore.get()) };
            uPtrToStore.release();
            return NOTNULL_PTR_TO_RETURN;
        }

        const misc::NotNull<T *> Store(T * ptrToStore)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ptrToStore != nullptr),
                "misc::NotNullWarehouse<" << boost::typeindex::type_id<T>().pretty_name()
                                          << ">::Store() given a nullptr.");

            std::size_t indexToSaveAt{ uPtrVec_.size() };

            // Ensure this object is not already stored, and along the way,
            // look for an abandoned slot to use as indexToSaveAt.
            auto const NUM_SLOTS{ indexToSaveAt };
            for (std::size_t i(0); i < NUM_SLOTS; ++i)
            {
                auto const STORED_PTR{ uPtrVec_[i].get() };
                if (STORED_PTR == ptrToStore)
                {
                    std::ostringstream ss;
                    ss << "misc::Warehouse<" << boost::typeindex::type_id<T>().pretty_name()
                       << ">::Store(" << ptrToStore->ToString() << ") was already stored.";

                    throw std::runtime_error(ss.str());
                }
                else if (STORED_PTR == nullptr)
                {
                    indexToSaveAt = i;
                    break;
                }
            }

            if (indexToSaveAt < NUM_SLOTS)
            {
                uPtrVec_[indexToSaveAt].reset(ptrToStore);
            }
            else
            {
                uPtrVec_.emplace_back(std::unique_ptr<T>(ptrToStore));
            }

            return misc::NotNull<T *>(ptrToStore);
        }

        void Free(std::vector<NotNull<T *>> & notNullVec)
        {
            for (auto const & NOT_NULL : notNullVec)
            {
                Free(NOT_NULL.Ptr());
            }

            notNullVec.clear();
        }

        void Free(std::vector<T *> & ptrVec)
        {
            for (auto ptr : ptrVec)
            {
                Free(ptr);
            }

            ptrVec.clear();
        }

        void Free(const NotNull<T *> NOT_NULL_PTR) { Free(NOT_NULL_PTR.Ptr()); }

        void Free(T * const ptrToFree)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ptrToFree != nullptr),
                "misc::NotNullWarehouse<" << boost::typeindex::type_id<T>().pretty_name()
                                          << ">::Free() given a nullptr.");

            for (auto & uPtr : uPtrVec_)
            {
                if (uPtr.get() == ptrToFree)
                {
                    uPtr.reset();
                    return;
                }
            }

            M_HP_LOG_ERR(
                "misc::NotNullWarehouse<" << boost::typeindex::type_id<T>().pretty_name()
                                          << ">::Free(" << ptrToFree->ToString() << ") not found.");
        }

    private:
        std::vector<std::unique_ptr<T>> uPtrVec_;
    };

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_NOT_NULLWAREHOUSE_HPP_INCLUDED
