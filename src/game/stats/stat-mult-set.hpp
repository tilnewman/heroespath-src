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
#ifndef GAME_STATS_STATMULTSET_HPP_INCLUDED
#define GAME_STATS_STATMULTSET_HPP_INCLUDED
//
// stat-mult-set.hpp
//
#include "game/stats/stat-enum.hpp"
#include "game/stats/types.hpp"

#include "misc/boost-serialize-includes.hpp"

#include <string>


namespace game
{
namespace stats
{

    class StatSet;


    //Stores modifications to stats as multiples for each.
    class StatMultSet
    {
    public:
        explicit StatMultSet(const float STR = 1.0f,
                             const float ACC = 1.0f,
                             const float CHA = 1.0f,
                             const float LCK = 1.0f,
                             const float SPD = 1.0f,
                             const float INT = 1.0f);

        inline float Str() const { return str_; }
        inline float Acc() const { return acc_; }
        inline float Cha() const { return cha_; }
        inline float Lck() const { return lck_; }
        inline float Spd() const { return spd_; }
        inline float Int() const { return int_; }

        float Get(const stat::Enum) const;

        void Invert();

        const std::string ToString(const bool WILL_WRAP = false) const;

        const std::string ToStringDesc(const stats::StatSet * STAT_SET_PTR) const;

        friend bool operator==(const StatMultSet & L, const StatMultSet & R);
        friend bool operator<(const StatMultSet & L, const StatMultSet & R);

    private:
        const std::string ToStringHelper(const std::string & NAME,
                                         const Stat_t        VALUE,
                                         const float         MULT,
                                         const bool          WILL_PREFIX = true) const;

    private:
        float str_;
        float acc_;
        float cha_;
        float lck_;
        float spd_;
        float int_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & str_;
            ar & acc_;
            ar & cha_;
            ar & lck_;
            ar & spd_;
            ar & int_;
        }
    };


    bool operator==(const StatMultSet & L, const StatMultSet & R);


    inline bool operator!=(const StatMultSet & L, const StatMultSet & R)
    {
        return ! (L == R);
    }


    bool operator<(const StatMultSet & L, const StatMultSet & R);

}
}

#endif //GAME_STATS_STATMULTSET_HPP_INCLUDED
