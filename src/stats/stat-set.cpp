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
// stat-set.cpp
//
#include "stat-set.hpp"

#include <exception>
#include <sstream>
#include <string>

namespace heroespath
{
namespace stats
{

    StatSet::StatSet(
        const Strength_t & STR,
        const Accuracy_t & ACC,
        const Charm_t & CHA,
        const Luck_t & LCK,
        const Speed_t & SPD,
        const Intell_t & INT)
        : str_(STR)
        , acc_(ACC)
        , cha_(CHA)
        , lck_(LCK)
        , spd_(SPD)
        , int_(INT)
    {}

    stats::Trait_t StatSet::Get(const stats::Traits::Enum E) const
    {
        if (E == Traits::Strength)
        {
            return str_.Get();
        }
        if (E == Traits::Accuracy)
        {
            return acc_.Get();
        }
        if (E == Traits::Charm)
        {
            return cha_.Get();
        }
        if (E == Traits::Luck)
        {
            return lck_.Get();
        }
        if (E == Traits::Speed)
        {
            return spd_.Get();
        }
        if (E == Traits::Intelligence)
        {
            return int_.Get();
        }

        std::ostringstream ss;
        ss << "stats::StatSet::Get(" << E << ")_InvalidValueError.";
        throw std::range_error(ss.str());
    }

    void StatSet::Set(const stats::Traits::Enum E, const stats::Trait_t NEW_VALUE)
    {
        if (E == Traits::Strength)
        {
            str_ = Strength_t(NEW_VALUE);
            return;
        }
        if (E == Traits::Accuracy)
        {
            acc_ = Accuracy_t(NEW_VALUE);
            return;
        }
        if (E == Traits::Charm)
        {
            cha_ = Charm_t(NEW_VALUE);
            return;
        }
        if (E == Traits::Luck)
        {
            lck_ = Luck_t(NEW_VALUE);
            return;
        }
        if (E == Traits::Speed)
        {
            spd_ = Speed_t(NEW_VALUE);
            return;
        }
        if (E == Traits::Intelligence)
        {
            int_ = Intell_t(NEW_VALUE);
            return;
        }

        std::ostringstream ss;

        ss << "stats::StatSet::Set(" << E << ", new_value=" << NEW_VALUE << ")_InvalidValueError.";

        throw std::range_error(ss.str());
    }

    const std::string StatSet::ToString(const bool WILL_WRAP) const
    {
        std::ostringstream ss;

        if (str_.IsNonZero())
        {
            ss << "Str " << ((str_ > 0_str) ? "+" : "") << str_;
        }

        if (acc_.IsNonZero())
        {
            ss << "Acc " << ((acc_ > 0_acc) ? "+" : "") << acc_;
        }

        if (cha_.IsNonZero())
        {
            ss << "Cha " << ((cha_ > 0_cha) ? "+" : "") << cha_;
        }

        if (lck_.IsNonZero())
        {
            ss << "Lck " << ((lck_ > 0_lck) ? "+" : "") << lck_;
        }

        if (spd_.IsNonZero())
        {
            ss << "Spd " << ((spd_ > 0_spd) ? "+" : "") << spd_;
        }

        if (int_.IsNonZero())
        {
            ss << "Int " << ((int_ > 0_int) ? "+" : "") << int_;
        }

        if (WILL_WRAP)
        {
            return "(" + ss.str() + ")";
        }
        else
        {
            return ss.str();
        }
    }
} // namespace stats
} // namespace heroespath
