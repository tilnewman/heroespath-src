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

#include "misc/nameof.hpp"

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
        using NotNullOfType_t = NotNull<T *>;

    public:
        NotNullWarehouse(const NotNullWarehouse &) = delete;
        NotNullWarehouse(NotNullWarehouse &&) = delete;
        NotNullWarehouse & operator=(const NotNullWarehouse &) = delete;
        NotNullWarehouse & operator=(NotNullWarehouse &&) = delete;

        NotNullWarehouse(const std::size_t RESERVE_COUNT)
            : uPtrVec_()
        {
            uPtrVec_.reserve(RESERVE_COUNT);
        }

        ~NotNullWarehouse()
        {
            const auto SIZE { Size() };

            std::ostringstream ss;
            ss << "misc::NotNullWarehouse<" << NAMEOF_TYPE_T_STR(T) << "> destructing, ";

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
                            << NAMEOF_TYPE_T_STR(T)
                            << ">::Destructor found an object that was not free'd: "
                            << uPtr->ToString());
                    }
                }
            }
        }

        std::size_t Size() const
        {
            std::size_t count { 0 };
            for (const auto & UPTR : uPtrVec_)
            {
                if (UPTR)
                {
                    ++count;
                }
            }

            return count;
        }

        const NotNullOfType_t Store(const NotNullOfType_t NOT_NULL_PTR)
        {
            return StoreImpl(NOT_NULL_PTR);
        }

        const NotNullOfType_t Store(std::unique_ptr<T> uPtrToStore)
        {
            const auto NOTNULL_PTR_TO_RETURN { StoreImpl(uPtrToStore.get()) };
            uPtrToStore.release();
            return NOTNULL_PTR_TO_RETURN;
        }

        const NotNullOfType_t Store(T * const ptrToStore) { return StoreImpl(ptrToStore); }

        void Free(std::vector<NotNull<T *>> & notNullVec)
        {
            for (const auto & NOT_NULL : notNullVec)
            {
                FreeImpl(NOT_NULL);
            }

            notNullVec.clear();
        }

        void Free(std::vector<T *> & ptrVec)
        {
            for (auto ptr : ptrVec)
            {
                FreeImpl(ptr);
            }

            ptrVec.clear();
        }

        void Free(const NotNullOfType_t NOT_NULL_PTR) { FreeImpl(NOT_NULL_PTR); }

        void Free(T * ptrToFree) { FreeImpl(ptrToFree); }

    private:
        const NotNullOfType_t StoreImpl(T * const ptrToStore)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (ptrToStore != nullptr),
                "misc::NotNullWarehouse<" << NAMEOF_TYPE_T_STR(T)
                                          << ">::StoreImpl() given a nullptr.");

            const auto NUM_SLOTS { uPtrVec_.size() };
            std::size_t indexToSaveAt { NUM_SLOTS };

            // Ensure this object is not already stored, and along the way,
            // look for an abandoned slot to use as indexToSaveAt.
            for (std::size_t i(0); i < NUM_SLOTS; ++i)
            {
                const auto STORED_PTR { uPtrVec_[i].get() };
                if (STORED_PTR == ptrToStore)
                {
                    std::ostringstream ss;
                    ss << "misc::Warehouse<" << NAMEOF_TYPE_T_STR(T) << ">::StoreImpl("
                       << ptrToStore->ToString() << ") was already stored.";

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

            return NotNullOfType_t(ptrToStore);
        }

        void FreeImpl(T * ptrToFree)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (ptrToFree != nullptr),
                "misc::NotNullWarehouse<" << NAMEOF_TYPE_T_STR(T)
                                          << ">::FreeImpl() given a nullptr.");

            for (auto & uPtr : uPtrVec_)
            {
                if (uPtr.get() == ptrToFree)
                {
                    uPtr.reset();
                    ptrToFree = nullptr;
                    return;
                }
            }

            M_HP_LOG_ERR(
                "misc::NotNullWarehouse<"
                << NAMEOF_TYPE_T_STR(T) << ">::FreeImpl(" << ptrToFree->ToString()
                << ") not found.  Will delete manually.  Cross your fingers...");

            delete ptrToFree;
            ptrToFree = nullptr;
        }

        std::vector<std::unique_ptr<T>> uPtrVec_;
    };

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_NOT_NULLWAREHOUSE_HPP_INCLUDED
