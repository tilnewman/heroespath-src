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
