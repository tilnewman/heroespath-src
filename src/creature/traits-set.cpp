// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// traits-set.cpp
//
#include "traits-set.hpp"

#include "misc/assertlogandthrow.hpp"

#include <sstream>
#include <string>

namespace heroespath
{
namespace creature
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
            (Traits::IsValid(E)), "TraitSet::Get(" << E << ")_InvalidValueError");

        return traitVec_[static_cast<std::size_t>(E)];
    }

    const Trait & TraitSet::GetCopy(const Traits::Enum E) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (Traits::IsValid(E)), "TraitSet::GetCopy(" << E << ")_InvalidValueError");

        return traitVec_[static_cast<std::size_t>(E)];
    }

    const std::string TraitSet::ToString(
        const bool WILL_WRAP,
        const bool WILL_ABBR,
        const bool WILL_PREVENT_NEGATIVE,
        const bool WILL_PREFIX_PERCENT) const
    {
        std::ostringstream ss;

        for (misc::EnumUnderlying_t i(0); i < Traits::Count; ++i)
        {
            auto const NEXT_ENUM { static_cast<Traits::Enum>(i) };
            auto const NEXT_CURR { traitVec_[static_cast<std::size_t>(i)].Current() };

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

        auto const RESULT_STR { ss.str() };
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

        ss << StatStringHelper(Traits::Strength, false) << StatStringHelper(Traits::Accuracy)
           << StatStringHelper(Traits::Charm) << StatStringHelper(Traits::Luck)
           << StatStringHelper(Traits::Speed) << StatStringHelper(Traits::Intelligence);

        if (WILL_WRAP)
        {
            return "(" + ss.str() + ")";
        }
        else
        {
            return ss.str();
        }
    }

    const std::string TraitSet::StatStringHelper(const Traits::Enum E, const bool WILL_PREFIX) const
    {
        std::ostringstream ss;

        if (WILL_PREFIX)
        {
            ss << ", ";
        }

        auto const TRAIT { traitVec_[static_cast<std::size_t>(E)] };

        ss << Traits::Abbr(E) << "=";

        if (TRAIT.Current() != TRAIT.Normal())
        {
            ss << TRAIT.Current() << "/";
        }

        ss << TRAIT.Normal();

        return ss.str();
    }

} // namespace creature
} // namespace heroespath
