// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_TREASURESCORES_HPP_INCLUDED
#define HEROESPATH_ITEM_TREASURESCORES_HPP_INCLUDED
//
// treasure-scores.hpp
//
#include "game/strong-types.hpp"

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

        Score_t Coin() const { return coin_; }
        Score_t Gem() const { return gem_; }
        Score_t Magic() const { return magic_; }
        Score_t Religious() const { return religious_; }

        void Coin(const Score_t & SCORE) { coin_ = SCORE; }
        void Gem(const Score_t & SCORE) { gem_ = SCORE; }
        void Magic(const Score_t & SCORE) { magic_ = SCORE; }
        void Religious(const Score_t & SCORE) { religious_ = SCORE; }

        Score_t AdjustCoin(const Score_t & SCORE) { return coin_ += SCORE; }
        Score_t AdjustGem(const Score_t & SCORE) { return gem_ += SCORE; }
        Score_t AdjustMagic(const Score_t & SCORE) { return magic_ += SCORE; }
        Score_t AdjustReligious(const Score_t & SCORE) { return religious_ += SCORE; }

        bool IsEmpty() const
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
