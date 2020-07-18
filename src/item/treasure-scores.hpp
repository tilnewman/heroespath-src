// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_TREASURE_SCORE_SET_HPP_INCLUDED
#define HEROESPATH_ITEM_TREASURE_SCORE_SET_HPP_INCLUDED
//
// treasure-score-set.hpp
//
#include "game/strong-types.hpp"

#include <ostream>
#include <string>
#include <tuple>

namespace heroespath
{
namespace item
{

    // Describes the chance or the amount of treasure with integers from roughly [0, 400].
    struct TreasureScores
    {
        constexpr explicit TreasureScores(
            const Score_t & COIN = 0_score,
            const Score_t & GEM = 0_score,
            const Score_t & MAGIC = 0_score,
            const Score_t & RELIGIOUS = 0_score) noexcept
            : coin(COIN)
            , gem(GEM)
            , magic(MAGIC)
            , religious(RELIGIOUS)
        {}

        constexpr TreasureScores(const TreasureScores &) noexcept = default;
        constexpr TreasureScores(TreasureScores &&) noexcept = default;
        constexpr TreasureScores & operator=(const TreasureScores &) noexcept = default;
        constexpr TreasureScores & operator=(TreasureScores &&) noexcept = default;

        constexpr bool IsEmpty() const noexcept
        {
            return !(!coin.IsZero() || !gem.IsZero() || !magic.IsZero() | !religious.IsZero());
        }

        const std::string ToString(const bool WILL_WRAP = false) const
        {
            std::string str;
            str.reserve(32);

            if (WILL_WRAP)
            {
                str += '(';
            }

            str += "Coin=";
            str += coin.ToString();
            str += ", Gem=";
            str += gem.ToString();
            str += ", Magic=";
            str += magic.ToString();
            str += ", Religious=";
            str += religious.ToString();

            if (WILL_WRAP)
            {
                str += ')';
            }

            return str;
        }

        constexpr TreasureScores & operator+=(const TreasureScores & RHS) noexcept
        {
            coin += RHS.coin;
            gem += RHS.gem;
            magic += RHS.magic;
            religious += RHS.religious;
            return *this;
        }

        constexpr TreasureScores & operator-=(const TreasureScores & RHS) noexcept
        {
            coin -= RHS.coin;
            gem -= RHS.gem;
            magic -= RHS.magic;
            religious -= RHS.religious;
            return *this;
        }

        constexpr TreasureScores & operator*=(const TreasureScores & RHS) noexcept
        {
            coin *= RHS.coin;
            gem *= RHS.gem;
            magic *= RHS.magic;
            religious *= RHS.religious;
            return *this;
        }

        constexpr TreasureScores & operator/=(const TreasureScores & RHS) noexcept
        {
            coin /= RHS.coin;
            gem /= RHS.gem;
            magic /= RHS.magic;
            religious /= RHS.religious;
            return *this;
        }

        Score_t coin;
        Score_t gem;
        Score_t magic;
        Score_t religious;
    };

    //

    inline std::ostream & operator<<(std::ostream & os, const TreasureScores & R)
    {
        os << R.ToString();
        return os;
    }

    //

    constexpr bool operator==(const TreasureScores & L, const TreasureScores & R) noexcept
    {
        return !(
            (L.coin != R.coin) || (L.gem != R.gem) || (L.magic != R.magic)
            || (L.religious != R.religious));
    }

    constexpr bool operator!=(const TreasureScores & L, const TreasureScores & R) noexcept
    {
        return !(L == R);
    }

    constexpr bool operator<(const TreasureScores & L, const TreasureScores & R) noexcept
    {
        return (
            std::tie(L.coin, L.gem, L.magic, L.religious)
            < std::tie(R.coin, R.gem, R.magic, R.religious));
    }

    constexpr bool operator>(const TreasureScores & L, const TreasureScores & R) noexcept
    {
        return (R < L);
    }

    constexpr bool operator<=(const TreasureScores & L, const TreasureScores & R) noexcept
    {
        return !(L > R);
    }

    constexpr bool operator>=(const TreasureScores & L, const TreasureScores & R) noexcept
    {
        return !(L < R);
    }

    //

    constexpr TreasureScores operator+(const TreasureScores & L, const TreasureScores & R) noexcept
    {
        return (TreasureScores(L) += R);
    }

    constexpr TreasureScores operator-(const TreasureScores & L, const TreasureScores & R) noexcept
    {
        return (TreasureScores(L) -= R);
    }

    constexpr TreasureScores operator*(const TreasureScores & L, const TreasureScores & R) noexcept
    {
        return (TreasureScores(L) *= R);
    }

    constexpr TreasureScores operator/(const TreasureScores & L, const TreasureScores & R) noexcept
    {
        return (TreasureScores(L) /= R);
    }

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_TREASURE_SCORE_SET_HPP_INCLUDED
