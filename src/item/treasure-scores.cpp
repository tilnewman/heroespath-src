// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// treasure-scores.cpp
//
#include "treasure-scores.hpp"

#include <sstream>

namespace heroespath
{
namespace item
{

    TreasureScores::TreasureScores(
        const Score_t & COIN, const Score_t & GEM, const Score_t & MAGIC, const Score_t & RELIGIOUS)
        : coin_(COIN)
        , gem_(GEM)
        , magic_(MAGIC)
        , religious_(RELIGIOUS)
    {}

    const std::string TreasureScores::ToString(const bool WILL_WRAP) const
    {
        std::ostringstream ss;

        ss << "Coin=" << coin_ << ", Gem=" << gem_ << ", Magic=" << magic_
           << ", Religious=" << religious_;

        if (WILL_WRAP)
        {
            return "(" + ss.str() + ")";
        }
        else
        {
            return ss.str();
        }
    }

    TreasureScores & TreasureScores::operator+=(const TreasureScores & RHS)
    {
        coin_ += RHS.coin_;
        gem_ += RHS.gem_;
        magic_ += RHS.magic_;
        religious_ += RHS.religious_;
        return *this;
    }

    TreasureScores & TreasureScores::operator-=(const TreasureScores & RHS)
    {
        coin_ -= RHS.coin_;
        gem_ -= RHS.gem_;
        magic_ -= RHS.magic_;
        religious_ -= RHS.religious_;
        return *this;
    }

    TreasureScores & TreasureScores::operator*=(const TreasureScores & RHS)
    {
        coin_ *= RHS.coin_;
        gem_ *= RHS.gem_;
        magic_ *= RHS.magic_;
        religious_ *= RHS.religious_;
        return *this;
    }

    TreasureScores & TreasureScores::operator/=(const TreasureScores & RHS)
    {
        coin_ /= RHS.coin_;
        gem_ /= RHS.gem_;
        magic_ /= RHS.magic_;
        religious_ /= RHS.religious_;
        return *this;
    }

    TreasureScores TreasureScores::operator+(const TreasureScores & RHS)
    {
        return TreasureScores(*this) += RHS;
    }

    TreasureScores TreasureScores::operator-(const TreasureScores & RHS)
    {
        return TreasureScores(*this) -= RHS;
    }

    TreasureScores TreasureScores::operator*(const TreasureScores & RHS)
    {
        return TreasureScores(*this) *= RHS;
    }

    TreasureScores TreasureScores::operator/(const TreasureScores & RHS)
    {
        return TreasureScores(*this) /= RHS;
    }
} // namespace item
} // namespace heroespath
