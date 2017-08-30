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
// treasure-info.cpp
//
#include "treasure-info.hpp"

#include <sstream>


namespace game
{
namespace item
{

    TreasureInfo::TreasureInfo(const stats::Trait_t COIN,
                               const stats::Trait_t GEM,
                               const stats::Trait_t MAGIC,
                               const stats::Trait_t RELIGIOUS)
    :
        coin_     (COIN),
        gem_      (GEM),
        magic_    (MAGIC),
        religious_(RELIGIOUS)
    {}


    const std::string TreasureInfo::ToString(const bool WILL_WRAP) const
    {
        std::ostringstream ss;

        ss << "Coin=" << coin_ << ", Gem=" << gem_ << ", Magic=" << magic_ << ", Religious=" << religious_;

        if (WILL_WRAP)
        {
            return "(" + ss.str() + ")";
        }
        else
        {
            return ss.str();
        }
    }


    TreasureInfo & TreasureInfo::operator=(const TreasureInfo & RHS)
    {
        if (this != & RHS)
        {
            coin_ = RHS.coin_;
            gem_ = RHS.gem_;
            magic_ = RHS.magic_;
            religious_ = RHS.religious_;
        }

        return * this;
    }


    TreasureInfo & TreasureInfo::operator+=(const TreasureInfo & RHS)
    {
        coin_ += RHS.coin_;
        gem_ += RHS.gem_;
        magic_ += RHS.magic_;
        religious_ += RHS.religious_;
        return * this;
    }


    TreasureInfo & TreasureInfo::operator-=(const TreasureInfo & RHS)
    {
        coin_ -= RHS.coin_;
        gem_ -= RHS.gem_;
        magic_ -= RHS.magic_;
        religious_ -= RHS.religious_;
        return *this;
    }


    TreasureInfo & TreasureInfo::operator*=(const TreasureInfo & RHS)
    {
        coin_ *= RHS.coin_;
        gem_ *= RHS.gem_;
        magic_ *= RHS.magic_;
        religious_ *= RHS.religious_;
        return * this;
    }


    TreasureInfo & TreasureInfo::operator/=(const TreasureInfo & RHS)
    {
        coin_ /= RHS.coin_;
        gem_ /= RHS.gem_;
        magic_ /= RHS.magic_;
        religious_ /= RHS.religious_;
        return * this;
    }


    TreasureInfo TreasureInfo::operator+(const TreasureInfo & RHS)
    {
        return TreasureInfo( * this) += RHS;
    }


    TreasureInfo TreasureInfo::operator-(const TreasureInfo & RHS)
    {
        return TreasureInfo( * this) -= RHS;
    }


    TreasureInfo TreasureInfo::operator*(const TreasureInfo & RHS)
    {
        return TreasureInfo( * this) *= RHS;
    }


    TreasureInfo TreasureInfo::operator/(const TreasureInfo & RHS)
    {
        return TreasureInfo( * this) /= RHS;
    }

}
}