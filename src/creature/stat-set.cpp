// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// stat-set.cpp
//
#include "stat-set.hpp"

#include "misc/log-macros.hpp"

namespace heroespath
{
namespace creature
{

    StatSet::StatSet(
        const Strength_t & STR,
        const Accuracy_t & ACC,
        const Charm_t & CHA,
        const Luck_t & LCK,
        const Speed_t & SPD,
        const Intel_t & INT)
        : str_(STR)
        , acc_(ACC)
        , cha_(CHA)
        , lck_(LCK)
        , spd_(SPD)
        , int_(INT)
    {}

    Trait_t StatSet::Get(const Traits::Enum ENUM) const
    {
        if (ENUM == Traits::Strength)
        {
            return str_.Get();
        }
        if (ENUM == Traits::Accuracy)
        {
            return acc_.Get();
        }
        if (ENUM == Traits::Charm)
        {
            return cha_.Get();
        }
        if (ENUM == Traits::Luck)
        {
            return lck_.Get();
        }
        if (ENUM == Traits::Speed)
        {
            return spd_.Get();
        }
        if (ENUM == Traits::Intelligence)
        {
            return int_.Get();
        }

        M_HP_LOG_ERR(
            "StatSet::Get(trait_enum=" << NAMEOF_ENUM(ENUM) << ") but that value is invalid.");

        return 0;
    }

    void StatSet::Set(const Traits::Enum ENUM, const Trait_t NEW_VALUE)
    {
        if (ENUM == Traits::Strength)
        {
            str_ = Strength_t::Make(NEW_VALUE);
            return;
        }
        if (ENUM == Traits::Accuracy)
        {
            acc_ = Accuracy_t::Make(NEW_VALUE);
            return;
        }
        if (ENUM == Traits::Charm)
        {
            cha_ = Charm_t::Make(NEW_VALUE);
            return;
        }
        if (ENUM == Traits::Luck)
        {
            lck_ = Luck_t::Make(NEW_VALUE);
            return;
        }
        if (ENUM == Traits::Speed)
        {
            spd_ = Speed_t::Make(NEW_VALUE);
            return;
        }
        if (ENUM == Traits::Intelligence)
        {
            int_ = Intel_t::Make(NEW_VALUE);
            return;
        }

        M_HP_LOG_ERR(
            "StatSet::Set(" << ENUM << ", new_value=" << NEW_VALUE
                            << ") but that trait value is invalid.");
    }

    const std::string StatSet::ToString(const bool WILL_WRAP) const
    {
        std::string str;
        str.reserve(16);

        if (WILL_WRAP)
        {
            str += '(';
        }

        auto appendStatIfNeeded = [&](const auto & STAT, const std::string & NAME) {
            if (!STAT.IsZero())
            {
                str += NAME + ((STAT.Get() > 0) ? " +" : " ") + STAT.ToString();
            }
        };

        appendStatIfNeeded(str_, "Str");
        appendStatIfNeeded(acc_, "Acc");
        appendStatIfNeeded(cha_, "Cha");
        appendStatIfNeeded(lck_, "Lck");
        appendStatIfNeeded(spd_, "Spd");
        appendStatIfNeeded(int_, "Int");

        if (WILL_WRAP)
        {
            str += ')';
        }

        return str;
    }

} // namespace creature
} // namespace heroespath
