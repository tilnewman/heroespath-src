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
#include "misc/enum-util.hpp"

#include <string>

namespace heroespath
{
namespace creature
{

    TraitSet::TraitSet(const TraitValueVec_t & TRAITS_VEC)
        : traitVec_()
    {
        traitVec_.resize(static_cast<std::size_t>(Traits::Count));

        for (const auto & NEXT_TRAIT_PAIR : TRAITS_VEC)
        {
            traitVec_[NEXT_TRAIT_PAIR.first].CurrAndNormSet(NEXT_TRAIT_PAIR.second);
        }
    }

    Trait & TraitSet::Get(const Traits::Enum ENUM)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (EnumUtil<Traits>::IsValid(ENUM)),
            "TraitSet::Get(trait_enum=" << NAMEOF_ENUM(ENUM)
                                        << ") but that trait value is invalid.");

        return traitVec_[static_cast<std::size_t>(ENUM)];
    }

    const Trait & TraitSet::GetCopy(const Traits::Enum ENUM) const
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (EnumUtil<Traits>::IsValid(ENUM)),
            "TraitSet::GetCopy(" << NAMEOF_ENUM(ENUM) << ") but that trait value is invalid.");

        return traitVec_[static_cast<std::size_t>(ENUM)];
    }

    const std::string TraitSet::ToString(
        const bool WILL_WRAP,
        const bool WILL_ABBR,
        const bool WILL_PREVENT_NEGATIVE,
        const bool WILL_PREFIX_PERCENT) const
    {
        std::string str;
        str.reserve(128);

        if (WILL_WRAP)
        {
            str += '(';
        }

        for (EnumUnderlying_t i(0); i < Traits::Count; ++i)
        {
            const auto NEXT_ENUM { static_cast<Traits::Enum>(i) };
            const auto NEXT_CURR { traitVec_[static_cast<std::size_t>(i)].Current() };

            if (NEXT_CURR != 0)
            {
                if ((NEXT_CURR < 0) && WILL_PREVENT_NEGATIVE)
                {
                    continue;
                }

                if (!str.empty())
                {
                    str += ", ";
                }

                if (WILL_ABBR)
                {
                    str += Traits::Abbr(NEXT_ENUM);
                }
                else
                {
                    str += Traits::Name(NEXT_ENUM);
                }

                str += ' ';
                str += std::to_string(NEXT_CURR);

                if (WILL_PREFIX_PERCENT)
                {
                    str += '%';
                }
            }
        }

        if (str.compare("(") == 0)
        {
            str.clear();
        }
        else if (WILL_WRAP)
        {
            str += ')';
        }

        return str;
    }

    const std::string TraitSet::StatsString(const bool WILL_WRAP) const
    {
        std::string str;
        str.reserve(128);

        if (WILL_WRAP)
        {
            str += '(';
        }

        str += StatStringHelper(Traits::Strength, false);
        str += StatStringHelper(Traits::Accuracy);
        str += StatStringHelper(Traits::Charm);
        str += StatStringHelper(Traits::Luck);
        str += StatStringHelper(Traits::Speed);
        str += StatStringHelper(Traits::Intelligence);

        if (WILL_WRAP)
        {
            str += ')';
        }

        return str;
    }

    const std::string
        TraitSet::StatStringHelper(const Traits::Enum ENUM, const bool WILL_PREFIX) const
    {
        std::string str;
        str.reserve(8);

        if (WILL_PREFIX)
        {
            str += ", ";
        }

        str += Traits::Abbr(ENUM);
        str += '=';

        const auto & TRAIT { traitVec_[static_cast<std::size_t>(ENUM)] };

        if (TRAIT.Current() != TRAIT.Normal())
        {
            str += std::to_string(TRAIT.Current());
            str += '/';
        }

        str += std::to_string(TRAIT.Normal());

        return str;
    }

    Score_t TraitSet::Score() const
    {
        auto score { 0_score };

        for (EnumUnderlying_t i(0); i < creature::Traits::Count; ++i)
        {
            const auto NEXT_TRAIT_ENUM { static_cast<creature::Traits::Enum>(i) };
            const auto NEXT_TRAIT_VALUE { GetCopy(NEXT_TRAIT_ENUM).Current() };

            if (NEXT_TRAIT_VALUE == 0)
            {
                continue;
            }

            auto traitScore { [NEXT_TRAIT_VALUE]() {
                const auto MULTIPLIER { ((NEXT_TRAIT_VALUE >= 0) ? 10 : 5) };
                const auto NEXT_TRAIT_VALUE_ABS { misc::Abs(NEXT_TRAIT_VALUE) };

                const Trait_t MAX_VALUE_BEFORE_REDUCTION = 100;
                if (NEXT_TRAIT_VALUE_ABS < MAX_VALUE_BEFORE_REDUCTION)
                {
                    return NEXT_TRAIT_VALUE_ABS * MULTIPLIER;
                }
                else
                {
                    double traitReduced { static_cast<double>(MAX_VALUE_BEFORE_REDUCTION) };

                    traitReduced += std::sqrt(
                        static_cast<double>(NEXT_TRAIT_VALUE_ABS)
                        - static_cast<double>(MAX_VALUE_BEFORE_REDUCTION));

                    return static_cast<int>(traitReduced) * MULTIPLIER;
                }
            }() };

            if ((NEXT_TRAIT_ENUM == creature::Traits::HealthGainAll)
                || (NEXT_TRAIT_ENUM == creature::Traits::HealthGainMelee))
            {
                traitScore *= 10;
            }
            else if (
                (NEXT_TRAIT_ENUM == creature::Traits::AnimalResist)
                || (NEXT_TRAIT_ENUM == creature::Traits::ArmorRating)
                || (NEXT_TRAIT_ENUM == creature::Traits::Backstab)
                || (NEXT_TRAIT_ENUM == creature::Traits::CurseOnDamage)
                || (NEXT_TRAIT_ENUM == creature::Traits::DamageBonusAll)
                || (NEXT_TRAIT_ENUM == creature::Traits::DamageBonusMelee)
                || (NEXT_TRAIT_ENUM == creature::Traits::DamageBonusProj)
                || (NEXT_TRAIT_ENUM == creature::Traits::FindCoinsAmount)
                || (NEXT_TRAIT_ENUM == creature::Traits::PoisonOnAll)
                || (NEXT_TRAIT_ENUM == creature::Traits::PoisonOnMelee))
            {
                traitScore *= 2;
            }
            else if (NEXT_TRAIT_ENUM == creature::Traits::DamageBonusFist)
            {
                traitScore /= 4;
            }

            score += Score_t::Make(traitScore);
        }

        return score;
    }

} // namespace creature
} // namespace heroespath
