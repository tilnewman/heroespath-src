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
#ifndef GAME_STATS_TRAIT_HPP_INCLUDED
#define GAME_STATS_TRAIT_HPP_INCLUDED
//
// trait.hpp
//
//  Maintains a current and normal value.  Sometimes the numbers are interpreted as
//  percent bonus values, so ...Ratio() functions are provided.  Initially (and typically)
//  the current_ value stays at normal_, until something temorary changes it to be
//  something other than normal.  Eventually, the temporary condition will expire
//  and the current will return to 'normal'.
//
#include "game/stats/types.hpp"
#include "game/stats/trait-enum.hpp"

#include "misc/real.hpp"
#include "misc/boost-serialize-includes.hpp"

#include <tuple>
#include <string>
#include <vector>


namespace game
{
namespace stats
{

    //Maintains a creature variable or 'trait' as a current and normal value.
    //This class is also used to store trait percent bonuses.
    class Trait
    {
    public:
        Trait(const Traits::Enum TYPE   = Traits::Count,
              const stats::Trait_t      NORMAL = 0);

        inline Traits::Enum Which() const           { return type_; }
        inline const std::string Name() const       { return Traits::Name(type_); }
        inline const std::string Desc() const       { return Traits::Desc(type_); }
        inline stats::Trait_t Normal() const               { return normal_; }
        inline stats::Trait_t Current() const              { return current_; }
        
        inline float NormalRatio() const    { return static_cast<float>(current_) / 100.0f; }
        inline float CurrentRatio() const   { return static_cast<float>(normal_) / 100.0f; }

        inline bool ResetCurrentToNormal() const    { return (current_ == normal_); }

        inline void NormalAdj(const stats::Trait_t X)      { normal_ += X; }
        inline void CurrentAdj(const stats::Trait_t X)     { current_ += X; }
        inline void CurrAndNormAdj(const stats::Trait_t X) { NormalAdj(X); CurrentAdj(X); }

        inline void NormalSet(const stats::Trait_t X)      { normal_ = X; }
        inline void CurrentSet(const stats::Trait_t X)     { current_ = X; }
        inline void CurrAndNormSet(const stats::Trait_t X) { NormalSet(X); CurrentSet(X); }

        inline void SetCurrentToNormal()            { current_ = normal_; }

        //the given Trait's current value is used to modify
        inline void NormalAdj(const Trait RS)       { normal_ += RS.current_; }
        inline void CurrentAdj(const Trait RS)      { current_ += RS.current_; }

        const std::string ToString(const bool WILL_PREPEND_PLUS = true) const;

        friend bool operator==(const Trait & L, const Trait & R);
        friend bool operator<(const Trait & L, const Trait & R);

    public:
        static const stats::Trait_t STAT_MAX_INITIAL_ = 20;
        static const stats::Trait_t STAT_MAX_ESTIMATED_ = 100;

    private:
        Traits::Enum type_;
        stats::Trait_t normal_;
        stats::Trait_t current_;

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

    using TraitVec_t = std::vector<Trait>;


    inline bool operator==(const Trait & L, const Trait & R)
    {
        return std::tie(L.type_, L.normal_, L.current_)
                ==
               std::tie(R.type_, R.normal_, R.current_);
    }


    inline bool operator!=(const Trait & L, const Trait & R)
    {
        return ! (L == R);
    }


    inline bool operator<(const Trait & L, const Trait & R)
    {
        return std::tie(L.type_, L.normal_, L.current_)
                <
               std::tie(R.type_, R.normal_, R.current_);
    }

}
}

#endif //GAME_STATS_TRAIT_HPP_INCLUDED
