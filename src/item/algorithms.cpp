// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// algorithms.cpp
//
#include "algorithms.hpp"

#include "item/item.hpp"

namespace heroespath
{
namespace item
{

    const std::string
        Algorithms::Names(const ItemPVec_t & ITEM_PVEC, const misc::JoinHow & JOIN_HOW)
    {
        return misc::Join(
            ITEM_PVEC, JOIN_HOW, [](const auto & ITEM_PTR) { return ITEM_PTR->Name(); });
    }

    const ItemPVec_t Algorithms::FindByCategory(
        const ItemPVec_t & ITEM_PVEC,
        const Category::Enum CATEGORY_TYPE,
        const MatchOpt MATCH_OPTION)
    {
        ItemPVec_t resultSVec;

        if (ITEM_PVEC.empty())
        {
            return resultSVec;
        }

        resultSVec.reserve(ITEM_PVEC.size());

        std::copy_if(
            ITEM_PVEC.begin(),
            ITEM_PVEC.end(),
            back_inserter(resultSVec),
            [CATEGORY_TYPE, MATCH_OPTION](const auto & ITEM_PTR) {
                return (ITEM_PTR->HasCategory(CATEGORY_TYPE) == (MATCH_OPTION == MatchOpt::Equal));
            });

        return resultSVec;
    }

    const ItemPVec_t Algorithms::FindByWeaponType(
        const ItemPVec_t & ITEM_PVEC, const Weapon::Enum WEAPON_TYPE, const MatchOpt MATCH_OPTION)
    {
        ItemPVec_t resultSVec;

        if (ITEM_PVEC.empty())
        {
            return resultSVec;
        }

        resultSVec.reserve(ITEM_PVEC.size());

        std::copy_if(
            ITEM_PVEC.begin(),
            ITEM_PVEC.end(),
            back_inserter(resultSVec),
            [WEAPON_TYPE, MATCH_OPTION](const auto & ITEM_PTR) {
                return (
                    (ITEM_PTR->WeaponType() == WEAPON_TYPE) == (MATCH_OPTION == MatchOpt::Equal));
            });

        return resultSVec;
    }

    const ItemPVec_t Algorithms::FindByArmorType(
        const ItemPVec_t & ITEM_PVEC, const Armor::Enum ARMOR_TYPE, const MatchOpt MATCH_OPTION)
    {
        ItemPVec_t resultSVec;

        if (ITEM_PVEC.empty())
        {
            return resultSVec;
        }

        resultSVec.reserve(ITEM_PVEC.size());

        std::copy_if(
            ITEM_PVEC.begin(),
            ITEM_PVEC.end(),
            back_inserter(resultSVec),
            [ARMOR_TYPE, MATCH_OPTION](const auto & ITEM_PTR) {
                return ((ITEM_PTR->ArmorType() == ARMOR_TYPE) == (MATCH_OPTION == MatchOpt::Equal));
            });

        return resultSVec;
    }

    const ItemPVec_t Algorithms::FindByMiscType(
        const ItemPVec_t & ITEM_PVEC, const Misc::Enum MISC_TYPE, const MatchOpt MATCH_OPTION)
    {
        ItemPVec_t resultSVec;

        if (ITEM_PVEC.empty())
        {
            return resultSVec;
        }

        resultSVec.reserve(ITEM_PVEC.size());

        std::copy_if(
            ITEM_PVEC.begin(),
            ITEM_PVEC.end(),
            back_inserter(resultSVec),
            [MISC_TYPE, MATCH_OPTION](const auto & ITEM_PTR) {
                return ((ITEM_PTR->MiscType() == MISC_TYPE) == (MATCH_OPTION == MatchOpt::Equal));
            });

        return resultSVec;
    }

    const ItemPVec_t
        Algorithms::FindByPixie(const ItemPVec_t & ITEM_PVEC, const MatchOpt MATCH_OPTION)
    {
        ItemPVec_t resultSVec;

        if (ITEM_PVEC.empty())
        {
            return resultSVec;
        }

        resultSVec.reserve(ITEM_PVEC.size());

        std::copy_if(
            ITEM_PVEC.begin(),
            ITEM_PVEC.end(),
            back_inserter(resultSVec),
            [MATCH_OPTION](const auto & ITEM_PTR) {
                return (ITEM_PTR->IsPixie() == (MATCH_OPTION == MatchOpt::Equal));
            });

        return resultSVec;
    }

    const ItemPVec_t
        Algorithms::FindByBroken(const ItemPVec_t & ITEM_PVEC, const BrokenOpt BROKEN_OPTION)
    {
        ItemPVec_t resultSVec;

        if (ITEM_PVEC.empty())
        {
            return resultSVec;
        }

        resultSVec.reserve(ITEM_PVEC.size());

        std::copy_if(
            ITEM_PVEC.begin(),
            ITEM_PVEC.end(),
            back_inserter(resultSVec),
            [BROKEN_OPTION](const auto & ITEM_PTR) {
                return (ITEM_PTR->IsBroken() == (BROKEN_OPTION == BrokenOpt::Keep));
            });

        return resultSVec;
    }

} // namespace item
} // namespace heroespath
