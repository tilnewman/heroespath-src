// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_SERIALIZE_HELPERS_HPP_INCLUDED
#define HEROESPATH_MISC_SERIALIZE_HELPERS_HPP_INCLUDED
//
// serialize-helpers.hpp
//
#include "misc/not-null.hpp"
#include <vector>

namespace heroespath
{
namespace misc
{

    struct SerializeHelp
    {
        template <typename T>
        static void BeforeSerialize(
            std::vector<misc::NotNull<T *>> & notNullPtrVec,
            std::vector<T *> & rawPtrToSerializeVec)
        {
            rawPtrToSerializeVec.clear();
            for (auto const & NOT_NULL_PTR : notNullPtrVec)
            {
                rawPtrToSerializeVec.emplace_back(NOT_NULL_PTR.Ptr());
            }
            notNullPtrVec.clear();
        }

        template <typename T, typename W>
        static void AfterSerialize(std::vector<T *> & rawPtrVec, W & warehouse)
        {
            warehouse.Free(rawPtrVec);
        }

        template <typename T, typename W>
        static void AfterDeserialize(
            std::vector<misc::NotNull<T *>> & notNullPtrVec,
            std::vector<T *> & rawPtrToSerializeVec,
            W & warehouse)
        {
            notNullPtrVec.clear();
            for (auto const RAW_PTR : rawPtrToSerializeVec)
            {
                notNullPtrVec.emplace_back(warehouse.Store(RAW_PTR));
            }
            rawPtrToSerializeVec.clear();
        }
    };

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_SERIALIZE_HELPERS_HPP_INCLUDED
