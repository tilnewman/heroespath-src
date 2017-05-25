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
// algorithms.cpp
//
#include "algorithms.hpp"

#include "misc/random.hpp"
#include "misc/vectors.hpp"

#include "game/item/item.hpp"

#include <string>


namespace game
{
namespace item
{

    const std::string Algorithms::Names(const ItemSVec_t & ITEM_SVEC,
                                        const bool         WILL_WRAP,
                                        const bool         WILL_APPEND_AND,
                                        const std::size_t  MAX_COUNT,
                                        const bool         WILL_ELLIPSIS)
    {
        return misc::Vector::Join<ItemSPtr_t>(ITEM_SVEC,
                                               WILL_WRAP,
                                               WILL_APPEND_AND,
                                               MAX_COUNT,
                                               WILL_ELLIPSIS,
                                               [](const ItemSPtr_t SPTR) -> const std::string
                                                {
                                                    return SPTR->Name();
                                                });
    }


    const ItemSVec_t Algorithms::FindByCategory(const ItemSVec_t &   ITEM_SVEC,
                                                const category::Enum CATEGORY_TYPE,
                                                const bool           IS_MATCH)
    {
        ItemSVec_t resultSVec;

        if (ITEM_SVEC.empty() == false)
            std::copy_if(ITEM_SVEC.begin(),
                         ITEM_SVEC.end(),
                         back_inserter(resultSVec),
                         [CATEGORY_TYPE, IS_MATCH] (const ItemSPtr_t & SPTR) { return ((SPTR->IsCategoryType(CATEGORY_TYPE)) == IS_MATCH); });

        return resultSVec;
    }


    const ItemSVec_t Algorithms::FindByWeaponType(const ItemSVec_t &      ITEM_SVEC,
                                                  const weapon_type::Enum WEAPON_TYPE,
                                                  const bool              IS_MATCH)
    {
        ItemSVec_t resultSVec;

        if (ITEM_SVEC.empty() == false)
            std::copy_if(ITEM_SVEC.begin(),
                         ITEM_SVEC.end(),
                         back_inserter(resultSVec),
                         [WEAPON_TYPE, IS_MATCH] (const ItemSPtr_t & SPTR) { return ((SPTR->IsWeaponType(WEAPON_TYPE)) == IS_MATCH); });

        return resultSVec;
    }


    const ItemSVec_t Algorithms::FindByArmorType(const ItemSVec_t &     ITEM_SVEC,
                                                 const armor_type::Enum ARMOR_TYPE,
                                                 const bool             IS_MATCH)
    {
        ItemSVec_t resultSVec;

        if (ITEM_SVEC.empty() == false)
            std::copy_if(ITEM_SVEC.begin(),
                         ITEM_SVEC.end(),
                         back_inserter(resultSVec),
                         [ARMOR_TYPE, IS_MATCH] (const ItemSPtr_t & SPTR) { return ((SPTR->IsArmorType(ARMOR_TYPE)) == IS_MATCH); });

        return resultSVec;
    }


    const ItemSVec_t Algorithms::FindByMiscType(const ItemSVec_t &    ITEM_SVEC,
                                                const misc_type::Enum MISC_TYPE,
                                                const bool            IS_MATCH)
    {
        ItemSVec_t resultSVec;

        if (ITEM_SVEC.empty() == false)
            std::copy_if(ITEM_SVEC.begin(),
                         ITEM_SVEC.end(),
                         back_inserter(resultSVec),
                         [MISC_TYPE, IS_MATCH] (const ItemSPtr_t & SPTR) { return ((SPTR->IsMiscType(MISC_TYPE)) == IS_MATCH); });

        return resultSVec;
    }


    const ItemSVec_t Algorithms::FindByPixie(const ItemSVec_t & ITEM_SVEC, const bool IS_MATCH)
    {
        ItemSVec_t resultSVec;

        if (ITEM_SVEC.empty() == false)
            std::copy_if(ITEM_SVEC.begin(),
                         ITEM_SVEC.end(),
                         back_inserter(resultSVec),
                         [IS_MATCH] (const ItemSPtr_t & SPTR) { return (SPTR->IsPixie() == IS_MATCH); });

        return resultSVec;
    }


    const ItemSVec_t Algorithms::FindByBroken(const ItemSVec_t & ITEM_SVEC, const bool WILL_KEEP_BROKEN)
    {
        ItemSVec_t resultSVec;

        if (ITEM_SVEC.empty() == false)
            std::copy_if(ITEM_SVEC.begin(),
                         ITEM_SVEC.end(),
                         back_inserter(resultSVec),
                         [WILL_KEEP_BROKEN](const ItemSPtr_t & SPTR) { return (SPTR->IsBroken() == WILL_KEEP_BROKEN); });

        return resultSVec;
    }

}
}
