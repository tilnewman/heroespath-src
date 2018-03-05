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

#include "item/item.hpp"

#include "misc/random.hpp"

namespace heroespath
{
namespace item
{

    const std::string Algorithms::Names(
        const ItemPVec_t & ITEM_PVEC,
        const std::size_t MAX_COUNT,
        const misc::Vector::JoinOpt OPTIONS)
    {
        return misc::Vector::Join<ItemPtr_t>(
            ITEM_PVEC, MAX_COUNT, OPTIONS, [](const ItemPtr_t PTR) -> const std::string {
                return PTR->Name();
            });
    }

    const ItemPVec_t Algorithms::FindByCategory(
        const ItemPVec_t & ITEM_PVEC,
        const category::Enum CATEGORY_TYPE,
        const MatchOpt MATCH_OPTION)
    {
        ItemPVec_t resultSVec;

        if (ITEM_PVEC.empty() == false)
        {
            std::copy_if(
                ITEM_PVEC.begin(),
                ITEM_PVEC.end(),
                back_inserter(resultSVec),
                [CATEGORY_TYPE, MATCH_OPTION](const ItemPtr_t PTR) {
                    return PTR->IsCategoryType(CATEGORY_TYPE) == (MATCH_OPTION == MatchOpt::Equal);
                });
        }

        return resultSVec;
    }

    const ItemPVec_t Algorithms::FindByWeaponType(
        const ItemPVec_t & ITEM_PVEC,
        const weapon_type::Enum WEAPON_TYPE,
        const MatchOpt MATCH_OPTION)
    {
        ItemPVec_t resultSVec;

        if (ITEM_PVEC.empty() == false)
        {
            std::copy_if(
                ITEM_PVEC.begin(),
                ITEM_PVEC.end(),
                back_inserter(resultSVec),
                [WEAPON_TYPE, MATCH_OPTION](const ItemPtr_t PTR) {
                    return (PTR->IsWeaponType(WEAPON_TYPE) == (MATCH_OPTION == MatchOpt::Equal));
                });
        }
        return resultSVec;
    }

    const ItemPVec_t Algorithms::FindByArmorType(
        const ItemPVec_t & ITEM_PVEC,
        const armor_type::Enum ARMOR_TYPE,
        const MatchOpt MATCH_OPTION)
    {
        ItemPVec_t resultSVec;

        if (ITEM_PVEC.empty() == false)
        {
            std::copy_if(
                ITEM_PVEC.begin(),
                ITEM_PVEC.end(),
                back_inserter(resultSVec),
                [ARMOR_TYPE, MATCH_OPTION](const ItemPtr_t PTR) {
                    return (PTR->IsArmorType(ARMOR_TYPE) == (MATCH_OPTION == MatchOpt::Equal));
                });
        }

        return resultSVec;
    }

    const ItemPVec_t Algorithms::FindByMiscType(
        const ItemPVec_t & ITEM_PVEC, const misc_type::Enum MISC_TYPE, const MatchOpt MATCH_OPTION)
    {
        ItemPVec_t resultSVec;

        if (ITEM_PVEC.empty() == false)
        {
            std::copy_if(
                ITEM_PVEC.begin(),
                ITEM_PVEC.end(),
                back_inserter(resultSVec),
                [MISC_TYPE, MATCH_OPTION](const ItemPtr_t PTR) {
                    return (PTR->IsMiscType(MISC_TYPE) == (MATCH_OPTION == MatchOpt::Equal));
                });
        }

        return resultSVec;
    }

    const ItemPVec_t
        Algorithms::FindByPixie(const ItemPVec_t & ITEM_PVEC, const MatchOpt MATCH_OPTION)
    {
        ItemPVec_t resultSVec;

        if (ITEM_PVEC.empty() == false)
        {
            std::copy_if(
                ITEM_PVEC.begin(),
                ITEM_PVEC.end(),
                back_inserter(resultSVec),
                [MATCH_OPTION](const ItemPtr_t PTR) {
                    return (PTR->IsPixie() == (MATCH_OPTION == MatchOpt::Equal));
                });
        }

        return resultSVec;
    }

    const ItemPVec_t
        Algorithms::FindByBroken(const ItemPVec_t & ITEM_PVEC, const BrokenOpt BROKEN_OPTION)
    {
        ItemPVec_t resultSVec;

        if (ITEM_PVEC.empty() == false)
        {
            std::copy_if(
                ITEM_PVEC.begin(),
                ITEM_PVEC.end(),
                back_inserter(resultSVec),
                [BROKEN_OPTION](const ItemPtr_t PTR) {
                    return (PTR->IsBroken() == (BROKEN_OPTION == BrokenOpt::Keep));
                });
        }

        return resultSVec;
    }
} // namespace item
} // namespace heroespath
