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
#ifndef GAME_ITEM_TREASUREINFO_HPP_INCLUDED
#define GAME_ITEM_TREASUREINFO_HPP_INCLUDED
//
// treasure-info.hpp
//
#include "game/stats/types.hpp"

#include <tuple>
#include <string>


namespace game
{
namespace item
{

    //Describes the chance or the amount of treasure with integers from roughly [0, 300].
    class TreasureInfo
    {
    public:
        explicit TreasureInfo(const stats::Trait_t COIN      = 0,
                              const stats::Trait_t GEM       = 0,
                              const stats::Trait_t MAGIC     = 0,
                              const stats::Trait_t RELIGIOUS = 0);

        inline stats::Trait_t Coin() const              { return coin_; }
        inline stats::Trait_t Gem() const               { return gem_; }
        inline stats::Trait_t Magic() const             { return magic_; }
        inline stats::Trait_t Religious() const         { return religious_; }

        inline void Coin(const stats::Trait_t X)        { coin_ = X; }
        inline void Gem(const stats::Trait_t X)         { gem_ = X; }
        inline void Magic(const stats::Trait_t X)       { magic_ = X; }
        inline void Religious(const stats::Trait_t X)   { religious_ = X; }

        inline stats::Trait_t AdjustCoin(const stats::Trait_t X)        { return coin_ += X; }
        inline stats::Trait_t AdjustGem(const stats::Trait_t X)         { return gem_ += X; }
        inline stats::Trait_t AdjustMagic(const stats::Trait_t X)       { return magic_ += X; }
        inline stats::Trait_t AdjustReligious(const stats::Trait_t X)   { return religious_ += X; }

        TreasureInfo & operator=(const TreasureInfo &);
        TreasureInfo & operator+=(const TreasureInfo &);
        TreasureInfo & operator-=(const TreasureInfo &);
        TreasureInfo & operator*=(const TreasureInfo &);
        TreasureInfo & operator/=(const TreasureInfo &);
        TreasureInfo operator+(const TreasureInfo &);
        TreasureInfo operator-(const TreasureInfo &);
        TreasureInfo operator*(const TreasureInfo &);
        TreasureInfo operator/(const TreasureInfo &);

        const std::string ToString(const bool WILL_WRAP = false) const;

        friend bool operator==(const TreasureInfo & L, const TreasureInfo & R);
        friend bool operator<(const TreasureInfo & L, const TreasureInfo & R);

    private:
        stats::Trait_t coin_;
        stats::Trait_t gem_;
        stats::Trait_t magic_;
        stats::Trait_t religious_;
    };


    inline bool operator==(const TreasureInfo & L, const TreasureInfo & R)
    {
        return (std::tie(L.coin_, L.gem_, L.magic_, L.religious_)
                ==
                std::tie(R.coin_, L.gem_, R.magic_, R.religious_));
    }


    inline bool operator!=(const TreasureInfo & L, const TreasureInfo & R)
    {
        return ! (L == R);
    }


    inline bool operator<(const TreasureInfo & L, const TreasureInfo & R)
    {
        return (std::tie(L.coin_, L.gem_, L.magic_, L.religious_)
                <
                std::tie(R.coin_, R.gem_, R.magic_, R.religious_));
    }

}
}

#endif //GAME_ITEM_TREASUREINFO_HPP_INCLUDED
