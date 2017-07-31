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
#ifndef GAME_STATS_RATIOSTAT_HPP_INCLUDED
#define GAME_STATS_RATIOSTAT_HPP_INCLUDED
//
// ratio-stat.hpp
//
#include "trait-enum.hpp"

#include "misc/real.hpp"
#include "misc/boost-serialize-includes.hpp"

#include <tuple>
#include <string>
#include <vector>


namespace game
{
namespace stats
{

    //Maintains a ratio current and normal value as a x100 int.
    //Initially (and typically) the current_ value stays at normal_,
    //until something temorary changes it to be something other than
    //normal.  Eventually, the temporary condition will expire and
    //the current will return to normal.
    class RatioStat
    {
    public:
        RatioStat(const Traits::Enum TYPE   = Traits::Count,
                  const int          NORMAL = 0);

        inline Traits::Enum Which() const           { return type_; }
        inline const std::string Name() const       { return Traits::Name(type_); }
        inline const std::string Desc() const       { return Traits::Desc(type_); }
        inline int Normal() const                   { return normal_; }
        inline int Current() const                  { return current_; }
        
        inline float NormalRatio() const    { return static_cast<float>(current_) / 100.0f; }
        inline float CurrentRatio() const   { return static_cast<float>(normal_) / 100.0f; }

        inline bool ResetCurrentToNormal() const    { return (current_ == normal_); }

        inline void NormalAdj(const int X)          { normal_ += X; }
        inline void CurrentAdj(const int X)         { current_ += X; }
        inline void CurrAndNormAdj(const int X)     { NormalAdj(X); CurrentAdj(X); }

        inline void NormalSet(const int X)          { normal_ = X; }
        inline void CurrentSet(const int X)         { current_ = X; }
        inline void CurrAndNormSet(const int X)     { NormalSet(X); CurrentSet(X); }

        inline void SetCurrentToNormal()            { current_ = normal_; }

        //the given RatioStat's current value is used to modify
        inline void NormalAdj(const RatioStat RS)   { normal_ += RS.current_; }
        inline void CurrentAdj(const RatioStat RS)  { current_ += RS.current_; }

        const std::string ToString(const bool WILL_PREPEND_PLUS = true) const;

        friend bool operator==(const RatioStat & L, const RatioStat & R);
        friend bool operator<(const RatioStat & L, const RatioStat & R);

    private:
        Traits::Enum type_;
        int normal_;
        int current_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & type_;
            ar & normal_;
            ar & current_;
        }
    };

    using RatioStatVec_t = std::vector<RatioStat>;


    inline bool operator==(const RatioStat & L, const RatioStat & R)
    {
        return std::tie(L.type_, L.normal_, L.current_)
                ==
               std::tie(R.type_, R.normal_, R.current_);
    }


    inline bool operator!=(const RatioStat & L, const RatioStat & R)
    {
        return ! (L == R);
    }


    inline bool operator<(const RatioStat & L, const RatioStat & R)
    {
        return std::tie(L.type_, L.normal_, L.current_)
                <
               std::tie(R.type_, R.normal_, R.current_);
    }

}
}

#endif //GAME_STATS_RATIOSTAT_HPP_INCLUDED
