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


namespace game
{
namespace stats
{

    TraitSet::TraitSet(const TraitValueVec_t & TRAITS_VEC)
    :
        statVec_()
    {
        statVec_.resize(static_cast<std::size_t>(Traits::Count));

        for (auto const & NEXT_TRAIT_PAIR : TRAITS_VEC)
        {
            statVec_[NEXT_TRAIT_PAIR.first].CurrAndNormSet(NEXT_TRAIT_PAIR.second);
        }
    }


    RatioStat & TraitSet::Get(const Traits::Enum E)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((E < Traits::Count), "game::stats::TraitSet::Get("
            << E << ")_InvalidValueError");

        return statVec_[E];
    }


    const RatioStat & TraitSet::GetCopy(const Traits::Enum E) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((E < Traits::Count), "game::stats::TraitSet::GetCopy("
            << E << ")_InvalidValueError");

        return statVec_[E];
    }


    const std::string TraitSet::ToString(const bool WILL_WRAP,
                                          const bool WILL_ABBR) const
    {
        std::ostringstream ss;

        for (std::size_t i(0); i < Traits::Count; ++i)
        {
            auto const NEXT_ENUM{ static_cast<Traits::Enum>(i) };
            auto const NEXT_CURR{ statVec_[i].Current() };

            if (NEXT_CURR != 0)
            {
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

                ss << "=" << NEXT_CURR;
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

}
}
