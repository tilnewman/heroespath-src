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
// traits-set.cpp
//
#include "traits-set.hpp"

#include "misc/assertlogandthrow.hpp"

#include <sstream>
#include <string>

namespace heroespath
{
namespace stats
{

    TraitSet::TraitSet(const TraitValueVec_t & TRAITS_VEC)
        : traitVec_()
    {
        traitVec_.resize(static_cast<std::size_t>(Traits::Count));

        for (auto const & NEXT_TRAIT_PAIR : TRAITS_VEC)
        {
            traitVec_[NEXT_TRAIT_PAIR.first].CurrAndNormSet(NEXT_TRAIT_PAIR.second);
        }
    }

    Trait & TraitSet::Get(const Traits::Enum E)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (E < Traits::Count), "stats::TraitSet::Get(" << E << ")_InvalidValueError");

        return traitVec_[static_cast<std::size_t>(E)];
    }

    const Trait & TraitSet::GetCopy(const Traits::Enum E) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (E < Traits::Count), "stats::TraitSet::GetCopy(" << E << ")_InvalidValueError");

        return traitVec_[static_cast<std::size_t>(E)];
    }

    const std::string TraitSet::ToString(
        const bool WILL_WRAP,
        const bool WILL_ABBR,
        const bool WILL_PREVENT_NEGATIVE,
        const bool WILL_PREFIX_PERCENT) const
    {
        std::ostringstream ss;

        for (int i(0); i < Traits::Count; ++i)
        {
            auto const NEXT_ENUM{ static_cast<Traits::Enum>(i) };
            auto const NEXT_CURR{ traitVec_[static_cast<std::size_t>(i)].Current() };

            if (NEXT_CURR != 0)
            {
                if ((NEXT_CURR < 0) && WILL_PREVENT_NEGATIVE)
                {
                    continue;
                }

                if (ss.str().empty() == false)
                {
                    ss << ", ";
                }

                if (WILL_ABBR)
                {
                    ss << Traits::Abbr(NEXT_ENUM);
                }
                else
                {
                    ss << Traits::Name(NEXT_ENUM);
                }

                ss << " " << NEXT_CURR;

                if (WILL_PREFIX_PERCENT)
                {
                    ss << "%";
                }
            }
        }

        auto const RESULT_STR{ ss.str() };
        if ((RESULT_STR.empty() == false) && WILL_WRAP)
        {
            return "(" + RESULT_STR + ")";
        }
        else
        {
            return RESULT_STR;
        }
    }

    const std::string TraitSet::StatsString(const bool WILL_WRAP) const
    {
        std::ostringstream ss;

        ss << StatStringHelper(stats::Traits::Strength, false)
           << StatStringHelper(stats::Traits::Accuracy) << StatStringHelper(stats::Traits::Charm)
           << StatStringHelper(stats::Traits::Luck) << StatStringHelper(stats::Traits::Speed)
           << StatStringHelper(stats::Traits::Intelligence);

        if (WILL_WRAP)
        {
            return "(" + ss.str() + ")";
        }
        else
        {
            return ss.str();
        }
    }

    const std::string
        TraitSet::StatStringHelper(const stats::Traits::Enum E, const bool WILL_PREFIX) const
    {
        std::ostringstream ss;

        if (WILL_PREFIX)
        {
            ss << ", ";
        }

        auto const TRAIT{ traitVec_[static_cast<std::size_t>(E)] };

        ss << stats::Traits::Abbr(E) << "=";

        if (TRAIT.Current() != TRAIT.Normal())
        {
            ss << TRAIT.Current() << "/";
        }

        ss << TRAIT.Normal();

        return ss.str();
    }
} // namespace stats
} // namespace heroespath
