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
// stat-mult-set.cpp
//
#include "stat-mult-set.hpp"

#include "utilz/real.hpp"

#include <tuple>
#include <sstream>


namespace game
{
namespace stats
{

    StatMultSet::StatMultSet(const float STR,
                             const float ACC,
                             const float CHA,
                             const float LCK,
                             const float SPD,
                             const float INT)
    :
        str_(STR),
        acc_(ACC),
        cha_(CHA),
        lck_(LCK),
        spd_(SPD),
        int_(INT)
    {}


    float StatMultSet::Get(const stat::Enum E) const
    {
        switch (E)
        {
            case stat::Strength:     { return str_; }
            case stat::Accuracy:     { return acc_; }
            case stat::Charm:        { return cha_; }
            case stat::Luck:         { return lck_; }
            case stat::Speed:        { return spd_; }
            case stat::Intelligence: { return int_; }
            case stat::Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::stats::StatMultSet::Get(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    void StatMultSet::Invert()
    {
        if (utilz::IsRealZero(str_)) str_ = 1.0f; else str_ = 1.0f / str_;
        if (utilz::IsRealZero(acc_)) acc_ = 1.0f; else acc_ = 1.0f / acc_;
        if (utilz::IsRealZero(cha_)) cha_ = 1.0f; else cha_ = 1.0f / cha_;
        if (utilz::IsRealZero(lck_)) lck_ = 1.0f; else lck_ = 1.0f / lck_;
        if (utilz::IsRealZero(spd_)) spd_ = 1.0f; else spd_ = 1.0f / spd_;
        if (utilz::IsRealZero(int_)) int_ = 1.0f; else int_ = 1.0f / int_;
    }


    const std::string StatMultSet::ToString(const bool WILL_WRAP) const
    {
        std::ostringstream ss;

        ss << "Str=" << str_ << ", "
           << "Acc=" << acc_ << ", "
           << "Cha=" << cha_ << ", "
           << "Lck=" << lck_ << ", "
           << "Spd=" << spd_ << ", "
           << "Int=" << int_;

        if (WILL_WRAP)
        {
            return "(" + ss.str() + ")";
        }
        else
        {
            return ss.str();
        }
    }


    bool operator==(const StatMultSet & L, const StatMultSet & R)
    {
        return (utilz::IsRealClose(L.str_, R.str_) &&
                utilz::IsRealClose(L.acc_, R.acc_) &&
                utilz::IsRealClose(L.cha_, R.cha_) &&
                utilz::IsRealClose(L.lck_, R.lck_) &&
                utilz::IsRealClose(L.spd_, R.spd_) &&
                utilz::IsRealClose(L.int_, R.int_));
    }


    bool operator<(const StatMultSet & L, const StatMultSet & R)
    {
        return std::tie(L.str_, L.acc_, L.cha_, L.lck_, L.spd_, L.int_)
                <
               std::tie(R.str_, R.acc_, R.cha_, R.lck_, R.spd_, R.int_);
    }

}
}
