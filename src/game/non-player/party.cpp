// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// party.hpp (non-player)
//
#include "party.hpp"

#include "utilz/boost-string-includes.hpp"

#include "game/non-player/character.hpp"

#include <map>
#include <sstream>
#include <utility>


namespace game
{
namespace non_player
{


    Party::Party(const CharacterSVec_t & CHARACTERS_SVEC)
    :
        charactersSVec_(CHARACTERS_SVEC)
    {}


    Party::~Party()
    {}


    bool Party::Add(const CharacterSPtr_t & CHARACTER_SPTR)
    {
        if (charactersSVec_.size() < charactersSVec_.max_size())
        {
            charactersSVec_.push_back(CHARACTER_SPTR);
            return true;
        }
        else
            return false;
    }



    bool Party::Remove(const CharacterSPtr_t & CHARACTER_SPTR)
    {
        const CharacterSVecCIter_t FOUND_ITER( std::find(charactersSVec_.begin(), charactersSVec_.end(), CHARACTER_SPTR) );

        if (FOUND_ITER == charactersSVec_.end())
            return false;
        else
        {
            charactersSVec_.erase(FOUND_ITER);
            return true;
        }
    }


    CharacterSPtr_t Party::FindByCreaturePtr(creature::CreatureCPtrC_t CREATURE_CPTRC) const
    {
        for (auto const & NEXT_CHARACTER_SPTR : charactersSVec_)
            if (NEXT_CHARACTER_SPTR.get() == CREATURE_CPTRC)
                return NEXT_CHARACTER_SPTR;

        return CharacterSPtr_t();
    }


    const std::string Party::Summary() const
    {
        using RaceRolePair_t = std::pair<std::string, std::string>;
        using RaceRoleCountMap_t = std::map<RaceRolePair_t, std::size_t>;
        RaceRoleCountMap_t raceRoleMap;

        //count all race/role combinations
        for (auto const & NEXT_CHAR_SPTR : charactersSVec_)
            raceRoleMap[std::make_pair(NEXT_CHAR_SPTR->Race().Name(), NEXT_CHAR_SPTR->Role().Name())]++;

        //make a single string summary of all race/role combinations
        std::ostringstream ss;
        using RaceRoleCountMapIter_t = RaceRoleCountMap_t::iterator;
        for (RaceRoleCountMapIter_t itr(raceRoleMap.begin()); itr != raceRoleMap.end(); ++itr)
        {
            auto const NEXT_RACEROLECOUNT_PAIR{ * itr };

            if (itr != raceRoleMap.begin())
            {
                ss << ", ";

                auto nextItr{ itr };
                std::advance(nextItr, 1);
                if (nextItr == raceRoleMap.end())
                {
                    ss << "and ";
                }
            }

            ss << NEXT_RACEROLECOUNT_PAIR.second
               << " " << NEXT_RACEROLECOUNT_PAIR.first.first
               << " " << NEXT_RACEROLECOUNT_PAIR.first.second
               << ((NEXT_RACEROLECOUNT_PAIR.second > 1) ? "s" : "");
        }

        return ss.str();
    }

}
}
