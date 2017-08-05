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
#include "trait.hpp"
#include "misc/boost-serialize-includes.hpp"

#include <vector>


namespace game
{
namespace stats
{

    class TraitSet
    {
    public:
        TraitSet(const TraitValueVec_t & TRAITS_VEC = {});

        Trait & Get(const Traits::Enum E);
        const Trait & GetCopy(const Traits::Enum E) const;

        const std::string ToString(const bool WILL_WRAP,
                                   const bool WILL_ABBR,
                                   const bool WILL_PREVENT_NEGATIVE,
                                   const bool WILL_PREFIX_PERCENT) const;

        const std::string StatsString(const bool WILL_WRAP) const;

        friend bool operator==(const TraitSet & L, const TraitSet & R);
        friend bool operator<(const TraitSet & L, const TraitSet & R);

    private:
        const std::string StatStringHelper(const stats::Traits::Enum,
                                           const bool                WILL_PREFIX = true) const;

    private:
        TraitVec_t traitVec_;
    
    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & traitVec_;
        }
    };


    inline bool operator==(const TraitSet & L, const TraitSet & R)
    {
        return (L.traitVec_ == R.traitVec_);
    }


    inline bool operator!=(const TraitSet & L, const TraitSet & R)
    {
        return ! (L == R);
    }


    inline bool operator<(const TraitSet & L, const TraitSet & R)
    {
        return (L.traitVec_ < R.traitVec_);
    }

}
}

#endif //GAME_STATS_TRAITSSET_HPP_INCLUDED
