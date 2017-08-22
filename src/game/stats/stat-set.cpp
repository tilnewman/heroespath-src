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

#include <string>
#include <sstream>
#include <exception>


namespace game
{
namespace stats
{

    StatSet::StatSet(const stats::Trait_t STR,
                     const stats::Trait_t ACC,
                     const stats::Trait_t CHA,
                     const stats::Trait_t LCK,
                     const stats::Trait_t SPD,
                     const stats::Trait_t INT)
    :
        str_(STR),
        acc_(ACC),
        cha_(CHA),
        lck_(LCK),
        spd_(SPD),
        int_(INT)
    {}


    stats::Trait_t StatSet::Get(const stats::Traits::Enum E) const
    {
        if (E == Traits::Strength)      { return str_; }
        if (E == Traits::Accuracy)      { return acc_; }
        if (E == Traits::Charm)         { return cha_; }
        if (E == Traits::Luck)          { return lck_; }
        if (E == Traits::Speed)         { return spd_; }
        if (E == Traits::Intelligence)  { return int_; }

        std::ostringstream ss;
        ss << "game::stats::StatSet::Get(" << E << ")_InvalidValueError.";
        throw std::range_error(ss.str());
    }


    void StatSet::Set(const stats::Traits::Enum E, const stats::Trait_t NEW_VALUE)
    {
        if (E == Traits::Strength)      { str_ = NEW_VALUE; return; }
        if (E == Traits::Accuracy)      { acc_ = NEW_VALUE; return; }
        if (E == Traits::Charm)         { cha_ = NEW_VALUE; return; }
        if (E == Traits::Luck)          { lck_ = NEW_VALUE; return; }
        if (E == Traits::Speed)         { spd_ = NEW_VALUE; return; }
        if (E == Traits::Intelligence)  { int_ = NEW_VALUE; return; }

        std::ostringstream ss;

        ss << "game::stats::StatSet::Set(" << E << ", new_value=" << NEW_VALUE
            << ")_InvalidValueError.";

        throw std::range_error(ss.str());
    }


    const std::string StatSet::ToString(const bool WILL_WRAP) const
    {
        std::ostringstream ss;

        if (str_ != 0)
        {
            ss << "Str " << ((str_ > 0) ? "+" : "") << str_;
        }

        if (acc_ != 0)
        {
            ss << "Acc " << ((acc_ > 0) ? "+" : "") << acc_;
        }

        if (cha_ != 0)
        {
            ss << "Cha " << ((cha_ > 0) ? "+" : "") << cha_;
        }

        if (lck_ != 0)
        {
            ss << "Lck " << ((lck_ > 0) ? "+" : "") << lck_;
        }

        if (spd_ != 0)
        {
            ss << "Spd " << ((spd_ > 0) ? "+" : "") << spd_;
        }

        if (int_ != 0)
        {
            ss << "Int " << ((int_ > 0) ? "+" : "") << int_;
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

}
}
