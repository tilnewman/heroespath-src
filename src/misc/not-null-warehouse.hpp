// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_NOT_NULLWAREHOUSE_HPP_INCLUDED
#define HEROESPATH_MISC_NOT_NULLWAREHOUSE_HPP_INCLUDED
//
// not-null-warehouse.hpp
//
#include "misc/assertlogandthrow.hpp"
#include "misc/log-macros.hpp"
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
            auto const SIZE { Size() };

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
            std::size_t count { 0 };
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
            auto const NOTNULL_PTR_TO_RETURN { Store(uPtrToStore.get()) };
            uPtrToStore.release();
            return NOTNULL_PTR_TO_RETURN;
        }

        const misc::NotNull<T *> Store(T * ptrToStore)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (ptrToStore != nullptr),
                "misc::NotNullWarehouse<" << boost::typeindex::type_id<T>().pretty_name()
                                          << ">::Store() given a nullptr.");

            std::size_t indexToSaveAt { uPtrVec_.size() };

            // Ensure this object is not already stored, and along the way,
            // look for an abandoned slot to use as indexToSaveAt.
            auto const NUM_SLOTS { indexToSaveAt };
            for (std::size_t i(0); i < NUM_SLOTS; ++i)
            {
                auto const STORED_PTR { uPtrVec_[i].get() };
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
            M_HP_ASSERT_OR_LOG_AND_THROW(
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
