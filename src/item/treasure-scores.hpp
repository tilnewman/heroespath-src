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
#ifndef HEROESPATH_ITEM_TREASURESCORES_HPP_INCLUDED
#define HEROESPATH_ITEM_TREASURESCORES_HPP_INCLUDED
//
// treasure-scores.hpp
//
#include "misc/types.hpp"

#include <string>
#include <tuple>

namespace heroespath
{
namespace item
{

    // Describes the chance or the amount of treasure with integers from roughly [0, 400].
    class TreasureScores
    {
    public:
        explicit TreasureScores(
            const Score_t & COIN = 0_score,
            const Score_t & GEM = 0_score,
            const Score_t & MAGIC = 0_score,
            const Score_t & RELIGIOUS = 0_score);

        inline Score_t Coin() const { return coin_; }
        inline Score_t Gem() const { return gem_; }
        inline Score_t Magic() const { return magic_; }
        inline Score_t Religious() const { return religious_; }

        inline void Coin(const Score_t & SCORE) { coin_ = SCORE; }
        inline void Gem(const Score_t & SCORE) { gem_ = SCORE; }
        inline void Magic(const Score_t & SCORE) { magic_ = SCORE; }
        inline void Religious(const Score_t & SCORE) { religious_ = SCORE; }

        inline Score_t AdjustCoin(const Score_t & SCORE) { return coin_ += SCORE; }
        inline Score_t AdjustGem(const Score_t & SCORE) { return gem_ += SCORE; }
        inline Score_t AdjustMagic(const Score_t & SCORE) { return magic_ += SCORE; }
        inline Score_t AdjustReligious(const Score_t & SCORE) { return religious_ += SCORE; }

        inline bool IsEmpty() const
        {
            return (
                (0_score == coin_) && (0_score == gem_) && (0_score == magic_)
                && (0_score == religious_));
        }

        TreasureScores & operator+=(const TreasureScores &);
        TreasureScores & operator-=(const TreasureScores &);
        TreasureScores & operator*=(const TreasureScores &);
        TreasureScores & operator/=(const TreasureScores &);
        TreasureScores operator+(const TreasureScores &);
        TreasureScores operator-(const TreasureScores &);
        TreasureScores operator*(const TreasureScores &);
        TreasureScores operator/(const TreasureScores &);

        const std::string ToString(const bool WILL_WRAP = false) const;

        friend bool operator==(const TreasureScores & L, const TreasureScores & R);
        friend bool operator<(const TreasureScores & L, const TreasureScores & R);

    private:
        Score_t coin_;
        Score_t gem_;
        Score_t magic_;
        Score_t religious_;
    };

    inline bool operator==(const TreasureScores & L, const TreasureScores & R)
    {
        return (
            std::tie(L.coin_, L.gem_, L.magic_, L.religious_)
            == std::tie(R.coin_, L.gem_, R.magic_, R.religious_));
    }

    inline bool operator!=(const TreasureScores & L, const TreasureScores & R) { return !(L == R); }

    inline bool operator<(const TreasureScores & L, const TreasureScores & R)
    {
        return (
            std::tie(L.coin_, L.gem_, L.magic_, L.religious_)
            < std::tie(R.coin_, R.gem_, R.magic_, R.religious_));
    }
} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_TREASUREINFO_HPP_INCLUDED
