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
#ifndef GAME_STATS_TRAITSSET_HPP_INCLUDED
#define GAME_STATS_TRAITSSET_HPP_INCLUDED
//
// traits-set.hpp
//
#include "trait-enum.hpp"
#include "ratio-stat.hpp"

#include "misc/boost-serialize-includes.hpp"

#include <vector>


namespace game
{
namespace stats
{

    class TraitSet
    {
    public:
        TraitSet(const TraitValueVec_t & TRAITS_VEC = TraitValueVec_t());

        RatioStat & Get(const Traits::Enum E);
        const RatioStat & GetCopy(const Traits::Enum E);

        const std::string ToString(const bool WILL_WRAP = false,
                                   const bool WILL_ABBR = false) const;

        friend bool operator==(const TraitSet & L, const TraitSet & R);
        friend bool operator<(const TraitSet & L, const TraitSet & R);

    private:
        RatioStatVec_t statVec_;
    
    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & statVec_;
        }
    };


    inline bool operator==(const TraitSet & L, const TraitSet & R)
    {
        return (L.statVec_ == R.statVec_);
    }


    inline bool operator!=(const TraitSet & L, const TraitSet & R)
    {
        return ! (L == R);
    }


    inline bool operator<(const TraitSet & L, const TraitSet & R)
    {
        return (L.statVec_ < R.statVec_);
    }

}
}

#endif //GAME_STATS_TRAITSSET_HPP_INCLUDED
